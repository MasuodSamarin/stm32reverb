//Header including the saturating math functions.  Also includes signed to unsigned and unsigned to signed integer conversion
//functions.
 
#include "params.h"
#include <math.h>
 
//Header including the saturating math functions.  Also includes signed to unsigned and unsigned to signed integer conversion
//functions.
 
//saturation addition
float satSubtract(float x, float y){            //x - y
    if(x < (MIN + y))                           //if((x - y) < MIN), add y to both sides to yields if(x < (MIN + y))
        return MIN;
    if(-y > (MAX - x))                          //if((x - y) > MAX), subtract x from both sides to yield if(-y > (MAX - x)) 
        return MAX;
    return (x - y);                             //if it's not limiting then just return x - y
}
 
//saturation subtraction
float satAdd(float x, float y){                 //x + y
    if(x < (MIN - y))                           //if((x + y) < MIN)
        return MIN;
    if(x > (MAX - y))                           //if((x + y) > MAX)
        return MAX;
    return (x + y);                             //if it's not limiting then just return x + y
}
 
//signed to unsigned integer conversion
float intToFloat(int input){
    uint16_t output = 0;
    float output_f = 0;
    if(0x8000 && input){
        output = input - 0x8000;                //then it's a negative number
    }else{
        output = input + 0x8000;                //then it's a postive number
    }
    output_f = output/2.0;
    output_f -= 32768.0/2.0;                          //remove DC offsset from signed to unsigned conversion.
    output_f *= float(2.0);                                //amplify a little.   
    return output_f;
}
 
//unsigned to signed integer conversion
int unsignedToSigned(int input){
    int output = 0;
    if(input >= 0x8000)                         //then convert it to a positive signed numbber
        output = ((input >> 1) & 0x7FFF);
    if(input < 0x8000)         
        output = input + 0x8000;
    return output + 32768/2;
}
 
//these ideas taken from paulbourke.net/miscellaneous/interpolation/
//linear
float linterp(float y1, float y2, float frac){
    return(y1*(1-frac) + y2*frac);
}
//cosine
/*   no cos function
float cosinterp(float y1, float y2, float frac){
    float frac2;
    frac2 = (1 - cos(frac*float(3.14159)))/2;
    return(y1*(1-frac2) + y2*frac2);
}
*/



//1 pole lowpass filter from ear level engineering's website
#include <math.h>
class OnePoleLp {
public:
    OnePoleLp() {a0 = 1.0; b1 = 0.0; z1 = 0.0;};
    OnePoleLp(float Fc) {z1 = 0.0; setFc(Fc);};
    //~OnePole();
    void setFc(float Fc);
    float process(float in);
protected:    
    float a0, b1, z1;
};
inline void OnePoleLp::setFc(float Fc) {
    b1 = exp(-2.0 * 3.14159 * Fc);
    a0 = float(1.0) - b1;
}
inline float OnePoleLp::process(float in) {
    return z1 = in * a0 + z1 * b1;
}
//1 pole highpass filter
class OnePoleHp {
public:
    OnePoleHp() {a0 = 1.0; b1 = 0.0; z1 = 0.0;};
    OnePoleHp(float Fc) {z1 = 0.0; setFc(Fc);};
    //~OnePole();
    void setFc(float Fc);
    float process(float in);
protected:    
    float a0, b1, z1;
};
inline void OnePoleHp::setFc(float Fc) {
    b1 = -exp(-2.0 * 3.14159 * (float(0.5) - Fc));
    a0 = float(1.0) + b1;
}
inline float OnePoleHp::process(float in) {
    return z1 = in * a0 + z1 * b1;
}
 
//make a delay class
class Delay {
public:
    Delay() {
        index1 = 0;
        index2 = DELAYLEN*.99;
        for(int i = 0; i < DELAYLEN; i++){
            buffer[i] = 0;
        }
    };
    void setLength(float length);
    float process(float in, float fbin, float length);
protected:
    float buffer[DELAYLEN];
    int index1, index2, intlength;
    float fraclength, out;
};
inline float Delay::process(float in, float fbin, float length){
    //update index values
    index1++;
    index2++;
    if(index1 >= (DELAYLEN))
        index1 = 0;
    if(index2 >= (DELAYLEN))
        index2 = 0;
        
    //separate length into integer and fractional parts for interpolation
    intlength = length;
    fraclength = length - intlength;
    
    //update indices based on delay length
    index2 = index1 + intlength;
    if(index2 >= DELAYLEN)
        index2 = index1 + intlength - DELAYLEN;
     
    //write to delay buffer   
    buffer[index1] = satAdd(in,fbin);
 
    //read the delays out of the delay array.  do some crazy interpolation stuff.
    if(index2+1 < DELAYLEN){
        out = linterp(buffer[index2],buffer[index2+1],fraclength);    //worry about the converion from float to uint outside of this.
    }else{
        out = buffer[index2];                                                    //probably just want to take the output of the delay and not the dry.
    }
    return out;
}