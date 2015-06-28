/*
 * pwm1.h
 *
 *  Created on: 23/03/2015
 *      Author: Joao
 */

#ifndef PWM_H_
#define PWM_H_

#include "DSP28x_Project.h"
#include "parametros.h"



extern volatile float divisor;

//Configuração PWM
void pwm1Setup();
void pwm2Setup();
void pwm3Setup();

void configuraPWM();

void pwmSet(unsigned int);
void pwmDisable();
void pwmEnable();

__interrupt void pwm_isr(void);

#endif /* PWM_H_ */
