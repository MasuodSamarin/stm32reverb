//This header file includes the initialization of the codec.  Note that it uses the constant SAMPLINGFREQ to choose
//how to initialize the sampling frequency register.

#include "params.h"

#define WM8731_addr     (0x1a)
 
//taken from main.c in the private variables section
I2C_HandleTypeDef hi2c1;

//I2C i2c(I2C_SDA,I2C_SCL);
//Serial pc(SERIAL_TX, SERIAL_RX); 
 
uint8_t data_write[2];
int status = 0;

void i2c_write(uint8_t* data){
    HAL_I2C_Master_Transmit(&hi2c1,WM8731_addr<<1,data,2,100);
};
 
void codecInit(void){
    //initialization sequence taken from arduino library for open music labs codec shield.  works with both the 
    //mikroelektronika codec board and the openmusiclabs audio codec shield.  They're based on the same codec so that's good.
    
    data_write[0] = 0x0c;   //power reduction register
    data_write[1] = 0x60;   //turn everything on
    i2c_write(data_write);
 
    data_write[0] = 0x0e;   //digital data format
    data_write[1] = 0x01;   //MSB left justified, 16b, slave
    i2c_write(data_write);
 
    data_write[0] = 0x00;   //left in setup register
    data_write[1] = 0x17;   //
    i2c_write(data_write);
 
    data_write[0] = 0x02;   //right in setup register
    data_write[1] = 0x17;   //
    i2c_write(data_write);
 
    data_write[0] = 0x04;   //left hp setup register
    data_write[1] = 0x70;   //
    i2c_write(data_write);
 
    data_write[0] = 0x06;   //right hp setup register
    data_write[1] = 0x70;   //
    i2c_write(data_write);
 
    data_write[0] = 0x0a;   //digital audio path
    data_write[1] = 0x00;
    i2c_write(data_write);
 
    data_write[0] = 0x08;   //analog audio config
    data_write[1] = (0 << 6)|(0 << 5)|(1 << 4)|(0 << 3)|(0 << 2)|(1 << 1)|(0 << 0);   //from the open music labs library
    i2c_write(data_write);
   
    data_write[0] = 0x10;   //clock config
    if(SAMPLINGFREQ == 22)
        data_write[1] = 0x00;     //22khz
    if(SAMPLINGFREQ == 29)
        data_write[1] = 0x22;   //29khz sampling frequency    changed from a2 to 22
    if(SAMPLINGFREQ == 44)
        data_write[1] = 0xa0;   //44.1khz sampling frequency. changed from a0 to 20
    if(SAMPLINGFREQ == 88)
        data_write[1] = 0xbc;   //88khz?
    i2c_write(data_write);
 
    data_write[0] = 0x12;   //enable codec
    data_write[1] = 0x01;   //
    i2c_write(data_write);
 
}