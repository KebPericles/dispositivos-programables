#define BOTON GPIOC_IDR.B0
#define DISPLAY_1_2 GPIOA_ODR
#define DISPLAY_3_4 GPIOB_ODR

#define ESPACIO 0xFF
#define C_F 0x8E
#define C_E 0x86
#define C_R 0xAF
#define C_N 0xAB
#define C_A 0x88
#define C_D 0xA1
#define C_Z 0xA4
#define C_G 0xC2
#define C_U 0xE3
#define C_O 0xA3
#define C_K 0x89
#define C_B 0x83
#define C_S 0x92
#define C_T 0x87
#define C_I 0xCF
#define C_M 0xC9
#define C_L 0xC7
#define C_P 0x8C
#define C_J 0xE1


// Declara variables globales
const unsigned short int segmentos[] = {
      C_F, C_E, C_R, C_N, C_A, C_N, C_D, C_E, C_Z, ESPACIO,
      C_G, C_U, C_E, C_R, C_R, C_E, C_R, C_O,      ESPACIO,
      C_K, C_E, C_B,                               ESPACIO,
      C_S, C_E, C_B, C_A, C_S, C_T, C_I, C_A, C_N, ESPACIO,
      ESPACIO,ESPACIO,ESPACIO,ESPACIO,
      C_B, C_A, C_Z, C_A, C_N,                     ESPACIO,
      C_M, C_A, C_R, C_T, C_I, C_N, C_E, C_Z,      ESPACIO,
      C_L, C_E, C_O, C_N, C_A, C_R, C_D, C_O,      ESPACIO,
      C_A, C_N, C_G, C_E, C_L,                     ESPACIO,
      ESPACIO,ESPACIO,ESPACIO,ESPACIO,
      C_P, C_E, C_R, C_E, C_Z,                     ESPACIO,
      C_R, C_O, C_M, C_E, C_R, C_O,                ESPACIO,
      C_J, C_O, C_S, C_E,                          ESPACIO,
      C_A, C_N, C_G, C_E, C_L,                     ESPACIO,
      ESPACIO,ESPACIO,ESPACIO,ESPACIO
};

unsigned short int contador;
unsigned short int mensaje[4];

unsigned short int getBit(unsigned short int num, unsigned short int position);

/**
 * @brief Muestra el mensaje en los displays
 * 
 */
void showMessage();

void displayToPorts(unsigned short int segmentos);
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

  GPIO_Digital_Input(
    &GPIOC_BASE,
    _GPIO_PINMASK_0
  );

  GPIO_Config( // Puerto A
    &GPIOA_BASE, _GPIO_PINMASK_ALL,
    _GPIO_CFG_MODE_OUTPUT | _GPIO_CFG_SPEED_2MHZ |
    _GPIO_CFG_OTYPE_PP
  );

  GPIO_Config( // Puerto B
    &GPIOB_BASE, _GPIO_PINMASK_ALL,
    _GPIO_CFG_MODE_OUTPUT | _GPIO_CFG_SPEED_2MHZ |
    _GPIO_CFG_OTYPE_PP
  );
}
void Ini_var() { contador = 0; }

void showMessage() {
  // Se debe tener en cuenta que los pines de los puertos A y B son de 16 bits
  // Resetear los bits de los puertos A y B
  DISPLAY_1_2 = 0;
  DISPLAY_3_4 = 0;
  //No están disponibles los pines: PA2, PA3, PB11, PC14, PC15.
  // Escribir el mensaje en los bits de cada puerto de salida
  // Para el display 1 (PA) los bits son 0- 1- 4- 5- 6- 7- 8
  
  DISPLAY_1_2 = mensaje[0] & 0x03;
  DISPLAY_1_2 |= (mensaje[0] & 0x7C) << 2;
  // Para el display 2 (PA) los bits son 9-10-11-12-13-14-15
  DISPLAY_1_2 |= (mensaje[1] & 0x7F) << 9;
  // Para el display 3 (PB) los bits son 0- 1- 2- 3- 4- 5- 6
  DISPLAY_3_4 = mensaje[2] & 0x7F;
  // Para el display 4 (PB) los bits son 7- 8- 9-10-12-13-14
  DISPLAY_3_4 |= (mensaje[3] & 0x0F) << 7;
  DISPLAY_3_4 |= (mensaje[3] & 0x70) << 8;
}

void Proceso() {
  if (BOTON==0)  //Verifica si se oprime el boton
  {
    // Mover
    mensaje[3] = mensaje[2];
    mensaje[2] = mensaje[1];
    mensaje[1] = mensaje[0];
    mensaje[0] = segmentos[contador] & 0xFF;

    showMessage();
    if (++contador == 98)
      contador = 0;
    delay_ms(300);
  }
}