#define RESET GPIOC_IDR.B0
#define START GPIOC_IDR.B1

#define DISPLAY_1_2 GPIOA_ODR
#define DISPLAY_3_4 GPIOB_ODR

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

unsigned short int contador;
unsigned short int minutos = 0;
unsigned short int segundos = 0;

unsigned short int digito4 = 2;
unsigned short int digito3 = 3;
unsigned short int digito2 = 4;
unsigned short int digito1 = 5;
unsigned short int mensaje[4];

/**
 * @brief Muestra el mensaje en los displays
 *
 */
void showMessage();
void Conf_maq();
void Ini_var();
void Proceso();

void main() {
  Conf_maq();
  Ini_var();
  while (1) // Bucle principal
    Proceso();
}

void Conf_maq() {
  // PC ENABLE
  RCC_AHB1ENR |= 1 << GPIOCEN;
  // ENTRADA EN PC0 Y PC1 FLOTANTE
  GPIOC_MODER = 0 << 31;
  GPIOC_PUPDR |= 0b0000;

  // PA PB ENABLE
  RCC_AHB1ENR |= (1 << GPIOAEN) | (1 << GPIOBEN);

  // TODOS LOS PUERTOS SALIDA  PUSH PULL
  GPIOA_MODER = 0x55555555;
  GPIOB_MODER = 0x55555555;
  GPIOA_OTYPER |= 0ul;
  GPIOB_OTYPER |= 0ul;

  // VELOCIDAD MINIMA
  GPIOA_OSPEEDR = 0 << 31;
  GPIOB_OSPEEDR = 0 << 31;
}

void Ini_var() {
  contador = 1800;
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
  showMessage();
}

void showMessage() {
  // Se debe tener en cuenta que los pines de los puertos A y B son de 16 bits
  // Resetear los bits de los puertos A y B
  DISPLAY_1_2 = 0;
  DISPLAY_3_4 = 0;
  // No est�n disponibles los pines: PA2, PA3, PB11, PC14, PC15.
  //  Escribir el mensaje en los bits de cada puerto de salida
  //  Para el display 1 (PA) los bits son 0- 1- 4- 5- 6- 7- 8

  DISPLAY_1_2 = mensaje[0] & 0x03;
  DISPLAY_1_2 |= (mensaje[0] & 0x7C) << 2;
  // Para el display 2 (PA) los bits son 9-10-11-12-13-14-15
  DISPLAY_1_2 |= (mensaje[1] & 0x7F) << 9;// 7 bits del primer msj + 
                                          // 2 bits de los puertos no usables
                                          
  // Para el display 3 (PB) los bits son 0- 1- 2- 3- 4- 5- 6
  DISPLAY_3_4 = mensaje[2] & 0x7F;
  // Para el display 4 (PB) los bits son 7- 8- 9-10-12-13-14
  DISPLAY_3_4 |= (mensaje[3] & 0x0F) << 7;
  DISPLAY_3_4 |= (mensaje[3] & 0x70) << 8;
}

void updateContador()
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
  
  showMessage();
}

void Proceso() {

  if (START == 0) // Verifica si se oprime el boton
  {
    if (contador != 0)
      contador--;
    delay_ms(500);
  }

  if (RESET == 1)
  {
    contador = 1800;
    updateContador();
  }
}