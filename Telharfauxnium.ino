// Designed by Ryan Page in April 2017 for the 
// Music Thing Modular Radio Music eurorack module by Tom Whitwell.
// Much of this code is a fork of the module-test firmware
// Many thanks to Tom for his amazing designs

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine	upperHarmonics[8];
AudioSynthWaveformSine	lowerHarmonics[8];
AudioMixer4              mixer2;         //xy=355,277
AudioMixer4              mixer1;         //xy=358,157
AudioMixer4              mixer4;         //xy=358,379
AudioMixer4              mixer5;         //xy=388,853
AudioMixer4              mixer3;         //xy=511,269
AudioOutputAnalog        dac1;           //xy=623,136
AudioConnection          patchCord1(upperHarmonics[0], 0, mixer1, 0);
AudioConnection          patchCord2(upperHarmonics[2], 0, mixer1, 2);
AudioConnection          patchCord3(upperHarmonics[3], 0, mixer1, 3);
AudioConnection          patchCord4(upperHarmonics[1], 0, mixer1, 1);

AudioConnection          patchCord5(upperHarmonics[4], 0, mixer2, 0);
AudioConnection          patchCord6(upperHarmonics[7], 0, mixer2, 3);
AudioConnection          patchCord7(upperHarmonics[5], 0, mixer2, 1);
AudioConnection          patchCord8(upperHarmonics[6], 0, mixer2, 2);

AudioConnection          patchCord9(lowerHarmonics[0], 0, mixer4, 0);
AudioConnection          patchCord10(lowerHarmonics[1], 0, mixer4, 1);
AudioConnection          patchCord11(lowerHarmonics[3], 0, mixer4, 3);
AudioConnection          patchCord14(lowerHarmonics[2], 0, mixer4, 2);

AudioConnection          patchCord12(lowerHarmonics[5], 0, mixer5, 1);
AudioConnection          patchCord13(lowerHarmonics[4], 0, mixer5, 0);
AudioConnection          patchCord15(lowerHarmonics[6], 0, mixer5, 2);
AudioConnection          patchCord16(lowerHarmonics[7], 0, mixer5, 3);
AudioConnection          patchCord17(mixer2, 0, mixer3, 1);
AudioConnection          patchCord18(mixer1, 0, mixer3, 0);
AudioConnection          patchCord19(mixer4, 0, mixer3, 2);
AudioConnection          patchCord20(mixer5, 0, mixer3, 3);
AudioConnection          patchCord21(mixer3, dac1);
AudioControlSGTL5000     sgtl5000_1;     //xy=111,1063
// GUItool: end automatically generated code
 

#define LED0 6
#define LED1 5
#define LED2 4
#define LED3 3
#define CHAN_POT_PIN 9 // pin for Channel pot
#define CHAN_CV_PIN 6 // pin for Channel CV 
#define TIME_POT_PIN 7 // pin for Time pot
#define TIME_CV_PIN 8 // pin for Time CV
#define RESET_BUTTON 8 // Reset button 
#define RESET_LED 11 // Reset LED indicator 
#define RESET_CV 9 // Reset pulse input 

void setup() {
    AudioMemory(15);
  pinMode(RESET_BUTTON, INPUT);
  pinMode(RESET_CV, INPUT);
  pinMode(RESET_LED, OUTPUT);
  pinMode(LED0,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);

  mixer1.gain(0, 0.24);
  mixer1.gain(1, 0.24);
  mixer1.gain(2, 0.24);
  mixer1.gain(3, 0.24);

  mixer2.gain(0, 0.24);
  mixer2.gain(1, 0.24);
  mixer2.gain(2, 0.24);
  mixer2.gain(3, 0.24);

  mixer4.gain(0, 0.24);
  mixer4.gain(1, 0.24);
  mixer4.gain(2, 0.24);
  mixer4.gain(3, 0.24);

  mixer5.gain(0, 0.24);
  mixer5.gain(1, 0.24);
  mixer5.gain(2, 0.24);
  mixer5.gain(3, 0.24);

  mixer3.gain(0, 0.2);
  mixer3.gain(1, 0.2);
  mixer3.gain(2, 0.2);
  mixer3.gain(3, 0.2);
  
  Serial.begin(9600);
}

int PinAverage(int pinID, int numSamples)
{
	int sample = 0;
	for(int i = 0; i < numSamples; ++i)
	{
		sample += analogRead(pinID);
	}
	return sample / numSamples;
}

void PrintDebugInfo(int pot1, int pot2, int cv1, int cv2)
{
	static elapsedMillis update = 0;
	if (update > 50)
	{
		Serial.print("Channel pot=");
		Serial.print(pot1);
		Serial.print(" Time pot=");
		Serial.print(pot2);
		Serial.print(" Channel CV=");
		Serial.print(cv1);
		Serial.print(" Time CV=");
		Serial.println(cv2);
		update = 0;
	}
}

void UpdateLEDs(int pot1, int pot2, int cv1, int cv2)
{
	analogWrite(LED3, pot1 / 4);
	analogWrite(LED2, pot2 / 4);
	analogWrite(LED1, cv1 / 4);
	analogWrite(LED0, cv2 / 4);
}

void loop()
{
	int averagecount = 50;
	int pot1 = PinAverage(CHAN_POT_PIN, averagecount);
	int pot2 = PinAverage(TIME_POT_PIN, averagecount);
	int cv1 = PinAverage(CHAN_CV_PIN, averagecount);
	int cv2 = PinAverage(TIME_CV_PIN, averagecount);
	PrintDebugInfo(pot1, pot2, cv1, cv2);
	UpdateLEDs(pot1, pot2, cv1, cv2);
	
	auto fundamental = pot1*2 + cv1*2;
	auto offset = pot2 + cv2 * 0.001;
	for (int i = 0; i < 8; ++i)
	{
		upperHarmonics[i].frequency(fundamental * pow(offset, i));
		upperHarmonics[i].amplitude(0.9);
		lowerHarmonics[i].frequency(fundamental / pow(offset, i));
		lowerHarmonics[i].amplitude(i > 0 ? 0.9 : 0);
	}
}

// WRITE A 4 DIGIT BINARY NUMBER TO LED0-LED3 
void ledWrite(int n){
  digitalWrite(RESET_LED, HIGH && (n & B00010000));
  digitalWrite(LED0, HIGH && (n & B00001000));
  digitalWrite(LED1, HIGH && (n & B00000100));
  digitalWrite(LED2, HIGH && (n & B00000010));
  digitalWrite(LED3, HIGH && (n & B00000001)); 
}
