/// ---------------------------------------------------------------------
///
/// Objet           : JUANITO_ANALOG_LED
/// Auteur          : Juanito del Pepito
/// Date            : 02/04/2023
/// Description     : Objet permettant la gestion d'une LED
///                   Branchez la LED sur une PIN analogique afin de pouvoir piloter le niveau
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"

// Déclaration des constantes propres à l'objet
#define DEFAULT_SIMPLE_BLINK_INTERVAL       500                       // Intervalle de temps (ms) d'un clignotement simple
#define DEFAULT_DOUBLE_BLINK_INTERVAL       100                       // Intervalle de temps (ms) d'un clignotement double
#define DEFAULT_ON_INTERVAL                 100                       // Intervalle de temps (ms) sans "renvoi" de la commande ON
#define DEFAULT_OFF_INTERVAL                100                       // Intervalle de temps (ms) sans "renvoi" de la commande OFF
#define DEFAULT_OSCILLATE_CHRONO_INTERVAL   5                         // Intervalle de temps (ms) pour l'oscillation minimum
#define DEFAULT_OSCILLATE_COEF_STD          20                        // Coef standard d'adaptation de l'oscillation
#define DEFAULT_OSCILLATE_LEVEL_INTERVAL    5                         // Intervalle de niveau entre chaque oscillation

/// ---------------------
/// JUANITO_HCSR04 : Objet permettant la gestion d'une LED
///
class JUANITO_ANALOG_LED
{
public:
	JUANITO_ANALOG_LED();
  Init(int pin, int minValue, int maxValue);
  On(bool setToMaxValue = false, int interval = DEFAULT_ON_INTERVAL);
  Off(bool setToMinValue = false, int interval = DEFAULT_OFF_INTERVAL);
  Blink(int interval = DEFAULT_SIMPLE_BLINK_INTERVAL);
  DoubleBlink(int interval = DEFAULT_DOUBLE_BLINK_INTERVAL);
  Oscillate(double coefInterval = -1, bool reverseOscillation = false);
  SetValue (int value);

private:
  int _pin;
  int _minValue;
  int _maxValue;

  // Champs internes
  bool _ledON = false;
  bool _ledOscillateSensUp = true;
  int _ledDoubleBlinkStage = 1;
  int _ledOscillateCurrentLevel;

  // Chronos
  long _chronoOnLoop;
  long _chronoOffLoop;
  long _chronoBlinkLoop;
  long _chronoDoubleBlinkLoop;
  long _chronoOscillateLoop;
};