#include <Arduino.h>
#include "Q10Keyboard.h"

Q10Keyboard::Q10Keyboard(byte colPins_[5], byte rowPins_[7], byte backlightPins_[2]) {
    uint8_t size_c = sizeof(colPins_)/sizeof(colPins_[0]);
    uint8_t size_r = sizeof(rowPins_)/sizeof(rowPins_[0]);
    uint8_t size_b = sizeof(backlightPins_)/sizeof(backlightPins_[0]);

    for(int i=0; i < size_c && i < this->colSize; i++) {
        this->colPins[i] = colPins_[i];
    }

    for(int i=0; i < size_r && i < this->rowSize; i++) {
        this->rowPins[i] = rowPins_[i];
    }

    for(int i=0; i < size_b; i++) {
        this->backlightPins[i] = backlightPins_[i];
    }
}

void Q10Keyboard::init() {
    for (int x=0; x < this->colSize; x++) {
        pinMode(this->colPins[x], INPUT_PULLUP);
    }

    for(int x=0; x < this->rowSize; x++) {
        pinMode(this->rowPins[x], INPUT);
    }

    for(int x=0; x < 2; x++) {
        pinMode(this->backlightPins[x], OUTPUT);
    }
}

void Q10Keyboard::loop() {
    this->read();

    for(int x=0; x < 2; x++) {
        analogWrite(this->backlightPins[x], this->backlightValue);
    }
}

void Q10Keyboard::read() {
    for (int colIndex=0; colIndex < this->colSize; colIndex++) {
        // col: set to output to low
        byte curCol = this->colPins[colIndex];
        pinMode(curCol, OUTPUT);
        digitalWrite(curCol, LOW);
 
        // row: interate through the rows
        for (int rowIndex=0; rowIndex < this->rowSize; rowIndex++) {
            byte rowCol = this->rowPins[rowIndex];
            pinMode(rowCol, INPUT_PULLUP);
            delay(0); // arduino is not fast enought to switch input/output modes so wait 1 ms

            bool buttonPressed = (digitalRead(rowCol) == LOW);
            
            this->keys[colIndex][rowIndex] = buttonPressed;
            if ((this->lastValue[colIndex][rowIndex] != buttonPressed)) {
              this->changedValue[colIndex][rowIndex] = true;
            } else {
              this->changedValue[colIndex][rowIndex] = false;
            }

            this->lastValue[colIndex][rowIndex] = buttonPressed;
            
            pinMode(rowCol, INPUT);
        }
        // disable the column
        pinMode(curCol, INPUT);
    }

    if (this->keyPressed(0, 2)) {
        this->symbolSelected = true;
    }
}

bool Q10Keyboard::keyPressed(uint8_t col, uint8_t row) {
    return this->keyChanged(col, row) && this->keyActive(col, row);
}

bool Q10Keyboard::keyChanged(uint8_t col, uint8_t row) {
    return this->changedValue[col][row] == true;
}

bool Q10Keyboard::keyActive(uint8_t col, uint8_t row) {
    return this->keys[col][row] == true;
}