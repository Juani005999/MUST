/// ---------------------------------------------------------------------
///
/// Projet          : MUST - Mon Ultra Sonic Télémètre
/// Auteur          : Juanito del Pepito
/// Version         : 1.1.0.5
/// Date            : 09/04/2023
/// Description     : Télémètre à Ultra-Sons
///                     - Cet application nécessite l'ajout de la bibliothèque "JUANITO_LIB"
///                     - GitHub : https://github.com/Juani005999/ARDUINO-JUANITO_LIB
///
/// ---------------------------------------------------------------------
// Librairies
#include "MUST_App.h"

// Déclaration des constantes correspondant aux PIN
#define PIN_LED_CARD_LEFT     10                            // LED de la Protoshield
#define PIN_LED_CARD_RIGHT    11
#define PIN_LED_RED           5                             // LED et PUSH Button
#define PIN_LED_GREEN         3
#define PIN_PUSH_ACTION       A0
#define PIN_US_TRIGGER        6                             // TRIGGER et ECHO du Module Ultra Son HC-SR04
#define PIN_US_ECHO           9
#define PIN_DHT_SENSOR        7                             // Sensor de température et d'humidité

// Déclaration des constantes propres à l'application
#define DBL_CLICK_TIME        400                           // Temps (ms) considéré pour un double-clic afin d'éviter la prise en compte
#define LONG_CLICK_TIME       750                           // Temps (ms) considéré pour un clic long pour passage en mode Compare

// Déclaration des variables globales
ActionStatus actionStatus = ACTION_STATUS_MEASURE_OFF;      // Flag de Status d'action en cours [ACTION_STATUS_MESURE_OFF|ACTION_STATUS_MESURE_ON|ACTION_STATUS_MESURE_COMPARE]
bool actionDone = false;                                    // Flag de status HIGH / LOW du Push button
bool buttonPressed = false;                                 // Flag de status HIGH / LOW du Push button
long lastPushBtnTime = 0;                                   // FlagTime du dernier Push button

// Déclaration des variables nécessaire aux mesures des temps de Loop
long lastLoopTime = 0;                                      // DEBUG : Flag de lecture de l'interval mini
long minLoopTime = 0;                                       // DEBUG : Interval mini de la fonction loop
long maxLoopTime = 0;                                       // DEBUG : Interval mini de la fonction loop
float avgLoopTime = 0;                                      // DEBUG : Moyenne du temps d'un Interval
long nbTotalLoops = 0;                                      // DEBUG : Nombre total d'intervalles

// Déclaration des objets
MUST_App app = MUST_App(PIN_LED_CARD_LEFT,                  // Application MUST
                        PIN_LED_CARD_RIGHT,
                        PIN_LED_RED,
                        PIN_LED_GREEN,
                        PIN_PUSH_ACTION,
                        PIN_US_ECHO,
                        PIN_US_TRIGGER,
                        PIN_DHT_SENSOR);

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
  app.CardLedOscillate(actionStatus);

  // Green Led
  app.UpdateGreenLedState(actionStatus);

  // Mise à jour de l'affichage LCD
  app.UpdateScreen(actionStatus);
}

/// --------------------------------
/// Mise à jour de l'état d'Action en cours sur action du Push button
///
void SetMesureActionState()
{
  // Bouton Relaché
  if (digitalRead(PIN_PUSH_ACTION) == HIGH)
  {
    // En mode MesureON ou Compare, sur clic court, on passe en mode MesureOFF
    if ((actionStatus == ACTION_STATUS_MEASURE_ON || actionStatus == ACTION_STATUS_MEASURE_COMPARE)
        && buttonPressed == true
        && actionDone == false)
    {
      actionStatus = ACTION_STATUS_MEASURE_OFF;
    }

    // Update des Flags
    buttonPressed = false;
    actionDone = false;
  }
  // Bouton Pressé
  else
  {
    // Verif de dbl-click
    bool dblClick = false;

    // Actualisation du chrono si nécessaire
    if (buttonPressed == false)
    {
      dblClick = millis() < lastPushBtnTime + DBL_CLICK_TIME;
      if (!dblClick)
        lastPushBtnTime = millis();
    }
  
    // En mode MesureOFF => On passe en MesureON
    if (actionStatus == ACTION_STATUS_MEASURE_OFF
        && buttonPressed == false
        && !dblClick)
    {
      actionStatus = ACTION_STATUS_MEASURE_ON;
      app.ResetCompare();
      actionDone = true;
    }

    // En mode MesureON ou Compare, sur clic long, on passe en mode Compare
    else if ((actionStatus == ACTION_STATUS_MEASURE_ON || actionStatus == ACTION_STATUS_MEASURE_COMPARE)
        && buttonPressed == true
        && actionDone == false
        && millis() > lastPushBtnTime + LONG_CLICK_TIME)
    {
      actionStatus = ACTION_STATUS_MEASURE_COMPARE;
      app.UpdateCompare();
      actionDone = true;
    }

    // Actualisation du status du bouton
    buttonPressed = true;  
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
      Serial.println("LoopTime : min = " + String(minLoopTime) + " ms");
  }

  // Temps MAX
  if (maxLoopTime == 0 || currentDif > maxLoopTime)
  {
    maxLoopTime = currentDif;
    if (maxLoopTime != 0)
      Serial.println("LoopTime : MAX = " + String(maxLoopTime) + " ms");
  }

  // Actualisation moyenne
  avgLoopTime = avgLoopTime + ((currentDif - avgLoopTime) / (nbTotalLoops + 1));
  // On trace la moyenne tous les XX Loop
  if (nbTotalLoops % 100000 == 0)
    Serial.println("LoopTime : AVG = " + String(avgLoopTime, 5) + " ms / Nb. LOOPS = " + String(nbTotalLoops));
  
  // Actualisation du chrono et flag
  lastLoopTime = millis();
  nbTotalLoops++;
}