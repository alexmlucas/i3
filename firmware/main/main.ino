
#include <Trill.h>
#include<Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include "Violin.h"

Trill trillSensors[4];
int trillAddresses[] = {0x23, 0x22, 0x21, 0x20};
const int NUM_TRILL_SENSORS = 4;
boolean touchFlags[] = {false, false, false, false};
boolean lastTouchFlags[] = {false, false, false, false}; 
int touchLocations[] = {-1, -1, -1, -1};

const char* freqParamNames[] = {"freq0", "freq1", "freq2", "freq3"};
const char* velocityParamNames[] = {"velocity0", "velocity1", "velocity2", "velocity3"};
const char* pluckTriggerParamNames[] = {"pluckTrigger0", "pluckTrigger1", "pluckTrigger2", "pluckTrigger3"};
const char* pluckGainParamNames[] = {"pluckGain0", "pluckGain1", "pluckGain2", "pluckGain3"};

int maxTouchSizes[] = {-1, -1, -1, -1};
const int PRESSURE_THRESHOLD = 400;
const int PRESSURE_NOISE_FILTER = 10;
elapsedMillis sensorReadTimer;

boolean pluckResetFlags[] = {false, false, false, false};
elapsedMillis pluckResetTimers[] = {0, 0, 0, 0};

float minStringFreqs[] = {196.00, 293.67, 440.00, 659.46};                // G3, D4, A4, E5 
float maxStringFreqs[] = {261.63, 392, 587.33, 987.77};                   // C4, G4, D5, B5

const int LED_PIN = 9;

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
AudioConnection          patchCord2(amp1, 0 , i2s1, 0);
AudioConnection          patchCord3(amp1, 0 , i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;

void setup()
{
  AudioMemory(2);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);
  sgtl5000_1.lineOutLevel(13);
  amp1.gain(0.9);
  
  Serial.begin(9600);
  pinMode(pressureSensorPin, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  for(int i = 0; i < NUM_TRILL_SENSORS; i++)
  {
    trillSensors[i].begin(Trill::TRILL_BAR, trillAddresses[i]);
    trillSensors[i].setPrescaler(4); // a value from 1-8
    //trillSensors[i].setThreshold(2); // a value from 0 - 255
  }

  digitalWrite(LED_PIN, HIGH);

  delay(1000);

  Serial.println("we're running");
}

void loop() 
{
  
  if(touchReadTimer > 10)                                                                       // read 20 times per second.
  {
    // ### * CHECK FOR TOUCHES * ###
    
    for(int i = 0; i < NUM_TRILL_SENSORS; i++)                                 
    {
      trillSensors[i].read();

      if(trillSensors[i].getNumTouches() > 0)
      {
        Serial.print("Touch at Trill address ");
        Serial.print(trillAddresses[i], HEX);
        Serial.print(" = ");
        Serial.println(trillSensors[i].touchLocation(0));

        touchFlags[i] = true;
        touchLocations[i] = trillSensors[i].touchLocation(0);
        
        if(touchLocations[i] >= 2880)                                   // if touch within first 10 cm if strip, assign lowest freq
        {
          violin.setParamValue(freqParamNames[i], minStringFreqs[i]);   
        } else if (touchLocations[i] <= 320)                            // if touch within last 10 cm if strip, assign highest freq
        {
          violin.setParamValue(freqParamNames[i], maxStringFreqs[i]);  
        } else                                                          // otherwise, just assign mapped freq
        {
          violin.setParamValue(freqParamNames[i], map(touchLocations[i], 320, 2880, maxStringFreqs[i], minStringFreqs[i]));   
        }
        
        int currentTouchSize = constrain(trillSensors[i].touchSize(0), 0, 6000);                                            // get a constrained version of the touch size

        if(currentTouchSize > maxTouchSizes[i])                                                                             // get the largest touch size during this touch event.
        {
          maxTouchSizes[i] = currentTouchSize;
          //Serial.println(maxTouchSizes[i]);                            
        }
        
      } else                                                                                    // no touch is present so reset variables
      {
        touchFlags[i] = false;
      }
    }

    // ### * CHECK FOR PRESSURE * ###
    
    int pressureValue = constrain(analogRead(pressureSensorPin), 0, 1000);       // read the pressure sensor
    //int pressureValue = analogRead(pressureSensorPin);       // read the pressure sensor

    //Serial.println(pressureValue);

    if(pressureValue > PRESSURE_THRESHOLD)                                      // is pressure being applied?
    {      
      pressureWasApplied = true;
     
      if(pressureValue > (lastPressureValue + PRESSURE_NOISE_FILTER) | pressureValue < (lastPressureValue - PRESSURE_NOISE_FILTER) | pressureValue == 1000)      // filter noise
      {  
        lastPressureValue = pressureValue;                                      // store value for next iteration.
        bowPressureValue = pressureValue / 1000.0;                              // map to a value appropriate for bow pressure

        //Serial.println(bowPressureValue);
        
        for(int i = 0; i < NUM_TRILL_SENSORS; i++)
        {        
          if(touchFlags[i] == true)
          {  
            violin.setParamValue(velocityParamNames[i], bowPressureValue);      // apply pressure if the touch point is active.
          } else 
          {
            violin.setParamValue(velocityParamNames[i], 0);                     // otherwise set pressure to zero.
          }
        }
      }
    } else                                                                      
    { 
      for(int i = 0; i < NUM_TRILL_SENSORS; i++)                                // set all to zero - to catch any events falling outside 50ms timer
      {
        violin.setParamValue(velocityParamNames[i], 0);
      }
    }

    // ### * CHECK FOR TOUCH Release * ###

    for(int i = 0; i < NUM_TRILL_SENSORS; i++)
    {
      if(touchFlags[i] == false && lastTouchFlags[i] == true)                   // pluck if a transition from true to false has occured and no pressure has been applied.
      {
        Serial.println("release");

        if(pressureWasApplied == false)                    
        {
          Serial.println("no pressure applied");
          String pluckGain = "pluckGain";
          String pluckTrigger = "pluckTrigger";

          pluckGain += i;
          pluckTrigger += i;

          float gainValue = map(maxTouchSizes[i], 0, 6000, 0, 100) / 100.0;

          Serial.print("gainValue = ");
          Serial.println(gainValue);

          violin.setParamValue(pluckGainParamNames[i], 0.9);                    
          violin.setParamValue(pluckTriggerParamNames[i], 1);

          pluckResetFlags[i] = true;
          pluckResetTimers[i] = 0;
        }

        maxTouchSizes[i] = 0;
      }
    }

    if(!touchFlags[0] && !touchFlags[1] && !touchFlags[2] && !touchFlags[3])
    {
      pressureWasApplied = false;                                             // reset (only reset if all touches have been released)
    }
    

    for(int i = 0; i < NUM_TRILL_SENSORS; i++)                                // update lastTouchFlag[] states.
    {
      lastTouchFlags[i] = touchFlags[i];
    }

    touchReadTimer = 0;
  }

  for(int i = 0; i < NUM_TRILL_SENSORS; i++)                                    // needed to switch off pluck 0/1 switch
  {
    if(pluckResetFlags[i])
    {
      if(pluckResetTimers[i] > 10)
      {
        violin.setParamValue(pluckTriggerParamNames[i], 0);
        pluckResetFlags[i] = false;
      }
    }    
  }
}
