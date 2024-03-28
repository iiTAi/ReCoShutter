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
uint8_t sw;
uint16_t value;

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
        cp--;
        beep();
      } else if (sw == 3 && cp != 3) {
        cp++;
        beep();
      } else if (sw == 4) {
        switch (cp) {
          case 2: mode = setSelect; break;
          case 3: mode = edit; break;
        }
        cp = (cp == 3) ? 2 : 3;
        oled.clearDisplay();
        beep();
      }
      break;
    case setSelect:
      break;
    case timer:
      break;
    case edit:
      break;
  }
}