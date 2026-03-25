#include "Esp_Chisme.h"

extern Adafruit_NeoPixel pixels;
extern char Pantalla[51];
// --- Callback estático de confirmación ---
void Esp_Chisme::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Estado del envío: ");
  
  
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("ENTREGADO ✅");
    pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Verde si llegó bien
    strcpy(Pantalla, "ENTREGADO ✅");
  } else {
    Serial.println("PERDIDO ❌");
    pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Rojo si no encontró al carro
    strcpy(Pantalla, "SE PERDIÓ ESA MIERDA ❌");
  }
  pixels.show();
}
// Oreja parada pal chisme
void Esp_Chisme::OjoPelao(const uint8_t * mac, const uint8_t *datosNuevos, int longitud) {
  char trama[longitud + 1];
  memcpy(trama, datosNuevos, longitud);
  trama[longitud] = '\0'; // Sellamos la verga

  // Escupe la trama directa por el cable al maldito PIC
  
  Serial2.print(trama); 
  Serial2.print('\n');
  Serial.printf("📡 Chisme atrapado en el aire y escupido al Serial2: %s\n", trama);
  sprintf(Pantalla, "Carro>%s>PIC", trama);
}

// --- Constructor ---
Esp_Chisme::Esp_Chisme(uint8_t* mac_destino) {
  memcpy(_mac_esclavo, mac_destino, 6);
}

// --- Inicialización ---
void Esp_Chisme::inicio() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    
    Serial.println("❌ Error iniciando ESP-NOW");
    strcpy(Pantalla, "ERROR INICIANDO ESP-NOW ❌");
    return;
  }
  esp_now_register_send_cb(Esp_Chisme::onDataSent);
  esp_now_register_recv_cb(Esp_Chisme::OjoPelao);

  memset(&_info_partner, 0, sizeof(_info_partner));
  memcpy(_info_partner.peer_addr, _mac_esclavo, 6);
  
  _info_partner.ifidx = WIFI_IF_STA; 
  _info_partner.channel = 0; 
  _info_partner.encrypt = false;

  if (esp_now_add_peer(&_info_partner) != ESP_OK){
    Serial.println("❌ Error al enlazar con el Carro Esclavo");
    strcpy(Pantalla, "ERROR ENLAZANDO CON CARRO ❌");
  }
}


// --- Empaquetado de trama ---
void Esp_Chisme::EnviarTrama(char estado, int tiempo) {
  //buffer temporal para armar el texto
  char trama_texto[15]; 
  
  //  [Estado,Tiempo]
  sprintf(trama_texto, "[%c,%d]", estado, tiempo);
  
  //  ENVÍO DE LA TRAMA
  esp_err_t resultado = esp_now_send(_mac_esclavo, (uint8_t *) trama_texto, strlen(trama_texto) + 1);
  sprintf(Pantalla, "Enviando: %s", trama_texto);
  if (resultado != ESP_OK) {
    Serial.println("⚠️ Error interno enviando trama ESP-NOW");
  }
}
// --- Apagar el módulo  ---
void Esp_Chisme::apagar() {
  esp_now_deinit();
  WiFi.mode(WIFI_OFF);
  Serial.println("⚡ Antena ESP-NOW APAGADA");
  strcpy(Pantalla, "Antena ESP-NOW APAGADA");
}