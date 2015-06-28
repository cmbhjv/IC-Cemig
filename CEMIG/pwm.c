/*
 * pwm1.c
 *
 *  Created on: 23/03/2015
 *      Author: Joao
 */


#include "pwm.h"

#pragma DATA_SECTION(sine_table,"IQmathTables");
long sine_table[512];

volatile unsigned count = 0;
volatile float divisor=0;

void pwm1Setup() {
	InitEPwm1Gpio(); //Habilita GPIO pro PWM1
	pwmDisable();

	EPwm1Regs.TBPRD = PERIODO_PWM;       // Periodo
	EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // Fase 0
	EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
	EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;

	// Setup shadow register load on ZERO
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

	//ISR
	EALLOW;
	PieVectTable.EPWM1_INT = &pwm_isr; //função de isr
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1; //habilita interrupção 3.1 (PWMA)
	EDIS;
	// This is needed to disable write to EALLOW protected registers
	IER |= M_INT3;

	//ISR PWM
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
	EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
	EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 3rd event

	// Set actions
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM1A on Zero
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;     // Clear PWM1A on event A, up count

	EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;   // Clear PWM1B on Zero
	EPwm1Regs.AQCTLB.bit.CAU = AQ_SET;     // Set PWM1B on event A, up count

	EPwm1Regs.CMPA.half.CMPA = 0;

	// Habilita disparo do ADC
	EPwm1Regs.ETSEL.bit.SOCAEN = 1;        // Enable SOC on A group
	EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA;       // Select SOC from from CPMA on upcount
	EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;        // Generate pulse on 1st event

	// Deadband
	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm1Regs.DBRED = DEAD_BAND;
	EPwm1Regs.DBFED = 0;


}

void pwm2Setup() {
	InitEPwm2Gpio(); //Habilita GPIO pro PWM3

	EPwm2Regs.TBPRD = PERIODO_PWM;       // Periodo
	EPwm2Regs.TBPHS.half.TBPHS = 0x0000;       // Fase 0
	EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
	EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
	EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
	EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;	// Usar shadow registers
	EPwm2Regs.ETSEL.bit.INTEN = 0;                // Disabilitar INT

	// Setup shadow register load on ZERO
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

	// Set actions
	EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM2A on Zero
	EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;     // Clear PWM2A on event A, up count

	EPwm2Regs.AQCTLB.bit.ZRO = AQ_CLEAR;   // Clear PWM2B on Zero
	EPwm2Regs.AQCTLB.bit.CAU = AQ_SET;     // Set PWM2B on event A, up count

	EPwm2Regs.CMPA.half.CMPA = 0;

	//Deadband
	EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm2Regs.DBRED = DEAD_BAND;
	EPwm2Regs.DBFED = 0;

}

void pwm3Setup() {
	InitEPwm3Gpio(); //Habilita GPIO pro PWM3

	EPwm3Regs.TBPRD = PERIODO_PWM;       // Periodo
	EPwm3Regs.TBPHS.half.TBPHS = 0x0000;       // Fase 0
	EPwm3Regs.TBCTR = 0x0000;                  // Clear counter
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
	EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
	EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
	EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;	// Usar shadow registers
	EPwm3Regs.ETSEL.bit.INTEN = 0;                // Disabilitar INT

	// Setup shadow register load on ZERO
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

	// Set actions
	EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM3A on Zero
	EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;     // Clear PWM3A on event A, up count

	EPwm3Regs.AQCTLB.bit.ZRO = AQ_CLEAR;   // Clear PWM3B on Zero
	EPwm3Regs.AQCTLB.bit.CAU = AQ_SET;     // Set PWM3B on event A, up count

	EPwm3Regs.CMPA.half.CMPA = 0;

	//Deadband
	EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm3Regs.DBRED = DEAD_BAND;
	EPwm3Regs.DBFED = 0;
}

void configuraPWM() {
	pwmDisable();
	pwm1Setup();
	pwm2Setup();
	pwm3Setup();
}

void pwmEnable() {
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}

void pwmDisable() {

	//Duty cycles para 0
	EPwm1Regs.CMPA.half.CMPA = 0;
	EPwm2Regs.CMPA.half.CMPA = 0;
	EPwm3Regs.CMPA.half.CMPA = 0;


	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;
}

void pwm1Set(unsigned int duty) {
	EPwm1Regs.CMPA.half.CMPA = duty;
}

void pwm2Set(unsigned int duty) {
	EPwm2Regs.CMPA.half.CMPA = duty;
}

void pwm3Set(unsigned int duty) {
	EPwm3Regs.CMPA.half.CMPA = duty;
}

__interrupt void pwm_isr(void) {

	unsigned va = (unsigned int)( (sine_table[count]>>18) * divisor + (PERIODO_PWM >> 1) );
	unsigned vb = (unsigned int)( (sine_table[(count + 171) % 512]>>18) * divisor + (PERIODO_PWM >> 1) );
	unsigned vc = (unsigned int)( (sine_table[(count + 341) % 512]>>18) * divisor + (PERIODO_PWM >> 1) );

	pwm1Set(va);
	pwm2Set(vb);
	pwm3Set(vc);

	count = (count + 2) % 512;

	// Clear INT flag for this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

	// Acknowledge this interrupt to receive more interrupts from group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}
