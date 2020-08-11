#include <Arduino.h>
#include <TM1637Display.h>
#include <Encoder.h>

Encoder myEnc(0, 2);

// Module connection pins (Digital Pins)
#define CLK 5
#define DIO 4

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};

const uint8_t SEG_c137[] = {
  SEG_A | SEG_F | SEG_E | SEG_D,           // C
  SEG_B | SEG_C,                           // 1
  SEG_A | SEG_B | SEG_G | SEG_C | SEG_D,   // 3
  SEG_A | SEG_B | SEG_C                    // 7
};

const uint8_t SEG_c132[] = {
  SEG_A | SEG_F | SEG_E | SEG_D,           // C
  SEG_B | SEG_C,                           // 1
  SEG_A | SEG_B | SEG_G | SEG_C | SEG_D,   // 3
  SEG_A | SEG_B | SEG_D | SEG_E | SEG_G    // 2
};

const uint8_t SEG_d716[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,         // d
  SEG_A | SEG_B | SEG_C,                         // 7
  SEG_B | SEG_C,                                 // 1
  SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G  // 6
};


uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };

TM1637Display display(CLK, DIO);
int button = 3;
int bulb = 16;

//Led Sequence setup
long interval = 80;           // interval at which to blink (milliseconds)
long multiplier = 10;
unsigned long previousMillis = 0;        // will store last time LED was updated
int ledState = 2;
int ledBrightness = 40;

void setup()
{
  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(bulb, OUTPUT);
  digitalWrite(bulb, LOW);
  digitalWrite(14, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);
  display.setBrightness(0x0f);
}

long oldPosition  = -999;
int curDimension = 0;
int numberOfDimensions = 2;
bool pos = true;
int count = 0;

void updateDimension(int dimension){
    switch (dimension) {
    case 0:
      display.setSegments(SEG_c137);
      break;
    case 1:
      display.setSegments(SEG_c132);
      break;
    case 2:
      display.setSegments(SEG_d716);
      break;
    default:
      display.setSegments(SEG_DONE);
      break;
  }
}

void updateEncoder(){
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    if (newPosition > oldPosition){
      pos = true;
    } else {
      pos = false;
    }
    oldPosition = newPosition;
    if (newPosition%4 == 0){
      if (pos){
        curDimension++;
      } else{
        curDimension--;
      }
      if (curDimension > numberOfDimensions || curDimension < (numberOfDimensions*-1)){
        curDimension = 0;
      }
      updateDimension(abs(curDimension));
      digitalWrite(bulb, LOW);
    }
  }
}


void updateLEDs(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == 1){
      analogWrite(14, 0);
      analogWrite(12, ledBrightness);
      analogWrite(13, ledBrightness);
      ledState = 2;
      interval = interval/multiplier;
    } else if (ledState == 2) {
      analogWrite(14, ledBrightness);
      analogWrite(12, 0);
      analogWrite(13, ledBrightness);
      ledState = 3;
    } else if (ledState == 3) {
      analogWrite(14, ledBrightness);
      analogWrite(12, ledBrightness);
      analogWrite(13, 0);
      ledState = 4;
    } else if (ledState == 4) {
      analogWrite(14, ledBrightness);
      analogWrite(12, ledBrightness);
      analogWrite(13, ledBrightness);
      ledState = 1;
      interval = interval*multiplier;
    } else {
      //This should never happen
      analogWrite(14, 0);
      analogWrite(12, 0);
      analogWrite(13, 0);
      ledState = 1;
    }
  }
}

void checkButton(){
    if (digitalRead(button) == LOW){
      digitalWrite(bulb, HIGH);
      analogWrite(14, 0);
      analogWrite(12, 0);
      analogWrite(13, 0);
      delay(100);
      analogWrite(14, 255);
      analogWrite(12, 255);
      analogWrite(13, 255);
      delay(400);
      digitalWrite(14, 0);
      digitalWrite(12, 0);
      digitalWrite(13, 0);
      delay(100);
  }
}

void loop(){  
  updateEncoder();
  updateLEDs();
  checkButton();
  
}
