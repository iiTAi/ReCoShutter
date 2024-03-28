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
    
}