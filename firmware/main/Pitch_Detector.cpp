#include "Pitch_Detector.h"

Pitch_Detector::Pitch_Detector()
{
  for(int i = 0; i < 18; i++)
  {
    m_pitchSwitches[i] = 0; 
  }

  m_pitchHz[0] = 100;
  m_pitchHz[1] = 200;
  m_pitchHz[2] = 300;
  m_pitchHz[3] = 400;
  m_pitchHz[4] = 500;
  m_pitchHz[5] = 600;
  m_pitchHz[6] = 700;
  m_pitchHz[7] = 800;
  m_pitchHz[8] = 900;
  m_pitchHz[9] = 1000;
  m_pitchHz[10] = 1100;
  m_pitchHz[11] = 1200;
  m_pitchHz[12] = 1300;
  m_pitchHz[13] = 1400;
  m_pitchHz[14] = 1500;
  m_pitchHz[15] = 1600;
  m_pitchHz[16] = 1700;
  m_pitchHz[17] = 1800;
}

int Pitch_Detector::getStringPitch(bool* touchSwitches, int firstTouchIndex, int lastTouchIndex)
{
  resetPitchSwitches();

  for(int i = firstTouchIndex; i <= lastTouchIndex; i++)
  {
    int pitchSwitchesIndex = (i - firstTouchIndex) * 2;     // get a new index, used when writing values to pitchSwitches
    
    if(touchSwitches[i] == true)                            // is the touch switch active?
    {
      m_pitchSwitches[pitchSwitchesIndex] = true;           // set the corresponding value in the pitchSwitch array

      if(i != lastTouchIndex)                               // filter lastTouchIndex, this is a special case.
      {
        if(touchSwitches[i+1] == true)                      // is the touchSwitch to the RH side active?
        {
          m_pitchSwitches[pitchSwitchesIndex] = false;      // reset the previous switch.
          m_pitchSwitches[pitchSwitchesIndex + 1] = true;   // set a quartertone.
        }  
      }
    }
  }

  int pitchIndex = getLowestPitchIndex();

  if(pitchIndex != -1)
  {
    return m_pitchHz[pitchIndex];
  } else 
  {
    return pitchIndex;
  }

  
}

void Pitch_Detector::resetPitchSwitches()
{
  for(int i = 0; i < 18; i++)
  {
    m_pitchSwitches[i] = false;
  }
}

int Pitch_Detector::getLowestPitchIndex()
{

  int lowestPitchIndex = -1;
  
  for(int i = 0; i < 18; i++)
  {
    if(m_pitchSwitches[i] == true)
    {
      lowestPitchIndex = i;
      break; 
    }
  }

  return lowestPitchIndex;
}

bool Pitch_Detector::stringIsPressed(bool* touchSwitches, int firstTouchIndex, int lastTouchIndex)
{
  bool stringPressedFlag = false;
  
  for(int i = firstTouchIndex; i <= lastTouchIndex; i++)
  {
    if(touchSwitches[i] == true)
    {
      stringPressedFlag = true; 
    }
  }

  return stringPressedFlag;
}
