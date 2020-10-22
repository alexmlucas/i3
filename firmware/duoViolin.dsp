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
pressure0 = hslider("pressure0", 0, 0, 1, 0.01) : ba.lin2LogGain : si.smooth(0.999);
freq1 = hslider("freq1", 660, 660, 1100, 0.01) : si.smoo;
pressure1 = hslider("pressure1", 0, 0, 1, 0.01) : ba.lin2LogGain : si.smooth(0.999);
freq2 = hslider("freq2", 660, 660, 1100, 0.01) : si.smoo;
pressure2 = hslider("pressure2", 0, 0, 1, 0.01) : ba.lin2LogGain : si.smooth(0.999);
freq3 = hslider("freq3", 660, 660, 1100, 0.01) : si.smoo;
pressure3 = hslider("pressure3", 0, 0, 1, 0.01) : ba.lin2LogGain : si.smooth(0.999);

pluckGain0 = hslider("pluckGain0", 0.7, 0, 1, 0.01);
pluckTrigger0 = button("pluckTrigger0");
pluckGain1 = hslider("pluckGain1", 0.7, 0, 1, 0.01);
pluckTrigger1 = button("pluckTrigger1");
pluckGain2 = hslider("pluckGain2", 0.7, 0, 1, 0.01);
pluckTrigger2 = button("pluckTrigger2");
pluckGain3 = hslider("pluckGain3", 0.7, 0, 1, 0.01);
pluckTrigger3 = button("pluckTrigger3");

process = (violinBowedModel(freq0, pressure0, pressure0, 0.75), violinPluckedModel(freq0, pluckGain0, pluckTrigger0) : + ), (violinBowedModel(freq1, pressure1, pressure1, 0.75), violinPluckedModel(freq1, pluckGain1, pluckTrigger1) : + ) : + ;


 