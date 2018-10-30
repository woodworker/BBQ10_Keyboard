#include <Arduino.h>
#include <RingBufCPP.h>
#include <Wire.h>

#include "Q10Keyboard.h"

#define MAX_NUM_ELEMENTS 20

byte rows[] = {2,3,4,5,6,7,8};
const uint8_t rowCount = sizeof(rows)/sizeof(rows[0]);
 
byte cols[] = {A0, A1, A2, 12, 11};
const uint8_t colCount = sizeof(cols)/sizeof(cols[0]);

byte bgPins[2] = {9, 10};

Q10Keyboard kb(cols, rows, bgPins);

//**

char keyboard[colCount][rowCount];
char keyboard_symbol[colCount][rowCount];

int keyboardLightSteps = 20;
int keyboardLight = 20;

// Declare as volatile, since modofied in ISR
volatile unsigned int index = 0;

// Stack allocate the buffer to hold event structs
RingBufCPP<byte, MAX_NUM_ELEMENTS> buf;

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  int i = 0;
  int elementCount = buf.numElements();
  byte byteArray[ elementCount ];
  byte e;
  while (buf.pull(&e))
  {
    byteArray[i] = e;
    i++;
  }
  Wire.write(byteArray, i);
}

bool isPrintableKey(int colIndex, int rowIndex) {
  return keyboard_symbol[colIndex][rowIndex] != NULL || keyboard[colIndex][rowIndex] != NULL;
}

void printPackage(byte package) {
    Serial.println("===");
    Serial.print( (package & B11100000) >> 5 , DEC );
    Serial.println(" Row");

    Serial.print( (package & B00011100) >> 2 , DEC );
    Serial.println(" Col");

    Serial.print( (package & B00000010) >> 1 , BIN );
    Serial.println(" Symbol");

    Serial.print( (package & B00000001) >> 0 , BIN );
    Serial.println(" KeyDown");
}

//**
void printMatrix(Q10Keyboard kb) {
    for (int rowIndex=0; rowIndex < kb.rowSize; rowIndex++) {
        for (int colIndex=0; colIndex < kb.colSize; colIndex++) {
          // we only want to print if the key is pressed and it is a printable character

          bool isKeyPressed = kb.keyPressed(colIndex, rowIndex);
          if (kb.keyChanged(colIndex, rowIndex)) {
            byte package = (rowIndex << 5 | colIndex << 2 | (int)kb.symbolSelected << 1 | (int)isKeyPressed << 0);

            if (buf.isFull())
            {
              Serial.println("The ring buffer is currently full");
              byte e;
              buf.pull(&e);
            }

            if (buf.isFull())
            {
              Serial.println("The ring buffer is STIL currently full");
            } else {
              buf.add(package);
            }

            
            Serial.print("There are: ");
            Serial.print(buf.numElements());
            Serial.println(" element(s) in the ring buffer");
          }
          
          if (isKeyPressed && isPrintableKey(colIndex, rowIndex)) {
            String toPrint;
            if (kb.symbolSelected) {
              kb.symbolSelected = false;
              toPrint = String(keyboard_symbol[colIndex][rowIndex]);
            } else {
              toPrint = String(keyboard[colIndex][rowIndex]);
            }

            // keys 1,6 and 2,3 are Shift keys, so we want to upper case
            if (kb.keyActive(1,6) || kb.keyActive(2,3)) {
              toPrint.toUpperCase();
            }
            
            Serial.println(toPrint);
          }
        }   
    }
}
// **/

void setup() {
    keyboard[0][0] = 'q';
    keyboard[0][1] = 'w';
    keyboard[0][2] = '\0'; // symbol
    keyboard[0][3] = 'a';
    keyboard[0][4] = '\0'; // ALT
    keyboard[0][5] = ' ';
    keyboard[0][6] = '\0'; // Mic
    
    keyboard[1][0] = 'e';
    keyboard[1][1] = 's';
    keyboard[1][2] = 'd';
    keyboard[1][3] = 'p';
    keyboard[1][4] = 'x';
    keyboard[1][5] = 'z';
    keyboard[1][6] = '\0'; // Left Shift
    
    keyboard[2][0] = 'r';
    keyboard[2][1] = 'g';
    keyboard[2][2] = 't';
    keyboard[2][3] = '\0'; // Right Shit
    keyboard[2][4] = 'v';
    keyboard[2][5] = 'c';
    keyboard[2][6] = 'f';
    
    keyboard[3][0] = 'u';
    keyboard[3][1] = 'h';
    keyboard[3][2] = 'y';
    keyboard[3][3] = '\0'; // Enter
    keyboard[3][4] = 'b';
    keyboard[3][5] = 'n';
    keyboard[3][6] = 'j';
    
    keyboard[4][0] = 'o';
    keyboard[4][1] = 'l';
    keyboard[4][2] = 'i';
    keyboard[4][3] = '\0'; // Backspace
    keyboard[4][4] = '$';
    keyboard[4][5] = 'm';
    keyboard[4][6] = 'k';

    keyboard_symbol[0][0] = '#';
    keyboard_symbol[0][1] = '1';
    keyboard_symbol[0][2] = '\0';
    keyboard_symbol[0][3] = '*';
    keyboard_symbol[0][4] = '\0';
    keyboard_symbol[0][5] = '\0';
    keyboard_symbol[0][6] = '0';
    
    keyboard_symbol[1][0] = '2';
    keyboard_symbol[1][1] = '4';
    keyboard_symbol[1][2] = '5';
    keyboard_symbol[1][3] = '@';
    keyboard_symbol[1][4] = '8';
    keyboard_symbol[1][5] = '7';
    keyboard_symbol[1][6] = '\0';
    
    keyboard_symbol[2][0] = '3';
    keyboard_symbol[2][1] = '/';
    keyboard_symbol[2][2] = '(';
    keyboard_symbol[2][3] = '\0';
    keyboard_symbol[2][4] = '?';
    keyboard_symbol[2][5] = '9';
    keyboard_symbol[2][6] = '6';
    
    keyboard_symbol[3][0] = '_';
    keyboard_symbol[3][1] = ':';
    keyboard_symbol[3][2] = ')';
    keyboard_symbol[3][3] = '\0';
    keyboard_symbol[3][4] = '!';
    keyboard_symbol[3][5] = ',';
    keyboard_symbol[3][6] = ';';
    
    keyboard_symbol[4][0] = '+';
    keyboard_symbol[4][1] = '"';
    keyboard_symbol[4][2] = '-';
    keyboard_symbol[4][3] = '\0';
    keyboard_symbol[4][4] = '\0';
    keyboard_symbol[4][5] = '.';
    keyboard_symbol[4][6] = '\'';
  
    Serial.begin(115200);
 
    kb.init();

    Wire.begin(10);               // join i2c bus with address #10
    Wire.onRequest(requestEvent); // register event
}

void loop() {
    kb.loop();
    
    printMatrix(kb);

    // key 3,3 is the enter key
    if (kb.keyPressed(3,3)) {
      Serial.println();
    }

    // increase backlight if symbol key is pressed
    if (kb.keyPressed(0,2)) {
      kb.backlightValue = kb.backlightValue + keyboardLightSteps;
    }

    // decrease backlight if right shift key is pressed
    if (kb.keyPressed(2,3)) {
      kb.backlightValue = kb.backlightValue - keyboardLightSteps;
    }
    delay(10);
}
// */