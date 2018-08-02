#include <Wire.h> //comunicación I2C
#define DIR 1

void setup() {
  Wire.begin(); //I2C (Master)
  Wire.setClock(10000); //baja velocidad para mayor seguridad
  
  Serial.begin(9600); //serial
  Serial.println("Test I2C 10kHz");
}

void loop() {
  uint8_t x = random(255);
  Wire.beginTransmission(DIR);
  Wire.write(x);
  Wire.endTransmission();
  Serial.print("x: "); Serial.println(x);

  delay(500);

  uint8_t y;
  Wire.requestFrom(DIR, 1);
  y = Wire.read();
  Serial.print("y: "); Serial.println(y);

  delay(500);
}
