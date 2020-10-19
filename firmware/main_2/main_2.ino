
#include <Trill.h>
#include<Audio.h>

#include "Violin.h"

// ** New Variables **

Trill trillSensors[4];
int trillAddresses[] = {0x20, 0x21, 0x22, 0x23}
const int NUM_TRILL_SENSORS = 4;
boolean touchFlags[] = {false, false, false, false};
int touchLocations[] = {-1, -1, -1, -1};
String freqString = "freq";
String pressureString = "pressure";
String pluckString = "pluck";
String pluckGainString = "pluckGain";
int maxTouchSizes[] = {-1, -1, -1, -1};
const PRESSURE_THRESHOLD = 20;
const PRESSURE_NOISE_FILTER = 10;
elaspedMillis sensorReadTimer;

// **

Violin violin;
Trill trillSensor;
elapsedMillis touchReadTimer;
elapsedMillis pluckResetTimer; 

boolean pressureWasApplied = false;
boolean touchWasPresent = false;
boolean pressureActive = false;
boolean pluckResetRequired = false;
int pressureSensorPin = A3;
int lastPressureValue = 0;
int maxTouchSize = 0;
float bowPressureValue = 0;

AudioAmplifier           amp1;          
AudioOutputI2S           i2s1;          
AudioConnection          patchCord1(violin, amp1);
AudioConnection          patchCord2(amp1, 0, i2s1, 0);
AudioConnection          patchCord3(amp1, 0, i2s1, 1);

void setup()
{
  pinMode(A3, INPUT);
  AudioMemory(2);
  amp1.gain(0.3);

  Serial.begin(115200);
  
  for(int i = 0; i < NUM_TRILL_SENSORS; i++)
  {
    trillSensors[i].begin(Trill::TRILL_BAR, trillAddresses[i]);
  }
  
  /*int ret = trillSensor.begin(Trill::TRILL_BAR);
  if(ret != 0) 
  {
    Serial.println("failed to initialise trillSensor");
    Serial.print("Error code: ");
    Serial.println(ret);
  }*/

   
}

void loop() 
{
  
  if(touchReadTimer > 50)                                                       // read 20 times per second.
  {
    for(int i = 0; i < NUM_TRILL_SENSORS; i++)                                  // get touches.
    {
      trillSensors[i].read();

      if(trillSensors[i].getNumTouches() > 0)
      {
        Serial.print("Touch at Trill address ");
        Serial.print(trillAddresses[i]);
        Serial.print(" = ");
        Serial.println(trillSensors[i].touchLocation(0));

        touchflags[i] = true;
        touchLocations[i] = trillSensors[i].touchLocation(0);                   // get the location of the first touch.
        String freq = "freq";                                                   // create a string to set freq paramter.
        freq += i;
        violin.setParamValue(freq, map(touchLocation[i], 0, 3200, 1100, 660));  // set string frequency.

        int currentTouchSize = constrain(trillSensor.touchSize(0), 0, 3000);    // get a constrained version of the touch size

        if(currentTouchSize > maxTouchSizes[i])                                 // get the largest touch size during this touch event.
        {
          maxTouchSizes[i] = currentTouchSize;                              
        }
        
      } else                                                                  // no touch is present so reset variables
      {
        touchFlags[i] = false;
        maxTouchSizes[i] = 0;
      }
    }

    int pressureValue = constrain(analogRead(pressureSensorPin), 0, 900);                                                                 // read the pressure sensor

    if(pressureValue > PRESSURE_THRESHOLD)                                                                                                // is pressure being applied?
    {
      if(pressureValue > (lastPressureValue + PRESSURE_NOISE_FILTER) || pressureValue < (lastPressureValue - PRESSURE_NOISE_FILTER))      // filter noise
      {
        pressureWasApplied = true;
        lastPressureValue = pressureValue;                                      // store value for next iteration.
        bowPressureValue = pressureValue / 1000.0;                              // map to a value appropriate for bow pressure
        
        for(int i = 0; i < NUM_TRILL_SENSORS; i++)
        {
          String pressure = "pressure";                                         // create a string to set the pressure parameter.
          pressure += i;
          
          if(touchFlags[i] == true)
          {  
            violin.setParamValue(pressure, bowPressureValue);                   // apply pressure if the touch point is active.
          } else 
          {
            violin.setParamValue(0, bowPressureValue);                          // otherwise set pressure to zero.
          }
        }
      }
    }

    // if there's no touch, but there was on the last interation then pluck. (pressure will handle itself)

    for(int i = 0; i < NUM_TRILL_SENSORS; i++)
    {
      if(touchFlags[i] == false)
      {
        if(lastTouchFlags[i] == true)
        {
          violin.setParamValue("pluck_gain", 1);                                // fixed at 1 currently.
          violin.setParamValue("pluck", 1);
        }
      }
    }

    



    



    

    // read pressure sensor here.

    
  }






    





    
    trillSensor.read();                                                       // read a value from the trill.
  
    if(trillSensor.getNumTouches() > 0)                                       // if we have a touch
    {
      touchWasPresent = true;
      int touchLocation = trillSensor.touchLocation(0);                       // get location of first touch.
      violin.setParamValue("freq", map(touchLocation, 0, 3200, 1100, 660));   // set the frequency of the violin string
      int currentTouchSize = constrain(trillSensor.touchSize(0), 0, 3000);    // get a constrained version of the touch size
      
      if(currentTouchSize > maxTouchSize)                                     // if the touch size is greater than previously logged...
      {
        maxTouchSize = currentTouchSize;                                      // ...reset the touchsize
        //Serial.println(maxTouchSize);
      }
      
      int pressureValue = constrain(analogRead(pressureSensorPin), 0, 900);   // read the pressure sensor
      
      if(pressureValue > 20)                                                 // is pressure being applied?
      {
        if(pressureValue > (lastPressureValue + 10) || pressureValue < (lastPressureValue - 10))    // filter noise
        {
          pressureWasApplied = true;
          lastPressureValue = pressureValue;                                                        // store value for next iteration.
          bowPressureValue = pressureValue / 1000.0;                         // map to a value appropriate for bow pressure
          violin.setParamValue("pressure", bowPressureValue);                                       // ...and pressure of the bowed string.
        }
        //Serial.println(pressureValue);
      }
    } else
    {
      if(touchWasPresent && pressureWasApplied)                   // bowed action just occured
      {
        touchWasPresent = false;                                  // ...simply reset everything.
        pressureWasApplied = false;
        violin.setParamValue("pressure", 0);                     
      }

      if(touchWasPresent && !pressureWasApplied)                 // execute pluck action
      {
        float pluckGain = map(maxTouchSize, 0, 3000, 0, 100) / 1000.0;                         
        violin.setParamValue("pluck_gain", 1);                  // fixed at 1 currently.
        violin.setParamValue("pluck", 1);
        pluckResetTimer = 0;
        pluckResetRequired = true;
        touchWasPresent = false;
        pressureWasApplied = false;
        maxTouchSize = 0;
      } 
    }
    
    touchReadTimer = 0;
  }
  
  if(pluckResetRequired)
  {
    if(pluckResetTimer > 10)
    {
      violin.setParamValue("pluck", 0);
      pluckResetRequired = false;
    }
  }
}
