#ifndef FRENO_H
#define FRENO_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class FrenoSexual {
  public:
    // Constructor
    FrenoSexual(){
      _frenado = true; 
    }
    
    // Método principal de chequeo
    void chequeosexual(uint8_t conectao, int8_t &x, int8_t &y, Adafruit_NeoPixel &leds);
    
    // Método para consultar si el freno está activo
    bool EstaFrenao(); 

  private:
    bool _frenado; // Variable interna que guarda el estado de la emergencia
};

#endif