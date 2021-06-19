#ifndef MQ135_H
#define MQ135_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/// RESISTENCIA DE LA PCB
#define RLOAD 1.0
/// CALIBRACION DE RESISTENCIA ADMOSFERICA
#define RZERO 76.63
/// PARAMETRO PARA CALCULAR PPM DE CO2
#define PARA 116.6020682
#define PARB 2.769034857

/// PARAMETROS DE TEMPERATURA Y HUMEDAD SEGUN DATASHEET
#define CORA 0.00035
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018

/// NIVEL DE CO2 EN ATMOSFERA PARA CALIBRAR
#define ATMOCO2 397.13

class MQ135 {
 private:
  uint8_t _pin;

 public:
  MQ135(uint8_t pin);
  float getCorrectionFactor(float t, float h);
  float getResistance();
  float getCorrectedResistance(float t, float h);
  float getPPM();
  float getCorrectedPPM(float t, float h);
  float getRZero();
  float getCorrectedRZero(float t, float h);
};
#endif
