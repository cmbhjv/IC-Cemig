/*
 * io.h
 *
 *  Created on: 13/05/2015
 *      Author: Joao
 */

#ifndef IO_H_
#define IO_H_

#include "DSP28x_Project.h"
#include "sinais.h"
#include "pwm.h"
#include "adc.h"
#include "parametros.h"


void configuraGPIO(void);


__interrupt void start_isr(void);
__interrupt void timer0_isr(void);
__interrupt void fault_isr(void);

extern volatile char led_estado[4];

#endif /* IO_H_ */
