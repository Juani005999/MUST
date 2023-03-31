#include "Arduino.h"

#define MEASURE_TIMEOUT   25000UL                 // 25ms = ~8m à 340m/s
const float SOUND_SPEED = 340.0 / 1000;           // Vitesse du son dans l'air en mm/us

class MUST_HCSR04
{
public:
	MUST_HCSR04();
  Init(int pinEcho, int triggerPin);
  float Distance();

private:
  int _pinEcho = -1;
  int _pinTrigger = -1;
};