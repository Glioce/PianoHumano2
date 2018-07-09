/* PIANO HUMANO (Master)
  Versión de hardware: 2
  Versión de firmware: 3
  Hipercubo
  
  Hardware:
    Arduino Nano ATmega328P old bootloader
    LCD
    Botones pulsadores
    Regulador de voltaje
    
  IDE 1.8.2
*/

// Canciones ==================================================================
// Archivos -------------------------------------------------------------------
#include "Tune.h"
#include "Escala0.h"
#include "Mary.h"
#include "Sonidito.h"
#define NUM_TUNES 3 //numero de canciones
// Lista ----------------------------------------------------------------------
Tune *tuneList[NUM_TUNES] = {
  &Escala0,
  &Mary,
  &Sonidito,
};

// ¿Depurar por puerto serial?
#define DEBUG 0

// Bibliotecas ================================================================
#include <LiquidCrystal.h> //LCD
#include <Wire.h> //comunicación I2C
//Wire utiliza los siguientes pines en Arduino Uno y Nano
//  A4 - Serial data (SDA)
//  A5 - Serial clock (SCL)
//Wire utiliza los siguientes pines en Arduino Mega2560
//  20 - SDA
//  21 - SCL

// Definiciciones =============================================================
// Definición de pines para la LCD --------------------------------------------
#define RS 8
#define EN 9 //veficar!!
#define D4 4
#define D5 5
#define D6 6
#define D7 7
// Definición de pines para los botones ---------------------------------------
#define BTN_PRE A0 //previo
#define BTN_SIG A1 //siguiente
#define BTN_SEL A2 //seleccionar
#define BTN_CAN A3 //cancelar
// Los botones se leen utilizando la variable PINC
// Número de bit en la variable PINC
#define BIT_PRE 1;
#define BIT_SIG 2;
#define BIT_SEL 3;
#define BIT_CAN 4;

// Varaibles globales =========================================================
// Objeto lcd -----------------------------------------------------------------
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// Registros de botones -------------------------------------------------------
byte regBtn = 0; //estado actual de los botones
byte regPrv = 0; //estado previo
byte regPrs = 0; //botones presionados
byte regSlt = 0; //botones soltados

// Enumeración de menús -------------------------------------------------------
enum menu_t {
  PRINCIPAL, //0 menú inicial
  CANCIONES, //1 lista de canciones
  JUEGOS,    //2 lista de juegos
  TOCANDO    //3 tocando una cación
};
menu_t menuState = PRINCIPAL; //menu acutal
bool menuStart = true; //indica si el menú inicia en este ciclo
byte menuIndex[] = {0, 0, 0}; //indices de los elementos seleccionados

//=============================================================================
void setup() {
  // Iniciar LCD y puertos de comunicación
  lcd.begin(16, 2); //usar 16 columnas y 2 filas
  Wire.begin(); //I2C (Master)
#if DEBUG //si se usa depuración por puerto serial
  Serial.begin(9600); //serial
#endif

  // Configurar entrada de botones
  pinMode(BTN_SEL, INPUT_PULLUP); //seleccionar
  pinMode(BTN_CAN, INPUT_PULLUP); //cancelar
  pinMode(BTN_SIG, INPUT_PULLUP); //siguiente
  pinMode(BTN_PRE, INPUT_PULLUP); //previo

  // Mostrar mensaje inicial
  lcd.print("  Piano Humano");
  lcd.setCursor(3, 1);
  lcd.print("Hipercubo");

  delay(600); //esperar

  // Probar teclas
  //En total son 14 teclas, con direcciones 8 a 21.
  //Las direcciones de 0 a 7 están reservadas.
  // En la nueva versión probar con direcciones de 1 a 14 !!
  char com = 'T'; //comando tocar
  uint16_t val = 300; //valor 300 ms
  byte *x = (byte *)&val; //manejar los bytes de val separadamente
#if DEBUG
  Serial.println(x[0], HEX);
  Serial.println(x[1], HEX);
#endif

  for (byte dir = 8; dir <= 21; dir++) {
    Wire.beginTransmission(dir); //iniciar buffer
    Wire.write(com); //enviar comando tocar
    Wire.write(x, 2); //enviar tiempo
    byte error = Wire.endTransmission(); //recibir respuesta

#if DEBUG
    Serial.print(dir); //inprimir dirección
    if (error == 0) //si transmisión exitosa
      Serial.println(" conectado");
    else Serial.println(" NO conectado");
#endif
  }//fin for()
  
  delay(600);
}//fin setup()

//=============================================================================
void loop() {
  botones(); //actualizar registros de botones
  switch (menuState) {
    case PRINCIPAL: menuPrincipal(); break;
    case CANCIONES: menuCanciones(); break;
    case    JUEGOS: menuJuegos();    break;
    case   TOCANDO: menuTocando();   break;
  }
}//fin loop()

// Funciones ==================================================================
void botones() { //------------------------------------------------------------
  //Actualizar registros de los botones
  //Las entradas tienen activadas la resistencias pullup, por lo
  //que la presión de un botón se detecta como nivel bajo.
  
  regPrv = regBtn; //nuevo estado previo
  regBtn = ~PINC & 0xF; //estado actual (solo se conservan 4 bits)
  regPrs = regBtn & ~regPrv; //presionados
  regSlt = ~regBtn & regPrv; //soltados
}//fin botones()

void menuPrincipal() { //------------------------------------------------------
  String menuItem[] = {"1 Canciones", "2 Juegos   "}; //items en este menú
  if (menuStart) {//si el menu acaba de iniciar
    menuStart = false; //no ejecutar estas líneas en el sig ciclo
    lcd.clear();
    lcd.print("Menu Principal");
    lcd.setCursor(0, 1);
    lcd.print(menuItem[ menuIndex[PRINCIPAL]]);
  }
  if (regPrs & 1 or regPrs & 2) { //si se presiona BTN_PRE o BTN_SIG
    menuIndex[PRINCIPAL] ^= 1; //cambiar indice (0 o 1)
    //lcd.clear();
    //lcd.print("Menu Principal");
    lcd.setCursor(0, 1);
    lcd.print(menuItem[ menuIndex[PRINCIPAL]]);
  }
  if (regPrs & 4) { //si se presiona BTN_SEL
    if (menuIndex[PRINCIPAL] == 0) {//si item "Canciones" seleccionado
      menuState = CANCIONES; //ir al menú CANCIONES
      menuStart = true; //menú acaba de iniciar
    }
    else { //si item "Juegos" seleccionado
      menuState = JUEGOS; //ir al menú JUEGOS
      menuStart = true; //menú acaba de iniciar
    }
  }
  delay(20);
}//fin menuPrincipal()

void menuCanciones() { //------------------------------------------------------
  if (menuStart) {//si el menu acaba de iniciar
    menuStart = false; //no ejecutar este bloque en el siguiente loop()
    lcd.clear();
    lcd.print("Canciones");
    lcd.setCursor(0, 1);
    lcd.print(menuIndex[CANCIONES] + 1); lcd.print(" "); //numero de canción
    lcd.print(tuneList[ menuIndex[CANCIONES]] -> name); //nombre de la canción
  }
  if (regPrs & 1) { //si se presiona BTN_PRE
    if (menuIndex[CANCIONES] > 0) //si indice mayor a 0
      menuIndex[CANCIONES] --; //indice anterior
    else
      menuIndex[CANCIONES] = NUM_TUNES - 1; //indice último
    lcd.clear();
    lcd.print("Canciones");
    lcd.setCursor(0, 1);
    lcd.print(menuIndex[CANCIONES] + 1); lcd.print(" ");
    lcd.print(tuneList[ menuIndex[CANCIONES]] -> name);
  }
  if (regPrs & 2) { //si se presiona BTN_SIG
    menuIndex[CANCIONES] ++; //indice siguiente
    if (menuIndex[CANCIONES] >= NUM_TUNES) //si pasó del último indice
      menuIndex[CANCIONES] = 0; //indice primero
    lcd.clear();
    lcd.print("Canciones");
    lcd.setCursor(0, 1);
    lcd.print(menuIndex[CANCIONES] + 1); lcd.print(" ");
    lcd.print(tuneList[ menuIndex[CANCIONES]] -> name);
  }
  if (regPrs & 4) { //si se presiona BTN_SEL
    //Tocar la canción seleccionada
    menuState = TOCANDO;
    menuStart = true;
  }
  if (regPrs & 8) {//si se presiona BTN_CAN
    //Regresar al menú principal
    menuState = PRINCIPAL;
    menuStart = true;
  }
  delay(20);
}//fin menuCanciones()

void menuJuegos() { //---------------------------------------------------------
  if (menuStart) {//si el menu acaba de iniciar
    menuStart = false;
    lcd.clear();
    lcd.print("Juegos");
    lcd.setCursor(0, 1);
    lcd.print("- No hay juegos");
  }
  if (regPrs & 8) { //si se presiona BTN_CAN
    //Regresar al menú principal
    menuState = PRINCIPAL;
    menuStart = true;
  }
  delay(20);
}

void menuTocando() { //--------------------------------------------------------
  //Esta función no utiliza la variable menuStart
  //Inicializar variables locales
  uint16_t pos = 0; //posición en las listas de comandos
  uint16_t marca = 0; //posición de una marca para repetir
  bool pausa = false; //no está en pausa
  Tune *rola = tuneList[ menuIndex[CANCIONES]]; //referencia a la canción
  byte adr; //dirección (address)
  char com; //comando
  uint16_t val; //valor del comando
  byte *x = (byte *)&val; //valor como array de bytes
  uint16_t del; //tiempo de espera (delay)

  lcd.clear();
  lcd.print("Tocando");
  lcd.setCursor(0, 1);
  lcd.print(rola -> name);

  unsigned long tCom = millis(); //momento de leer comando
  unsigned long tBtn = tCom + 20; //momento de leer botones

  while (true) //se ejecuta hasta que termina la canción o se cancela
  {
    if (millis() >= tBtn) { //si es momento de leer botones
      botones(); //leer botones
      if (regBtn & 4) pausa ^= 1; //si se presiona BTN_SEL, cambia la pausa
      if (regBtn & 8) { //si se presiona BTN_CAN
        menuState = CANCIONES; //regresar al menú canciones
        return; //salir de esta función
      }
      tBtn = millis() + 20; //volver a leer en 20 ms
    }
    while (millis() >= tCom) { //mientras es momento de ejecutar un comando
      //Leer valores
      adr = rola -> adr[pos];
      com = rola -> com[pos];
      val = rola -> val[pos];
      del = rola -> del[pos];
#if DEBUG
      Serial.print(pos); Serial.print("\t");
      Serial.print(adr); Serial.print("\t");
      Serial.print(com); Serial.print("\t");
      Serial.print(val); Serial.print("\t");
      Serial.println(del);
#endif
      //
      switch (com) //ejecutar comando
      {
        case 'B': //controlar bocina
          Wire.beginTransmission(adr);
          Wire.write(com); //enviar comando
          Wire.write(x, 2); //enviar tiempo
          Wire.endTransmission();
          break;

        case 'L': //controlar LED
          Wire.beginTransmission(adr);
          Wire.write(com); //enviar comando
          Wire.write(x, 2); //enviar tiempo
          Wire.endTransmission();
          break;

        case 'M': //asignar marca
          marca = pos;
          break;

        case 'N': //cambiar nota
          Wire.beginTransmission(adr);
          Wire.write(com); //enviar comando
          Wire.write(x, 2); //enviar numero de nota
          Wire.endTransmission();
          break;

        case 'R': //repetir desde marca
          pos = marca;
          break;

        case 'S': //habilitar o deshabilitar sensor
          Wire.beginTransmission(adr);
          Wire.write(com); //enviar comando
          Wire.write(x, 2); //enviar booleano
          Wire.endTransmission();
          break;

        case 'T': //tocar nota
          Wire.beginTransmission(adr);
          Wire.write(com); //enviar comando
          Wire.write(x, 2); //enviar tiempo
          Wire.endTransmission();
          break;
      }//fin switch()

      tCom += del; //agregar el retardo
      pos ++; //incrementar posición
      if (pos >= rola -> len) { //si es el final de la secuancia
        menuState = CANCIONES; //regresar al menú canciones
        delay(1000); //esperar un poco para poder leer la LCD
        return; //salir de esta función
      }

    }//fin while (millis() >= tCom)
  }//fin while (true)
}//fin menuTocando()
