#define DELAYLEN        2900                 //24,000 is about as far as you can go in the ram.
#define SAMPLINGFREQ    88                   //need to make sense of this44
#define MIN             -15000.0f            //so you don't get negative vlaues because the stuff is in unsigned format
#define MAX             15000.0f             //limit at maximum 16 bit value so it doesn't wrap around