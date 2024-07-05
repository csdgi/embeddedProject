#ifndef __ADC_H
#define __ADC_H



#include "main.h"

void APP_ADCConfig(void);
void APP_Tim1Config(void);

void adc_ch_change(uint8_t channel);
float adc_vref(void);

#endif


