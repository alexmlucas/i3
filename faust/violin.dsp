import ("stdfaust.lib");
import ("physmodels.lib");

violinPluckedModel(stringFreq, pluckGain, pluckTrigger) = pm.endChain(modelChain)
with
{
    trigger = pm.pluckString(stringL, 1, 1, 0.75, pluckGain, pluckTrigger);

    stringTuning = 0.08;
    stringL = pm.f2l(stringFreq) - stringTuning;
    modelChain = pm.chain(
        violinNuts : 
        pm.openString(stringL, 0.25, 0.25, trigger) :
        violinBridge :
        violinBody :
        out
    );
};

violinBowedModel(stringFreq, bowPressure, bowVelocity, bowPosition) = pm.endChain(modelChain)
with
{
    stringTuning = 0.08;
    stringL = pm.f2l(stringFreq) - stringTuning;
    modelChain = pm.chain(
        violinNuts : violinBowedString(stringL, bowPressure, bowVelocity, bowPosition) :
        violinBridge :
        violinBody :
        out
    );
};

freq0 = hslider("freq0", 660, 660, 1100, 0.01) : si.smoo;
velocity0 = hslider("velocity0", 0, 0, 0.1, 0.01) : si.smooth(0.999);
freq1 = hslider("freq1", 660, 660, 1100, 0.01) : si.smoo;
velocity1 = hslider("velocity1", 0, 0, 0.1, 0.01) : si.smooth(0.999);
freq2 = hslider("freq2", 660, 660, 1100, 0.01) : si.smoo;
velocity2 = hslider("velocity2", 0, 0, 0.1, 0.01) : si.smooth(0.999);
freq3 = hslider("freq3", 660, 660, 1100, 0.01) : si.smoo;
velocity3 = hslider("velocity3", 0, 0, 0.1, 0.01) : si.smooth(0.999);

pluckGain0 = hslider("pluckGain0", 0.7, 0, 1, 0.01);
pluckTrigger0 = button("pluckTrigger0");
pluckGain1 = hslider("pluckGain1", 0.7, 0, 1, 0.01);
pluckTrigger1 = button("pluckTrigger1");
pluckGain2 = hslider("pluckGain2", 0.7, 0, 1, 0.01);
pluckTrigger2 = button("pluckTrigger2");
pluckGain3 = hslider("pluckGain3", 0.7, 0, 1, 0.01);
pluckTrigger3 = button("pluckTrigger3");

process = ((violinBowedModel(freq0, 0, velocity0, 0.75) *(velocity0) *(0.2), violinPluckedModel(freq0, pluckGain0, pluckTrigger0) *(pluckGain0): + ), 

(violinBowedModel(freq1, 0, velocity1, 0.75) *(velocity1) *(0.2), violinPluckedModel(freq1, pluckGain1, pluckTrigger1) *(pluckGain1): + ) : + ), 

((violinBowedModel(freq2, 0, velocity2, 0.75) *(velocity2) *(0.2), violinPluckedModel(freq2, pluckGain2, pluckTrigger2) *(pluckGain2): + ), 

(violinBowedModel(freq3, 0, velocity3, 0.75) *(velocity3) *(0.2), violinPluckedModel(freq3, pluckGain3, pluckTrigger3) *(pluckGain3): + ) : + ) : +;