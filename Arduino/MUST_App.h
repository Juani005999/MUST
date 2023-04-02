/// ---------------------------------------------------------------------
///
/// Projet          : MUST - Mon Ultra Sonic Télémètre
/// Auteur          : Juanito del Pepito
/// Date            : 02/04/2023
/// Description     : Télémètre à Ultra-Sons
/// Objet           : MUST_App
///                   Objet applicatif de l'application MUST
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include "JUANITO_HCSR04.h"
#include "JUANITO_LCD_I2C.h"
#include "JUANITO_ANALOG_LED.h"

// Déclaration des constantes propres à l'application
#define BOX_WIDTH_MM                      75                  // Largeur de la boîte pour le calcul de la distance
#define LED_GREEN_MIN_VALUE               1                   // Valeur minimale de la LED verte
#define LED_GREEN_MAX_VALUE               5                   // Valeur maximale de la LED verte
#define LED_RED_MIN_VALUE                 1                   // Valeur minimale de la LED verte
#define LED_RED_MAX_VALUE                 25                  // Valeur maximale de la LED verte
#define LED_CARD_MIN_VALUE                1                   // Valeur minimale des LED de la protoShield
#define LED_CARD_MAX_VALUE                20                  // Valeur maximale des LED de la protoShield

// Définition des status de Mesure
#define ACTION_STATUS_MESURE_OFF          1                   // Etat d'action en cours : Mesure OFF
#define ACTION_STATUS_MESURE_ON           2                   // Etat d'action en cours : Mesure ON
#define ACTION_STATUS_MESURE_COMPARE      3                   // Etat d'action en cours : Mesure ON en mode CComparaison

/// ---------------------
/// MIUST_App : Objet applicatif de l'application MUST
///
class MUST_App
{
public:
	MUST_App(int pinLedCardLeft, int pinLedCardRight, int pinLedRed, int pinLedGreen, int pinPushAction, int pinEcho, int pinTrigger);
  void Init();
  void CardLedOscillate(int actionStatus);
  void UpdateGreenLedState(int actionStatus);
  void UpdateCompare();
  void ResetCompare();
  void UpdateScreen(int actionStatus);

private:
  void DisplayStartApp();
  void DisplayMesure(int actionStatus);
  String FormatedDistance();
  String FormatedComparedDistance();
  String FormatedFullComparedDistance();
  void PrintCompareGlyphe();
  int GetCompareGlyphe();
  void SimpleBlinkGreenLed();
  void DoubleBlinkGreenLed();

  // Champs correspondant aux PIN connectés
  int _pinLedCardLeft;
  int _pinLedCardRight;
  int _pinLedRed;
  int _pinLedGreen;
  int _pinPushAction;
  int _pinEcho;
  int _pinTrigger;

  // Initialisation des objets internes
  JUANITO_LCD_I2C _lcd = JUANITO_LCD_I2C();                   // Ecran LCD 16 x 2 I2C
  JUANITO_HCSR04 _hcsr04 = JUANITO_HCSR04();                  // Module Ultra-Son HCSR04
  JUANITO_ANALOG_LED _cardLeftLed = JUANITO_ANALOG_LED();     // LED interne gauche de la protoShield
  JUANITO_ANALOG_LED _cardRightLed = JUANITO_ANALOG_LED();    // LED interne droite de la protoShield
  JUANITO_ANALOG_LED _redLed = JUANITO_ANALOG_LED();          // LED rouge : voyant de Standby
  JUANITO_ANALOG_LED _greenLed = JUANITO_ANALOG_LED();        // LED verte : voyant d'état de l'action en cours
  
  // Paramètres du démarrage de l'application   
  const int _delayTitreScroll = 25;
  const int _delayTexte = 100;
  const int _delayChargement = 500;
  const int _delayFinal = 1000;
  const String _titreApp = "MUST            ";
  const String _texteChargement = "chargement ";

  // // Paramètres de l'oscillation des Led de la carte Protoshield
  bool _ledCardSensUp = true;                                 // Sens d'oscillation ds leds
  int _ledCardLevel = 2;                                      // Niveau de départ (démarrage application, première oscillation) des leds

  // Paramètres des mesures US
  int _actionStatus = 1;                                      // Status d'action en cours : [1] Mesure OFF / [2] Mesure ON / [3] Compare
  long _lastMesure;                                           // Sauvegarde de la dernière mesure effectuée
  long _compareMesure;                                        // Sauvegarde de la mesure effectuée à titre de comparaison
  const int _validiteMesure = 350;                            // Durée (ms) de validité d'une mesure afin de limiter les prises de mesures

  // Paramètres d'affichage (LCD)
  const int _lcdTauxRefreshMillis = 100;                      // Taux de rafraichissement de l'écran LCD à 100ms

  // Chronos internes
  long _chronoLedCardLoop;
  long _chronoStatusChange;                                   // Timestamp du dernier changement de status : Mesure OFF / Mesure ON / Compare
  long _chronoLastMesure;                                     // Timestamp de la dernière prise de mesure
  long _chronoLCDRefresh;
};