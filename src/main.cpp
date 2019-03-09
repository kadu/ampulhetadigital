#include <SimpleTimer.h>
#include <NonBlockingRtttl.h>
#include <Button.h>
#include "LedControl.h"
#include "pitches.h"

#ifdef ARDUINO_AVR_PRO
  #define BUZZER 8
  LedControl lc=LedControl(12,11,10,1);
  Button button1(9); 
#else
  #define BUZZER D1
  LedControl lc=LedControl(D6, D7, D2, 1);
  Button button1(D3); 
#endif

const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char * arkanoid = "Arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6";
const char * mario = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
const char * missonimp = "MissionImp:d=16,o=6,b=95:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d";
const char * entertainer = "Entertainer:d=4,o=5,b=140:8d,8d#,8e,c6,8e,c6,8e,2c.6,8c6,8d6,8d#6,8e6,8c6,8d6,e6,8b,d6,2c6,p,8d,8d#,8e,c6,8e,c6,8e,2c.6,8p,8a,8g,8f#,8a,8c6,e6,8d6,8c6,8a,2d6";


int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};


SimpleTimer timer;

unsigned long delaytime=1171;
// unsigned long delaytime=300;
int counter  = 0;
int columCounter = 0;
int rowCounter = 0;
bool blinkAllFlag = false;

int timerBlinkAll = 0;
int timerCountDownAll = 0;
int timerBlinkAllCounter = 0;
bool buttonState = false;
  // true -> inicia a contagem
  // false -> para a contagem

void checkButton() {
  counter  = 0;
  columCounter = 0;
  rowCounter = 0;
  timerBlinkAllCounter = 0;
  timer.disable(timerBlinkAll);
  timer.disable(timerCountDownAll);
  rtttl::stop();
  lc.clearDisplay(0);
  if (buttonState) {
    timer.enable(timerCountDownAll);
  } else {
    timer.disable(timerBlinkAll);
    timer.disable(timerCountDownAll);
  }
}

void allLedsState(bool state) {
  for(size_t i = 0; i < 8; i++) {
    if (state) {
      lc.setRow(0,i, B00000000);  
    } else {
      lc.setRow(0,i, B11111111);  
    }
  }
}

void blinkAll() {
  Serial.println("BlinkAllllll");
  blinkAllFlag = !blinkAllFlag;
  allLedsState(blinkAllFlag);
  timerBlinkAllCounter++;
}

void countDown() {  
  // Serial.printf("Col: %i  Row: %i\n", columCounter, rowCounter);

  if(rowCounter > 7) {
    rowCounter = 0;
    columCounter++;
  }

  if (columCounter > 7) {
    lc.clearDisplay(0);
    columCounter = 0;
    rowCounter = 0;
  }
  
  lc.setLed(0,rowCounter, columCounter, true);
  rowCounter++;  
  counter++;
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  for(size_t i = 0; i < 10; i++)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  button1.begin();
  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);  
  timerCountDownAll = timer.setInterval(delaytime, countDown);
  timerBlinkAll     = timer.setInterval(300, blinkAll);
  
  timer.disable(timerBlinkAll);
  timer.disable(timerCountDownAll);
  // timer.enable(timerCountDownAll);
  
  lc.clearDisplay(0);
}

void loop() { 
  if(buttonState)  {
    if (counter >= 64) {
      timer.disable(timerCountDownAll);
      timer.enable(timerBlinkAll);     
      if ( !rtttl::isPlaying() ) {
        rtttl::begin(BUZZER, missonimp);
      } 
      // buttonState = false;
      columCounter = 0;    
      rowCounter = 0;
      counter = 0;
    }

    if(timerBlinkAllCounter > 50) {
      lc.clearDisplay(0);
      Serial.println("Blink");
      timerBlinkAllCounter = 0;
      timer.disable(timerBlinkAll);
      timer.disable(timerCountDownAll);
      buttonState = !buttonState;
    }
  }

  timer.run();
  rtttl::play();
  if (button1.pressed()) {
    buttonState = !buttonState;   
    checkButton();
		Serial.println("Button 1 pressed");
    Serial.println(buttonState);
  }
}
