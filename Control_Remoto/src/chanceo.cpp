// librerías cogias
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <WiFi.h> 
#include "esp32-hal-cpu.h" // Librería para bajar frecuencia CPU
#include <Freno.h> //agrega el modulo de seguridad
#include <Adafruit_NeoPixel.h>
#include <esp_wifi.h> //control de bajo nivel en el wifi
#include <Esp_Chisme.h> //libreria de comunicacion sexual
// Configuracion de pines
#define RX_Pin 17   // RX_PIN (Eje X)
#define RY_Pin (12+1)   // RY_PIN (Eje Y)
#define LED    48  // Pin del LED RGB 
#define NUMPIXELS 1
#define Record  5
#define Manual 4
#define Replay 6
#define Rx_Pic 16 //Mosca aqui hay que poner un divisor de tension entre pic y esp para 3.3V
#define Tx_Pic 18 
#define DuaLipa 7  // Pin para activar el modo de grabación (Dua Lipa Mode)
// Variables de control
int potenciaAvance = 127; 
int potenciaGiro   = 127; 
int valorX = 0;
int valorY = 0;
char Pantalla[51] = "Hola Culito!"; // Variable para mostrar en la interfaz Bluetooth


// 🔥 INTERFAZ COGIA POR BLE (Bluetooth Low Energy) 🔥
#define REMOTEXY_MODE__ESP32CORE_BLE
#include <RemoteXY.h>

// Nombre que aparecerá en tu celular al buscar el Bluetooth
#define REMOTEXY_BLUETOOTH_NAME "Culito" 


// Interfaz de usuario cogia
#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 236 bytes V19 
  { 255,9,0,51,0,229,0,19,0,0,0,67,117,108,105,116,111,0,24,2,
  106,200,200,84,2,1,0,2,0,5,19,64,58,58,29,17,41,41,0,2,
  26,31,131,8,6,22,17,133,30,39,13,2,17,2,31,82,101,99,111,114,
  100,101,114,32,109,111,100,101,0,6,9,0,1,23,48,33,33,25,6,16,
  16,0,2,31,85,112,0,1,1,83,28,28,6,25,16,16,0,2,31,76,
  101,102,116,0,1,24,116,31,31,24,44,17,17,0,2,31,68,111,119,110,
  0,1,52,81,30,30,45,25,17,17,0,2,31,82,105,103,104,116,0,131,
  74,149,21,33,79,65,40,11,2,17,21,31,77,97,110,117,97,108,32,77,
  111,100,101,0,9,10,24,76,57,57,147,6,19,19,48,65,204,31,79,78,
  0,31,79,70,70,0,3,53,230,39,105,145,31,49,18,131,2,26,67,40,
  7,21,24,51,3,93,11,100,2,26,51,10,62,237,57,57,171,6,17,17,
  48,6,14,31,83,116,111,112,0,31,83,116,97,114,116,0 };
  
struct {
  int8_t joystick_01_x; 
  int8_t joystick_01_y; 
  uint8_t up; 
  uint8_t left; 
  uint8_t down; 
  uint8_t right; 
  uint8_t ActivoMenudeo; 
  uint8_t Modos; 
  uint8_t grabacion; // switch (Start/Stop)
  char Estado_Envio[51]; //  La cajita de texto cogia
  uint8_t connect_flag;  
} RemoteXY;   
#pragma pack(pop)

//Variable del mensaje en pantalla
unsigned long tiempo_violao = 0;

//CONFIGURACION NEOPIXEL
Adafruit_NeoPixel pixels(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);

//Creacion de objetos
uint8_t mac[] = {0x00, 0x4B, 0x12, 0x33, 0x98, 0xFC}; //MAC PRUEBAS
// uint8_t mac[] = {0x90, 0x70, 0x69, 0x18, 0x9B, 0x40}; //MAC del carro esclavo
FrenoSexual Freno;
Esp_Chisme Chisme(mac); 

void setup() {
  setCpuFrequencyMhz(80); 
  
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, Rx_Pic, Tx_Pic); // Configura el puerto serial para el pic
  
  pixels.begin(); 
  pixels.setBrightness(20); 
  pixels.clear();
  pixels.show();
  
  pinMode(Record,OUTPUT); 
  pinMode(Manual,OUTPUT); 
  pinMode(Replay,OUTPUT); 
  pinMode(DuaLipa, OUTPUT);
  
  // --- CONFIGURACIÓN PWM  ---
  ledcSetup(0, 20000, 8); // Canal Giro
  ledcSetup(1, 20000, 8); // Canal Avance
  ledcAttachPin(RX_Pin, 0);
  ledcAttachPin(RY_Pin, 1);

  // 🔥 Inicio de la app cogia por Bluetooth 🔥
RemoteXY_Init();   
delay(200); 
}
void loop() {
  RemoteXY_Handler(); 
  uint8_t culo = (RemoteXY.connect_flag == 1) ? 1 : 0; 
  
  // Freno cogio
  Freno.chequeosexual(culo, RemoteXY.joystick_01_x, RemoteXY.joystick_01_y, pixels);
  pixels.show();

  // --- Lógica de control segun modo de operación ---
  digitalWrite(Record, LOW);
  digitalWrite(Manual, LOW);    
  digitalWrite(Replay, LOW);
  digitalWrite(DuaLipa, LOW);
  potenciaAvance = 127;
  potenciaGiro = 127;

  // 🕹️ MODO 1: CONTROL POR JOYSTICK ANALÓGICO
  if (RemoteXY.Modos == 0) {
    digitalWrite(Manual, HIGH);
    if (culo == 1) pixels.setPixelColor(0, pixels.Color(255, 69, 0)); 
    valorX = RemoteXY.joystick_01_x; 
    valorY = RemoteXY.joystick_01_y; 
    potenciaAvance = map(valorY, -100, 100, 0, 255); 
    potenciaGiro   = map(valorX, -100, 100, 0, 255); 
  } 
  // ➕ MODO 2: CONTROL DIGITAL POR D-PAD
  else if (RemoteXY.Modos == 1) {
    digitalWrite(Record, HIGH);
    if (RemoteXY.grabacion == 1){ 
      digitalWrite(DuaLipa, HIGH);
    strcpy(Pantalla, "GRABANDO MODO DUA LIPA 🎤");
    }

    else if (RemoteXY.grabacion == 0){
       digitalWrite(DuaLipa, LOW);
       strcpy(Pantalla, "MODO D-PAD");
    }
    if (culo == 1) pixels.setPixelColor(0, pixels.Color(0, 0, 50));
    if (RemoteXY.up == 1) { potenciaAvance = 255; valorY = 100; } 
    else if (RemoteXY.down == 1) { potenciaAvance = 0; valorY = -100; }
    if (RemoteXY.right == 1) { potenciaGiro = 255; valorX = 100; } 
    else if (RemoteXY.left == 1) { potenciaGiro = 0; valorX = -100; }
  }
  // 🔄 MODO 3: REPLAY
  else if (RemoteXY.Modos == 2) {
    digitalWrite(Replay, HIGH);
    if (culo == 1) pixels.setPixelColor(0, pixels.Color(250, 0, 0)); 
  } 

  // Envio de ordenes físicas al PIC1
  ledcWrite(1, constrain(potenciaAvance, 0, 255));
  ledcWrite(0, constrain(potenciaGiro, 0, 255));

  // ⚡ Modulo de Comunicación Sexual ESP-NOW ⚡
  static unsigned long ultimo_menudeo = 0;
  if (RemoteXY.ActivoMenudeo != ultimo_menudeo){
    if(RemoteXY.ActivoMenudeo == 1){
      Chisme.inicio();
      Serial.println("⚡ Antena ESP-NOW ENCENDIDA");
      strcpy(Pantalla, "Antena ESP-NOW ENCENDIDA");
    } else {
      Chisme.apagar();
      Serial.println("💤 Antena ESP-NOW APAGADA");
      strcpy(Pantalla, "Antena ESP-NOW APAGADA");
    }
    ultimo_menudeo = RemoteXY.ActivoMenudeo;
  }

  // 📡 EL PUENTE: ESCUCHAR AL PIC Y MANDAR AL CARRO COGIO
  if (Serial2.available() && RemoteXY.ActivoMenudeo == 1) {
    String tramaPIC = Serial2.readStringUntil('\n'); 
    tramaPIC.trim(); 
    if (tramaPIC.startsWith("[") && tramaPIC.endsWith("]")) {
      char estado = tramaPIC.charAt(1);
      int tiempo = tramaPIC.substring(tramaPIC.indexOf(',') + 1, tramaPIC.length() - 1).toInt();
      Chisme.EnviarTrama(estado, tiempo); 
    }
  }
//Enlace con el codigo y la pantalla del celular
strcpy(RemoteXY.Estado_Envio, Pantalla);
  //--- Tiempo de Impresion de valores ---
  if (millis() - tiempo_violao >= 500) {
    tiempo_violao = millis();
    float Vx= (potenciaGiro*3.3)/255.0;
    float Vy= (potenciaAvance*3.3)/255.0;
    Serial.printf("X: %d\t [ %.2fV ]\t | \tY: %d\t [ %.2fV ]\n", valorX, Vx, valorY, Vy);
  }
}