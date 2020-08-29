#include <Trill.h>
#include<Audio.h>
#include "Bowed_Violin.h"
#include "Plucked_Violin.h"

Bowed_Violin bowedViolin;
Plucked_Violin pluckedViolin;
Trill trillSensor;
elapsedMillis touchReadTimer;
elapsedMillis triggerTimer;

boolean triggerToggle = false;
boolean pressureActive = false;
boolean touchActive = false; 
int pressureSensorPin = A8;
int lastPressureValue = 0;
float bowPressureValue = 0;


AudioMixer4              mixer1;         //xy=225,197
AudioMixer4              mixer2;         //xy=223,275
AudioOutputI2S           i2s1;           //xy=388,203
AudioConnection          patchCord1(bowedViolin, 0, mixer1, 0);
AudioConnection          patchCord2(pluckedViolin, 0, mixer2, 0);
AudioConnection          patchCord3(mixer1, 0, i2s1, 1);
AudioConnection          patchCord4(mixer2, 0, i2s1, 0);

void setup()
{
  pinMode(A8, INPUT);
  AudioMemory(2);
  mixer1.gain(0,0.6);
  mixer2.gain(0,0.6);

  Serial.begin(115200);
  int ret = trillSensor.begin(Trill::TRILL_BAR);
  if(ret != 0) 
  {
    Serial.println("failed to initialise trillSensor");
    Serial.print("Error code: ");
    Serial.println(ret);
  }

  bowedViolin.setParamValue("freq",670);
}

void loop() {
  if(triggerTimer > 5000){
    triggerToggle = !triggerToggle;
    bowedViolin.setParamValue("Gate", triggerToggle);
    triggerTimer = 0;
  }


  
  // Read 20 times per second
  
  if(touchReadTimer > 50)
  {
    
    trillSensor.read();

    if(trillSensor.getNumTouches() > 0) {
      int touchLocation = trillSensor.touchLocation(0);
      //Serial.print(touchLocation);
      //Serial.print(" ");

      int touchSize = trillSensor.touchSize(0);
      
      //Serial.print(touchSize);
      //Serial.println("");

      touchActive = true;

      constrain(touchSize, 0, 3000);
      
      bowedViolin.setParamValue("freq", map(touchLocation, 0, 3200, 1100, 660));
    } else if(touchActive) {
      // Print a single line when touch goes off
      //Serial.println("0 0");
      
      touchActive = false;
      touchReadTimer = 0;   // reset the timer
    }

    int pressureValue = constrain(analogRead(pressureSensorPin), 0, 700);                       // read pressure value but clip upper end.

    if(pressureValue > 200)
    {
     if(pressureValue > (lastPressureValue + 10) || pressureValue < (lastPressureValue - 10))   // filter noise
     {
      //Serial.println(pressureValue);
      
      lastPressureValue = pressureValue;
      bowPressureValue = map(pressureValue, 200, 700, 0, 100) / 1000.0;
      Serial.println(bowPressureValue);
      
     }
    }

    bowedViolin.setParamValue("pressure", bowPressureValue);
  }
}
