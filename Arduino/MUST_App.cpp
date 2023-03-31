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
                    int pinTrigger)
{
  // Initialisation des objets internes avec les paramètres de construction
  _hcsr04.Init(pinEcho, pinTrigger);
  
  // Update des membres internes
  _pinLedCardLeft = pinLedCardLeft;
  _pinLedCardRight = pinLedCardRight;
  _pinLedRed = pinLedRed;
  _pinLedGreen = pinLedGreen;
  _pinPushAction = pinPushAction;

  // Définition des PIN
  pinMode(_pinLedCardLeft, OUTPUT);
  pinMode(_pinLedCardRight, OUTPUT);
  pinMode(_pinLedRed, OUTPUT);
  pinMode(_pinLedGreen, OUTPUT);
  pinMode(_pinPushAction, INPUT_PULLUP);

  // Initialisation des flags de mesure
  _lastSavedActionON = false;
  _lastMesure = -1;
}

/// -------------------------------------
/// Initialisation de l'application
///
void MUST_App::Init()
{
  // LED de la Protoshield à 20 au démarrage
  analogWrite(_pinLedCardLeft, 20);
  analogWrite(_pinLedCardRight, 20);

  // LED RED et GREEN
  analogWrite(_pinLedRed, _ledRedLevelMax);
  analogWrite(_pinLedGreen, _ledGreenLevelMax);

  // Initialisation de l'écran LCD
  InitDisplay();
  
  // Splash Screen App
  DisplayStartApp();

  // Trace
  Serial.println("Initialisation MUST App OK");

  // Initialisation Chrono
  _chronoLedCardLoop = millis();
  _chronoGreenLedLoop = millis();
  _chronoLCDRefresh = millis();
  _chronoLastMesure = millis();
}

/// -----------------------------------
/// Initialisation de l'écran LCD
///
void MUST_App::InitDisplay()
{
  // Initialisation écran
  _lcd.init();
  _lcd.backlight();
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
  _lcd.print(_texteChargement);
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
void MUST_App::CardLedOscillate(bool shortOscillation)
{
  // Traitement si plus dans l'intervalle défini
  if (millis() > _chronoLedCardLoop + _ledCardLevelIntervalMillis)
  {
    // Validation des bornes min et max
    if (_ledCardLevel < _ledCardLevelMin)
      _ledCardLevel = _ledCardLevelMin;
    if (_ledCardLevel > _ledCardLevelMax)
      _ledCardLevel = _ledCardLevelMax;

    // On positionne les LED à leur valeur respectives
    analogWrite(_pinLedCardRight, _ledCardLevel);
    analogWrite(_pinLedCardLeft, _ledCardLevelMax - _ledCardLevel + _ledCardLevelMin);

    // On update les niveaux pour le prochain affichage
    if (_ledCardSensUp)
      _ledCardLevel += shortOscillation ? _ledCardLevelIntervalShort : _ledCardLevelIntervalLong;
    else
      _ledCardLevel -= shortOscillation ? _ledCardLevelIntervalShort : _ledCardLevelIntervalLong;
    
    // On change le sens si on atteint la borne
    if (_ledCardLevel >= _ledCardLevelMax || _ledCardLevel <= _ledCardLevelMin)
      _ledCardSensUp = !_ledCardSensUp;

    // Actualisation du chrono
    _chronoLedCardLoop = millis();
  }
}


/// --------------------------------------------------
/// Mise à jour de l'état de la Green LED
///
void MUST_App::UpdateGreenLedState(bool mesureON)
{
  // Pas de mesure en cours, on éteint la LED
  if (!mesureON)
  {
    digitalWrite(_pinLedGreen, LOW);
  }
  else
  {
    // Traitement si plus dans l'intervalle défini
    if (millis() > _chronoGreenLedLoop + _ledGreenLevelIntervalMillis)
    {
      // Validation des bornes min et max
      if (_ledGreenLevel < _ledGreenLevelMin)
        _ledGreenLevel = _ledGreenLevelMin;
      if (_ledGreenLevel > _ledGreenLevelMax)
        _ledGreenLevel = _ledGreenLevelMax;

      // On positionne les LED à leur valeur respectives
      analogWrite(_pinLedGreen, _ledGreenLevel);
    
      // On update les niveaux pour le prochain affichage
      if (_ledGreenSensUp)
        _ledGreenLevel += _ledGreenLevelInterval;
      else
        _ledGreenLevel -= _ledGreenLevelInterval;
      
      // On change le sens si on atteint la borne
      if (_ledGreenLevel >= _ledGreenLevelMax || _ledGreenLevel <= _ledGreenLevelMin)
        _ledGreenSensUp = !_ledGreenSensUp;

    // Actualisation du chrono
      _chronoGreenLedLoop = millis();
    }
  }
}

/// ------------------------------
/// Actualisation de l'affichage LCD
///
void MUST_App::UpdateScreen(bool mesureON)
{
  // Pas d'actualisation de l'affichage si on est en dessous de l'intervalle de rafraichissement
  if (millis() > _chronoLCDRefresh + _lcdTauxRefreshMillis)
  {
    // Action mesure en cours : ON
    if (mesureON)
    {
      // On lance la mesure de la distance uniquement si l'ancienne mesure n'est plus valide
      if (millis() > _chronoLastMesure + _validiteMesure || _lastMesure == -1)
      {
        // Ultra-sons Sensor
        _lastMesure = _hcsr04.Distance();
        // Actualisation du chrono si la mesure à fonctionnée
        if (_lastMesure != 0)
          _chronoLastMesure = millis();
      }
    }

    // Actualisation de l'affichage
    DisplayMesure(mesureON);

    // Actualisation du chrono
    _chronoLCDRefresh = millis();
  }  
}

/// -----------------------------------------
/// Actualisation de l'affichage LCD en fonction de mesureON et de _lastMesure
///
void MUST_App::DisplayMesure(bool mesureON)
{
  // Sauvegarde de l'état d'action en cours et clear du LCD si changement
  if (_lastSavedActionON != mesureON)
  {
    _lcd.clear();
    _lastSavedActionON = mesureON;
  }

  // Aucune mesure encore effectuée
  if (_lastMesure == -1)
  {
    // Titre & Texte
    _lcd.setCursor(0, 0);
    _lcd.print("Presse le bouton");
    _lcd.setCursor(0, 1);
    _lcd.print("pour mesurer");
  }
  else
  {
    // Mesure en cours
    if (mesureON)
    {
      // Titre
      _lcd.setCursor(0, 0);
      _lcd.print("Mesure en cours");
    }
    // Dernière mesure effectuée
    else
    {
      // Titre
      _lcd.setCursor(0, 0);
      _lcd.print("Derniere mesure");
    }
      
    // Texte
    _lcd.setCursor(0, 1);
    _lcd.print(FormatedDistance());
  }  
}

/// ------------------------------
/// Formatte pour l'affichage la valeur de _lastMesure
///
String MUST_App::FormatedDistance()
{
  // Si la mesure vaut 0, c'est une erreur remontée par le module => On traite la mesure comme "hors limites"
  // Gestion des valeurs limites constructeur : 2cm -> 4m
  if (_lastMesure < 20 || _lastMesure > 5000)
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

  // On rajoute des espaces jusqu'a atteindre 16 caractères
  while (formatedMesure.length() < 16)
  {
    formatedMesure += " ";
  }

  return formatedMesure;
}