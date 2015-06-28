/*
 * adc.h
 *
 *  Created on: 01/04/2015
 *      Author: Joao
 */

#ifndef ADC_H_
#define ADC_H_

#include "DSP28x_Project.h"
#include "sinais.h"
#include "parametros.h"

/*
 * A0 - Ias
 * A1 - Ibs
 * A2 - Ics
 * B0 - Vab
 * B1 - Vbc
 * B2 - Vca
 * B3 - Iap
 * B4 - Ibp
 * A3 - Icp
 * A4 - Bat_out
*/

void configuraADC(void);
void adcDisable(void);
void adcEnable(void);
void resetAmostras(void);
__interrupt void adc_isr(void);
#endif /* ADC_H_ */
