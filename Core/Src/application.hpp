#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

uint16_t rxBuf[4];
uint16_t txBuf[4];

void setup();
void do_process(uint16_t* in1, uint16_t* in2, uint16_t* out1, uint16_t* out2);

#ifdef __cplusplus
}
#endif