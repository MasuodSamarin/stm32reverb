//make a delay class
class AllPass {
public:
    AllPass() {
        index1 = 0;
        index2 = DELAYLEN*.99;
        //for(int i = 0; i <= DELAYLEN; i++){
        for(int i = 0; i < DELAYLEN; i++){
            buffer[i] = 0;
        }
        fraclength = 0.0;
        buffout = 0.0;
        buffin = 0.0;
        out = 0.0;
    };
    void setLength(float length);
    float process(float in, float fbin, float length);
protected:
    float buffer[DELAYLEN];
    int index1, index2, intlength;
    float fraclength, buffout, buffin, out;
};
inline float AllPass::process(float in, float feedback, float length){
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
    buffin = satAdd(in,buffout*feedback);
    buffer[index1] = buffin;
 
    //read the delays out of the delay array.  do some crazy interpolation stuff.
    if(index2+1 < DELAYLEN){
        buffout = linterp(buffer[index2],buffer[index2+1],fraclength);    //worry about the converion from float to uint outside of this.
    }else{
        buffout = buffer[index2];                                                    //probably just want to take the output of the delay and not the dry.
    }
    
    out = satSubtract(buffout,buffin*feedback);
    
    return out;
}