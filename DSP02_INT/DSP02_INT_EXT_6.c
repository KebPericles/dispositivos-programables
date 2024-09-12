#define RESET GPIOB_IDR.B1
#define START GPIOB_IDR.B0

#define DISPLAY_1_2 GPIOA_ODR
#define DISPLAY_3_4 GPIOC_ODR

#define N_0 0b1000000
#define N_1 0xF9
#define N_2 0b0100100
#define N_3 0b0110000
#define N_4 0b0011001
#define N_5 0b0010010
#define N_6 0b0000010
#define N_7 0b1111000
#define N_8 0b0000000
#define N_9 0b0010000

// Declara variables globales
const unsigned short int numeros[] = {N_0, N_1, N_2, N_3, N_4,
                                      N_5, N_6, N_7, N_8, N_9};

char contar; // Flag para cuenta regresiva
unsigned int contador; // Es int por razones misteriosas en la division

unsigned short int minutos;
unsigned short int segundos;

unsigned short int digito4;
unsigned short int digito3;
unsigned short int digito2;
unsigned short int digito1;

unsigned short int mensaje[4];

/**
 * @brief Muestra el mensaje en los displays
 *
 */
void actualizarDisplays();
/**
 * @brief Calcula y actualiza los digitos para los displays
 *
 */
void eventoContador();

void Conf_maq();
void Ini_var();
void Proceso();

void start_int() iv IVT_INT_EXTI0 ics ICS_AUTO {
 EXTI_PR.B0 = 1;
 contar = contar ^ 1;
}

void reset_int() iv IVT_INT_EXTI1 ics ICS_AUTO {
 EXTI_PR.B1 = 1;
 contador = 1800;
 contar = 0;
 eventoContador();
}

void main() {
  Conf_maq();
  Ini_var();
  while (1) // Bucle principal
    Proceso();
}

void Conf_maq() {
  // PB0 Y PB1
  // Botones en PULL-DOWN (flotante)
  GPIO_config(&GPIOB_BASE,
              _GPIO_PINMASK_0 | _GPIO_PINMASK_1,
              _GPIO_CFG_PULL_NO | _GPIO_CFG_DIGITAL_INPUT
  );
  
  // PA 0-15 excluyendo 2 y 3
  GPIO_config(&GPIOA_BASE,
              _GPIO_PINMASK_ALL ^ _GPIO_PINMASK_2 ^ _GPIO_PINMASK_3,
              _GPIO_CFG_DIGITAL_OUTPUT | _GPIO_CFG_OTYPE_PP | _GPIO_CFG_SPEED_LOW
  );
  
  // PC 0-13
  GPIO_config(&GPIOC_BASE,
              _GPIO_PINMASK_ALL ^ _GPIO_PINMASK_14 ^ _GPIO_PINMASK_15,
              _GPIO_CFG_DIGITAL_OUTPUT | _GPIO_CFG_OTYPE_PP | _GPIO_CFG_SPEED_LOW
  );
  
  SYSCFGEN_bit = 1;
  // Falling trigger Interruption
  EXTI_RTSR = 0x00000000;
  EXTI_FTSR = 0x00000000 | 1 | (1 << 1);
  // Mapear la interrupcion EXTI0 y EXTI1 a PB
  EXTI_IMR = 0x00000000 | 1 | (1 << 1);
  SYSCFG_EXTICR1 = 0x00000000 | 0b0001 | (0b0001 << 4);
  
  // Activar los vectores de interrupción
  NVIC_IntEnable(IVT_INT_EXTI0);
  NVIC_IntEnable(IVT_INT_EXTI1);
  
  EnableInterrupts();
}

void Ini_var() {
  contar = 0;
  contador = 1800;
  eventoContador();
}

void actualizarDisplays() {
  // Se debe tener en cuenta que los pines de los puertos A y B son de 16 bits
  // Resetear los bits de los puertos A y B
  DISPLAY_1_2 = 0;
  DISPLAY_3_4 = 0;
  // No est?n disponibles los pines: PA2, PA3, PB11, PC14, PC15.
  //  Escribir el mensaje en los bits de cada puerto de salida
  //  Para el display 1 (PA) los bits son 0- 1- 4- 5- 6- 7- 8

  DISPLAY_1_2 = mensaje[0] & 0x03;
  DISPLAY_1_2 |= (mensaje[0] & 0x7C) << 2;
  // Para el display 2 (PA) los bits son 9-10-11-12-13-14-15
  DISPLAY_1_2 |= (mensaje[1] & 0x7F) << 9;// 7 bits del primer msj +
                                          // 2 bits de los puertos no usables

  // Para el display 3 (PC) los bits son 0- 1- 2- 3- 4- 5- 6
  DISPLAY_3_4 = mensaje[2] & 0x7F;
  // Para el display 4 (PC) los bits son 7- 8- 9-10-11-12-13
  DISPLAY_3_4 |= (mensaje[3] & 0x7F) << 7;
}

void eventoContador()
{
  minutos = contador / 60;
  segundos = contador % 60;

  digito4 = minutos / 10;
  digito3 = minutos % 10;
  digito2 = segundos / 10;
  digito1 = segundos % 10;

  mensaje[3] = numeros[digito4];
  mensaje[2] = numeros[digito3];
  mensaje[1] = numeros[digito2];
  mensaje[0] = numeros[digito1];

  actualizarDisplays();
}

void Proceso()
{
  if (contar == 1 && contador != 0)
  {
    // Cuenta 1 segundo, si el contador no ha llegado a cero
    contador--;
    eventoContador();
    delay_ms(10);
  }
}