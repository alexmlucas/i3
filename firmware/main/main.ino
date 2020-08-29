#include <Trill.h>
#include "Pitch_Detector.h";

Trill trillSensor;

int pressureSensorPin = A6;
int counter = 0;
bool touchSwitches[30];



Pitch_Detector pitchDetector;

void setup()
{
  pinMode(pressureSensorPin, INPUT);
  
  Serial.begin(115200);
  int ret = trillSensor.begin(Trill::TRILL_CRAFT);

  delay(1000);
  
  if(ret != 0)
  {
    Serial.println("failed to intialise trillSensor");
    Serial.print("Error code: ");
    Serial.print(ret);
  }

  for(int i = 0; i < 30; i++)           // intialise touchBuffer
  {
    touchSwitches[i] = false;
  }

  Serial.println("Hi there");
}

void loop() {

  //Serial.println("Hey!");
  delay(50);
  trillSensor.requestRawData();

  if(trillSensor.rawDataAvailable() > 0)
  {
    
    while(trillSensor.rawDataAvailable() > 0)
    {
      int data = trillSensor.rawDataRead();

      if(data > 500) 
      {
        touchSwitches[counter] = true;
      } else
      {
        touchSwitches[counter] = false;
      }

      if(counter < 29){
        counter ++;
      } else {
        counter = 0;
      }
    }

    int stringPitchHz = pitchDetector.getStringPitch(touchSwitches, 15, 24);

    if(stringPitchHz != -1)
    {
      Serial.println(stringPitchHz);
    }
  }


  Serial.println(analogRead(pressureSensorPin));
}
