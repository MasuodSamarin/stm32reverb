#include "application.hpp"

#include "wm8731.h"
#include "dsp.h"
#include "osc.h"
#include "allpass.h"

extern "C" {


float modL = 0, modL2 = 0, modL3 = 0, modL4, modR = 0, modR2 = 0, modR3 = 0, modR4 = 0, 
modLos = 0, modL2os = 0, modL3os = 0, modL4os = 0, modRos = 0, modR2os = 0, modR3os = 0, modR4os = 0;

float left_in = 0, right_in = 0;

float upperLim, lowerLim;

float feedback = 0.92;

float left_out_1 = 0, right_out_1 = 0, left_out_2 = 0, right_out_2 = 0, left_out_3 = 0, right_out_3 = 0, loopfbL = 0, loopfbR = 0;

float left_mix = 0, right_mix = 0;

//OnePoleLp filter, filter2;

OnePoleLp aliasFilterL;
OnePoleLp aliasFilterR;
OnePoleLp dampingFilterL;
OnePoleLp dampingFilterR;
OnePoleLp lengthFilter;
OnePoleLp dampingFilter;
AllPass delayLeft;
AllPass delayLeft2;
AllPass delayLeft3;
AllPass delayLeft4;
AllPass delayRight;
AllPass delayRight2;
AllPass delayRight3;
AllPass delayRight4;
gsOsc OscillatorL;
gsOsc OscillatorL2;
gsOsc OscillatorL3;
gsOsc OscillatorL4;
gsOsc OscillatorR;
gsOsc OscillatorR2;
gsOsc OscillatorR3;
gsOsc OscillatorR4;



void setup() {

    for(int i = 0 ; i < 4; i++){
        rxBuf[i] = 0;
        txBuf[i] = 0;
    }

    //initialize fixed filters
    double aliasCutoff = 15000.0/(SAMPLINGFREQ*1000);       //adjust these cutoffs according to taste.
    aliasFilterL.setFc(aliasCutoff);
    aliasFilterR.setFc(aliasCutoff);
    double dampingCutoff = 9000.0/(SAMPLINGFREQ*1000);
    dampingFilterL.setFc(dampingCutoff);
    dampingFilterR.setFc(dampingCutoff);
    double lengthCutoff = 3.0/(SAMPLINGFREQ*1000);
    lengthFilter.setFc(lengthCutoff);

    //initize oscillators and mod offsets
    modLos = DELAYLEN*.74;
    modL2os = DELAYLEN*.63;
    modL3os = DELAYLEN*.43; 
    modL4os = DELAYLEN*.27;
    OscillatorL.setF(0.53,DELAYLEN/146);
    OscillatorL2.setF(0.93,DELAYLEN/395);
    OscillatorL3.setF(0.65,DELAYLEN/188);
    OscillatorL4.setF(0.332,DELAYLEN/128);
    //right delay mod
    modRos = DELAYLEN*.75;
    modR2os = DELAYLEN*.69;
    modR3os = DELAYLEN*.42;
    modR4os = DELAYLEN*.27;
    OscillatorR.setF(0.41,DELAYLEN/149);
    OscillatorR2.setF(0.92,DELAYLEN/397);
    OscillatorR3.setF(0.64,DELAYLEN/198);
    OscillatorR4.setF(0.33,DELAYLEN/128);

    upperLim = 0.9*DELAYLEN;
    lowerLim = 0.05*DELAYLEN;

    //filter.setFc(0.01);
    //filter2.setFc(0.01);

    codecInit();

}

void do_process(uint16_t* in1, uint16_t* in2, uint16_t* out1, uint16_t* out2) {
    float inL_f = intToFloat(*in1);
    float inR_f = intToFloat(*in2);
    float outL_f = 0.0;
    float outR_f = 0.0;

    //dsp loop/////////////////////////////////////////////////////////////////////////////////////
    ///*
    modL =  modLos +  OscillatorL.process(1);
    modL2 = modL2os + OscillatorL2.process(1);
    modL3 = modL3os + OscillatorL3.process(1);
    modL4 = modL4os + OscillatorL3.process(0);
    //right delay mod
    modR =  modRos +  OscillatorR.process(0);
    modR2 = modR2os + OscillatorR2.process(0);
    modR3 = modR3os + OscillatorR3.process(1);
    modR4 = modR4os + OscillatorR4.process(1);
    //*/
    if(modL > upperLim)
        modL = upperLim;
    if(modL < lowerLim)
        modL = lowerLim;
    if(modL2 > upperLim)
        modL2 = upperLim;
    if(modL2 < lowerLim)
        modL2 = lowerLim;
    if(modL3 > upperLim)
        modL3 = upperLim;
    if(modL3 < lowerLim)
        modL3 = lowerLim;
    if(modL4 > upperLim)
        modL4 = upperLim;
    if(modL4 < lowerLim)
        modL4 = lowerLim;
    if(modR > upperLim)
        modR = upperLim;
    if(modR < lowerLim)
        modR = lowerLim;
    if(modR2 > upperLim)
        modR2 = upperLim;
    if(modR2 < lowerLim)
        modR2 = lowerLim;
    if(modR3 > upperLim)
        modR3 = upperLim;
    if(modR3 < lowerLim)
        modR3 = lowerLim;
    if(modR4 > upperLim)
        modR4 = upperLim;
    if(modR4 < lowerLim)
        modR4 = lowerLim;

    left_in = aliasFilterL.process(inL_f);
    right_in = aliasFilterR.process(inR_f);

    left_out_1 = delayLeft2.process(satAdd(left_in,loopfbL),.5,modL);
    left_out_2 = delayLeft3.process(left_out_1,.5,modL2);
    left_out_3 = delayLeft4.process(left_out_2,.5,modL3);
    outL_f = delayLeft.process(left_out_3,.5,modL4);
    right_out_1 = delayRight2.process(satAdd(right_in,loopfbR),.5,modR);
    right_out_2 = delayRight3.process(right_out_1,.5,modR2);
    right_out_3 = delayRight4.process(right_out_2,.5,modR3);
    outR_f = delayRight.process(right_out_3,.5,modR4);

    loopfbL = dampingFilterL.process(outL_f*feedback);                     //swizzle the feedbacks left to right
    loopfbR = dampingFilterR.process(outR_f*feedback);

    left_mix = satAdd(left_out_3/2,left_out_2/4);
    right_mix = satAdd(right_out_3/2,right_out_2/4);

    outL_f = satAdd(outL_f,left_mix);
    outR_f = satAdd(outR_f,right_mix);

    //out1_f = filter.process(in1_f);
    //out2_f = filter2.process(in2_f);

    //end of dsp loop////////////////////////////////////////////////////////////////////////////////////////////
    
    *out1 = unsignedToSigned(outL_f);
    *out2 = unsignedToSigned(outR_f);
}

}
