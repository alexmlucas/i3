#ifndef PITCH_DETECTOR
#define PITCH_DETECTOR
#include <Arduino.h>

class Pitch_Detector
{
  private:
    bool m_pitchSwitches[18];
    int m_pitchHz[18];

  public:
    Pitch_Detector::Pitch_Detector();
    int getStringPitch(bool* touchSwitches, int firstTouchIndex, int lastTouchIndex);
    void resetPitchSwitches();
    int getLowestPitchIndex();
    bool stringIsPressed(bool* touchSwitches, int firstTouchIndex, int lastTouchIndex);
};

#endif
