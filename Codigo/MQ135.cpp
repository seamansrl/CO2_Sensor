#include "MQ135.h"


MQ135::MQ135(uint8_t pin) {
  _pin = pin;
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
