#include <EEPROM.h>
#include <TinyWireM.h>
#include <TinyOzOLED.h>

/*--- definition ---*/
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
uint16_t buf;  // Use to edit settings
uint8_t timerScene = 0;
uint32_t start = 0, now = 0;
uint16_t timeOfOset, timeOfRemain, timeOfTimint;

/*--- function ---*/
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

void release() {
  digitalWrite(PINB1, HIGH);
  delay(200);
  digitalWrite(PINB1, LOW);
}

/*
* PB2: SCL
* PB0: SDA
* PB3: ADC
* PB4: Digital Out
* PB1: Digital Out
*/

/*---------- main ----------*/
void setup() {
  pinMode(PINB4, OUTPUT);
  pinMode(PINB1, OUTPUT);

  oled.init();
  oled.clearDisplay();
  oled.setCursorXY(0, 0);
  oled.printString("wait please...");

  mode = mainMenu;

  for (int i = 0; i < 3; i++) {
    EEPROM.get(0 + i * 6, set[i].range);
    EEPROM.get(2 + i * 6, set[i].offset);
    EEPROM.get(3 + i * 6, set[i].loop);
    EEPROM.get(4 + i * 6, set[i].interval);

    if (set[i].range == 65535) {
      EEPROM.put(0 + i * 6, 0b0000000000001010);
      EEPROM.put(2 + i * 6, 0b00000000);
      EEPROM.put(3 + i * 6, 0b00000000);
      EEPROM.put(4 + i * 6, 0b0000000000000101);
      EEPROM.get(0 + i * 6, set[i].range);
      EEPROM.get(2 + i * 6, set[i].offset);
      EEPROM.get(3 + i * 6, set[i].loop);
      EEPROM.get(4 + i * 6, set[i].interval);

    }
  }
  digitalWrite(PINB1, HIGH);
  delay(3000);
  digitalWrite(PINB1, LOW);
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
        beep();
        oled.clearDisplay();
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
        beep();
        oled.clearDisplay();
      } else if (sw == 2 && sp != 1) {
        sp--;
        beep();
      } else if (sw == 3 && sp != 3) {
        sp++;
        beep();
      } else if (sw == 4) {
        mode = timer;
        beep();
        oled.clearDisplay();
      }
      break;
    case timer:
      oled.setCursorXY(0, 0);
      oled.printString("TIMER");
      oled.setCursorXY(0, 1);
      oled.printString("----------------");

      oled.setCursorXY(0, 2);
      oled.printString("REMAIN:");
      oled.printNumber((long)(set[sp - 1].range - timeOfRemain));
      oled.setCursorXY(0, 4);
      oled.printString("OFFSET:");
      oled.printNumber((long)(set[sp - 1].offset - timeOfOset));
      if (set[sp - 1].loop) {
        oled.setCursorXY(0, 5);
        oled.printString("TIMINT:");
        oled.printNumber((long)(set[sp - 1].interval - timeOfTimint));
      }

      switch (timerScene) {
        case 0:
          if (sw == 4) {
            start = millis();
            beep();
          }
          now = millis();
          if (start) {
            timeOfOset = (now - start) * 0.001;
            oled.setCursorXY(7, 4);
            oled.printString("     ");
          }
          if (timeOfOset == set[sp - 1].offset && start) {
            timerScene = 1;
            timeOfOset = 0;
            start = 0;
            beep();
          }
          break;
        case 1:
          if (!(start)) {
            start = millis();
            release();
          }
          now = millis();
          if (start) {
            timeOfRemain = (now - start) * 0.001;
            oled.setCursorXY(7, 2);
            oled.printString("     ");
          }
          if (timeOfRemain == set[sp - 1].range) {
            release();
            timerScene = (set[sp - 1].loop) ? 2 : 0;
            timeOfRemain = 0;
            start = 0;
            beep();
            delay(30);
            beep();
          }
          break;
        case 2:
          if (!(start))
            start = millis();
          now = millis();
          if (start) {
            timeOfTimint = (now - start) * 0.001;
            delay(30);
            oled.setCursorXY(7, 5);
            oled.printString("     ");
          }
          if (timeOfTimint == set[sp - 1].interval) {
            timerScene = 1;
            timeOfTimint = 0;
            start = 0;
            beep();
          }
          break;
      }

      if (sw == 1) {
        mode = setSelect;
        start = 0;
        now = 0;
        timerScene = 0;
        timeOfOset = 0;
        timeOfRemain = 0;
        timeOfTimint = 0;
        beep();
        oled.clearDisplay();
      }

      break;
    case edit:
      oled.setCursorXY(0, 0);
      oled.printString("EDIT");
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
      oled.setCursorXY(setFlag * 8, cp);
      oled.printString("-");

      if (sw == 1) {
        beep();
        if (cp == 2) {
          mode = mainMenu;
          sp = 1;
          oled.clearDisplay();
        } else if (!(setFlag)) {
          oled.setCursorXY(0, cp);
          oled.printChar(' ');
          cp = 2;
        } else {
          setFlag = 0;
          switch (cp) {
            case 3:
              set[sp - 1].range = buf;
              break;
            case 4: 
              set[sp - 1].offset = buf;
              break;
            case 5: 
              set[sp - 1].loop = buf;
              break;
            case 6: 
              set[sp - 1].interval = buf;
              break;
          }
          oled.setCursorXY(8, cp);
          oled.printChar(' ');
        }
      } else if (sw == 2) {
        if (cp == 2) {
          sp -= (sp != 1) ? 1 : 0;
        } else if (!(setFlag)) {
          if (cp != 3) {
            oled.setCursorXY(0, cp);
            oled.printChar(' ');
            cp--;
          }
        } else {
          switch (cp) {
            case 3:
              set[sp - 1].range -= (set[sp - 1].range != 10) ? 10 : 0;
              break;
            case 4: 
              set[sp - 1].offset -= (set[sp - 1].offset) ? 1 : 0;
              break;
            case 5: 
              set[sp - 1].loop = 0;
              break;
            case 6: 
              set[sp - 1].interval -= (set[sp - 1].interval != 5) ? 5 : 0;
              break;
          }
          oled.setCursorXY(9, cp);
          oled.printString("     ");
        }
        beep();
      } else if (sw == 3) {
        if (cp == 2) {
          sp += (sp != 3) ? 1 : 0;
        } else if (!(setFlag)) {
          if (cp != 6) {
            oled.setCursorXY(0, cp);
            oled.printChar(' ');
            cp++;
          }
        } else {
          switch (cp) {
            case 3:
              set[sp - 1].range += (set[sp - 1].range != 3600) ? 10 : 0;
              break;
            case 4: 
              set[sp - 1].offset += (set[sp - 1].offset != 60) ? 1 : 0;
              break;
            case 5: 
              set[sp - 1].loop = 1;
              break;
            case 6: 
              set[sp - 1].interval += (set[sp - 1].interval != 5) ? 1800 : 0;
              break;
          }
        }
        beep();
      } else if (sw == 4) {
        if (cp == 2) {
          oled.setCursorXY(0, cp);
          oled.printChar(' ');
          cp = 3;
        } else if (!(setFlag)) {
          switch (cp) {
            case 3:
              buf = set[sp - 1].range;
              break;
            case 4: 
              buf = set[sp - 1].offset;
              break;
            case 5: 
              buf = set[sp - 1].loop;
              break;
            case 6: 
              buf = set[sp - 1].interval;
              break;
          }
          setFlag = 1;
          oled.setCursorXY(0, cp);
          oled.printChar(' ');
        } else {
          switch (cp) {
            case 3:
              EEPROM.update((sp - 1) * 6, set[sp - 1].range);
              break;
            case 4: 
              EEPROM.update(sp * 6 - 4, set[sp - 1].offset);
              break;
            case 5: 
              EEPROM.update(sp * 6 - 3, set[sp - 1].loop);
              break;
            case 6: 
              EEPROM.update(sp * 6 - 2, set[sp - 1].interval);
              break;
          }
          setFlag = 0;
          oled.setCursorXY(8, cp);
          oled.printChar(' ');

        }
        beep();
      }
      break;
  }
}