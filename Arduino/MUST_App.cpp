#include "MUST_App.h"

/// -----------------------
/// Constructeur
///
MUST_App::MUST_App (int pinLedCardLeft,
                    int pinLedCardRight,
                    int pinLedRed,
                    int pinLedGreen,
                    int pinPushAction,
                    int pinEcho,
                    int pinTrigger,
                    int pinDht)
{
  // Update des membres internes
  _pinLedCardLeft = pinLedCardLeft;
  _pinLedCardRight = pinLedCardRight;
  _pinLedRed = pinLedRed;
  _pinLedGreen = pinLedGreen;
  _pinPushAction = pinPushAction;
  _pinEcho = pinEcho;
  _pinTrigger = pinTrigger;
  _pinDht = pinDht;

  // Définition des PIN
  pinMode(_pinPushAction, INPUT_PULLUP);

  // Initialisation des flags de mesure
  _actionStatus = ACTION_STATUS_MEASURE_OFF;
  _lastMesure = -1;
  _compareMesure = -1;
}

/// -------------------------------------
/// Initialisation de l'application
///
void MUST_App::Init()
{
  // Initialisation des objets internes avec les paramètres de construction
  _hcsr04.Init(_pinEcho, _pinTrigger, HCSR04_MEASURE_INTERVAL);
  _lcd.Init(LCD_ADRESS, LCD_COLS, LCD_ROWS);
  _cardLeftLed.Init(_pinLedCardLeft, true, LED_CARD_MIN_VALUE, LED_CARD_MAX_VALUE);
  _cardRightLed.Init(_pinLedCardRight, true, LED_CARD_MIN_VALUE, LED_CARD_MAX_VALUE);
  _redLed.Init(_pinLedRed, LED_RED_MIN_VALUE, true, LED_RED_MAX_VALUE);
  _greenLed.Init(_pinLedGreen, true, LED_GREEN_MIN_VALUE, LED_GREEN_MAX_VALUE);
  _dht.Init(_pinDht, DHT_SENSOR_TYPE, DHT_SENSOR_INTERVAL);

  // Allumage des Leds (positionnement à maxValue) au démarrage de l'App
  _cardLeftLed.On(true);
  _cardRightLed.On(true);
  _redLed.On(true);
  _greenLed.On(true);
  
  // Splash Screen App
  DisplayStartApp();

  // Trace
  Serial.println("Initialisation MUST App OK");
}

/// -------------------------------------
/// Affichage du message d'intro de l'application
///
void MUST_App::DisplayStartApp()
{
  // Scroll du Titre de l'appli
  for (int i=0; i<=15; i++)
  {
    _lcd.setCursor(15-i, 0);
    _lcd.print(_titreApp);
    delay(_delayTitreScroll);
  }

  // Affichage Auteur
  _lcd.setCursor(5, 0);
  _lcd.print(" b");
  delay(_delayTexte);
  _lcd.print("y");
  delay(_delayTexte);
  _lcd.print(" ");
  delay(_delayTexte);
  _lcd.print("J");
  delay(_delayTexte);
  _lcd.print("u");
  delay(_delayTexte);
  _lcd.print("a");
  delay(_delayTexte);
  _lcd.print("n");
  delay(_delayTexte);
  _lcd.print("i");
  delay(_delayTexte);
  _lcd.print("t");
  delay(_delayTexte);
  _lcd.print("o");
  delay(_delayTexte);
  
  // Affichage chargement en cours
  _lcd.setCursor(0, 1);
  _lcd.Smiley();
  _lcd.print(" " + _texteChargement);
  delay(_delayChargement);
  _lcd.print(".");
  delay(_delayChargement);
  _lcd.print(".");
  delay(_delayChargement);
  _lcd.print(".");
  delay(_delayChargement);
  
  delay(_delayFinal);
  _lcd.clear();
}

/// --------------------------------------------------
/// Prise en charge de l'oscillation des leds de la protoshield
///
void MUST_App::CardLedOscillate(int actionStatus)
{
  // // Si on est en mode Mesure ON ou Compare, et qu'on est dans les limites pour la dernière mesure, on applique un coef au proprata de la distance
  // //  afin de faire varier les oscillations avec la distance mesurée
  // if ((actionStatus == ACTION_STATUS_MESURE_ON || actionStatus == ACTION_STATUS_MESURE_COMPARE)
  //     && _lastMesure > HCSR04_MESURE_BORNE_MIN && _lastMesure < HCSR04_MESURE_BORNE_MAX)
  // {
  //   _cardLeftLed.Oscillate(((double)_lastMesure / HCSR04_MESURE_BORNE_MAX));
  //   _cardRightLed.Oscillate(((double)_lastMesure / HCSR04_MESURE_BORNE_MAX), true);
  // }
  // else
  // {
  //   _cardLeftLed.Oscillate();
  //   _cardRightLed.Oscillate(-1, true);
  // }
  
  // ---------------------
  // EDIT : Comme on souhaite une synchronisation des oscillations entre LED droite et gauche,
  //  on gère l'oscillation ici dans l'objet applicatif et non via l'objet LED indépendamment
  // ---------------------

  // Intervalle de référence pour l'oscillation
  float intervalle = ((float)(LED_DEFAULT_OSCILLATE_CHRONO_INTERVAL / 2)) * LED_DEFAULT_OSCILLATE_COEF_STD;

  // Si on est en mode Mesure ON ou Compare, et qu'on est dans les limites pour la dernière mesure, on applique un coef au proprata de la distance
  //  afin de faire varier les oscillations avec la distance mesurée
  if ((actionStatus == ACTION_STATUS_MEASURE_ON || actionStatus == ACTION_STATUS_MEASURE_COMPARE)
      && _lastMesure > HCSR04_MEASURE_BORNE_MIN && _lastMesure < HCSR04_MEASURE_BORNE_MAX)
    intervalle = ((float)(LED_DEFAULT_OSCILLATE_CHRONO_INTERVAL / 2))
                 + ((LED_DEFAULT_OSCILLATE_CHRONO_INTERVAL * ((float)_lastMesure / HCSR04_MEASURE_BORNE_MAX)) * 50);

  // Traitement si plus dans l'intervalle défini
  if (millis() > _chronoLedCardLoop + intervalle)
  {
    // Validation des bornes min et max
    if (_ledCardLevel < LED_CARD_MIN_VALUE)
      _ledCardLevel = LED_CARD_MIN_VALUE;
    if (_ledCardLevel > LED_CARD_MAX_VALUE)
      _ledCardLevel = LED_CARD_MAX_VALUE;

    // On positionne les LED à leur valeur respectives
    _cardLeftLed.SetValue (_ledCardLevel);
    _cardRightLed.SetValue (LED_CARD_MAX_VALUE - _ledCardLevel + LED_CARD_MIN_VALUE);

    // On update les niveaux pour le prochain affichage
    _ledCardLevel = _ledCardSensUp  ? _ledCardLevel + LED_DEFAULT_OSCILLATE_LEVEL_INTERVAL
                                    : _ledCardLevel - LED_DEFAULT_OSCILLATE_LEVEL_INTERVAL;
    
    // On change le sens si on a atteint les bornes
    if (_ledCardLevel >= LED_CARD_MAX_VALUE || _ledCardLevel <= LED_CARD_MIN_VALUE)
      _ledCardSensUp = !_ledCardSensUp;

    // Actualisation du chrono
    _chronoLedCardLoop = millis();
  }
}

/// --------------------------------------------------
/// Mise à jour de l'état de la Green LED
///
void MUST_App::UpdateGreenLedState(int actionStatus)
{
  // Pas de mesure en cours, on éteint la LED
  if (actionStatus == ACTION_STATUS_MEASURE_OFF)
  {
    _greenLed.Off();
  }
  else
  {
    if (actionStatus == ACTION_STATUS_MEASURE_ON)
      _greenLed.Blink();
    else
      _greenLed.DoubleBlink(85);
  }
}

/// ------------------------------
/// Mise à jour de la valeur de comparaison
///
void MUST_App::UpdateCompare()
{
  _compareMesure = _lastMesure;
}

/// ------------------------------
/// Reset de la valeur de comparaison
///
void MUST_App::ResetCompare()
{
  _compareMesure = -1;
}

/// ------------------------------
/// Actualisation de l'affichage LCD
///
void MUST_App::UpdateScreen(int actionStatus)
{
  // Pas d'actualisation de l'affichage si on est en dessous de l'intervalle de rafraichissement
  if (millis() > _chronoLCDRefresh + LCD_REFRESH_SCREEN)
  {
    // Action mesure en cours : ON
    if (actionStatus == ACTION_STATUS_MEASURE_ON || actionStatus == ACTION_STATUS_MEASURE_COMPARE)
    {
      // Lecture des mesures d'environnement
      float humidite = _dht.Humidite();
      float temperature = _dht.Temperature();

      // Ultra-sons Sensor
      // _lastMesure = _hcsr04.Distance();
      _lastMesure = _hcsr04.Distance(temperature, humidite);
    }

    // Actualisation de l'affichage
    DisplayMesure(actionStatus);

    // Actualisation du chrono
    _chronoLCDRefresh = millis();
  }  
}

/// -----------------------------------------
/// Actualisation de l'affichage LCD en fonction de mesureON et de _lastMesure
///
void MUST_App::DisplayMesure(int actionStatus)
{
  // Modification de l'action en cours => on clear l'écran et on update le nouveau status
  if (actionStatus >= 1 && actionStatus <= 3 && _actionStatus != actionStatus)
  {
    _actionStatus = actionStatus;
    _lcd.clear();
  }

  // Aucune mesure encore effectuée
  if (_lastMesure == -1)
  {
    // Titre & Texte
    _lcd.setCursor(0, 0);
    _lcd.print("Presse le bouton");
    _lcd.setCursor(0, 1);
    _lcd.print("pour mesurer ");
    _lcd.Wink();
  }
  else
  {
    // Mesure en cours
    // Titre
    _lcd.setCursor(0, 0);
    switch (_actionStatus)
    {
      case ACTION_STATUS_MEASURE_OFF:
        if (_compareMesure != -1)
          _lcd.print("Derniere compar.");
        else
          _lcd.print("Derniere mesure");
        break;
      case ACTION_STATUS_MEASURE_ON:
        _lcd.print("Mesure en cours");
        break;
      case ACTION_STATUS_MEASURE_COMPARE:
        _lcd.print("Compare:");
        _lcd.PrintRightJustify(FormatedComparedDistance(), 0);
        break;
      default:
        break;
    }
      
    // Texte
    _lcd.setCursor(0, 1);
    switch (_actionStatus)
    {
      case ACTION_STATUS_MEASURE_OFF:
        if (_compareMesure != -1)
        {
          _lcd.print(FormatedFullComparedDistance());
          PrintCompareGlyphe();
        }
        else
          _lcd.print(FormatedDistance());
        break;
      case ACTION_STATUS_MEASURE_ON:
        _lcd.print(FormatedDistance());
        break;
      case ACTION_STATUS_MEASURE_COMPARE:
        _lcd.print(FormatedDistance());
        PrintCompareGlyphe();
        break;
      default:
        break;
    }
  }  
}

/// ------------------------------
/// Formatte pour l'affichage la valeur de _lastMesure
///
String MUST_App::FormatedDistance()
{
  // Si la mesure vaut 0, c'est une erreur remontée par le module => On traite la mesure comme "hors limites"
  // Gestion des valeurs limites constructeur : 2cm -> 4m
  if (_lastMesure < HCSR04_MEASURE_BORNE_MIN || _lastMesure > HCSR04_MEASURE_BORNE_MAX)
  {
    return "Hors limites";
  }

  // On dissocie les mètres des centimètres
  String formatedMesure = "";
  int metres = (_lastMesure + BOX_WIDTH_MM) / 1000;
  long centimetres = (_lastMesure + BOX_WIDTH_MM) - (metres * 1000);
  if (metres > 0)
  {
    formatedMesure = String(metres) + " m ";
  }
  formatedMesure += String(((double)centimetres / 10), 1) + " cm";

  // On rajoute des espaces jusqu'a atteindre 15 caractères
  while (formatedMesure.length() < 15)
  {
    formatedMesure += " ";
  }

  return formatedMesure;
}

/// ------------------------------
/// Formatte pour l'affichage la valeur de _compareMesure
///
String MUST_App::FormatedComparedDistance()
{
  // Hors limies, on renvoi une chaine vide
  if (_compareMesure < HCSR04_MEASURE_BORNE_MIN || _compareMesure > HCSR04_MEASURE_BORNE_MAX)
  {
    return "";
  }

  // On dissocie les mètres des centimètres
  String formatedMesure = "";
  int metres = (_compareMesure + BOX_WIDTH_MM) / 1000;
  long centimetres = (_compareMesure + BOX_WIDTH_MM) - (metres * 1000);
  if (metres > 0)
  {
    formatedMesure = String(metres) + "m";
  }
  formatedMesure += String(((double)centimetres / 10), 1) + "cm";


  return formatedMesure;
}

/// ------------------------------
/// Formatte pour l'affichage la valeur de _compareMesure / _lastMesure
///
String MUST_App::FormatedFullComparedDistance()
{
  String formatedMesure = "";

  // Si la mesure vaut 0, c'est une erreur remontée par le module => On traite la mesure comme "hors limites"
  // Gestion des valeurs limites constructeur : 2cm -> 4m
  if (_lastMesure < HCSR04_MEASURE_BORNE_MIN || _lastMesure > HCSR04_MEASURE_BORNE_MAX)
  {
    return "Hors limites";
  }

  // On vérifie la valeur de comapraison
  if (_compareMesure >= HCSR04_MEASURE_BORNE_MIN && _compareMesure <= HCSR04_MEASURE_BORNE_MAX)
  {
    formatedMesure = FormatedComparedDistance();
    formatedMesure += "/";
  }
  // On ajoute la dernière lecture
  formatedMesure += FormatedDistance();

  // On ajuste le format
  formatedMesure.replace(" ", "");
  formatedMesure.replace("cm", "");

  return formatedMesure;
}

/// ------------------------------
/// Ecrit le caractère du glyphe en fonction de _lastMesure et _compareMesure
///
void MUST_App::PrintCompareGlyphe()
{
  _lcd.setCursor(15, 1);
  switch(GetCompareGlyphe())
  {
    case 0:
      _lcd.RightArrow();
      break;
    case 1:
      _lcd.UpArrow();
      break;
    case 2:
      _lcd.DownArrow();
      break;
    default:
      break;
  }
}

/// ------------------------------
/// Renvoi le numero de caractère du glyphe en fonction de _lastMesure et _compareMesure
///
int MUST_App::GetCompareGlyphe()
{
  // Sécurité sur la comparaison
  if (_lastMesure == -1 || _compareMesure == -1)
  return -1;

  if (_lastMesure == _compareMesure)
    return 0;
  else if (_lastMesure > _compareMesure)
    return 1;
  else if (_lastMesure < _compareMesure)
    return 2;
}