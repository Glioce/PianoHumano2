/************************************************************
  Programa para probar comunicación por I2C a baja velocidad

  Reset externo activado. Conectar pin 1 a VCC.
************************************************************/

#include <Wire.h>
#define DIR 1 //dirección I2C
uint8_t x;

void setup() {
  Wire.begin(DIR); //activar bus I2C (Slave)
  Wire.setClock(10000); //baja velocidad para mayor seguridad
  Wire.onReceive(rcEv); //recibe datos
  Wire.onRequest(rqEv); //se solicitan datos
}

void loop() {}

void rcEv(int n) { //receive event
  x = Wire.read();
}

void rqEv() { //request event
  Wire.write(x);
}

