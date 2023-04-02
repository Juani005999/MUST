#include "JUANITO_ANALOG_LED.h"

/// ---------------------
/// Constructeur
///
JUANITO_ANALOG_LED::JUANITO_ANALOG_LED ()
{
}

/// ---------------------
/// Initialisation du composant
///
JUANITO_ANALOG_LED::Init (int pin, int minValue, int maxValue)
{
  _pin = pin;
  _minValue = minValue;
  _maxValue = maxValue;

  // Sécurité sur les bornes min et max
  if (_minValue < 0)
    _minValue = 0;
  if (_maxValue > 255)
    _maxValue = 255;

  // Définition des PIN
  pinMode(_pin, OUTPUT);
}

/// ---------------------
/// Positionne la LED à HIGH ou a _maxValue
///
JUANITO_ANALOG_LED::On (bool setToMaxValue, int interval)
{
  // Si auncun intervalle est passé en paramètre, on éxecute direct la commande
  //  Sinon, traitement si plus dans l'intervalle défini
  if (interval == 0 || millis() > _chronoOnLoop + interval)
  {
    if (setToMaxValue)
      analogWrite(_pin, _maxValue);
    else
      digitalWrite(_pin, HIGH);

    // Actualisation des flags internes
    _ledON = true;
    _ledDoubleBlinkStage = 1;

    // Actualisation du chrono
    _chronoOnLoop = millis();
  }
}

/// ---------------------
/// Positionne la LED à LOW ou a _minValue
///
JUANITO_ANALOG_LED::Off (bool setToMinValue, int interval)
{
  // Si auncun intervalle est passé en paramètre, on éxecute direct la commande
  //  Sinon, traitement si plus dans l'intervalle défini
  if (interval == 0 || millis() > _chronoOffLoop + interval)
  {
    if (setToMinValue)
      analogWrite(_pin, _minValue);
    else
      digitalWrite(_pin, LOW);

    // Actualisation des flags internes
    _ledON = false;
    _ledDoubleBlinkStage = 1;

    // Actualisation du chrono
    _chronoOffLoop = millis();
  }
}

/// ---------------------
/// Positionne la LED à la valeur en paramètre
///
JUANITO_ANALOG_LED::SetValue (int value)
{
  // On valide les bornes min et max
  int newValue = value;
  // Validation des bornes min et max
  if (newValue < _minValue)
    newValue = _minValue;
  if (newValue > _maxValue)
    newValue = _maxValue;

  // On envoi la commande
  analogWrite(_pin, newValue);
}

/// ----------------------------------
/// Clignotement simple de la LED
///
JUANITO_ANALOG_LED::Blink(int interval)
{
  // Traitement si plus dans l'intervalle défini
  if (millis() > _chronoBlinkLoop + interval)
  {
    // On positionne la LED
    if (_ledON)
      analogWrite(_pin, _maxValue);
    else
      analogWrite(_pin, _minValue);

    // On change le sens pour le prochain passage
    _ledON = !_ledON;

    // Actualisation du chrono
    _chronoBlinkLoop = millis();
  }
}

/// ----------------------------------
/// Clignotement simple de la LED
///
JUANITO_ANALOG_LED::DoubleBlink(int interval)
{
  // Traitement si plus dans l'intervalle défini
  if (millis() > _chronoDoubleBlinkLoop + interval)
  {
    // On positionne la LED
    if (_ledDoubleBlinkStage  == 1 || _ledDoubleBlinkStage == 3)
    {
      _ledON = true;
      analogWrite(_pin, _maxValue);
    }    
    else
    {
      _ledON = false;
      analogWrite(_pin, _minValue);
    }

    // Actualisation du Stage
    if (_ledDoubleBlinkStage >= 8)
      _ledDoubleBlinkStage = 1;
    else
      _ledDoubleBlinkStage++;

    // Actualisation du chrono
    _chronoDoubleBlinkLoop = millis();
  }
}

/// ----------------------------------
/// Oscillation de la LED
///
JUANITO_ANALOG_LED::Oscillate(double coefInterval, bool reverseOscillation)
{
  int currentInterval = DEFAULT_OSCILLATE_CHRONO_INTERVAL * DEFAULT_OSCILLATE_COEF_STD;
  // Si présence d'un coef en paramètre, on applique le nouveau coef d'intervalle
  if (coefInterval != -1)
    currentInterval = DEFAULT_OSCILLATE_CHRONO_INTERVAL + (DEFAULT_OSCILLATE_CHRONO_INTERVAL * coefInterval * 50);

  // Traitement si plus dans l'intervalle défini
  if (millis() > _chronoOscillateLoop + currentInterval)
  {
    // Validation des bornes min et max
    if (_ledOscillateCurrentLevel < _minValue)
      _ledOscillateCurrentLevel = _minValue;
    if (_ledOscillateCurrentLevel > _maxValue)
      _ledOscillateCurrentLevel = _maxValue;

    // On positionne la valeur de la LED
    analogWrite(_pin, _ledOscillateCurrentLevel);
    //analogWrite(_pinLedCardLeft, _ledCardLevelMax - _ledCardLevel + _ledCardLevelMin);

    // On update le niveau courant pour le prochain affichage
    if (reverseOscillation)
    {      
      _ledOscillateCurrentLevel = _ledOscillateSensUp ? _ledOscillateCurrentLevel - DEFAULT_OSCILLATE_LEVEL_INTERVAL
                                                        : _ledOscillateCurrentLevel + DEFAULT_OSCILLATE_LEVEL_INTERVAL;
    }
    else
    {      
      _ledOscillateCurrentLevel = _ledOscillateSensUp ? _ledOscillateCurrentLevel + DEFAULT_OSCILLATE_LEVEL_INTERVAL
                                                        : _ledOscillateCurrentLevel - DEFAULT_OSCILLATE_LEVEL_INTERVAL;
    }
    
    // On change le sens si on a atteint les bornes
    if (_ledOscillateCurrentLevel >= _maxValue || _ledOscillateCurrentLevel <= _minValue)
      _ledOscillateSensUp = !_ledOscillateSensUp;

    // Actualisation du chrono
    _chronoOscillateLoop = millis();
  }
}
