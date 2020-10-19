import ("stdfaust.lib");

waveformGenerator = os.sawtooth(freq) 
with {
    freq = hslider("freq", 440, 50, 20000, 1);
};

lowpassFilter = fi.lowpass(2, cutoffFrequency)
with {
    cutoffFrequency = hslider("LP Cutoff Frequency", 4000, 1000, 6000, 1);
};

highpassFilter = fi.highpass(5, cutoffFrequency) 
with {
    cutoffFrequency = hslider("HP Cutoff Frequency", 300, 100, 1000, 1);
};

envelope = hgroup("[1]Envelope", en.adsr(attack, decay, sustain, release, gate))
  with{
  attack = hslider("[0]Attack[style:knob]", 900, 0, 1000, 1) * 0.001;
  decay = hslider("[1]Decay[style:knob]", 0, 0, 1000, 1) * 0.001;
  sustain = hslider("[2]Sustain[style:knob]", 0.9, 0.01, 1, 1);
  release = hslider("[3]Release[style:knob]", 300, 1, 1000, 1) * 0.001;
  gate = button("gate");
};

emph(fx) = fi.peak_eq_cq(6.0, fx, 20);

process = waveformGenerator : lowpassFilter : emph(300) : emph(700) : emph(4000) : highpassFilter : *(envelope) : *(0.25) <:_,_;