import("stdfaust.lib");

waveformGenerator = os.sawtooth(freq + lfo1) + os.sawtooth(freq - 0.2): *(0.5)
with {
    freq = hslider("freq", 440, 50, 20000, 1);
    lfo1 = no.lfnoise(300) * lfo1Depth;
    lfo1Depth = hslider("[1]LFO 1 Depth", 1, 0, 10, 0.01);
};

envelope = hgroup("[1]Envelope", en.adsr(attack, decay, sustain, release, gate))
  with{
  attack = hslider("[0]Attack[style:knob]", 900, 0, 1000, 1) * 0.001;
  decay = hslider("[1]Decay[style:knob]", 0, 0, 1000, 1) * 0.001;
  sustain = hslider("[2]Sustain[style:knob]", 0.9, 0.01, 1, 1);
  release = hslider("[3]Release[style:knob]", 600, 1, 1000, 1) * 0.001;
  gate = button("gate");
};

process = waveformGenerator * envelope /*fi.lowpass(4, envelope * 18000) */: *(0.3) <: _,_;
