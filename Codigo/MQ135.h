#ifndef MQ135_H
#define MQ135_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/// RESISTENCIA DE LA PCB
/// EN LA PARTE DE ATRÁS DEL SENSOR MQ135 DEL LADO CONTRARIO A LOS PINES DE
/// CONEXIÓN HAY UNA RESISTENCIA SMD CON EL NÚMERO 102 ESCRITO EN ELLA, ESTÁ
/// HACE DE DIVISOR DE CORRIENTE JUNTO AL SENSOR Y ENTREGA EL VALOE mV
/// AL PIN0 DE LA ARDUINO. TÍPICAMENTE VIENE CON ESTA RESISTENCIA DE 1K Y SI
/// BIEN FUNCIONA, NO ES LA MEJOR OPCIÓN POR QUE GENERA BASTANTE ERROR
/// POR LO QUE CONVIENE REMPLAZARLA POR UNA DE 10K o 22K y ESCRIBIR EN
/// RLOAD LOS VALORES DE 10.0 o 22.0 SEGÚN CORRESPONDA 

#define RLOAD 10.0

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
  bool SetRZero();
  float SetedRZero();
  float GetAtmos();
  float getCorrectionFactor(float t, float h);
  float getResistance();
  float getCorrectedResistance(float t, float h);
  float getPPM();
  float getCorrectedPPM(float t, float h);
  float getRZero();
  float getCorrectedRZero(float t, float h);
};
#endif
