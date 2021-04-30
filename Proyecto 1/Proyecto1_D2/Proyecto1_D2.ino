//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   IE3027: Electrónica Digital 2 - 2019
*/
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <cstdlib>
#include <TM4C123GH6PM.h>
//#include <graficos.c>
#include <SPI.h>
#include <SD.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1

#define jump1 PUSH1
#define duck1  PUSH2
#define jump2 PE_3 //12
#define duck2 PE_2 //13

extern uint8_t pastel [];
extern uint8_t grama [];
extern uint8_t dino [];
extern uint8_t nube [];
extern uint8_t dino_agachado [];
extern uint8_t globo [];
extern uint8_t regalo [];

volatile int d1_s = 0;
volatile int d2_s = 0;
volatile int d1_d = 0;
volatile int d2_d = 0;
volatile int lastd1_s = !d1_s;
volatile int lastd2_s = !d2_s;
volatile int lastd1_d = !d1_d;
volatile int lastd2_d = !d2_d;

int s = 0;
int s2 = 0;
int obj_f = 1;
int obj  = 0;
int contsalto = 0;
int contsalto2 = 0;
int agache_activo = 0;
int agache_activo2 = 0;
int jumping = 0;
int jumping2 = 0;

int coordy1 = 0;
int coordx1 = 0;
int obsty = 0;
int obstx = 0;
int coordy2 = 0;
int coordx2 = 0;

int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};
int i = 0;
int rx = 160;
int gx = 160;
int conta = 0;
int gameover1 = 0;

File myFile;
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);
void LCD_FONDO(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

void flag_d1s();
void flag_d2s();
void flag_d1d_r();
void flag_d2d_f();
void flag_d2d_r();

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  pinMode(jump1, INPUT_PULLUP);
  pinMode(jump2, INPUT_PULLUP);
  pinMode(duck1, INPUT);
  pinMode(duck2, INPUT);
  attachInterrupt(digitalPinToInterrupt(jump1), flag_d1s, FALLING);
  attachInterrupt(digitalPinToInterrupt(jump2), flag_d2s, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(duck1), flag_d1d_r, CHANGE);
  //  attachInterrupt(digitalPinToInterrupt(duck2), flag_d2d_r, CHANGE);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);


      delay(10);
      LCD_FONDO(0, 0, 320, 240);
      delay(1000);

  //FillRect(0, 0, 319, 239, 0x421b);
  FillRect(0, 0, 319, 223, 0xffff);
  String text1 = "Dino B-Day Party!";
  LCD_Print(text1, 20, 100, 2, 0x0000, 0xffff);
  //LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);

  //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);

  for (int x = 0; x < 340; x++) {
    LCD_Bitmap(x, 224, 16, 16, grama);
    x += 15;
  }

}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
    i = 150;
   LCD_Sprite(i, 40, 50, 17, nube, 1, 0, 0, 0); 
//   V_line (i-1, 40, 50, 0xffff);
  conta++;
  int anim = (conta / 11) % 2;

  //****************************************************Pastel y globo moviendose
//  if (obj_f == 0) {
//    switch (obj) {
//      case 0 :{
        LCD_Bitmap(rx, 201, 18, 22, pastel);
        obstx = rx;
        obsty = 201;
        rx = rx - 1;
        V_line (rx + 34, 202, 18, 0xffff);
        V_line (rx + 33, 202, 18, 0xffff);
        V_line (rx + 32, 202, 18, 0xffff);
             V_line (rx+31,202, 18, 0xffff);
             V_line (rx+30,202, 18, 0xffff);
             V_line (rx+29,202, 18, 0xffff);
             V_line (rx+28,202, 18, 0xffff);
             V_line (rx+27,202, 18, 0xffff);
             V_line (rx+26,202, 18, 0xffff);
             V_line (rx+25,202, 18, 0xffff);

        if (rx <= 0) {
          rx = 160;
          obj_f = 1;
          Serial.print("Bandera1");
        }
//      }
//        break;
//
//      case 1 : {
        LCD_Sprite(gx, 150, 12, 40, globo, 3, 0, 0, 0); // Globo
//        obstx = gx;
        obsty = 150;
        gx = gx + 1;
        V_line (gx - 10, 150, 40, 0xffff);
        V_line (gx - 9, 150, 40, 0xffff);
        V_line (gx - 8, 150, 40, 0xffff);
        V_line (gx - 7, 150, 40, 0xffff);
        V_line (gx - 6, 150, 40, 0xffff);
        V_line (gx - 5, 150, 40, 0xffff);
        V_line (gx - 4, 150, 40, 0xffff);
        V_line (gx - 3, 150, 40, 0xffff);

        if (gx >= 320) {
          gx = 160;
          obj_f = 1;
          Serial.print("bandera0");
        }
//      }
//        break;

//    }
//  }
  else if (obj_f == 1){
    //int obj  = 2;
   obj_f = 0;
   int obj = (rand() %2);
   Serial.println(obj);
    
  }

  //     delay(5);
  //     LCD_Sprite(gx, 150, 12, 40, globo, 3, 1, 0, 0);

  //**************************************************************************************

  if (d1_s) { // Comparación que ejecuta el saltio del jugador 1
    delay(5);
    contsalto++;
    jumping = 1;
    s = (contsalto) % 51;
    if (s < 25) {
      coordy1 = 180 -s;
      LCD_Sprite(0, coordy1, 31, 42, dino, 2 , 0, 0, 0);
      for (int sub1 = 0; sub1 < 6; sub1++ ) {
        H_line(0, (180 + 42) - s - sub1, 31, 0xffff);
      }
    }
    else if (s == 25) {
      coordy1 = 180-25;
      LCD_Sprite(0,coordy1, 31, 42, dino, 2 , 0, 0, 0); //Salto del dinosaurio 1
    }
    else if (s > 25 and s < 50) {
      coordy1 = 180 - 25 + (s - 25);
      LCD_Sprite(0,coordy1 , 31, 42, dino, 2 , 0, 0, 0);
      for (int cae1 = 0; cae1 < 6; cae1++ ) {
        H_line(0, 180 - 25 + ((s) - 27) - cae1, 31, 0xffff);
      }

    }
    else if (s >= 50) { //al terminar el salto Bajo bandera de salto y reinicio variables
      d1_s = 0;
      jumping = 0;
    }

  }
  else if (digitalRead(duck1) == HIGH and d1_s == LOW and agache_activo == 0 and jumping ==0) {
    coordy1 = 180;
    LCD_Sprite(0, coordy1, 31, 42, dino, 2 , anim, 0, 0);      //Dinosaurio 1

  }


  if (d2_s) { // Chequeo bandera de rutina salto
    delay(5);
    contsalto2++;
    jumping2 = 1;
    s2 = (contsalto2) % 51;
    if (s2 < 25) {
      coordy2 = 180-s2;
      LCD_Sprite(288, coordy2, 31, 42, dino, 2 , 0, 1, 0);
    }
    else if (s2 == 25) {
      coordy2 = 180-25;
      LCD_Sprite(288, coordy2, 31, 42, dino, 2 , 0, 1, 0);
    }
    else if (s2 > 25 and s2 < 50) {
      coordy2 = 180 - 25 + (s2 - 25);
      LCD_Sprite(288, coordy2, 31, 42, dino, 2 , 0, 1, 0);
      H_line(288, 180 - 25 + ((s2) - 26), 31, 0xffff);

    }
    else if (s2 == 50) { // apago bandera rutina y reinicio variable
      d2_s = 0;
      jumping2 = 0;
    }
  }
  else if (digitalRead(duck2) == HIGH and d2_s == LOW and agache_activo2 == 0 and jumping2 == 0) {
    coordy2 = 180;
    LCD_Sprite(288, 180, 31, 42, dino, 2 , anim, 1, 0);      //Dinosaurio 2
  }

  if (digitalRead(duck1) == LOW) {
    coordy1 = 180+11;
    LCD_Sprite(0, coordy1, 45, 31, dino_agachado, 2, anim, 0, 0);
    for (int dow = 0; dow <= 11; dow++) {
      H_line(0, 180 + 11 - dow, 45, 0xffff);
    }
    agache_activo = 1;
  }
  else if (digitalRead(duck1) == HIGH and d1_s == LOW and agache_activo == 1) {
    V_line(46 - 1, 180 + 11, 31, 0xffff);
    V_line(46 - 2, 180 + 11, 31, 0xffff);
    V_line(46 - 3, 180 + 11, 31, 0xffff);
    V_line(46 - 4, 180 + 11, 31, 0xffff);
    V_line(46 - 5, 180 + 11, 31, 0xffff);
    V_line(46 - 6, 180 + 11, 31, 0xffff);
    V_line(46 - 7, 180 + 11, 31, 0xffff);
    V_line(46 - 8, 180 + 11, 31, 0xffff);
    V_line(46 - 9, 180 + 11, 31, 0xffff);
    V_line(46 - 10, 180 + 11, 31, 0xffff);
    V_line(46 - 11, 180 + 11, 31, 0xffff);
    V_line(46 - 12, 180 + 11, 31, 0xffff);
    V_line(46 - 13, 180 + 11, 31, 0xffff);
    V_line(46 - 14, 180 + 11, 31, 0xffff);
    V_line(46 - 15, 180 + 11, 31, 0xffff);

    agache_activo = 0;
  }

  if (digitalRead(duck2) == LOW ) {
    coordy2 = 180+11;
    LCD_Sprite(275, coordy2, 45, 31, dino_agachado, 2, anim, 1, 0);
    for (int dow = 0; dow <= 11; dow++) {
      H_line(288, 180 + 11 - dow, 45, 0xffff);
    }
    agache_activo2 = 1;
  }
  else if (digitalRead(duck2) == HIGH and d2_s == LOW and agache_activo2 == 1) {
    V_line(275 + 1, 180 + 11, 31, 0xffff);
    V_line(275 + 2, 180 + 11, 31, 0xffff);
    V_line(275 + 3, 180 + 11, 31, 0xffff);
    V_line(275 + 4, 180 + 11, 31, 0xffff);
    V_line(275 + 5, 180 + 11, 31, 0xffff);
    V_line(275 + 6, 180 + 11, 31, 0xffff);
    V_line(275 + 7, 180 + 11, 31, 0xffff);
    V_line(275 + 8, 180 + 11, 31, 0xffff);
    V_line(275 + 9, 180 + 11, 31, 0xffff);
    V_line(275 + 10, 180 + 11, 31, 0xffff);
    V_line(275 + 11, 180 + 11, 31, 0xffff);
    V_line(275 + 12, 180 + 11, 31, 0xffff);
    V_line(275 + 13, 180 + 11, 31, 0xffff);
    V_line(275 + 14, 180 + 11, 31, 0xffff);
    V_line(275 + 15, 180 + 11, 31, 0xffff);

    agache_activo2 = 0;
  }


int ycol1 = coordy1 + 41;
int ycolpas = obsty + 22;
if ((coordy1<obsty<ycol1) or (obsty<ycolpas) and gameover1 == 0 and obstx < 31){
  Serial.println("game over 1");
  Serial.print(obstx);
  gameover1 = 1;
}  
}


//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y + i, w, c);
  }
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}


//***************************************************************************************************************************************
// Leer archivos de la SD y escribirlos x, y, base, altura, archivo txt []
//***************************************************************************************************************************************
void LCD_FONDO(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  char data [2];
  int dataindex = 0;
  char caracter;

  myFile = SD.open("PAN.txt");
  if (myFile) {
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      caracter = myFile.read();
      if ((caracter != ',') || (caracter != ' ')) {
        data[dataindex] = caracter;
        dataindex++;
        //         data.concat(caracter);
      }
      else{
      if (caracter == ',') {
      uint8_t mandar_data = (uint8_t)strtol(data, NULL, 16);
      LCD_DATA(mandar_data);
      dataindex = 0;
      }     
      } //end else
    } //end while
  } //end if my File
  // close the file:
  digitalWrite(LCD_CS, HIGH);
  myFile.close(); //end if myFile

} //End funcion LCD_Fondo

//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}


//***************************************************************************************************************************************
// Función Interrupciones
//***************************************************************************************************************************************

void flag_d1s() {
  if (lastd1_s != d1_s) {
    d1_s = !d1_s;
    lastd1_s = d1_s;
    delay(50);
  }
}

void flag_d2s() {
  if (lastd2_s != d2_s) {
    d2_s = !d2_s;
    lastd2_s = d2_s;
    delay(50);
  }
}


//void flag_d1d_r(){
//  if (lastd1_d != d1_d){
//  d1_d = !d1_d;
//  lastd1_d = d1_d;
//  delay(50);
//  }
//}
//
//void flag_d2d_r(){
//  if (lastd2_d != d2_d){
//  d2_d = !d2_d;
//  lastd2_d = d2_d;
//  delay(50);
//  }
//  }
