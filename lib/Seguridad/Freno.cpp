#include "Freno.h" // implementación de las funciones del freno sexual


void FrenoSexual::chequeosexual(uint8_t conectao, int8_t &x, int8_t &y, Adafruit_NeoPixel &leds){
  
  if (conectao == 0){
   // Caso: No hay sistema
   x=0;
   y=0;
   leds.setPixelColor(0,leds.Color(128,0,128)); // Morado No hay sistema
   _frenado = true; // Activa el freno
  } else {
    // Caso: Hay sistema
   // leds.setPixelColor(0,leds.Color(250,69,0)); // Naranja Sexual Si hay sistema
    _frenado = false; // Desactiva el freno
    
 
      }
}
bool FrenoSexual::EstaFrenao() {
    return _frenado;
}
