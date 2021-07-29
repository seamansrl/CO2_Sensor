#include "MQ135.h"

/// CALIBRACIÓN DE RESISTENCIA ATMOSFÉRICA: EL MQ135 ES UN SENSOR MUY MUY
/// BÁSICO, SOLO CAMBIA SU RESISTENCIA SEGÚN LA CANTIDAD DE C02 EN EL
/// AMBIENTE, AHORA BIEN DISTINTAS PARTIDAS DE SENSORES O MARCAS
/// PUEDEN HACER QUE ESTA RESISTENCIA VARÍE ES POR TANTO QUE SE DEBE
/// LLEVAR EL EQUIPO AL EXTERIOR Y DEJARLO POR AL MENOS 15 MINUTOS
/// MIENTRAS SE OBSERVA EL VALOR QUE SE IMPRIME EN PANTALLA DESDE
/// EL TERMINAL. UNA VEZ EL VALOR SE MANTENGA ESTABLE LO COPIAREMOS
/// EN REMPLAZO AL QUE ESTA PUESTO A CONTINUACIÓN POR DEFECTO
/// SI SALE TODO OK DEBERIAMOS MEDIR ENTRE 390 Y 490 PPM DE CO2
/// AL AIRE LIBRE.

float RZERO = 157.53;

float MQ135::SetedRZero()
{
  return RZERO;
}

MQ135::MQ135(uint8_t pin) {
  _pin = pin;
}

float MQ135::GetAtmos()
{
  return ATMOCO2;  
}

bool MQ135::SetRZero()
{
    RZERO = getRZero();

    return true;
}

float MQ135::getCorrectionFactor(float t, float h) {
  return CORA * t * t - CORB * t + CORC - (h-33.)*CORD;
}

float MQ135::getResistance() {
  int val = analogRead(_pin);
  return ((1023./(float)val) * 5. - 1.)*RLOAD;
}


float MQ135::getCorrectedResistance(float t, float h) {
  return getResistance()/getCorrectionFactor(t, h);
}


float MQ135::getPPM() {
  return PARA * pow((getResistance()/RZERO), -PARB);
}


float MQ135::getCorrectedPPM(float t, float h) {
  return PARA * pow((getCorrectedResistance(t, h)/RZERO), -PARB);
}


float MQ135::getRZero() {
  return getResistance() * pow((ATMOCO2/PARA), (1./PARB));
}


float MQ135::getCorrectedRZero(float t, float h) {
  return getCorrectedResistance(t, h) * pow((ATMOCO2/PARA), (1./PARB));
}
