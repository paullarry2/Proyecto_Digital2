


#include "wiring_private.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"

// Banco de notas

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_C4_1 260
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0


//Variables control
int m = 0;
int quarter = 0;
int half = 0;
int one = 0;


// Velocidad a la que se reproduce la nota.
int tempo = 200;

// Pin de salida que usaremos para el buzzer
  int buzzer = BLUE_LED;
// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
// notes in the melody:
int melody[] = {
   NOTE_C4_1,NOTE_C4, NOTE_D4, NOTE_C4,NOTE_F4,NOTE_E4,
   NOTE_C4_1,NOTE_C4,NOTE_D4,NOTE_C4,NOTE_G4,NOTE_F4,
   NOTE_C4_1,NOTE_C4,NOTE_C5,NOTE_A4,NOTE_F4,NOTE_F4, NOTE_E4,NOTE_D4,
   NOTE_AS4,NOTE_AS4,NOTE_A4,NOTE_F4,NOTE_G4,NOTE_F4};
   
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 4, 2, 2,2,1,
  4, 4, 2, 2,2,1,
  4, 4, 2, 2,4,4,2,1, 
  4, 4, 2, 2,2,1
  };

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;



void setup() {
  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  pinMode(buzzer, OUTPUT);
  configureTimer1A(); // llamado a configuración del timer.
  Serial.begin(115200);

}

void loop() {
  // no need to repeat the melody.
  Serial.print("Holi");
  delay(1000);
}

void configureTimer1A(){
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // Enable Timer 1 Clock
  ROM_IntMasterEnable(); // Enable Interrupts
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC); // Configure Timer Operation as Periodic
  
  // Configure Timer Frequencia
  // Si se quiere una frecuencia de 1 kHz, el CustomValue debe ser 80000: 80MHz/80k = 1 kHz
  ROM_TimerLoadSet(TIMER1_BASE, TIMER_A,11999400); // En esta configuración estoy creando una interrupción de 6.67hz, usare 125m
  // EL objetivo es tener una interrupción cada una octava es decir 1000/ 8 = 125ms, para que la interrupcion reproduzca octava cada vez que se active. 
  TimerIntRegister(TIMER1_BASE, TIMER_A, &Timer1AHandler);
  ROM_IntEnable(INT_TIMER1A);  // Enable Timer 1A Interrupt
  
  ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // Timer 1A Interrupt when Timeout
  ROM_TimerEnable(TIMER1_BASE, TIMER_A); // Start Timer 1A
}


void Timer1AHandler(void){

int duracion = 1000/noteDurations[m];
Serial.print(duracion);
if (duracion == 125){
  tone(buzzer, melody[m], 150);
  noTone(buzzer);
  m++;
  ROM_TimerIntClear(TIMER1_BASE, TIMER_A);
}
else if (duracion == 250){
  quarter++;
  Serial.print(quarter);
  if (quarter == 2){
    quarter = 0;
    m++;
  }
  tone(buzzer, melody[m], 150);
  noTone(buzzer);
  ROM_TimerIntClear(TIMER1_BASE, TIMER_A);
}

else if (duracion == 500){

  half++;
  if (half == 4){
    half = 0;
    m++;
  }
  tone(buzzer, melody[m], 150);
  noTone(buzzer);
  ROM_TimerIntClear(TIMER1_BASE, TIMER_A);

}
else if (duracion == 1000){

  one++;
  if (one == 8){
    one = 0;
    m++; 
  }
  tone(buzzer, melody[m], 150);
  noTone(buzzer);
  ROM_TimerIntClear(TIMER1_BASE, TIMER_A);
}

else if (m <26){
  m=0;
  ROM_TimerIntClear(TIMER1_BASE, TIMER_A);
}
  
}
