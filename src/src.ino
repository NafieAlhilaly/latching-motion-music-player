#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define PIR_MOTION_OUTPUT_PIN 2
#define LATCH_PIN 8

SoftwareSerial mySoftwareSerial(10, 11);
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
static int pir_state = LOW;
unsigned long lastMotionTime = 0;
const unsigned long timeoutPeriod = 30000;
bool motionDetected = false;

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  while (!myDFPlayer.begin(mySoftwareSerial)) { //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    if (myDFPlayer.begin(mySoftwareSerial)) {
      break;
    }
    delay(5000);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(20);
  myDFPlayer.play(1);
  pinMode(PIR_MOTION_OUTPUT_PIN, INPUT);
}

void loop()
{
  static unsigned long timer = millis();
  int volume_now = map(analogRead(A0), 0, 1023, 0, 30);
  myDFPlayer.volume(volume_now);
  pir_state = digitalRead(PIR_MOTION_OUTPUT_PIN);

  if (pir_state == HIGH) {
    // Motion detected
    if (!motionDetected) {
      Serial.println("Motion detected!");
      motionDetected = true;
      digitalWrite(LATCH_PIN, HIGH);
      delay(100);
    }
    lastMotionTime = millis();
  }
  else {
    // No motion
    if (motionDetected) {
      Serial.println("Motion stopped");
      motionDetected = false;
    }

    // Check how long since last motion
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - lastMotionTime;

    if (elapsedTime >= timeoutPeriod) {
      Serial.println("Timeout reached - no motion detected!");
    }
  }

  delay(200);
}
