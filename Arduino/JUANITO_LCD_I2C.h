/// ---------------------------------------------------------------------
///
/// Objet           : JUANITO_LCD_I2C
/// Auteur          : Juanito del Pepito
/// Date            : 02/04/2023
/// Description     : Objet permettant la gestion d'un écran LCD 16 X 2 sur bus I2C
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

// Déclaration des constantes pour l'association des caractères aux glyphes
#define CARACT_ARROW_UP           0                                     // Caractère associé au glyphe : Flèche haut
#define CARACT_ARROW_DOWN         1                                     // Caractère associé au glyphe : Flèche bas
#define CARACT_ARROW_RIGHT        2                                     // Caractère associé au glyphe : Flèche droite
#define CARACT_ARROW_LEFT         3                                     // Caractère associé au glyphe : Flèche gauche
#define CARACT_SMILEY             4                                     // Caractère associé au glyphe : Smiley
#define CARACT_WINK               5                                     // Caractère associé au glyphe : Clin d'oeil

// Déclaration des constantes de settings de l'écran
#define LCD_ROWS                  2                                     // Nombre de lignes du LCD
#define LCD_COLS                  16                                    // Nombre de colonnes du LCD

/// ---------------------
/// JUANITO_LCD_I2C : Objet permettant la gestion d'un écran LCD 16 X 2 sur bus I2C
///
class JUANITO_LCD_I2C
{
public:
	JUANITO_LCD_I2C();
  Init();
  clear();
  setCursor(int col, int row);
  print(String text);
  println(String text);
  printRightJustify (String text, int row);
  upArrow();
  downArrow();
  rightArrow();
  leftArrow();
  smiley();
  wink();

private:
  LiquidCrystal_I2C _lcd = LiquidCrystal_I2C (0x27, LCD_COLS, LCD_ROWS);      // Ecran LCD I2C

  // Caractères spéciaux : Pixels en colonnes (masque de 5 bits) sur 8 lignes
  byte _upArrow[8] = {                                                        // Caractère spécial : flèche haut
    B00100, B01110, B10101, B00100,
    B00100, B00100, B00100, B00000
  };
  byte _downArrow[8] = {                                                      // Caractère spécial : flèche bas
    B00100, B00100, B00100, B00100,
    B10101, B01110, B00100, B00000
  };
  byte _rightArrow[8] = {                                                     // Caractère spécial : flèche droite
    B00000, B00100, B00010, B11111,
    B00010, B00100, B00000, B00000
  };
  byte _leftArrow[8] = {                                                      // Caractère spécial : flèche gauche
    B00000, B00100, B01000, B11111,
    B01000, B00100, B00000, B00000
  };
  byte _smiley[8] = {                                                         // Caractère spécial : Smiley
    B00000, B11011, B11011, B00000,
    B00000, B10001, B01110, B00000
  };
  byte _wink[8] = {                                                           // Caractère spécial : Smiley clin d'oeil
    B00000, B00011, B11011, B00000,
    B00000, B10001, B01110, B00000
  };
};