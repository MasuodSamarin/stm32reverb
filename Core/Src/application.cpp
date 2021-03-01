#include "application.hpp"

#include "wm8731.h"
#include "dsp.h"
#include "osc.h"
#include "allpass.h"

extern "C" {


float modL = 0, modL2 = 0, modL3 = 0, modL4, modR = 0, modR2 = 0, modR3 = 0, modR4, 
modLos = 0, modL2os = 0, modL3os = 0, modL4os = 0, modRos = 0, modR2os = 0, modR3os = 0, modR4os = 0;

float upperLim, lowerLim;

float feedback = 0.9;

float left_out_1 = 0, right_out_1 = 0, left_out_2 = 0, right_out_2 = 0, left_out_3 = 0, right_out_3 = 0, loopfbL = 0, loopfbR = 0;

//OnePoleLp filter, filter2;

OnePoleLp feedbackFilter;
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

    //initialize fixed filters
    double feedbackCutoff = 10.0/(SAMPLINGFREQ*1000);       //adjust these cutoffs according to taste.
    feedbackFilter.setFc(feedbackCutoff);
    double lengthCutoff = 3.0/(SAMPLINGFREQ*1000);
    lengthFilter.setFc(lengthCutoff);

    //initize oscillators and mod offsets
    modLos = DELAYLEN*.24;
    modL2os = DELAYLEN*.33;
    modL3os = DELAYLEN*.43; 
    modL4os = DELAYLEN*.57;
    OscillatorL.setF(0.53,DELAYLEN/76);
    OscillatorL2.setF(2.93,DELAYLEN/195);
    OscillatorL3.setF(0.65,DELAYLEN/88);
    OscillatorL4.setF(0.332,DELAYLEN/78);
    //right delay mod
    modRos = DELAYLEN*.25;
    modR2os = DELAYLEN*.39;
    modR3os = DELAYLEN*.43;
    modR4os = DELAYLEN*.57;
    OscillatorR.setF(0.41,DELAYLEN/79);
    OscillatorR2.setF(2.92,DELAYLEN/197);
    OscillatorR3.setF(0.64,DELAYLEN/98);
    OscillatorR4.setF(0.33,DELAYLEN/78);

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
    
    modL =  modLos +  OscillatorL.process(1);
    modL2 = modL2os + OscillatorL2.process(1);
    modL3 = modL3os + OscillatorL3.process(1);
    modL4 = modL4os + OscillatorL3.process(0);
    //right delay mod
    modR =  modRos +  OscillatorR.process(0);
    modR2 = modR2os + OscillatorR2.process(0);
    modR3 = modR3os + OscillatorR3.process(1);
    modR4 = modR4os + OscillatorR4.process(1);

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

    left_out_1 = delayLeft2.process(satAdd(inL_f,loopfbL),.5,modL);
    left_out_2 = delayLeft3.process(left_out_1,.4,modL2);
    left_out_3 = delayLeft4.process(left_out_2,.2,modL3);
    outL_f = delayLeft.process(left_out_3,.2,modL4);
    right_out_1 = delayRight2.process(satAdd(inR_f,loopfbR),.5,modR);
    right_out_2 = delayRight3.process(right_out_1,.4,modR2);
    right_out_3 = delayRight4.process(right_out_2,.2,modR3);
    outR_f = delayRight.process(right_out_3,.2,modR4);

    loopfbL = outL_f*feedback;                     //swizzle the feedbacks left to right
    loopfbR =  outR_f*feedback;      

    outL_f = satAdd(outL_f/2,inL_f);
    outR_f = satAdd(outR_f/2,inR_f);

    //out1_f = filter.process(in1_f);
    //out2_f = filter2.process(in2_f);

    //end of dsp loop////////////////////////////////////////////////////////////////////////////////////////////
    
    *out1 = unsignedToSigned(outL_f);
    *out2 = unsignedToSigned(outR_f);
}

}
