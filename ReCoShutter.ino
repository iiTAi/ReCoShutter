#include <EEPROM.h>
#include <TinyWireM.h>
#include <TinyOzOLED.h>

typedef struct {
  uint16_t range;
  uint8_t offset;
  uint8_t loop;
  uint16_t interval;
} sets;

enum modes {
  mainMenu,
  setSelect,
  timer,
  edit
};

modes mode;
sets set[3];
OzOLED oled;

uint8_t sp = 1;  // Set pointer
uint8_t setFlag = 0;
uint8_t cp = 2;  // Cursor pointer
uint8_t sw;  // switch

uint8_t readSwitch() {
  uint16_t value = analogRead(PINB3);
  if (value > 908) {
    return 0;
  } else if (value > 704) {
    return 4;
  } else if (value > 455) {
    return 3;
  } else if (value > 150) {
    return 2;
  } else {
    return 1;
  }
}

void beep() {
  digitalWrite(PINB4, HIGH);
  delay(30);
  digitalWrite(PINB4, LOW);
}

/*
* PB2: SCL
* PB0: SDA
* PB3: ADC
* PB4: Digital Out
*/

void setup() {
  pinMode(PINB4, OUTPUT);

  oled.init();
  oled.clearDisplay();
  oled.setCursorXY(0, 0);
  oled.printString("wait please...");

  mode = mainMenu;

  for (int i = 0; i < 3; i++) {
    EEPROM.get(0 + i * 10, set[i].range);
    EEPROM.get(2 + i * 10, set[i].offset);
    EEPROM.get(3 + i * 10, set[i].loop);
    EEPROM.get(4 + i * 10, set[i].interval);
  }
  
  delay(3000);
  oled.clearDisplay();
}

void loop() {
  sw = readSwitch();
  switch (mode) {
    case mainMenu:
      oled.setCursorXY(0, 0);
      oled.printString("ReCoShutter");
      oled.setCursorXY(0, 1);
      oled.printString("----------------");

      oled.setCursorXY(1, 2);
      oled.printString("TIMER");
      oled.setCursorXY(1, 3);
      oled.printString("EDIT");
      oled.setCursorXY(0, cp);
      oled.printString("-");

      if (sw == 2 && cp != 2) {
        oled.setCursorXY(0, cp);
        oled.printChar(' ');
        cp--;
        beep();
      } else if (sw == 3 && cp != 3) {
        oled.setCursorXY(0, cp);
        oled.printChar(' ');
        cp++;
        beep();
      } else if (sw == 4) {
        switch (cp) {
          case 2: mode = setSelect; break;
          case 3: mode = edit; break;
        }
        cp = 2;
        oled.clearDisplay();
        beep();
      }
      break;
    case setSelect:
      oled.setCursorXY(0, 0);
      oled.printString("SELECT SET");
      oled.setCursorXY(0, 1);
      oled.printString("----------------");

      oled.setCursorXY(1, 2);
      oled.printString("SET");
      oled.printNumber((long)sp);
      oled.setCursorXY(1, 3);
      oled.printString("RANGE : ");
      oled.printNumber((long)set[sp - 1].range);
      oled.setCursorXY(1, 4);
      oled.printString("OFFSET: ");
      oled.printNumber((long)set[sp - 1].offset);
      oled.setCursorXY(1, 5);
      oled.printString("LOOP  : ");
      oled.printNumber((long)set[sp - 1].loop);
      oled.setCursorXY(1, 6);
      oled.printString("TIMINT: ");
      oled.printNumber((long)set[sp - 1].interval);

      if (sw == 1) {
        mode = mainMenu;
        sp = 1;
        oled.clearDisplay();
        beep();
      } else if (sw == 2 && sp != 1) {
        sp--;
        oled.printChar(' ');
        beep();
      } else if (sw == 3 && sp != 3) {
        sp++;
        oled.printChar(' ');
        beep();
      } else if (sw == 4) {
        mode = timer;
        oled.clearDisplay();
        beep();
      }
      break;
    case timer:
      break;
    case edit:
      break;
  }
}