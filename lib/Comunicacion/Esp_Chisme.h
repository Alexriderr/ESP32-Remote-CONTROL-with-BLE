#ifndef ESP_CHISME_H
#define ESP_CHISME_H
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

class Esp_Chisme {
  public:
    Esp_Chisme(uint8_t* mac_destino);

    // Métodos
    void inicio ();
    void EnviarTrama (char estado, int tiempo);
    void apagar();

    private:
    uint8_t _mac_esclavo[6];
    esp_now_peer_info_t _info_partner;

     static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
     static void OjoPelao(const uint8_t *mac, const uint8_t *datosNuevos, int longitud);
};
#endif
