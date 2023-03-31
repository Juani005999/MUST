// Librairies
#include "MUST_App.h"

// Déclaration des constantes correspondant aux PIN
#define PIN_LED_CARD_LEFT     10                            // LED de la Protoshield
#define PIN_LED_CARD_RIGHT    11
#define PIN_LED_RED           5                             // LED et PUSH Button
#define PIN_LED_GREEN         3
#define PIN_PUSH_ACTION       A0
#define TRIGGER_PIN           6                             // TRIGGER et ECHO du Module Ultra Son HC-SR04
#define ECHO_PIN              9

#define DBL_CLICK_TIME        400                           // Temps (ms) considéré pour un double-clic afin d'éviter la prise en compte

// Déclarations des variables globales
volatile bool mesureActionON = false;                       // Flag de lecture de mesure en cours. Volatile car potentiellement updaté depuis une interruption
long lastPushBtnTime = 0;                                   // FlagTime du dernier puch button (pour éviter les double-clic)
long lastLoopTime = 0;                                      // DEBUG : Flag de lecture de l'interval mini
long minLoopTime = 0;                                       // DEBUG : Interval mini de la fonction loop
long maxLoopTime = 0;                                       // DEBUG : Interval mini de la fonction loop
double avgLoopTime = 0;                                     // DEBUG : Moyenne du temps d'un Interval
long nbTotalLoops = 0;                                      // DEBUG : Nombre total d'intervalles

// Initialisation des objets
MUST_App app = MUST_App(PIN_LED_CARD_LEFT,                  // Application MUST
                        PIN_LED_CARD_RIGHT,
                        PIN_LED_RED,
                        PIN_LED_GREEN,
                        PIN_PUSH_ACTION,
                        ECHO_PIN,
                        TRIGGER_PIN);

/// ------------------------
/// ARDUINO : Setup
///
void setup() {

  // Démarrage SerialPort pour Debug
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Program started");

  // Initialisation App
  app.Init();

  // Initialisation des chronos interne
  lastPushBtnTime = millis();
  lastLoopTime = millis();
}

/// ------------------------
/// ARDUINO : Loop
///
void loop() {

  // DEBUG : On mesure les temps de Loop Mini, Maxi et AVG
  UpdateLoopMinMax();

  // Mise à jour de l'état d'action en cours
  SetMesureActionState();
  
  // Oscillation des leds de la Protoshield
  app.CardLedOscillate(mesureActionON);

  // Green Led
  app.UpdateGreenLedState(mesureActionON);

  // Mise à jour de l'affichage LCD
  app.UpdateScreen(mesureActionON);
}

/// --------------------------------
/// Mise à jour de l'état d'Action en cours sur action du Push button
///
void SetMesureActionState()
{
  // Sur pression du bouton (hors dbl-click), on inverse l'état d'action en cours
  if (digitalRead(PIN_PUSH_ACTION) == LOW
      && millis() - lastPushBtnTime > DBL_CLICK_TIME)
  {
    lastPushBtnTime = millis();
    mesureActionON = !mesureActionON;
  }
}

/// -------------------------
/// DEBUG : On mesure les temps de Loop Mini, Maxi et AVG
///
void UpdateLoopMinMax()
{
  // TimeSpan actuel
  long currentDif = millis() - lastLoopTime;

  // Temps min
  if (minLoopTime == 0
      || (currentDif != 0 && currentDif < minLoopTime))
  {
    minLoopTime = currentDif;
    if (minLoopTime != 0)
      Serial.println("LoopTime : min = " + String(minLoopTime));
  }

  // Temps MAX
  if (maxLoopTime == 0 || currentDif > maxLoopTime)
  {
    maxLoopTime = currentDif;
    if (maxLoopTime != 0)
      Serial.println("LoopTime : MAX = " + String(maxLoopTime));
  }

  // Actualisation moyenne
  avgLoopTime = avgLoopTime + ((currentDif - avgLoopTime) / (nbTotalLoops + 1));
  // On trace la moyenne tous les XX Loop
  if (nbTotalLoops % 100000 == 0)
    Serial.println("LoopTime : AVG = " + String(avgLoopTime, 5) + " / LOOPS = " + String(nbTotalLoops + 1));
  
  // Actualisation du chrono et flag
  lastLoopTime = millis();
  nbTotalLoops++;
}