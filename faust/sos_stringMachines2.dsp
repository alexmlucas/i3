import("stdfaust.lib");
freq = nentry("freq",50,50,1000,0.01);
gain = nentry("gain",0.5,0,1,0.01) : si.smoo;
cutoff = nentry("cutoff",9000,50,10000,0.01) : si.smoo;
lfo1 = no.lfnoise(400) * lfo1Depth;
lfo1Depth = hslider("[1]LFO 1 Depth", 1, 0, 10, 0.01);

envelope = hgroup("[1]Envelope", en.adsr(attack, decay, sustain, release, gate))
  with{
  attack = hslider("[0]Attack[style:knob]", 900, 0, 1000, 1) * 0.001;
  decay = hslider("[1]Decay[style:knob]", 0, 0, 1000, 1) * 0.001;
  sustain = hslider("[2]Sustain[style:knob]", 0.9, 0.01, 1, 1);
  release = hslider("[3]Release[style:knob]", 600, 1, 1000, 1) * 0.001;
  gate = button("gate");
};

process = (os.sawtooth(freq + (lfo1 * lfo1Depth)) + os.sawtooth(freq - 0.2)) * envelope: fi.lowpass(3, 7000 + (freq) : si.smoo) : *(0.2) <: _,_;