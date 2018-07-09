/* PIANO HUMANO (Slave)
  Versión de hardware: 2
  Versión de firmware: 3 (octubre - noviembre 2017)

  Hipercubo + Inventoteca
  Diseño de hardware:
  Programa: RAH

  Bibliotecas
  Microcontrolador: ATtiny85
  Nucleo: ATtinyCore 1.1.4 https://github.com/SpenceKonde/ATTinyCore
  El nucleo incluye la función tone() y la biblioteca "Wire" para I2C
  Ultrasónico: NewPing 1.8 http://playground.arduino.cc/Code/NewPing
  Arduino IDE 1.8.2

  Para deactivar el RESET
  https://github.com/RalphBacon/ATTiny85_Fuse_Resetter
*/

// Direccion I2C
#define DIR 15 //direccion I2C
//las direcciones 0 a 7 están reservadas
//la frecuencia inicial se calcula a partir de esta dirección

// Definición de pines
#define RESET_ACTIVADO 0
//Existen dos versiones del circuito impreso: En la primera versión el
//reset externo está desactivado y el pin correspondiente se conecta al
//LED. En la segunda versión el reset externo está activado y el pin no
//se utiliza, además las terminales TRIG y ECHO se conectan a un solo pin.
//La definición anterior permite elegir la configuración para cada versión.
#if RESET_ACTIVADO// == 1 segunda versión del PCB
#define SDA  0 //serial data (I2C)
#define TRIG 1 //trigger
#define ECHO 1 //echo
#define SCL  2 //serial clock (I2C)
#define LED  3 //LED
#define BOC  4 //bocina
#else //RESET_ACTIVADO == 0 primera versión del PCB
#define SDA  0 //serial data (I2C)
#define TRIG 1 //trigger
#define SCL  2 //serial clock (I2C)
#define ECHO 3 //echo
#define BOC  4 //bocina
#define LED  5 //LED
#endif

// Rango del sensor
#define DMIN  1 //distancia minima (cm)
#define DMAX 30 //distancia maxima (cm)
//#define DHIS 20 //distancia histéresis

// Bibliotecas
#include <Wire.h> //comunicación I2C
#include <NewPing.h> //sensor ultrasónico

// Variables globales
NewPing sonar(TRIG, ECHO, DMAX); //objeto que controla sensor
byte d = 0; //distancia detectada (en cm)

bool bocEnc = false; //bocina encendida = false
bool ledEnc = false; //LED encendido = false
bool senHab = true; //sensor habilitado = true

unsigned long tBoc = 0; //momento de apagar bocina
unsigned long tLed = 0; //momento de apagar LED
unsigned long tSen = 0; //momento de leer sensor

bool ledAuto = false; //LED no activado por comando
bool bocAuto = false; //bocina no activada por comando
// Cuando el controlador envia el comando TOCAR (T), el LED y la
// bocina funcionan en modo automático y el sensor no se toma en
// cuenta hasta que los actuadores se apagan y salen del modo auto.

// Número de nota
byte nota = DIR + 52;
//La primera tecla tiene dirección 8 y numero de nota 60, la segunda
//tecla tiene dirección 9 y número de nota 61, y así sucesivamente.

// Frecuencia
//El número de nota se usa para calcular la frecuencia
float calcularFrec(byte n) { //función para calcular frecuencia
  return 440.0 * pow(2.0, float(n - 69) / 12.0);
}
float frec = calcularFrec( nota );

//=============================================================================
void setup() {
  Wire.begin(DIR); //activar bus I2C (Slave)
  Wire.onReceive(receiveEvent); //recibe un comando
  Wire.onRequest(requestEvent); //envía estado del sensor

  pinMode(BOC, OUTPUT);
  pinMode(LED, OUTPUT);
}//fin setup()

//=============================================================================
void loop() {
  // Si la bocina está activada por comando del controlador
  // (modo auto), revisar si es momento de apagarse
  if ( bocAuto ) {
    if ( millis() >= tBoc ) {
      noTone(BOC); //apagar bocina
      bocAuto = false;
      bocEnc = false;
    }
  }
  // Si el LED está activado por comando del controlador
  // (modo auto), revisar si es momento de apagarse
  if (ledAuto) {
    if (millis() >= tLed) {
      digitalWrite(LED, LOW); //apagar LED
      ledAuto = false;
      ledEnc = false;
    }
  }

  // Si el sensor está habilitado y los actuadores no están
  // en modo auto, revisar si es momento de leer el sensor
  if (senHab and !bocAuto and !ledAuto) {
    if (millis() >= tSen)
    {
      d = sonar.ping_cm(); //leer distancia en centímetros
      tSen = millis() + 30; //volver a leer en 30 ms

      if ( DMIN <= d and d <= DMAX ) //si objeto en rango de deteccion
      {
        if (bocEnc == false) { //si bocina apagada
          tone(BOC, frec); //encender bocina
          bocEnc = true;
        }
        if (ledEnc == false) { //si LED apagado
          digitalWrite(LED, HIGH); //encender LED
          ledEnc = true;
        }
      }
      else //si no hay objeto en rango de detección
      {
        if (bocEnc == true) { //si bocina encendida
          noTone(BOC); //apagar bocina
          bocEnc = false;
        }
        if (ledEnc == true) { //si LED encendido
          digitalWrite(LED, LOW); //apagar LED
          ledEnc = false;
        }
      }
    }
  }
}//fin loop()

//=============================================================================
void requestEvent() {
  //En este evento se solicita el estado del sensor
  Wire.write(d); //enviar distancia detectada
}//fin requestEvent()

//=============================================================================
void receiveEvent(int numBytes) {
  // En este evento se recibe y se interpreta un comando.
  // En total se reciben 3 bytes de datos, el primer byte es el comando,
  // los siguientes bytes son el valor o argumento del comando.
  // Dependiendo del comando se utilizan 1 o 2 bytes del argumento.

  char com = Wire.read(); //comando
  uint8_t x[2]; //se reciben 2 bytes mas
  x[0] = Wire.read();
  x[1] = Wire.read();
  //los 2 bytes se ensamblan como un entero
  uint16_t *val = (uint16_t *)x;

  switch (com)
  {
    case 'B': //controlar bocina
      if (*val > 0) {
        bocEnc = true; //encender bocina
        bocAuto = true; //en modo automático
        tBoc = millis() + *val; //momento de apagar
        tone(BOC, frec); //usar frecuencia calculada
      }
      else {
        bocEnc = false; //apagar bocina
        bocAuto = false; //salir de modo automático
        noTone(BOC); //usar frecuencia calculada
      }
      break; //case 'B'

    case 'L': //controlar LED
      if (*val > 0) {
        ledEnc = true; //encender LED
        ledAuto = true; //en modo automático
        tLed = millis() + *val; //momento de apagar
        digitalWrite(LED, HIGH); //activar pin
      }
      else {
        ledEnc = false; //apagar LED
        ledAuto = false; //salir de modo automático
        digitalWrite(LED, LOW); //activar pin
      }
      break; //case 'L'

    case 'N': //cambiar nota
      nota = x[0];
      frec = calcularFrec(nota);
      break; //case 'N'

    case 'S': //habilitar o deshabilitar sensor
      senHab = (bool) x[0];
      break; //case 'S'

    case 'T': //tocar nota
      if (*val > 0) {
        bocEnc = true; //encender bocina
        bocAuto = true; //en modo automático
        ledEnc = true; //encender LED
        ledAuto = true; //en modo automático
        tBoc = millis() + *val; //momento de apagar
        tLed = tBoc; //momento igual para los dos actuadores
        tone(BOC, frec); //usar frecuencia calculada
        digitalWrite(LED, HIGH); //activar pin
      }
      else { //dejar de tocar nota
        bocEnc = false; //apagar bocina
        bocAuto = false; //modo manual
        ledEnc = false; //apagar LED
        ledAuto = false; //modo manual
        noTone(BOC); //sin sonido
        digitalWrite(LED, LOW); //desactivar pin
      }
      break; //case 'T'
  }
}//fin receiveEvent()
