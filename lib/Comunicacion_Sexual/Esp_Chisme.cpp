#include "Esp_Chisme.h"

extern Adafruit_NeoPixel pixels;
extern char Pantalla[51];

// --- Variables para guardar el último chisme enviado ---
char ultimo_chisme_enviado[15] = ""; 

// --- Callback estático de confirmación ---
void Esp_Chisme::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Estado del envío: ");
  
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("ENTREGADO ✅");
  //  pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Verde si llegó bien
    
    sprintf(Pantalla, "Enviado>%s ✅", ultimo_chisme_enviado); 
  } else {
    Serial.println("PERDIDO ❌");
    // pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Rojo si no encontró al carro
    sprintf(Pantalla, "Fallo>%s ❌", ultimo_chisme_enviado); 
  }
//  pixels.show();
}

// Oreja parada pal chisme (Recibe del ESP2)
void Esp_Chisme::OjoPelao(const uint8_t * mac, const uint8_t *datosNuevos, int longitud) {
  char trama[longitud + 1];
  memcpy(trama, datosNuevos, longitud);
  trama[longitud] = '\0'; // Sellamos la verga

  Serial.println("\n========================================");
  Serial.print("☁️ LLEGÓ CHISME DEL AIRE: ");
  Serial.println(trama);

  // --- TRADUCTOR DE STRING A BYTES CRUDOS PARA EL PIC ---
  String tramaStr = String(trama);
  
  if (tramaStr.startsWith("[") && tramaStr.endsWith("]") && tramaStr.indexOf(',') != -1) {
    char estado = tramaStr.charAt(1);
    uint8_t tiempo = tramaStr.substring(tramaStr.indexOf(',') + 1, tramaStr.length() - 1).toInt();
    
    Serial.printf("➡️ Desempaquetado exitoso -> Estado: %c | Tiempo: %d\n", estado, tiempo);
    
    // 🔥 AQUÍ ESTÁ EL FILTRO MÁGICO 🔥
    if (estado == 'S') {
      // Si el estado es 'S', simplemente imprimimos un aviso y NO hacemos los Serial2.write
      Serial.println("🛑 Los otros cogios mandaron una 'S'. Trama IGNORADA 🚫");
    } else {
      // Si es cualquier otra letra (F, B, L, R), la inyectamos normal al PIC
      Serial2.write(estado); // Mandamos la letra pura
      delay(10); // Pequeña pausa para asegurar que el PIC procese la letra antes del tiempo
      Serial2.write(tiempo); // Mandamos el tiempo Serial2.write(estado); // Mandamos la letra pura
      Serial.println("✅ Orden inyectada al PIC por UART");
    }
    
  } else {
    Serial.println("❌ Error: Trama corrupta, no se enviará al PIC.");
  }
  Serial.println("========================================\n");

  // Actualiza la pantalla global |
  sprintf(Pantalla, "Recibido>%s", trama);
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
    strcpy(Pantalla, "ERROR ESP-NOW ❌");
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
    Serial.println("❌ Error al enlazar con el otro cogio");
    strcpy(Pantalla, "ERROR ENLAZANDO ❌");
  }
}

// --- Empaquetado de trama ---
void Esp_Chisme::EnviarTrama(char estado, int tiempo) {
  //buffer temporal para armar el texto
  char trama_texto[15]; 
  
  //  [Estado,Tiempo]
  sprintf(trama_texto, "[%c,%d]", estado, tiempo);
  
  strcpy(ultimo_chisme_enviado, trama_texto);
 
  
  //  ENVÍO DE LA TRAMA
  esp_err_t resultado = esp_now_send(_mac_esclavo, (uint8_t *) trama_texto, strlen(trama_texto) + 1);
  
  //Pa saber si el chisme fallo
  if (resultado != ESP_OK) {
    Serial.println("⚠️ Error enviando trama por ESP-NOW");
    sprintf(Pantalla, "Fallo>%s ⚠️", trama_texto); 
  }
  sprintf(Pantalla, "Enviando>%s", trama_texto);
}

// --- Apagar el módulo  ---
void Esp_Chisme::apagar() {
  esp_now_deinit();
  WiFi.mode(WIFI_OFF);
  Serial.println("⚡ Antena ESP-NOW APAGADA");
  strcpy(Pantalla, "ANTENA APAGADA 💤");
}