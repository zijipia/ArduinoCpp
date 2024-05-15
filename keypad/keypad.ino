#include "Keypad.h"
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
//3x4     Arduino
// 1  -     2 12
// 2  -     3 13
// 3  -     4 15 
// 4  -     5 14
// 5  -     6 2
// 6  -     7 4
// 7  -     8 16
//byte rPins[Rows]= {16,4,2,14}; //Rows 0 to 3
//byte cPins[Cols]= {15,13,12}; //Columns 0 to 2
//byte rPins[Rows]= {8,7,6,5}; //Rows 0 to 3
//byte cPins[Cols]= {4,3,2}; //Columns 0 to 2
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {16,4,2,14};
byte colPins[COLS] = {15,13,12};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
}

void loop() {
  char key = keypad.getKey();

  if (key){
    Serial.println(key);
  }
}
