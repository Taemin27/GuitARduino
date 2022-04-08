
#include <Adafruit_GFX.h>
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
AudioMixer4              mixer_chorus;   //xy=127,613
AudioInputI2S            i2s1;           //xy=217,331
AudioEffectFlange        flange1;        //xy=282,584
AudioEffectWaveshaper    overdrive1;     //xy=363,301
AudioMixer4              mixer_flanger;  //xy=452,620
AudioMixer4              mixer_overdrive; //xy=533,343
AudioEffectDelay         delay1;         //xy=623,541
AudioEffectWaveshaper    distortion1;    //xy=705,308
AudioMixer4              mixer_decrease;         //xy=809,528
AudioMixer4              mixer_distortion; //xy=879,351
AudioMixer4              mixer_delay;    //xy=967,625
AudioEffectWaveshaper    fuzz1;          //xy=1049,303
AudioAmplifier           amp1;           //xy=1114,579
AudioMixer4              mixer_fuzz;     //xy=1181,366
AudioEffectReverb        reverb1;        //xy=1252,581
AudioEffectChorus        chorus1;        //xy=1377,351
AudioMixer4              mixer_reverb;   //xy=1425,631
AudioAmplifier           amp_master;     //xy=1599,629
AudioPlaySdWav           playSdWav1;     //xy=1684,311
AudioSynthSimpleDrum     drum1;          //xy=1697,360
AudioSynthSimpleDrum     drum2;          //xy=1697,406
AudioMixer4              mixer_master;   //xy=1794,611
AudioMixer4              mixer_metronome; //xy=1953,406
AudioPlaySdRaw           playSdRaw1;     //xy=1974,276
AudioMixer4              mixer_wav;      //xy=1975,333
AudioRecordQueue         queue1;         //xy=1979,658
AudioOutputI2S           i2s2;           //xy=1980,615
AudioRecordQueue         queue2;         //xy=1980,702
AudioConnection          patchCord1(mixer_chorus, 0, mixer_flanger, 1);
AudioConnection          patchCord2(mixer_chorus, flange1);
AudioConnection          patchCord3(i2s1, 1, overdrive1, 0);
AudioConnection          patchCord4(i2s1, 1, mixer_overdrive, 1);
AudioConnection          patchCord5(flange1, 0, mixer_flanger, 0);
AudioConnection          patchCord6(overdrive1, 0, mixer_overdrive, 0);
AudioConnection          patchCord7(mixer_flanger, delay1);
AudioConnection          patchCord8(mixer_flanger, 0, mixer_delay, 1);
AudioConnection          patchCord9(mixer_overdrive, distortion1);
AudioConnection          patchCord10(mixer_overdrive, 0, mixer_distortion, 1);
AudioConnection          patchCord11(delay1, 0, mixer_decrease, 0);
AudioConnection          patchCord12(delay1, 1, mixer_decrease, 1);
AudioConnection          patchCord13(delay1, 2, mixer_decrease, 2);
AudioConnection          patchCord14(delay1, 3, mixer_decrease, 3);
AudioConnection          patchCord15(distortion1, 0, mixer_distortion, 0);
AudioConnection          patchCord16(mixer_decrease, 0, mixer_delay, 0);
AudioConnection          patchCord17(mixer_distortion, fuzz1);
AudioConnection          patchCord18(mixer_distortion, 0, mixer_fuzz, 1);
AudioConnection          patchCord19(mixer_delay, 0, mixer_reverb, 1);
AudioConnection          patchCord20(mixer_delay, amp1);
AudioConnection          patchCord21(fuzz1, 0, mixer_fuzz, 0);
AudioConnection          patchCord22(amp1, reverb1);
AudioConnection          patchCord23(mixer_fuzz, 0, mixer_chorus, 1);
AudioConnection          patchCord24(mixer_fuzz, chorus1);
AudioConnection          patchCord25(reverb1, 0, mixer_reverb, 0);
AudioConnection          patchCord26(chorus1, 0, mixer_chorus, 0);
AudioConnection          patchCord27(mixer_reverb, amp_master);
AudioConnection          patchCord28(amp_master, 0, mixer_master, 3);
AudioConnection          patchCord29(playSdWav1, 0, mixer_wav, 0);
AudioConnection          patchCord30(playSdWav1, 1, mixer_wav, 1);
AudioConnection          patchCord31(drum1, 0, mixer_metronome, 0);
AudioConnection          patchCord32(drum2, 0, mixer_metronome, 1);
AudioConnection          patchCord33(mixer_master, 0, i2s2, 1);
AudioConnection          patchCord34(mixer_master, queue1);
AudioConnection          patchCord35(mixer_master, queue2);
AudioConnection          patchCord36(mixer_metronome, 0, mixer_master, 2);
AudioConnection          patchCord37(playSdRaw1, 0, mixer_master, 0);
AudioConnection          patchCord38(mixer_wav, 0, mixer_master, 1);
AudioConnection          patchCord39(i2s1, 1, tuner, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=1694,251
// GUItool: end automatically generated code



float ODShape[9] = { -1.0, -0.9, -0.7, -0.4, 0.0, 0.42, 0.75, 0.95, 1.0};
float DSShape[9] = { -0.9, -1.0, -0.9, -0.5, 0.0, 0.53, 0.95, 1.0, 0.94};
float FZShape[9] = { -1.0, -1.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 1.0};

#define CHORUS_DELAY_LENGTH (128*AUDIO_BLOCK_SAMPLES)
short delayline[CHORUS_DELAY_LENGTH];

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
File frec;

String fileList[20];
int fileNum = 0;
int backingTrackState = 0; //0=Stopped 1=Playing  2=Paused
int fileCount = 0;

float wavVolume = 5;
float metronomeVolume = 5;

int mode = 0;  // 0=stopped, 1=recording, 2=playing


int overdriveValue = 0;
int distortionValue = 0;
int fuzzValue = 0;
int chorusValue = 0;
int phaserValue = 0;
int flangerValue = 0;
int delayValue = 0;
int reverbValue = 0;
int gainValue = 0;

int osc = 0;

int next;
#define COUNT_OF(x) ((int32_t)(sizeof x / sizeof x[0]))

int16_t* dptrs;

bool outputEnabled = false;
void processQueues(void) {
  dptrs = queue2.readBuffer();
  if (outputEnabled)
  {
    for (int j = 0; j < AUDIO_BLOCK_SAMPLES; j++) {
      Serial.printf("%d ", dptrs[j]);
      Serial.println();
    }
  }

  queue2.freeBuffer();
}

void setup() {
  // put your setup code here, to run once:
  AudioNoInterrupts();
  Serial.begin(9600);
  pinMode(1, INPUT_PULLUP);

  display.initR(INITR_MINI160x80);
  display.setRotation(3);
  display.invertDisplay(true);

  delay(250);
  AudioMemory(700);
  delay(250);

  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(1);

  mixer_wav.gain(0, 0.5);
  mixer_wav.gain(1, 0.5);

  mixer_metronome.gain(0, 0.5);
  mixer_metronome.gain(1, 0.5);


  mixer_overdrive.gain(0, 0);
  mixer_distortion.gain(0, 0);
  mixer_fuzz.gain(0, 0);
  mixer_chorus.gain(0, 0);
  mixer_flanger.gain(0, 0);
  mixer_delay.gain(0, 0);
  mixer_reverb.gain(0, 0);

  mixer_decrease.gain(0, 0.4);
  mixer_decrease.gain(1, 0.3);
  mixer_decrease.gain(2, 0.2);
  mixer_decrease.gain(3, 0.1);

  amp1.gain(0.4);
  amp_master.gain(1);

  chorus1.begin(delayline, CHORUS_DELAY_LENGTH, 1);

  queue2.begin();

  drum1.frequency(220);
  drum1.length(50);
  drum1.pitchMod(0.5);

  drum2.frequency(440);
  drum2.length(50);
  drum2.pitchMod(0.5);

  overdrive1.shape(ODShape, 9);
  distortion1.shape(DSShape, 9);
  fuzz1.shape(FZShape, 9);


  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {

    display.fillScreen(BLACK);
    display.drawBitmap(0, 0, NoSDCard, 160, 80, WHITE);
    return;
  }


  sd = SD.open("/");




  attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPosition, CHANGE);

  tuner.begin(.15, fir_22059_HZ_coefficients, sizeof(fir_22059_HZ_coefficients), 2);

  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, Boot1, 160, 80, WHITE);
  display.drawBitmap(0, 0, Boot2, 160, 80, RED);
  delay(1500);
  menuEffects();
  AudioInterrupts();
}

void loop() {
  // put your main code here, to run repeatedly:
  static int pos = 0;
  encoder.tick();
  int newPos = encoder.getPosition() / 2;

  while (pos != newPos) {
    //AudioNoInterrupts();
    int dir = (int)(encoder.getDirection());
    if (dir == 1) { //Encoder Turned Right
      if (currentPage == "menuEffects") {
        menuTuner();
      }
      else if (currentPage == "fnEffects") {
        if (selectorSelected == false) {
          if (selectorValue < 4) {
            selectorValue ++;
          }
          fnEffects();
          switch (selectorValue) {
            case 0:
              display.setCursor(0, 0);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("<<<");
              break;
            case 1:
              display.setCursor(0, 16);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Drive       >");
              break;
            case 2:
              display.setCursor(0, 32);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Modulation  >");
              break;
            case 3:
              display.setCursor(0, 48);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Delay");
              break;
            case 4:
              display.setCursor(0, 64);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Reverb");
              break;
          }
        }
        else {
          switch (selectorValue) {
            case 3:
              if (delayValue < 9) {
                effectValue("delay", delayValue + 1, true);
              }
              break;
            case 4:
              if (reverbValue < 9) {
                effectValue("reverb", reverbValue + 1, true);
              }
              break;
          }
        }
      }
      else if (currentPage == "drive") {
        if (selectorSelected == false) {
          if (selectorValue < 4) {
            selectorValue ++;
          }
          drive();
          switch (selectorValue) {
            case 0:
              display.setCursor(0, 0);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("< Effects");
              break;
            case 1:
              display.setCursor(0, 16);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Overdrive");
              break;
            case 2:
              display.setCursor(0, 32);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Distortion");
              break;
            case 3:
              display.setCursor(0, 48);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Fuzz");
              break;
            case 4:
              display.setCursor(0, 64);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Gain");
              break;
          }
        }
        else {
          switch (selectorValue) {
            case 1:
              if (overdriveValue < 9) {
                effectValue("overdrive", overdriveValue + 1, true);
              }
              break;
            case 2:
              if (distortionValue < 9) {
                effectValue("distortion", distortionValue + 1, true);
              }
              break;
            case 3:
              if (fuzzValue < 9) {
                effectValue("fuzz", fuzzValue + 1, true);
              }
              break;
            case 4:
              if (gainValue < 9) {
                effectValue("gain", gainValue + 1, true);
              }
              break;
          }
        }
      }
      else if (currentPage == "modulation") {
        if (selectorSelected == false) {
          if (selectorValue < 3) {
            selectorValue ++;
          }
          modulation();
          switch (selectorValue) {
            case 0:
              display.setCursor(0, 0);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("< Effects");
              break;
            case 1:
              display.setCursor(0, 16);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Chorus");
              break;
            case 2:
              display.setCursor(0, 32);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Phaser");
              break;
            case 3:
              display.setCursor(0, 48);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Flanger");
              break;
          }
        }
        else {
          switch (selectorValue) {
            case 1:
              if (chorusValue < 9) {
                effectValue("chorus", chorusValue + 1, true);
              }
              break;
            case 2:
              if (phaserValue < 9) {
                effectValue("phaser", phaserValue + 1, true);
              }
              break;
            case 3:
              if (flangerValue < 9) {
                effectValue("flanger", flangerValue + 1, true);
              }
              break;
          }
        }
      }
      else if (currentPage == "menuTuner") {
        menuRecord();
      }
      else if (currentPage == "menuRecord") {
        menuBackingTrack();
      }
      else if (currentPage == "fnRecord") {
        if (selectorValue < 4) {
          selectorValue ++;
        }
        fnRecord();
        switch (selectorValue) {
          case 0:
            display.setTextSize(1);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 0);
            display.println("<<<");
            break;
          case 1:
            display.setTextSize(2);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 14);
            display.println("Start REC");
            break;
          case 2:
            display.setTextSize(2);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 30);
            display.println("Stop REC");
            break;
          case 3:
            display.setTextSize(2);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 46);
            display.println("Playback");
            break;
          case 4:
            display.setTextSize(2);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 62);
            display.println("Stop Playback");
            break;
        }
      }
      else if (currentPage == "menuBackingTrack") {
        menuMetronome();
      }
      else if (currentPage == "fnBackingTrack") {
        if (selectorSelected == false) {
          if (selectorValue < 4) {
            selectorValue ++;
            fnBackingTrack();
            switch (selectorValue) {
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
                display.print("<" + fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">" + String("          ").substring(0, 10 - fileList[fileNum].length()));
                break;
              case 2:
                display.setCursor(30, 50);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(2);
                display.print(String(wavVolume).substring(0, 1));
                break;
              case 3:
                display.fillRect(56, 50, 12, 15, BLACK);
                switch (backingTrackState) {
                  case 0:
                    display.fillTriangle(56, 50, 56, 65, 68, 57, BLUE);
                    break;
                  case 1:
                    display.fillRect(56, 50, 4, 15, BLUE);
                    display.fillRect(64, 50, 4, 15, BLUE);
                    break;
                  case 2:
                    display.fillTriangle(56, 50, 56, 65, 68, 57, BLUE);
                    break;
                }
                break;
              case 4:
                display.fillRect(89, 50, 15, 15, BLUE);
                break;
            }
          }
        }
        else {
          switch (selectorValue) {
            case 1:
              if (fileNum < fileCount - 1) {
                fileNum ++;
              }
              display.setCursor(14, 15);
              display.setTextColor(WHITE, BLUE);
              display.setTextSize(2);
              display.print("<" + fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">" + String("          ").substring(0, 10 - fileList[fileNum].length()));
              break;
            case 2:
              if (wavVolume < 9) {
                wavVolume ++;
              }
              display.setCursor(30, 50);
              display.setTextColor(WHITE, BLUE);
              display.setTextSize(2);
              display.print(String(wavVolume).substring(0, 1));
              mixer_wav.gain(0, wavVolume / 10);
              mixer_wav.gain(1, wavVolume / 10);
              break;
          }
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
      if (currentPage == "fnEffects") {
        if (selectorSelected == false) {
          if (selectorValue > 0) {
            selectorValue --;
          }
          fnEffects();
          switch (selectorValue) {
            case 0:
              display.setCursor(0, 0);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("<<<");
              break;
            case 1:
              display.setCursor(0, 16);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Drive       >");
              break;
            case 2:
              display.setCursor(0, 32);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Modulation  >");
              break;
            case 3:
              display.setCursor(0, 48);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Delay");
              break;
            case 4:
              display.setCursor(0, 64);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Reverb");
              break;
          }
        }
        else {
          switch (selectorValue) {
            case 3:
              if (delayValue > 0) {
                effectValue("delay", delayValue - 1, true);
              }
              break;
            case 4:
              if (reverbValue > 0) {
                effectValue("reverb", reverbValue - 1, true);
              }
              break;
          }
        }
      }
      else if (currentPage == "drive") {
        if (selectorSelected == false) {
          if (selectorValue > 0) {
            selectorValue --;
          }
          drive();
          switch (selectorValue) {
            case 0:
              display.setCursor(0, 0);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("< Effects");
              break;
            case 1:
              display.setCursor(0, 16);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Overdrive");
              break;
            case 2:
              display.setCursor(0, 32);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Distortion");
              break;
            case 3:
              display.setCursor(0, 48);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Fuzz");
              break;
            case 4:
              display.setCursor(0, 64);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Gain");
              break;
          }
        }
        else {
          switch (selectorValue) {
            case 1:
              if (overdriveValue > 0) {
                effectValue("overdrive", overdriveValue - 1, true);
              }
              break;
            case 2:
              if (distortionValue > 0) {
                effectValue("distortion", distortionValue - 1, true);
              }
              break;
            case 3:
              if (fuzzValue > 0) {
                effectValue("fuzz", fuzzValue - 1, true);
              }
              break;
            case 4:
              if (gainValue > 0) {
                effectValue("gain", gainValue - 1, true);
              }
              break;
          }
        }
      }
      else if (currentPage == "modulation") {
        if (selectorSelected == false) {
          if (selectorValue > 0) {
            selectorValue --;
          }
          modulation();
          switch (selectorValue) {
            case 0:
              display.setCursor(0, 0);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("< Effects");
              break;
            case 1:
              display.setCursor(0, 16);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Chorus");
              break;
            case 2:
              display.setCursor(0, 32);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Phaser");
              break;
            case 3:
              display.setCursor(0, 48);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.println("Flanger");
              break;
          }
        }
        else {
          switch (selectorValue) {
            case 1:
              if (chorusValue > 0) {
                effectValue("chorus", chorusValue - 1, true);
              }
              break;
            case 2:
              if (phaserValue > 0) {
                effectValue("phaser", phaserValue - 1, true);
              }
              break;
            case 3:
              if (flangerValue > 0) {
                effectValue("flanger", flangerValue - 1, true);
              }
              break;
          }
        }
      }
      else if (currentPage == "menuTuner") {
        menuEffects();
      }
      else if (currentPage == "menuRecord") {
        menuTuner();
      }
      else if (currentPage == "fnRecord") {
        if (selectorValue > 0) {
          selectorValue --;
        }
        fnRecord();
        switch (selectorValue) {
          case 0:
            display.setTextSize(1);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 0);
            display.println("<<<");
            break;
          case 1:
            display.setTextSize(2);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 14);
            display.println("Start REC");
            break;
          case 2:
            display.setTextSize(2);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 30);
            display.println("Stop REC");
            break;
          case 3:
            display.setTextSize(2);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 46);
            display.println("Playback");
            break;
          case 4:
            display.setTextSize(2);
            display.setTextColor(BLUE, BLACK);
            display.setCursor(0, 62);
            display.println("Stop Playback");
            break;
        }
      }
      else if (currentPage == "menuBackingTrack") {
        menuRecord();
      }
      else if (currentPage == "fnBackingTrack") {
        if (selectorSelected == false) {
          if (selectorValue > 0) {
            selectorValue --;
            fnBackingTrack();
            switch (selectorValue) {
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
                display.print("<" + fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">" + String("          ").substring(0, 10 - fileList[fileNum].length()));
                break;
              case 2:
                display.setCursor(30, 50);
                display.setTextColor(BLUE, BLACK);
                display.setTextSize(2);
                display.print(String(wavVolume).substring(0, 1));
                break;
              case 3:
                display.fillRect(56, 50, 12, 15, BLACK);
                switch (backingTrackState) {
                  case 0:
                    display.fillTriangle(56, 50, 56, 65, 68, 57, BLUE);
                    break;
                  case 1:
                    display.fillRect(56, 50, 4, 15, BLUE);
                    display.fillRect(64, 50, 4, 15, BLUE);
                    break;
                  case 2:
                    display.fillTriangle(56, 50, 56, 65, 68, 57, BLUE);
                    break;
                }
                break;
              case 4:
                display.fillRect(89, 50, 15, 15, BLUE);
                break;
            }
          }
        }
        else {
          switch (selectorValue) {
            case 1:
              if (fileNum > 0) {
                fileNum --;
              }
              display.setCursor(14, 15);
              display.setTextColor(WHITE, BLUE);
              display.setTextSize(2);
              display.print("<" + fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">" + String("          ").substring(0, 10 - fileList[fileNum].length()));
              break;
            case 2:
              if (wavVolume > 0) {
                wavVolume --;
              }
              display.setCursor(30, 50);
              display.setTextColor(WHITE, BLUE);
              display.setTextSize(2);
              display.print(String(wavVolume).substring(0, 1));
              mixer_wav.gain(0, wavVolume / 10);
              mixer_wav.gain(1, wavVolume / 10);
              break;
          }
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
    //AudioInterrupts();
  }

  if (digitalRead(1) == LOW) { //Button Pressed
    unsigned long buttonCurrentMillis = millis();
    if (buttonCurrentMillis - buttonPreviousMillis >= 300) {
      //AudioNoInterrupts();
      if (currentPage == "menuEffects") {
        display.fillScreen(BLACK);
        fnEffects();
        display.setCursor(0, 0);
        display.setTextColor(BLUE, BLACK);
        display.setTextSize(2);
        display.println("<<<");

      }
      else if (currentPage == "fnEffects") {
        switch (selectorValue) {
          case 0:
            menuEffects();
            break;
          case 1:
            display.fillScreen(BLACK);
            selectorValue = 0;
            drive();
            display.setCursor(0, 0);
            display.setTextColor(BLUE, BLACK);
            display.setTextSize(2);
            display.println("< Effects");
            break;
          case 2:
            display.fillScreen(BLACK);
            selectorValue = 0;
            modulation();
            display.setCursor(0, 0);
            display.setTextColor(BLUE, BLACK);
            display.setTextSize(2);
            display.println("< Effects");
            break;
          case 3:
            effectValue("delay", delayValue, !selectorSelected);
            break;
          case 4:
            effectValue("reverb", reverbValue, !selectorSelected);
            break;
        }
      }
      else if (currentPage == "drive") {
        switch (selectorValue) {
          case 0:
            display.fillScreen(BLACK);
            selectorValue = 1;
            fnEffects();
            display.setCursor(0, 16);
            display.setTextColor(BLUE, BLACK);
            display.setTextSize(2);
            display.println("Drive       >");
            break;
          case 1:
            effectValue("overdrive", overdriveValue, !selectorSelected);
            break;
          case 2:
            effectValue("distortion", distortionValue, !selectorSelected);
            break;
          case 3:
            effectValue("fuzz", fuzzValue, !selectorSelected);
            break;
          case 4:
            effectValue("gain", gainValue, !selectorSelected);
            break;

        }
      }
      else if (currentPage == "modulation") {
        switch (selectorValue) {
          case 0:
            display.fillScreen(BLACK);
            selectorValue = 2;
            fnEffects();
            display.setCursor(0, 32);
            display.setTextColor(BLUE, BLACK);
            display.setTextSize(2);
            display.println("Modulation  >");
            break;
          case 1:
            effectValue("chorus", chorusValue, !selectorSelected);
            break;
          case 2:
            effectValue("phaser", phaserValue, !selectorSelected);
            break;
          case 3:
            effectValue("flanger", flangerValue, !selectorSelected);
            break;
        }
      }
      else if (currentPage == "menuTuner") {
        fnTuner();
      }
      else if (currentPage == "fnTuner") {
        menuTuner();
      }
      else if (currentPage == "menuRecord") {
        display.fillScreen(BLACK);
        fnRecord();
        display.setCursor(0, 0);
        display.setTextColor(BLUE, BLACK);
        display.setTextSize(1);
        display.println("<<<");
      }
      else if (currentPage == "fnRecord") {
        switch (selectorValue) {
          case 0:
            menuRecord();
            break;
          case 1:
            if (mode == 2) stopPlaying();
            if (mode == 0) {
              startRecording();
              display.fillCircle(154, 3, 3, RED);
            }
            break;
          case 2:
            if (mode == 1) {
              stopRecording();
              display.fillCircle(154, 3, 3, BLACK);
            }
            break;
          case 3:
            if (mode == 1) {
              stopRecording();
              display.fillCircle(154, 3, 3, BLACK);
            }
            if (mode == 0) {
              startPlaying();
              display.fillCircle(154, 3, 3, YELLOW);
            }
            break;
          case 4:
            stopPlaying();
            display.fillCircle(154, 3, 3, BLACK);
            break;
        }
      }
      else if (currentPage == "menuBackingTrack") {
        display.fillScreen(BLACK);
        refreshSD();
        fnBackingTrack();
        display.setCursor(0, 0);
        display.setTextColor(BLUE, BLACK);
        display.setTextSize(1);
        display.println("<<<");
      }
      else if (currentPage == "fnBackingTrack") {
        if (selectorSelected == false) {
          switch (selectorValue) {
            case 0:
              menuBackingTrack();
              break;
            case 1:
              display.setCursor(14, 15);
              display.setTextColor(WHITE, BLUE);
              display.setTextSize(2);
              display.print("<" + fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">" + String("          ").substring(0, 10 - fileList[fileNum].length()));
              selectorSelected = true;
              break;
            case 2:
              display.setCursor(30, 50);
              display.setTextColor(WHITE, BLUE);
              display.setTextSize(2);
              display.print(String(wavVolume).substring(0, 1));
              selectorSelected = true;
              break;
            case 3:
              playBgm();
              break;
            case 4:
              stopBgm();
              break;
          }

        }
        else {
          switch (selectorValue) {
            case 1:
              display.setCursor(14, 15);
              display.setTextColor(WHITE, BLACK);
              display.setTextSize(2);
              display.print("<" + fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">" + String("          ").substring(0, 10 - fileList[fileNum].length()));
              selectorSelected = false;
              break;
            case 2:
              display.setCursor(30, 50);
              display.setTextColor(BLUE, BLACK);
              display.setTextSize(2);
              display.print(String(wavVolume).substring(0, 1));
              selectorSelected = false;
              break;
          }

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
      //AudioInterrupts();
    }
    //AudioInterrupts();
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

  if (outputEnabled == true) {
    if (osc % 1000 == 0) {
      osc = 0;
      if (queue2.available()) {
        //AudioNoInterrupts();
        processQueues();
        next = 250;
      }
      else
        //AudioInterrupts();

        if (Serial.available())
        {
          while (Serial.available())
            Serial.read();

          outputEnabled = !outputEnabled;
        }
      if (--next < 0)
      {
        next = 250;
        Serial.print('.');
      }
    }
    osc ++;
  }

  if (mode == 2) {
    continuePlaying();
  }
  if (mode == 1) {
    continueRecording();
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

void fnEffects() {
  currentPage = "fnEffects";
  display.setCursor(0, 0);
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(2);
  display.println("<<<");
  display.println("Drive       >");
  display.println("Modulation  >");
  display.println("Delay");
  display.println("Reverb");
  effectValue("delay", delayValue, false);
  effectValue("reverb", reverbValue, false);
}
void drive() {
  currentPage = "drive";
  display.setCursor(0, 0);
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(2);
  display.println("< Effects");
  display.println("Overdrive");
  display.println("Distortion");
  display.println("Fuzz");
  display.println("Gain");
  effectValue("overdrive", overdriveValue, false);
  effectValue("distortion", distortionValue, false);
  effectValue("fuzz", fuzzValue, false);
  effectValue("gain", gainValue, false);
}
void modulation() {
  currentPage = "modulation";
  display.setCursor(0, 0);
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(2);
  display.println("< Effects");
  display.println("Chorus");
  display.println("Phaser");
  display.println("Flanger");
  effectValue("chorus", chorusValue, false);
  effectValue("phaser", phaserValue, false);
  effectValue("flanger", flangerValue, false);
}

void effectValue(String effect, int value, boolean selected) {
  if (selected == true) {
    display.setTextColor(WHITE, BLUE);
    selectorSelected = true;
  }
  else if (selected == false) {
    display.setTextColor(WHITE, BLACK);
    selectorSelected = false;
  }

  if (effect == "overdrive") {
    overdriveValue = value;
    display.setCursor(145, 16);
    if (value == 0) {
      mixer_overdrive.gain(0, 0);
      mixer_overdrive.gain(1, 1);
    }
    else {
      mixer_overdrive.gain(0, value / 9);
      mixer_overdrive.gain(1, 1 - (value / 9));
    }
  }
  else if (effect == "distortion") {
    distortionValue = value;
    display.setCursor(145, 32);
    if (value == 0) {
      mixer_distortion.gain(0, 0);
      mixer_distortion.gain(1, 1);
    }
    else {
      mixer_distortion.gain(0, value / 9);
      mixer_distortion.gain(1, 1 - (value / 9));
    }
  }
  else if (effect == "fuzz") {
    fuzzValue = value;
    display.setCursor(145, 48);
    if (value == 0) {
      mixer_fuzz.gain(0, 0);
      mixer_fuzz.gain(1, 1);
    }
    else {
      mixer_fuzz.gain(0, value / 9);
      mixer_fuzz.gain(1, 1 - (value / 9));
    }
  }
  else if (effect == "chorus") {
    chorusValue = value;
    display.setCursor(145, 16);
    if (value == 0) {
      mixer_chorus.gain(0, 0);
    }
    else {
      mixer_chorus.gain(0, 1.0);
      chorus1.voices(value + 1);
    }
  }
  else if (effect == "phaser") {
    phaserValue = value;
    display.setCursor(145, 32);
  }
  else if (effect == "flanger") {
    flangerValue = value;
    display.setCursor(145, 48);
  }
  else if (effect == "delay") {
    delayValue = value;
    display.setCursor(145, 48);
    if(value == 0) {
      mixer_delay.gain(0, 0);
    }
    else {
      mixer_delay.gain(0, 0.5);
      delay1.delay(0, value * 50);
      delay1.delay(1, value * 100);
      delay1.delay(2, value * 150);
      delay1.delay(3, value * 200);
    }
  }
  else if (effect == "reverb") {
    reverbValue = value;
    display.setCursor(145, 64);
    if (value == 0) {
      mixer_reverb.gain(0, 0);
    }
    else {
      float rt = value;
      mixer_reverb.gain(0, 1);
      reverb1.reverbTime(3 * (rt / 9));
    }
  }
  else if (effect == "gain") {
    gainValue = value;
    display.setCursor(145, 64);
    amp_master.gain(1 + (value / 2));
  }
  display.println(value);
}


void fnTuner() {
  //AudioInterrupts();
  currentPage = "fnTuner";
  display.fillScreen(BLACK);
  display.setTextColor(WHITE, BLACK);
  delay(300);
  while (true) {
    int encoder_button = digitalRead(1);
    if (encoder_button == LOW) {
      //AudioNoInterrupts();
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

void fnRecord() {
  currentPage = "fnRecord";
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);
  display.println("<<<");
  display.setCursor(0, 14);
  display.setTextSize(2);
  display.println("Start REC");
  display.println("Stop REC");
  display.println("Playback");
  display.println("Stop Playback");
  display.fillCircle(117, 21, 7, RED);
  display.fillRoundRect(98, 31, 13, 13, 2, RED);
  if (mode == 1) {
    display.fillCircle(154, 3, 3, RED);
  }
  else if (mode == 2) {
    display.fillCircle(154, 3, 3, YELLOW);
  }
  else {
    display.fillCircle(154, 3, 3, BLACK);
  }
}

void fnBackingTrack() {
  currentPage = "fnBackingTrack";
  display.setCursor(0, 0);
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.print("<<<");
  display.setCursor(14, 15);
  display.setTextSize(2);
  display.print("<" + fileList[fileNum].substring(0, fileList[fileNum].length() - 4) + ">" + String("          ").substring(0, 10 - fileList[fileNum].length()));
  display.fillRect(56, 50, 12, 15, BLACK);
  display.fillRect(89, 50, 15, 15, WHITE);
  display.setCursor(30, 50);
  display.drawBitmap(7, 50, Volume, 20, 14, WHITE);
  display.print(String(wavVolume).substring(0, 1));
  switch (backingTrackState) {
    case 0:
      display.fillTriangle(56, 50, 56, 65, 68, 57, WHITE);
      break;
    case 1:
      display.fillRect(56, 50, 4, 15, WHITE);
      display.fillRect(64, 50, 4, 15, WHITE);
      break;
    case 2:
      display.fillTriangle(56, 50, 56, 65, 68, 57, WHITE);
      break;
  }
}

void refreshSD() {
  int i = 0;
  while (true) {
    File entry = sd.openNextFile();
    if (i > 19) {
      break;
    }
    if (!entry) {
      break;
    }
    if (!entry.isDirectory()) {
      if (String(entry.name()) != "RECORD.RAW") {
        fileList[i] = entry.name();
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

void startRecording() {
  if (SD.exists("RECORD.RAW")) {
    SD.remove("RECORD.RAW");
  }
  frec = SD.open("RECORD.RAW", FILE_WRITE);
  if (frec) {
    queue1.begin();
    mode = 1;
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer + 256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    frec.write(buffer, 512);
  }
}

void stopRecording() {
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    frec.close();
  }
  mode = 0;
}

void startPlaying() {
  playSdRaw1.play("RECORD.RAW");
  mode = 2;
}

void continuePlaying() {
  if (!playSdRaw1.isPlaying()) {
    playSdRaw1.stop();
    mode = 0;
  }
}

void stopPlaying() {
  if (mode == 2) {
    playSdRaw1.stop();
    mode = 0;
  }
}

void playBgm() {
  if (!playSdWav1.isPlaying()) {
    playSdWav1.play(fileList[fileNum].c_str());
  }
}

void stopBgm() {
  if (playSdWav1.isPlaying()) {
    playSdWav1.stop();
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
