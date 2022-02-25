
#include <Adafruit_GFX.h>
#include <splash.h>
#include <Adafruit_ST7735.h>
#include <RotaryEncoder.h>
#include <SPI.h>
#include <AudioTuner.h>
#include "coeff.h"
#include "bitmaps.h"


#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioTuner               tuner;
AudioPlaySdWav           playSdWav1;     //xy=768,855
AudioPlaySdRaw           playSdRaw1;     //xy=771,904
AudioInputI2S            i2s1;           //xy=864,785
AudioMixer4              mixer2;         //xy=940,869
AudioSynthSimpleDrum     drum1;          //xy=950,695
AudioSynthSimpleDrum     drum2;          //xy=950,737
AudioMixer4              mixer1;         //xy=1089,763
AudioOutputI2S           i2s2;           //xy=1251,761
AudioConnection          patchCord1(playSdWav1, 0, mixer2, 0);
AudioConnection          patchCord2(playSdWav1, 1, mixer2, 1);
AudioConnection          patchCord3(playSdRaw1, 0, mixer2, 2);
AudioConnection          patchCord4(i2s1, 1, mixer1, 2);
AudioConnection          patchCord5(mixer2, 0, mixer1, 3);
AudioConnection          patchCord6(drum1, 0, mixer1, 0);
AudioConnection          patchCord7(drum2, 0, mixer1, 1);
AudioConnection          patchCord8(mixer1, 0, i2s2, 1);
AudioConnection          patchCord9(i2s1, 1, tuner, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=428,457
// GUItool: end automatically generated code
// GUItool: end automatically generated code



#define TFT_CS  4
#define TFT_RST 9
#define TFT_DC 5

#define PIN_IN1 2
#define PIN_IN2 3
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
void checkPosition() {
  encoder.tick();
}

Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
const uint16_t BLACK = 0x0000;
const uint16_t WHITE = 0xffff;
const uint16_t BLUE = 0x001f;
const uint16_t RED =  0xF800;
const uint16_t YELLOW = 0xffe0;
const uint16_t GREEN = 0x07e0;


float guitarFreq[] = {82.41, 110, 146.80, 196.00, 246.90, 329.60};
String guitarNotes[] = {"E2", "A2", "D3", "G3", "B3", "E4"};

float minDifference = 99999;
float currentDifference = 0;
float freqDifference = 0;
int tunerPixel = 0;
int closestNote = 0;

int bpmin = 100;
int bpmsr = 4;
int currentBeat = 4;
boolean metronomeActive = false;
unsigned long metronomePreviousMillis = 0;


String currentPage = "Effects";

unsigned long buttonPreviousMillis = 0;

int selectorValue = 0;
boolean selectorSelected = false;

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
File sd;

String fileList[20];
int fileNum = 0;
int backingTrackState = 0; //0=Stopped 1=Playing  2=Paused
int fileCount = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(1, INPUT_PULLUP);

  display.initR(INITR_MINI160x80);
  display.setRotation(3);
  display.invertDisplay(true);


  delay(250);
  AudioMemory(30);
  delay(250);

  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(1);

  drum1.frequency(220);
  drum1.length(50);
  drum1.pitchMod(0.5);

  drum2.frequency(440);
  drum2.length(50);
  drum2.pitchMod(0.5);
  
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("initialization failed!");
    display.fillScreen(BLACK);
    display.drawBitmap(0, 0, NoSDCard, 160, 80, WHITE);
    return;
  }
  Serial.println("initialization done.");

  sd = SD.open("/");
  
  


  attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPosition, CHANGE);

  tuner.begin(.15, fir_22059_HZ_coefficients, sizeof(fir_22059_HZ_coefficients), 2);

  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, Boot1, 160, 80, WHITE);
  display.drawBitmap(0, 0, Boot2, 160, 80, RED);
  delay(1500);
  menuEffects();
}

void loop() {
  // put your main code here, to run repeatedly:
  static int pos = 0;
  encoder.tick();
  int newPos = encoder.getPosition() / 2;

  while (pos != newPos) {
    int dir = (int)(encoder.getDirection());
    if (dir == 1) { //Encoder Turned Right
      if (currentPage == "menuEffects") {
        menuTuner();
      }
      else if (currentPage == "menuTuner") {
        menuRecord();
      }
      else if (currentPage == "menuRecord") {
        menuBackingTrack();
      }
      else if(currentPage == "menuBackingTrack") {
        menuMetronome();
      }
      else if(currentPage == "fnBackingTrack") {
        if(selectorSelected == false) {
          if(selectorValue < 3) {
            selectorValue ++;
            fnBackingTrack();
            switch(selectorValue) {
              case 0:
               display.setCursor(0, 0);
               display.setTextColor(BLUE, BLACK);
               display.setTextSize(1);
               display.println("<<<");
               break;
              case 1:
                display.setCursor(14, 15);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(2);
                display.print("<" +fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">");
            }
          }
        }
        else {
          if(fileNum < fileCount-1) {
            fileNum ++;
          }
          display.setCursor(14, 15);
          display.setTextColor(WHITE, BLUE);
          display.setTextSize(2);
          display.print("<" +fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">");
        }
      }
      else if (currentPage == "fnMetronome") {
        if (selectorSelected == false) {
          if (selectorValue < 3) {
            selectorValue ++;
            fnMetronome();
            switch (selectorValue) {
              case 0:
                display.setCursor(0, 0);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(1);
                display.println("<<<");
                break;
              case 1:
                display.setCursor(32, 15);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(2);
                display.print(String(bpmsr) + "/4 ");
                break;
              case 2:
                display.setCursor(80, 15);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(2);
                display.print(bpmToString(bpmin));
                break;
              case 3:
                display.setCursor(50, 35);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(1);
                display.println("Start/Stop");
                break;
            }
          }
        }
        else {
          switch (selectorValue) {
            case 1:
              if (bpmsr < 4) {
                bpmsr ++;
                display.setCursor(32, 15);
                display.setTextColor(WHITE, BLUE);
                display.setTextSize(2);
                display.print(String(bpmsr) + "/4 ");
                display.fillRect(34, 52, 92, 17, BLACK);
                switch (bpmsr) {
                  case 0:
                    display.drawCircle(80, 60, 8, WHITE);
                    break;
                  case 1:
                    display.drawCircle(80, 60, 8, YELLOW);
                    break;
                  case 2:
                    display.drawCircle(67, 60, 8, YELLOW);
                    display.drawCircle(92, 60, 8, WHITE);
                    break;
                  case 3:
                    display.drawCircle(55, 60, 8, YELLOW);
                    display.drawCircle(80, 60, 8, WHITE);
                    display.drawCircle(105, 60, 8, WHITE);
                    break;
                  case 4:
                    display.drawCircle(42, 60, 8, YELLOW);
                    display.drawCircle(67, 60, 8, WHITE);
                    display.drawCircle(92, 60, 8, WHITE);
                    display.drawCircle(117, 60, 8, WHITE);
                    break;
                }
              }
              break;
            case 2:
              if (bpmin < 500) {
                bpmin += 5;
                display.setCursor(80, 15);
                display.setTextColor(WHITE, BLUE);
                display.setTextSize(2);
                display.print(bpmToString(bpmin));

              }
              break;
          }
        }
      }
    }
    else if (dir == -1) { //Encoder Turned Left
      if (currentPage == "menuTuner") {
        menuEffects();
      }
      else if (currentPage == "menuRecord") {
        menuTuner();
      }
      else if(currentPage == "menuBackingTrack") {
        menuRecord();
      }
      else if(currentPage == "fnBackingTrack") {
        if(selectorSelected == false) {
          if(selectorValue > 0) {
            selectorValue --;
            fnBackingTrack();
            switch(selectorValue) {
              case 0:
               display.setCursor(0, 0);
               display.setTextColor(BLUE, BLACK);
               display.setTextSize(1);
               display.println("<<<");
               break;
              case 1:
                display.setCursor(14, 15);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(2);
                display.print("<" +fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">");
            }
          }
        }
        else {
          if(fileNum > 0) {
            fileNum --;
          }
          display.setCursor(14, 15);
          display.setTextColor(WHITE, BLUE);
          display.setTextSize(2);
          display.print("<" +fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">");
        }
      }
      else if (currentPage == "menuMetronome") {
        menuBackingTrack();
      }
      else if (currentPage == "fnMetronome") {
        if (selectorSelected == false) {
          if (selectorValue > 0) {
            selectorValue --;
            fnMetronome();
            switch (selectorValue) {
              case 0:
                display.setCursor(0, 0);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(1);
                display.println("<<<");
                break;
              case 1:
                display.setCursor(32, 15);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(2);
                display.print(String(bpmsr) + "/4 ");
                break;
              case 2:
                display.setCursor(80, 15);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(2);
                display.print(bpmToString(bpmin));
                break;
              case 3:
                display.setCursor(50, 35);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(1);
                display.println("Start/Stop");
                break;
            }
          }
        }
        else {
          switch (selectorValue) {
            case 1:
              if (bpmsr > 0) {
                bpmsr --;
                display.setCursor(32, 15);
                display.setTextColor(WHITE, BLUE);
                display.setTextSize(2);
                display.print(String(bpmsr) + "/4 ");
                display.fillRect(34, 52, 92, 17, BLACK);
                switch (bpmsr) {
                  case 0:
                    display.drawCircle(80, 60, 8, WHITE);
                    break;
                  case 1:
                    display.drawCircle(80, 60, 8, YELLOW);
                    break;
                  case 2:
                    display.drawCircle(67, 60, 8, YELLOW);
                    display.drawCircle(92, 60, 8, WHITE);
                    break;
                  case 3:
                    display.drawCircle(55, 60, 8, YELLOW);
                    display.drawCircle(80, 60, 8, WHITE);
                    display.drawCircle(105, 60, 8, WHITE);
                    break;
                  case 4:
                    display.drawCircle(42, 60, 8, YELLOW);
                    display.drawCircle(67, 60, 8, WHITE);
                    display.drawCircle(92, 60, 8, WHITE);
                    display.drawCircle(117, 60, 8, WHITE);
                    break;
                }
              }
              break;
            case 2:
              if (bpmin > 5) {
                bpmin -= 5;
                display.setCursor(80, 15);
                display.setTextColor(WHITE, BLUE);
                display.setTextSize(2);
                display.print(bpmToString(bpmin));
              }
              break;
          }
        }
      }
    }
    pos = newPos;
  }

  if (digitalRead(1) == LOW) { //Button Pressed
    unsigned long buttonCurrentMillis = millis();
    if (buttonCurrentMillis - buttonPreviousMillis >= 300) {
      if (currentPage == "menuTuner") {
        fnTuner();
      }
      else if (currentPage == "fnTuner") {
        menuTuner();
      }
      else if(currentPage == "menuBackingTrack") {
        display.fillScreen(BLACK);
        fnBackingTrack();
        display.setCursor(0, 0);
        display.setTextColor(BLUE, BLACK);
        display.setTextSize(1);
        display.println("<<<");
      }
      else if(currentPage == "fnBackingTrack") {
        if(selectorSelected == false) {
          switch(selectorValue) {
            case 0:
              menuBackingTrack();
              break;
            case 1:
              display.setCursor(14, 15);
              display.setTextColor(WHITE, BLUE);
              display.setTextSize(2);
              display.print("<" +fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">");
              selectorSelected = true;
              break;
          }
          
        }
        else {
          display.setCursor(14, 15);
          display.setTextColor(WHITE, BLACK);
          display.setTextSize(2);
          display.print("<" +fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">");
          selectorSelected = false;
        }
      }
      else if (currentPage == "menuMetronome") {
        display.fillScreen(BLACK);
        selectorValue = 0;
        fnMetronome();
        display.setCursor(0, 0);
        display.setTextColor(BLUE, BLACK);
        display.setTextSize(1);
        display.println("<<<");
      }
      else if (currentPage == "fnMetronome") {
      
        if (selectorSelected == false) {
          switch (selectorValue) {
            case 0:
              menuMetronome();
              break;
            case 1:
              display.setCursor(32, 15);
              display.setTextColor(WHITE, BLUE);
              display.setTextSize(2);
              display.print(String(bpmsr) + "/4 ");
              selectorSelected = true;
              break;
            case 2:
              display.setCursor(80, 15);
              display.setTextColor(WHITE, BLUE);
              display.setTextSize(2);
              display.print(bpmToString(bpmin));
              selectorSelected = true;
              break;
            case 3:
              metronomeActive = !metronomeActive;
              metronomePreviousMillis = millis();
              Serial.println(metronomeActive);
              break;
          }
        }
        else {
          switch (selectorValue) {
            case 0:
              menuMetronome();
              break;
            case 1:
              display.setCursor(32, 15);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.print(String(bpmsr) + "/4 ");
              selectorSelected = false;
              break;
            case 2:
              display.setCursor(80, 15);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.print(bpmToString(bpmin));
              selectorSelected = false;
              break;
          }
        }
      }
      buttonPreviousMillis = buttonCurrentMillis;
    }
  }
  if (metronomeActive == true) {
    unsigned long metronomeCurrentMillis = millis();
    if (metronomeCurrentMillis - metronomePreviousMillis >= 60000 / bpmin) {
      if (bpmsr == 0) {
        currentBeat = 0;
        if (currentPage == "fnMetronome") {
          display.fillCircle(80, 60, 8, WHITE);
        }
      }
      else if (currentBeat < bpmsr) {
        currentBeat ++;
      }
      else if (currentBeat >= bpmsr) {
        currentBeat = 1;
      }
      if (currentBeat == 1) {
        drum2.noteOn();
        if (currentPage == "fnMetronome") {
          switch (bpmsr) {
            case 1:
              display.fillCircle(80, 60, 8, YELLOW);
              break;
            case 2:
              display.fillCircle(67, 60, 8, YELLOW);
              display.fillCircle(92, 60, 7, BLACK);
              break;
            case 3:
              display.fillCircle(55, 60, 8, YELLOW);
              display.fillCircle(105, 60, 7, BLACK);
              break;
            case 4:
              display.fillCircle(42, 60, 8, YELLOW);
              display.fillCircle(117, 60, 7, BLACK);
              break;
          }
        }
      }
      else {
        drum1.noteOn();
        if (currentPage == "fnMetronome") {
          switch (currentBeat) {
            case 2:
              switch (bpmsr) {
                case 2:
                  display.fillCircle(67, 60, 7, BLACK);
                  display.fillCircle(92, 60, 8, WHITE);
                  break;
                case 3:
                  display.fillCircle(55, 60, 7, BLACK);
                  display.fillCircle(80, 60, 8, WHITE);
                  break;
                case 4:
                  display.fillCircle(42, 60, 7, BLACK);
                  display.fillCircle(67, 60, 8, WHITE);
                  break;
              }
              break;
            case 3:
              switch (bpmsr) {
                case 3:
                  display.fillCircle(80, 60, 7, BLACK);
                  display.fillCircle(105, 60, 8, WHITE);
                  break;
                case 4:
                  display.fillCircle(67, 60, 7, BLACK);
                  display.fillCircle(92, 60, 8, WHITE);
                  break;
              }
              break;
            case 4:
              switch (bpmsr) {
                case 4:
                  display.fillCircle(92, 60, 7, BLACK);
                  display.fillCircle(117, 60, 8, WHITE);
                  break;
              }
              break;
          }
        }
      }
      metronomePreviousMillis += 60000 / bpmin;
    }
  }
}

void menuEffects() {
  currentPage = "menuEffects";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, Effects, 160, 80, WHITE);
}

void menuTuner() {
  currentPage = "menuTuner";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, Tuner, 160, 80, WHITE);
}

void menuRecord() {
  currentPage = "menuRecord";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, Record, 160, 80, WHITE);
}

void menuBackingTrack() {
  currentPage = "menuBackingTrack";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, BackingTrack, 160, 80, WHITE);
}
void menuMetronome() {
  currentPage = "menuMetronome";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, Metronome, 160, 80, WHITE);
}

void fnTuner() {
  currentPage = "fnTuner";
  display.fillScreen(BLACK);
  display.setTextColor(WHITE, BLACK);
  delay(300);
  while (true) {
    int encoder_button = digitalRead(1);
    if (encoder_button == LOW) {
      display.setTextSize(1);
      break;
    }
    if (tuner.available()) {
      float freq = float(tuner.read());
      minDifference = 99999;
      currentDifference = 0;
      for (int i = 0; i < 6; i++) {
        currentDifference = abs(freq - guitarFreq[i]);
        //Serial.println(String(currentDifference) + ", " + String(guitarFreq[i]) + ", " + String(freq));
        if (currentDifference < minDifference) {
          minDifference = currentDifference;
          freqDifference = freq - guitarFreq[i];
          closestNote = i;
        }
      }
      //Serial.println(String(guitarNotes[closestNote]));
    }

    display.fillRect(79, 0, 2, 30, WHITE);
    if (0 > round(78 + freqDifference * 10)) {
      tunerPixel = 0;
    }
    else if (159 < round(78 + freqDifference * 10)) {
      tunerPixel = 159;
    }
    else {
      tunerPixel = round(78 + freqDifference * 10);
    }
    display.fillRect(0, 0, 79, 30, BLACK);
    display.fillRect(80, 0, 80, 30, BLACK);
    display.fillRect(79, 0, 2, 30, WHITE);
    display.fillRect(tunerPixel , 4, 4, 18, BLUE);
    display.setCursor(70, 40);
    display.setTextSize(2);
    Serial.println(tunerPixel);
    if (75 < tunerPixel && tunerPixel < 84 ) {
      display.setTextColor(GREEN, BLACK);
    }
    else {
      display.setTextColor(WHITE, BLACK);
    }
    display.print(String(guitarNotes[closestNote]));
  }
}

void fnBackingTrack() {
  currentPage = "fnBackingTrack";
  refreshSD();
  display.setCursor(0, 0);
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.print("<<<");
  display.setCursor(14, 15);
  display.setTextSize(2);
  display.print("<" +fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">");
  display.fillRect(56, 50, 12, 15, BLACK);
  display.fillRect(89, 50, 15, 15, WHITE);
  switch(backingTrackState) {
    case 0:
      display.fillTriangle(56, 50, 56, 65, 68, 57, WHITE);
      break;
    case 1:
      display.fillRect(56, 50, 4, 15, WHITE);
      display.fillRect(64, 50, 4, 15, WHITE);
      break;
  }
}

void refreshSD() {
  int i = 0;
  while(true) {
    File entry = sd.openNextFile();
    if(i > 19) {
      break;
    }
    if(!entry) {
      break;
    }
    if(!entry.isDirectory()) {
      if(String(entry.name()) != "RECORD.RAW") {
        fileList[i] = entry.name();
        Serial.println(fileList[i]);
        i++;
      }
    }
    entry.close();
    fileCount = i;
  }
}

void fnMetronome() {
  currentPage = "fnMetronome";
  display.setCursor(0, 0);
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.println("<<<");
  display.setCursor(32, 15);
  display.setTextSize(2);
  display.print(String(bpmsr) + "/4 " + bpmToString(bpmin));
  display.setTextSize(1);
  display.println("BPM");
  display.setCursor(50, 35);
  display.setTextSize(1);
  display.println("Start/Stop");
  switch (bpmsr) {
    case 0:
      display.drawCircle(80, 60, 8, WHITE);
      break;
    case 1:
      display.drawCircle(80, 60, 8, YELLOW);
      break;
    case 2:
      display.drawCircle(67, 60, 8, YELLOW);
      display.drawCircle(92, 60, 8, WHITE);
      break;
    case 3:
      display.drawCircle(55, 60, 8, YELLOW);
      display.drawCircle(80, 60, 8, WHITE);
      display.drawCircle(105, 60, 8, WHITE);
      break;
    case 4:
      display.drawCircle(42, 60, 8, YELLOW);
      display.drawCircle(67, 60, 8, WHITE);
      display.drawCircle(92, 60, 8, WHITE);
      display.drawCircle(117, 60, 8, WHITE);
      break;
  }
}

String bpmToString(int bpm) {
  String sbpm = String(bpm);
  if (String(bpm).length() == 2) {
    sbpm = "0" + String(bpm);
  }
  else if (String(bpm).length() == 1) {
    sbpm = "00" + String(bpm);
  }
  return sbpm;
}
