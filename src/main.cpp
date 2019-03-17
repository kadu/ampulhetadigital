#include <SimpleTimer.h>
#include <NonBlockingRtttl.h>
#include <JC_Button.h>
#include "LedControl.h"
#include "pitches.h"

#ifdef ARDUINO_AVR_PRO
  #define BUZZER 8
  LedControl lc=LedControl(12,11,10,1);
  Button myBtn(9);
#else
  #define BUZZER D1
  LedControl lc=LedControl(D6, D7, D2, 1);
  Button myBtn(D3);
#endif

const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char * arkanoid = "Arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6";
const char * mario = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
const char * missonimp = "MissionImp:d=16,o=6,b=95:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d";
const char * entertainer = "Entertainer:d=4,o=5,b=140:8d,8d#,8e,c6,8e,c6,8e,2c.6,8c6,8d6,8d#6,8e6,8c6,8d6,e6,8b,d6,2c6,p,8d,8d#,8e,c6,8e,c6,8e,2c.6,8p,8a,8g,8f#,8a,8c6,e6,8d6,8c6,8a,2d6";
const unsigned long LONG_PRESS(2000);
int changeConfigCount = 0;

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

// enum states_t {ONOFF, TO_CONFIG, CONFIG, CONFIG_CHANGE, TO_ONOFF};
enum states_t {ONOFF, TO_CONFIG, CONFIG_CHANGE, TO_ONOFF};


SimpleTimer timer;

unsigned long delaytime[4]= {3513, 2342, 1171, 585};
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

void countDown() {
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

void initState(size_t times) {
  counter  = 0;
  columCounter = 0;
  rowCounter = 0;
  timerBlinkAllCounter = 0;
  timer.disable(timerBlinkAll);
  timer.disable(timerCountDownAll);
  rtttl::stop();
  lc.clearDisplay(0);
  for(size_t i = 0; i < times; i++)
  {
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
  }
}
void startSound() {
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(300);
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(300);

  digitalWrite(BUZZER, HIGH);
  delay(700);
  digitalWrite(BUZZER, LOW);
}

void checkButton() {
  initState(1);
  if (buttonState) {
    startSound();
    Serial.println("COUNTING");
    timer.enable(timerCountDownAll);
  } else {
    timer.disable(timerBlinkAll);
    timer.disable(timerCountDownAll);
    Serial.println("STOP COUNTING");
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

void setup() {
  Serial.begin(115200);
  myBtn.begin();

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  lc.shutdown(0,false);
  lc.setIntensity(0,8);

  timerCountDownAll = timer.setInterval(delaytime[2], countDown);
  timerBlinkAll     = timer.setInterval(300, blinkAll);

  timer.disable(timerBlinkAll);
  timer.disable(timerCountDownAll);
  initState(3);
}

void switchButton() {
  if (myBtn.wasReleased()) {
    buttonState = !buttonState;
    checkButton();
  }
}

void changeConfig() {
  Serial.print("changeconfig ");
  Serial.println(changeConfigCount);
  lc.clearDisplay(0);
  switch (changeConfigCount)
  {
    case 0:
      lc.setRow(0,0, B10000000);
      changeConfigCount = 1;
      break;
    case 1:
      lc.setRow(0,0, B00100000);
      changeConfigCount = 2;
      break;
    case 2:
      lc.setRow(0,0, B00001000);
      changeConfigCount = 3;
      break;
    case 3:
      lc.setRow(0,0, B00000010);
      changeConfigCount = 0;
      break;
  }
}

void loop() {

  static states_t STATE;      // current state machine state

  if(buttonState)  {
    if (counter >= 64) {
      timer.disable(timerCountDownAll);
      timer.enable(timerBlinkAll);
      if ( !rtttl::isPlaying() ) {
        rtttl::begin(BUZZER, missonimp);
      } 
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
  myBtn.read();
  rtttl::play();

  switch (STATE)
  {
    case ONOFF:
      if (myBtn.wasReleased())
        switchButton();
      else if (myBtn.pressedFor(LONG_PRESS))
        STATE = TO_CONFIG;
      break;

    case TO_CONFIG:
      if (myBtn.wasReleased()) {
        STATE = CONFIG_CHANGE;
        Serial.println("CONFIG MENU");
        changeConfig();
      }
      else
        initState(2);
        Serial.println("PRESSED TO_CONFIG");
      break;

    case CONFIG_CHANGE:
      if (myBtn.pressedFor(LONG_PRESS)) {
        STATE = TO_ONOFF;
        Serial.println("SAVE");
        Serial.println(changeConfigCount);
        timer.deleteTimer(timerCountDownAll);

        switch (changeConfigCount)
        {
          case 0:
            timerCountDownAll = timer.setInterval(delaytime[3], countDown);
            break;
          case 1:
            timerCountDownAll = timer.setInterval(delaytime[0], countDown);
            break;
          case 2:
            timerCountDownAll = timer.setInterval(delaytime[1], countDown);
            break;
          case 3:
            timerCountDownAll = timer.setInterval(delaytime[2], countDown);
            break;
        }
        changeConfigCount--;
        if (changeConfigCount < 0)
          changeConfigCount = 3;


        initState(3);
      }
      else if(myBtn.wasReleased()) {
        initState(1);
        Serial.println("CHANGE CONFIG ITEM");
        changeConfig();
        break;
      }

    case TO_ONOFF:
      if (myBtn.wasReleased()) {
        STATE = ONOFF;
      }
      break;
  }
}