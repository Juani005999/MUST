/// ---------------------------------------------------------------------
///
/// Projet          : MUST - Mon Ultra Sonic Télémètre
/// Auteur          : Juanito del Pepito
/// Version         : 1.1.0.5
/// Date            : 09/04/2023
/// Description     : Télémètre à Ultra-Sons
/// Objet           : MUST_App : Objet applicatif de l'application MUST
///                     - Cet objet nécessite l'ajout de la bibliothèque "JUANITO_LIB"
///                     - GitHub : https://github.com/Juani005999/ARDUINO-JUANITO_LIB
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <JUANITO_HCSR04.h>
#include <JUANITO_LCD_I2C.h>
#include <JUANITO_LED.h>
#include <JUANITO_DHT.h>

// Déclaration des constantes propres à l'application
#define BOX_WIDTH_MM                      75                  // Largeur de la boîte pour le calcul de la distance
#define LED_GREEN_MIN_VALUE               1                   // Valeur minimale de la LED verte
#define LED_GREEN_MAX_VALUE               5                   // Valeur maximale de la LED verte
#define LED_RED_MIN_VALUE                 1                   // Valeur minimale de la LED rouge
#define LED_RED_MAX_VALUE                 25                  // Valeur maximale de la LED rouge
#define LED_CARD_MIN_VALUE                1                   // Valeur minimale des LED de la protoShield
#define LED_CARD_MAX_VALUE                20                  // Valeur maximale des LED de la protoShield

// Déclarations des constantes nécessaire à l'affichage (LCD)
#define LCD_ADDRESS                       0x27                // Adresse du LCD sur le bus I2C
#define LCD_COLS                          16                  // Nombre de colonnes du LCD
#define LCD_ROWS                          2                   // Nombre de lignes du LCD
#define LCD_REFRESH_SCREEN                100                 // Taux de rafraichissement de l'écran LCD

// Déclarations des constantes nécessaire à la mesure de distance
#define HCSR04_MEASURE_INTERVAL           350                 // Taux de rafraichissement des mesures de distance

// Déclarations des constantes nécessaire à la mesure de l'environnement (température et humidité)
#define DHT_SENSOR_TYPE                   DHT11               // Type de Sensor [DHT11|DHT12|DHT21|DHT22|AM2301]
#define DHT_SENSOR_INTERVAL               2000                // Taux de rafraichissement des mesures de température et d'humidité

// Enum définissant les status de Mesure en cours
enum ActionStatus
{
  ACTION_STATUS_MEASURE_OFF        = 1,												// Etat d'action en cours : Mesure OFF
  ACTION_STATUS_MEASURE_ON         = 2,												// Etat d'action en cours : Mesure ON
  ACTION_STATUS_MEASURE_COMPARE    = 3										    // Etat d'action en cours : Mesure ON en mode CComparaison
};

/// ---------------------
/// MUST_App : Objet applicatif de l'application MUST
///
class MUST_App
{
public:
	MUST_App(int pinLedCardLeft, int pinLedCardRight, int pinLedRed, int pinLedGreen, int pinPushAction, int pinEcho, int pinTrigger, int pinDht);
  void Init();
  void CardLedOscillate(int actionStatus);
  void UpdateGreenLedState(int actionStatus);
  void UpdateCompare();
  void ResetCompare();
  void UpdateScreen(int actionStatus);

private:
  void DisplayStartApp();
  void DisplayMesure(int actionStatus);
  FormatedDistance(char* returnString);
  FormatedComparedDistance(char* returnString);
  FormatedFullComparedDistance(char* returnString);
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
  int _pinDht;

  // Initialisation des objets internes
  JUANITO_LCD_I2C _lcd = JUANITO_LCD_I2C();                   // Ecran LCD 16 x 2 I2C
  JUANITO_HCSR04 _hcsr04 = JUANITO_HCSR04();                  // Module Ultra-Son HCSR04
  JUANITO_LED _cardLeftLed = JUANITO_LED();                   // LED interne gauche de la protoShield
  JUANITO_LED _cardRightLed = JUANITO_LED();                  // LED interne droite de la protoShield
  JUANITO_LED _redLed = JUANITO_LED();                        // LED rouge : voyant de Standby
  JUANITO_LED _greenLed = JUANITO_LED();                      // LED verte : voyant d'état de l'action en cours
  JUANITO_DHT _dht = JUANITO_DHT();                           // Sensor DHT de température et d'humidité

  // Paramètres du démarrage de l'application   
  const int _delayTitreScroll = 25;
  const int _delayTexte = 100;
  const int _delayChargement = 500;
  const int _delayFinal = 1000;
  const char _titreApp[16] = "MUST            ";
  const char _texteChargement[16] = "chargement ";

  // // Paramètres de l'oscillation des Led de la carte Protoshield
  bool _ledCardSensUp = true;                                 // Sens d'oscillation ds leds
  int _ledCardLevel = 2;                                      // Niveau de départ (démarrage application, première oscillation) des leds

  // Paramètres des mesures Ultra-Sons
  ActionStatus _actionStatus = ACTION_STATUS_MEASURE_OFF;     // Status d'action en cours [ACTION_STATUS_MESURE_OFF|ACTION_STATUS_MESURE_ON|ACTION_STATUS_MESURE_COMPARE]
  long _lastMesure;                                           // Sauvegarde de la dernière mesure effectuée
  long _compareMesure;                                        // Sauvegarde de la mesure effectuée à titre de comparaison

  // Chronos internes
  long _chronoLedCardLoop;                                    // Timestamp pour l'oscillation des LED internes
  long _chronoStatusChange;                                   // Timestamp du dernier changement de status : Mesure OFF / Mesure ON / Compare
  long _chronoLCDRefresh;                                     // Timestamp pour le rafraichissement de l'écran LCD
};