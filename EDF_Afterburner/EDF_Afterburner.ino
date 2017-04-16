/*
 ****************************************
 *     EDF-Afterburner                  *
 ****************************************  
 
 Original Projekt von: http://www.mostfun.de/index.php/modellbau/flugzeuge-blogansicht/314-edf-afterburner2

 Je nach Anwendung müssen die #define Einstellungen angepasst werden. Es werden 60 LED unterstützt, 
 das LED Signal wird an Pin D13 und D6 ausgegeben. 

 Für die Fehlersuche die serielle Textausgabe mit einem Terminal verwenden (9600baud). 
 Unter Arduino: Werkzeuge > Serieller Monitor wählen 

Releas Notes:

 V1.2     16.04.17
 - LED Pin für Arduino pro Micro definiert

 V1.1     04.02.17
 - LED Anzahl von 24 auf 60 angepasst


 V1.0     26.12.16
 - Original Code übernommen und für BlinkyTape Board angepasst
 - PinChangeInterrupt angepasst


 */

// ********* Einstellungen ***************

// NeoPixels LED 
#define LED_Pin1       13         // BlinkyTape
#define LED_Pin2       6          // Arduino pro Micro
#define PIXELS         60         // Anzahl LED 


// Serial Debug 
#define serialBaudRate 9600 


// RC-Channel 
#define RC_Pin         9          // Pin RC-Channel
#define max_pwm        1940       // Motor-EIN (100%): entspricht Servoposition bei +100% (FUTABA: 1940ms)
#define min_pwm        1100       // Motor-AUS (0%): entspricht Servoposition bei -100% (FUTABA: 1100ms)


/*
       Servoposition      Motorposition
       +100%  +---+       +100%  |                           /
              |   |              |                         /
              |   |              |                       0     Pos4
              |   |              |                     /
              |   |              |                   /
              |   |              |                 0     Pos3
              |   |              |               /
          0%  +---+              |             /
              |   |              |           0     Pos2
              |   |              |         /
              |   |              |       /
              |   |              |     0     Pos1
              |   |              |   /
              |   |              | /
       -100%  +---+          0%  +------------------------------ Farbspektrum
                                    |     |     |     |     |
                                   AUS   rot  orange blau  weis           
*/

// Gas-Kurve: Farbspektrum bei Motorposition[%]
#define Pos4           90
#define Pos3           70
#define Pos2           40
#define Pos1           10



// ********* Ende Einstellungen **********







#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <YetAnotherPcInt.h>



// Globale Variablen 
uint8_t throttleValue;
uint8_t red,green,blue;
uint32_t tBegin; //Timestamp Begin
uint32_t tPulse; //Pulslänge



Adafruit_NeoPixel pixels1 = Adafruit_NeoPixel(PIXELS, LED_Pin1 , NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(PIXELS, LED_Pin2 , NEO_GRB + NEO_KHZ800);

void setup() {
  
  // Initialize Serial port
  Serial.begin(serialBaudRate);
  /*
  #if defined(__AVR_ATmega32U4__)
    while(!Serial);
  #endif
  */

  // Setup RC-Channel
  pinMode(RC_Pin, INPUT_PULLUP);
  PcInt::attachInterrupt(RC_Pin, PWMIRQ, CHANGE);

  // Setup LED
  pixels1.begin();
  pixels2.begin();
  randomSeed(tPulse);
}

void loop() {
  

    throttleValue = ((float)tPulse - min_pwm) / (max_pwm - min_pwm) * 100; 
    Serial.print("Throttle:");
    Serial.print(throttleValue);
    Serial.print("% ");
    
    if (throttleValue<Pos1){
      //ramp up
      red = 0;
      green = 0;
      blue = 0;
      printColor();      
      for(int i=0;i<PIXELS;i++){
        pixels1.setPixelColor(i, pixels1.Color(red,green,blue)); 
        pixels2.setPixelColor(i, pixels2.Color(red,green,blue));
      }
    }else if (throttleValue<Pos2){
      //Red comes
      red=uint8_t(throttleValue-Pos1)*(256/(Pos2-Pos1));
      printColor(); 
      for(int i=0;i<PIXELS;i++){
        pixels1.setPixelColor(i, pixels1.Color(red,random(0,red/8),random(0,red/8)));
        pixels2.setPixelColor(i, pixels2.Color(red,random(0,red/8),random(0,red/8)));  
      }
    }else if (throttleValue<Pos3){
      //red goes to orange
      green=uint8_t((throttleValue-Pos2)*(70/(Pos3-Pos2)));
      printColor(); 
      for(int i=0;i<PIXELS;i++){
        pixels1.setPixelColor(i, pixels1.Color(255,green,random(0,green/2)));
        pixels2.setPixelColor(i, pixels2.Color(255,green,random(0,green/2))); 
      }
    }else if (throttleValue<Pos4){
      //orange goes to blue
      green=70+uint8_t((throttleValue-Pos3+1)*(150/(Pos4-Pos3)));
      blue=uint8_t((throttleValue-Pos3)*(256/(Pos4-Pos3)));
      printColor(); 
      for(int i=0;i<PIXELS;i++){
        pixels1.setPixelColor(i, pixels1.Color(random(128,255),green,blue));
        pixels2.setPixelColor(i, pixels2.Color(random(128,255),green,blue));
      }
    }else{
      //max
      red = 255;
      green = 255;
      blue = 255;
      printColor();      
      for(int i=0;i<PIXELS;i++){
        pixels1.setPixelColor(i, pixels1.Color(red,green,blue));
        pixels2.setPixelColor(i, pixels2.Color(red,green,blue)); 
      }
    }
    pixels1.show(); // This sends the updated pixel color to the hardware.
    pixels2.show(); // This sends the updated pixel color to the hardware.
    

  // Wait before next reading
  delay(50);
}

void PWMIRQ()
{
  if (digitalRead(RC_Pin)==HIGH){
    tBegin=micros();
  }
  else{
    tPulse=micros()-tBegin;
    if (tPulse > max_pwm){
      tPulse = max_pwm;
    }
    if (tPulse < min_pwm){
      tPulse = min_pwm;
    }
  }
}


void printColor(){
  Serial.print("red/green/blue: ");
  Serial.print(red);
  Serial.print(":");
  Serial.print(green);
  Serial.print(":");
  Serial.println(blue);
}

