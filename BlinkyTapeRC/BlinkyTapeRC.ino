// Pattern Player Sketch
// Designed to work with PatternPaint and the BlinkyTape controller
//
// Tested with the following software:
// Arduino 1.8.1 (https://www.arduino.cc/en/Main/Software)
// FastLED 3.1.3 (https://github.com/FastLED/FastLED/releases/tag/v3.1.3)
// BlinkyTape 2.1.0 (https://github.com/Blinkinlabs/BlinkyTape_Arduino/releases/tag/2.1.0)

// Releasnotes:
/*
   V1.1.1   15.04.17
   - signal processing bug fixed 
   
   V1.1     08.03.17
   - PWM signal read optimized (with interrupt)
   - Firmware size reduced
   - Button is no longer supported
   - set brightness with Pattern Paint is no longer supported
   
   V1.0     15.01.17
   - PWM signal from an RC receiver is supported
   - Framedelay optimized

*/


#include <animation.h>

#include <EEPROM.h>
#include <FastLED.h>
#include <avr/pgmspace.h>

#include "BlinkyTape.h"
#include "SerialLoop.h"

// Pattern table definitions
#define PATTERN_TABLE_ADDRESS  (0x7000 - 0x80)   // Location of the pattern table in the flash memory
#define PATTERN_TABLE_HEADER_LENGTH     11       // Length of the header, in bytes
#define PATTERN_TABLE_ENTRY_LENGTH      7        // Length of each entry, in bytes

// Header data sections
#define PATTERN_COUNT_OFFSET    0    // Number of patterns in the pattern table (1 byte)
#define LED_COUNT_OFFSET        1    // Number of LEDs in the pattern (2 bytes)
#define BRIGHTNESS_OFFSET       3    // Brightness table (8 bytes)

// Entry data sections
#define ENCODING_TYPE_OFFSET    0    // Encoding (1 byte)
#define FRAME_DATA_OFFSET       1    // Memory location (2 bytes)
#define FRAME_COUNT_OFFSET      3    // Frame count (2 bytes)
#define FRAME_DELAY_OFFSET      5    // Frame delay (2 bytes)

// RC signal
bool RCread = false;
bool RCavailable = false;
bool signalRisingEdge = false;
bool patternChanged = false;
uint16_t pulsLenghtCount;
const int maxPulseLenghtCounts = maxPulseLenght / 7.85;
const int minPulseLenghtCounts = minPulseLenght  / 7.85;
const int centerPulseLenghtCounts = (maxPulseLenghtCounts-minPulseLenghtCounts)/2;

// LED data array
struct CRGB leds[MAX_LEDS];   // Space to hold the current LED data
CLEDController* controller;   // LED controller

// Pattern information
uint8_t patternCount;         // Number of available patterns
uint8_t currentPattern;       // Index of the current patter
Animation pattern;            // Current pattern

uint16_t ledCount;            // Number of LEDs used in the current sketch

uint8_t lastBrightness = 10;  // If no rc signal is present after the boot process, this brightness is used


#define EEPROM_START_ADDRESS  0
#define EEPROM_MAGIG_BYTE_0   0x12
#define EEPROM_MAGIC_BYTE_1   0x34
#define PATTERN_EEPROM_ADDRESS EEPROM_START_ADDRESS + 2

// Read the pattern data from the end of the program memory, and construct a new Pattern from it.
void setPattern(uint8_t newPattern) {
  currentPattern = newPattern % patternCount;

  if (EEPROM.read(PATTERN_EEPROM_ADDRESS) != newPattern) {
    EEPROM.write(PATTERN_EEPROM_ADDRESS, newPattern);
  }

  uint16_t patternEntryAddress =
    PATTERN_TABLE_ADDRESS
    + PATTERN_TABLE_HEADER_LENGTH
    + currentPattern * PATTERN_TABLE_ENTRY_LENGTH;


  Animation::Encoding encodingType = (Animation::Encoding)pgm_read_byte(patternEntryAddress + ENCODING_TYPE_OFFSET);

  PGM_P frameData =  (PGM_P)pgm_read_word(patternEntryAddress + FRAME_DATA_OFFSET);
  uint16_t frameCount = pgm_read_word(patternEntryAddress + FRAME_COUNT_OFFSET);
  uint16_t frameDelay = pgm_read_word(patternEntryAddress + FRAME_DELAY_OFFSET);

  pattern.init(frameCount, frameData, encodingType, ledCount, frameDelay);
}


ISR(PCINT0_vect) {
  // if ready to read rc signal
  if(RCread){
    uint8_t pwmState = !(PINB & (1 << PINB5)); // Reading state of the PB5
    
    if (!pwmState) { // at rising edge
      signalRisingEdge = true;
      // rc signal is available
      RCavailable = true;
      // Configure and start timer4 interrupt.
      TCNT4 = 0x00;  // Reset the counter
      TCCR4B = 0x08; // prescaler: 128
      TIMSK4 = _BV(TOV4);  // turn on the interrupt
    
    }else{

      // stop timer4
      TCCR4B = 0x00; // prescaler: 0 
      TIMSK4 = 0;  // turn off the interrupt
      
      if(RCavailable && signalRisingEdge){
        signalRisingEdge = false;
        // check if rc signal has any change
        if(TCNT4 > pulsLenghtCount+1 || TCNT4 < pulsLenghtCount-1){
          
          // read RC Signal
          pulsLenghtCount = TCNT4;
          int rcValue = int(pulsLenghtCount - minPulseLenghtCounts-centerPulseLenghtCounts)*100/centerPulseLenghtCounts;          
          //Serial.println(rcValue);
          
          if (rcValue < min_off_pos) {
            // change pattern
            if (patternChanged == false) {
              LEDS.setBrightness(lastBrightness);
              setPattern(currentPattern + 1);
              patternChanged = true;
            }

          } else if (rcValue > max_off_pos) {
            // run
            patternChanged = false;
            LEDS.setBrightness(rcValue);
            lastBrightness = rcValue;

          } else {
            // stop
            patternChanged = false;
            LEDS.setBrightness(0);
          }
          
        }
        
      }
      
    }
    
  }else{
    TIMSK4 = 0;  // turn off the interrupt
  }
  
}


ISR(TIMER4_OVF_vect) {
  // if timer overflow, no rc signal available
  if(RCread){
      RCavailable = false; 
  }
  TIMSK4 = 0;  // turn off the interrupt
}



void setup()
{
  Serial.begin(57600);

  // RC signal
  pinMode(ANALOG_INPUT, INPUT_PULLUP);

  // Interrupt set-up; see Atmega32u4 datasheet section 11
  PCIFR  |= (1 << PCIF0);  // Just in case, clear interrupt flag
  PCMSK0 |= (1 << PCINT5); // Set interrupt mask to the analog pin (PCINT5)
  PCICR  |= (1 << PCIE0);  // Enable interrupt

  // setup Timer1 for frame delay
  TCCR1A = 0;
  TCCR1B = 0x04; // prescaler: 256
  TIFR1 = 0xFF;

  // First, load the pattern count and LED geometry from the pattern table
  patternCount = pgm_read_byte(PATTERN_TABLE_ADDRESS + PATTERN_COUNT_OFFSET);
  ledCount     = pgm_read_word(PATTERN_TABLE_ADDRESS + LED_COUNT_OFFSET);

  // Bounds check for the LED count
  // Note that if this is out of bounds,the patterns will be displayed incorrectly.
  if (ledCount > MAX_LEDS) {
    ledCount = MAX_LEDS;
  }

  // If the EEPROM hasn't been initialized, do so now
  if ((EEPROM.read(EEPROM_START_ADDRESS) != EEPROM_MAGIG_BYTE_0)
      || (EEPROM.read(EEPROM_START_ADDRESS + 1) != EEPROM_MAGIC_BYTE_1)) {
    EEPROM.write(EEPROM_START_ADDRESS, EEPROM_MAGIG_BYTE_0);
    EEPROM.write(EEPROM_START_ADDRESS + 1, EEPROM_MAGIC_BYTE_1);
    EEPROM.write(PATTERN_EEPROM_ADDRESS, 0);
  }

  // Read in the last-used pattern
  currentPattern = EEPROM.read(PATTERN_EEPROM_ADDRESS);

  // Now, read the first pattern from the table
  setPattern(currentPattern);

  controller = &(LEDS.addLeds<WS2811, LED_OUT, GRB>(leds, ledCount));
  LEDS.show();

}



void loop()
{
    
  // load timer1 for frame delay
  TCNT1 = 65535 - pattern.getFrameDelay() * 63;
  TIFR1 = 0x01; //reset overflow flag

  // disable rc signal read, when draw pattern 
  noInterrupts();
  RCread = false;
  signalRisingEdge = false;
  interrupts();

  // draw pattern
  pattern.draw(leds);

  // If'n we get some data, switch to passthrough mode
  if (Serial.available() > 0) {
    serialLoop(leds);
  }
  
  // enable rc signal read 
  noInterrupts();
  RCread = true;
  interrupts();

  if (!RCavailable) {
    LEDS.setBrightness(lastBrightness);
  }

  // wait until timer1 overflow flag is set
  while (!(TIFR1 & 1)) {
  }

}


