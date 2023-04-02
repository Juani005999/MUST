#include "JUANITO_HCSR04.h"

/// ---------------------
/// Constructeur
///
JUANITO_HCSR04::JUANITO_HCSR04 ()
{
}

/// ---------------------
/// Initialisation du composant
///
JUANITO_HCSR04::Init (int pinEcho, int pinTrigger)
{
  _pinEcho = pinEcho;
  _pinTrigger = pinTrigger;
  
  // Définition des PIN
  pinMode(_pinEcho, INPUT);
  pinMode(_pinTrigger, OUTPUT);
}

/// ---------------------
/// Renvoi la distance (mm) mesurée par le HCSR04
///
float JUANITO_HCSR04::Distance()
{
  // Sécurité sur l'initialisation des PIN
  if (_pinEcho == -1 || _pinTrigger == -1)
    return 0;

  // Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER
  digitalWrite(_pinTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(_pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(_pinTrigger, LOW);
  delayMicroseconds(2);

  // Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe)
  long mesure = pulseIn(_pinEcho, HIGH, HCSR04_MEASURE_TIMEOUT);

  // Calcul la distance à partir du temps mesuré
  return (mesure / 2.0 * SOUND_SPEED);
}
