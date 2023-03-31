#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include "MUST_HCSR04.h"

#define BOX_WIDTH_MM    75          // Largeur de la boîte pour le calcul de la distance

class MUST_App
{
public:
	MUST_App(int pinLedCardLeft, int pinLedCardRight, int pinLedRed, int pinLedGreen, int pinPushAction, int pinEcho, int pinTrigger);
  void Init();
  void CardLedOscillate(bool shortOscillation);
  void UpdateGreenLedState(bool mesureON);
  void UpdateScreen(bool mesureON);

private:
  void InitDisplay();
  void DisplayStartApp();
  void DisplayMesure(bool mesureOn);
  String FormatedDistance();

  LiquidCrystal_I2C _lcd = LiquidCrystal_I2C (0x27, 16, 2);                        // Ecran LCD I2C

  int _pinLedCardLeft;
  int _pinLedCardRight;
  int _pinLedRed;
  int _pinLedGreen;
  int _pinPushAction;

  // Paramètres du démarrage de l'application   
  const int _delayTitreScroll = 25;
  const int _delayTexte = 100;
  const int _delayChargement = 500;
  const int _delayFinal = 1000;
  const String _titreApp = "MUST            ";
  const String _texteChargement = "chargement ";

  // Paramètres de l'oscillation des Led de la carte Protoshield
  long _chronoLedCardLoop;
  bool _ledCardSensUp = true;
  int _ledCardLevel = 2;
  const int _ledCardLevelMax = 5;
  const int _ledCardLevelMin = 1;
  const int _ledCardLevelIntervalLong = 1;
  const int _ledCardLevelIntervalShort = 5;
  const int _ledCardLevelIntervalMillis = 75;

  // Paramètres des mesures US
  MUST_HCSR04 _hcsr04 = MUST_HCSR04 ();                       // Module Ultra-Son HCSR04
  long _lastSavedActionON;                                    // Flag permettant de valider le changement d'état du statut d'action
  long _lastMesure;                                           // Sauvegarde de la dernière mesure effectuée
  long _chronoLastMesure;                                     // Timestamp de la dernière prise de mesure
  const int _validiteMesure = 350;                            // Durée (ms) de validité d'une mesure afin de limiter les prises de mesures

  // Paramètres de la Red Led
  const int _ledRedLevelMax = 25;                             // Niveau d'intensisté de la RED LED

  // Paramètres de l'oscillation de la Green Led
  long _chronoGreenLedLoop;
  bool _ledGreenSensUp = true;
  int _ledGreenLevel = 2;
  const int _ledGreenLevelMax = 5;
  const int _ledGreenLevelMin = 1;
  const int _ledGreenLevelInterval = 1;
  const int _ledGreenLevelIntervalMillis = 75;

  // Paramètres d'affichage (LCD)
  long _chronoLCDRefresh;
  const int _lcdTauxRefreshMillis = 100;                      // Taux de rafraichissement de l'écran LCD à 100ms
};