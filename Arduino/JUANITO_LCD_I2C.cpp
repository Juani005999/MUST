#include "JUANITO_LCD_I2C.h"


/// ---------------------
/// Constructeur
///
JUANITO_LCD_I2C::JUANITO_LCD_I2C ()
{
}

/// ---------------------
/// Initialisation du composant
///
JUANITO_LCD_I2C::Init ()
{
  // Initialisation écran
  _lcd.init();
  _lcd.backlight();

  // Association des caractères spéciaux
  _lcd.createChar(CARACT_ARROW_UP, _upArrow);
  _lcd.createChar(CARACT_ARROW_DOWN, _downArrow);
  _lcd.createChar(CARACT_ARROW_RIGHT, _rightArrow);
  _lcd.createChar(CARACT_ARROW_LEFT, _leftArrow);
  _lcd.createChar(CARACT_SMILEY, _smiley);
  _lcd.createChar(CARACT_WINK, _wink);
}

/// ---------------------
/// Wrapper de LiquidCrystal_I2C::clear
///
JUANITO_LCD_I2C::clear ()
{
  _lcd.clear();
}

/// ---------------------
/// Wrapper de LiquidCrystal_I2C::setCursor
///
JUANITO_LCD_I2C::setCursor (int col, int row)
{
  _lcd.setCursor(col, row);
}

/// ---------------------
/// Wrapper de LiquidCrystal_I2C::print
///
JUANITO_LCD_I2C::print (String text)
{
  _lcd.print(text);
}

/// ---------------------
/// Wrapper de LiquidCrystal_I2C::println
///
JUANITO_LCD_I2C::println (String text)
{
  _lcd.println(text);
}

/// ---------------------
/// Ecrit le texte aligné à droite
///
JUANITO_LCD_I2C::printRightJustify (String text, int row)
{
  if (text.length() > 0)
  {
    _lcd.setCursor(LCD_COLS - text.length(), row);
    _lcd.print(text);
  }
}

/// ---------------------
/// Ecrit le caractère flèche haut
///
JUANITO_LCD_I2C::upArrow ()
{
  _lcd.write(byte(CARACT_ARROW_UP));
}

/// ---------------------
/// Ecrit le caractère flèche bas
///
JUANITO_LCD_I2C::downArrow ()
{
  _lcd.write(byte(CARACT_ARROW_DOWN));
}

/// ---------------------
/// Ecrit le caractère flèche droite
///
JUANITO_LCD_I2C::rightArrow ()
{
  _lcd.write(byte(CARACT_ARROW_RIGHT));
}

/// ---------------------
/// Ecrit le caractère flèche gauche
///
JUANITO_LCD_I2C::leftArrow ()
{
  _lcd.write(byte(CARACT_ARROW_LEFT));
}

/// ---------------------
/// Ecrit le caractère Smiley
///
JUANITO_LCD_I2C::smiley ()
{
  _lcd.write(byte(CARACT_SMILEY));
}

/// ---------------------
/// Ecrit le caractère Clin d'oeil
///
JUANITO_LCD_I2C::wink ()
{
  _lcd.write(byte(CARACT_WINK));
}