#include "WiFi.h"
#include "NineDofSensor.h"
#include "LightSensor.h"
#include "ColorSensor.h"
#include "OrientationSensor.h"
#include "AnalogSensor.h"
#include "Feedback.h"

const int ESP8266_LED = 5;

const int sensorCount = 5;
Sensor *sensors[sensorCount];

Sensor *sensor = NULL;

const char *message = "101010";
const char *nextSignal;

uint32_t prevTime;

void setup() {
  pinMode(ESP8266_LED, OUTPUT);
  digitalWrite(ESP8266_LED, HIGH);
  delay(50);
  digitalWrite(ESP8266_LED, LOW);
  delay(50);

  Serial.begin(9600);

  WiFiSetup();
  feedbackSetup();

  sensors[0] = new NineDofSensor();
  sensors[1] = new LightSensor();
  sensors[2] = new ColorSensor();
  sensors[3] = new OrientationSensor();
  sensors[4] = new AnalogSensor();

  for(int sensorIndex=0; sensorIndex<sensorCount; sensorIndex++) {
    if(sensors[sensorIndex]->Initialized()) {
      sensor = sensors[sensorIndex];
      message = sensor->Message();
      break;
    }
  }
  nextSignal = message;

  prevTime = millis();
}

void loop() {
  digitalWrite(ESP8266_LED, *nextSignal == '1' ? LOW : HIGH);

  uint32_t t;
  sensor->Clear();
  do {
    sensor->Loop();
    t = millis();   
  } while((t - prevTime) < 90);
  
  prevTime = t;
  
  nextSignal++;
  if(!*nextSignal) {
    nextSignal = message;
  }

  if(!sensor) {
    return;
  }

  sensor->UpdateValues();
  sendMessage(sensor->Values());
  feedback(sensor->Values());
  delay(10);
}

