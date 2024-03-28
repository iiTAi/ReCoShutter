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

}

void loop() {
    
}