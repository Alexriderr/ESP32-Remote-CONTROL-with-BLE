// Proyecto: Carro RC - ESP32 Controlador Inalámbrico
// Betancourt Alexi
// Cancar Moisés
// Melendez Juan

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
#define Rx_Pic 15 
#define Tx_Pic 16 //Mosca aqui hay que poner un divisor de tension entre pic y esp para 3.3V
#define DuaLipa 7  // Pin para activar el modo de grabación (Dua Lipa Mode)

// Variables de control
int potenciaAvance = 127; 
int potenciaGiro   = 127; 
int valorX = 0;
int valorY = 0;
char Pantalla[51] = "Bienvemidos a Venezuela"; // Variable para mostrar en la interfaz
static unsigned long marca_tiempo_pic = 0;
unsigned long tiempo_violao = 0;
static unsigned long ultimo_menudeo = 0;
unsigned long tiempo_pantalla=0;


// 🔥 INTERFAZ COGIA POR BLE (Bluetooth Low Energy) 🔥
#define REMOTEXY_MODE__ESP32CORE_BLE

// Nombre que aparecerá en tu celular al buscar el Bluetooth
#define REMOTEXY_BLUETOOTH_NAME "Team Hard" 

#include <RemoteXY.h>

// Interfaz de usuario cogia 
#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 239 bytes V19 
  { 255,9,0,51,0,232,0,19,0,0,0,72,97,114,100,32,116,101,97,109,
  0,24,2,106,200,200,84,2,1,0,2,0,5,19,64,58,58,29,17,41,
  41,32,2,26,31,131,8,6,22,17,133,30,39,13,2,17,2,31,82,101,
  99,111,114,100,101,114,32,109,111,100,101,0,6,9,0,1,23,48,33,33,
  25,6,16,16,0,2,31,85,112,0,1,1,83,28,28,6,25,16,16,0,
  2,31,76,101,102,116,0,1,24,116,31,31,24,44,17,17,0,2,31,68,
  111,119,110,0,1,52,81,30,30,45,25,17,17,0,2,31,82,105,103,104,
  116,0,131,74,149,21,33,79,65,40,11,2,17,21,31,77,97,110,117,97,
  108,32,77,111,100,101,0,9,10,24,76,57,57,147,6,19,19,48,65,204,
  31,79,78,0,31,79,70,70,0,3,53,230,39,105,145,31,49,18,131,2,
  26,67,40,7,21,24,45,3,99,11,100,2,26,51,10,62,237,57,57,171,
  6,17,17,48,6,14,31,83,116,111,112,0,31,83,116,97,114,116,0 };
  
struct {
  // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  uint8_t up; // =1 if button pressed, else =0, from 0 to 1
  uint8_t left; // =1 if button pressed, else =0, from 0 to 1
  uint8_t down; // =1 if button pressed, else =0, from 0 to 1
  uint8_t right; // =1 if button pressed, else =0, from 0 to 1
  uint8_t ActivoMenudeo; // =1 if state is ON, else =0, from 0 to 1
  uint8_t Modos; // from 0 to 3
  uint8_t grabacion; // =1 if state is ON, else =0, from 0 to 1

  // output variables
  char Estado_Envio[51]; // string UTF8 end zero

  // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0
} RemoteXY;   
#pragma pack(pop)


//CONFIGURACION NEOPIXEL
Adafruit_NeoPixel pixels(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);

//Creacion de objetos
 // uint8_t mac[] = {0x00, 0x4B, 0x12, 0x33, 0x98, 0xFC}; //MAC PRUEBAS
 uint8_t mac[] = {0x90, 0x70, 0x69, 0x18, 0x9B, 0x40}; //MAC del  Pajuo de Jesus
FrenoSexual Freno;
Esp_Chisme Chisme(mac); 

void setup() {
setCpuFrequencyMhz(80); 
  
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, Rx_Pic, Tx_Pic); // Configura el puerto serial para el pic
  Serial2.setTimeout(100); 
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

  // 🔥 Inicio de la app cogia  🔥
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
  valorX = 0;
  valorY = 0;

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
    }
    if (culo == 1) pixels.setPixelColor(0, pixels.Color(0, 0, 50));
    if (RemoteXY.up == 1) { potenciaAvance = 255; valorY = 100; } 
    else if (RemoteXY.down == 1) { potenciaAvance = 0; valorY = -100; }
    if (RemoteXY.right == 1) { potenciaGiro = 255; valorX = 100; } 
    else if (RemoteXY.left == 1) { potenciaGiro = 0; valorX = -100; }
    if (millis() - tiempo_pantalla >= 2000) {
      tiempo_pantalla= millis();
      sprintf(Pantalla, "D-PAD || X: %d | Y: %d", valorX, valorY);
    }
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

  // =============================================================
  //             Bloque para pararle bola al PIC
  // =============================================================
  
  static char estado_guardado = '\0'; 
  

  if (Serial2.available() > 0) {
    
    // Obtencion del Estado  (Se busca F, B, L, R)
    if (estado_guardado == '\0') {
      char letra = Serial2.read();
      
      if (letra == 'F' || letra == 'B' || letra == 'L' || letra == 'R') {
        estado_guardado = letra; // 
        marca_tiempo_pic = millis(); // Tiempo de espera para el segundo dato
        
        Serial.print("👀 OJO Pelao el PIC Pajuo Mando estado: ");
        Serial.println(estado_guardado);
      }
    } 
     // Obtencion del valor del tiempo que manda el PIC
    else {
      uint8_t tiempo_crudo = Serial2.read(); // Lectura del byte de tiempo
      
      Serial.print("⏱️ tiempo: "); 
      Serial.println(tiempo_crudo, DEC); // Transforma el ASCII cogio a un numero decimal
   
      // Se envia solo si el menudeo está activo
      if (RemoteXY.ActivoMenudeo == 1) {
        Chisme.EnviarTrama(estado_guardado, tiempo_crudo); 
        tiempo_pantalla = millis();
        Serial.printf("🚀 Reenviando al Carro: [%c,%d]\n", estado_guardado, tiempo_crudo);
        sprintf(Pantalla, "Enviado>%c,%d", estado_guardado, tiempo_crudo);
      } else {
        Serial.println("💤 Trama limpiada del buffer (El ESP-NOW está apagado)");
      }

      // Vaciado de buffer
      estado_guardado = '\0'; 
    }
  }

  // SEGURO Por si no manda la cosita, el PIC mandó la letra pero el tiempo nunca llegó
  if (estado_guardado != '\0' && (millis() - marca_tiempo_pic > 250)) {
    Serial.println("⚠️ ERROR: El PIC mandó estado pero se le olvidó el tiempo. Abortando...");
    estado_guardado = '\0'; // Reseteo de la trampita
  }
  // =============================================================

  // Enlace con el codigo y la pantalla del celular
  strcpy(RemoteXY.Estado_Envio, Pantalla);
  
  // --- Tiempo de Impresion de valores ---
  if (millis() - tiempo_violao >= 500) {
    tiempo_violao = millis();
    float Vx= (potenciaGiro*3.3)/255.0;
    float Vy= (potenciaAvance*3.3)/255.0;
    // Serial.printf("X: %d\t [ %.2fV ]\t | \tY: %d\t [ %.2fV ]\n", valorX, Vx, valorY, Vy);
  }
}