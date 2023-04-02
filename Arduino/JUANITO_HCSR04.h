/// ---------------------------------------------------------------------
///
/// Objet           : JUANITO_HCSR04
/// Auteur          : Juanito del Pepito
/// Date            : 02/04/2023
/// Description     : Objet permettant la gestion du module Ultra-Sons de type HCSR04
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"

// Déclaration des constantes propres à l'objet
#define HCSR04_MEASURE_TIMEOUT                   25000UL      // 25ms = ~8m à 340m/s
#define HCSR04_MESURE_BORNE_MIN                  20           // Borne minimale acceptée pour les mesure (limites constructeur)
#define HCSR04_MESURE_BORNE_MAX                  4000         // Borne maximale acceptée pour les mesure (limites constructeur)
const float SOUND_SPEED = 340.0 / 1000;                       // Vitesse du son dans l'air en mm/us

/// ---------------------
/// JUANITO_HCSR04 : Objet permettant la gestion du module Ultra-Sons de type HCSR04
///
class JUANITO_HCSR04
{
public:
	JUANITO_HCSR04();
  Init(int pinEcho, int triggerPin);
  float Distance();

private:
  int _pinEcho = -1;
  int _pinTrigger = -1;
};