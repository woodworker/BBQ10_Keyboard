#ifndef Q10KEYBOARD_H
#define Q10KEYBOARD_H

#include <Arduino.h>

class Q10Keyboard
{
  private:
    byte colPins[8];
    byte rowPins[5];
    byte backlightPins[2];
    bool keys[5][7];
    bool lastValue[5][7];
    bool changedValue[5][7];

  public:
    bool symbolSelected = false;
    uint8_t backlightValue = 255;
    uint8_t colSize = 5;
    uint8_t rowSize = 7;

    Q10Keyboard(byte colPins_[5], byte rowPins_[7], byte backlightPins_[2]);
    bool keyPressed(uint8_t col, uint8_t row);
    bool keyChanged(uint8_t col, uint8_t row);
    bool keyActive(uint8_t col, uint8_t row);
    void read();
    void init();
    void loop();
};

#endif
