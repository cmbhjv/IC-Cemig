/*
 * io.c
 *
 *  Created on: 13/05/2015
 *      Author: Joao
 */

#include "io.h"


/**
 * Led_estado:
 * 0 - LED Amarelo
 * 1 - LED Azul
 * 2 - LED Verde
 * 3 - Led Vermelho
 */

volatile char led_estado[4] = {0,0,0,0};

void configuraGPIO(void)
{

	//Botoes: 17/48
	//Leds: 09/11/34/49
	EALLOW;

	// Configura Saidas
	GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0; //Pullup
	GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;   // Load output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;  // Funçao IO
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;   // Saida

	GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0; //Pullup
	GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;   // Load output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;  // Funçao IO
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;   // Saida

	GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0; //Pullup
	GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;   // Load output latch
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;  // Funçao IO
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;   // Saida

	GpioCtrlRegs.GPBPUD.bit.GPIO49 = 0; //Pullup
	GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;   // Load output latch
	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;  // Funçao IO
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;   // Saida
   //


	//Configura Entradas

	//Botao Start (IO17)
	GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0; //Pullup
	GpioDataRegs.GPASET.bit.GPIO17 = 1;   //
	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;  // Funçao IO
	GpioCtrlRegs.GPADIR.bit.GPIO17 = 0;   // Entrada
	GpioIntRegs.GPIOXINT1SEL.all = 17;    // COnfigura Interrupçao na GPIO17

	//Fault ISR (IO48)
	GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0; //Pullup
	GpioDataRegs.GPBSET.bit.GPIO48 = 1;   //
	GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;  // Funçao IO
	GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0;   // Entrada
	GpioIntRegs.GPIOXINT2SEL.all = 48;    // COnfigura Interrupçao na GPIO17

	//

	PieVectTable.XINT1 = &start_isr;
	PieVectTable.XINT2 = &fault_isr;
	PieVectTable.TINT0 = &timer0_isr;

	EDIS;
	IER |= M_INT1;
	PieCtrlRegs.PIEIER1.bit.INTx4 = 1; //xint1
	PieCtrlRegs.PIEIER1.bit.INTx5 = 1; //xint2
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1; //tint0

	XIntruptRegs.XINT1CR.bit.ENABLE = 1;        // Enable Xint1
	XIntruptRegs.XINT2CR.bit.ENABLE = 1; //Enable interrupt Xint2

	ConfigCpuTimer(&CpuTimer0, 150, 250000);
	CpuTimer0Regs.TCR.all = 0x4000;
}

__interrupt void fault_isr(void)
{
	pwmDisable();
	adcDisable();
	global_flags.estado = ESTADO_RAMPA;
	led_estado[1] = LED_PISCANDO;
	XIntruptRegs.XINT1CR.bit.ENABLE = 0;        // Enable Xint1
	PieCtrlRegs.PIEIER1.bit.INTx4 = 0;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void start_isr(void)
{
	adcEnable();
	pwmEnable();
	global_flags.estado = ESTADO_RAMPA;
	led_estado[1] = LED_PISCANDO;
	XIntruptRegs.XINT1CR.bit.ENABLE = 0;        // Enable Xint1
	PieCtrlRegs.PIEIER1.bit.INTx4 = 0;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void timer0_isr(void)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

	switch(led_estado[0]){
		case LED_APAGADO:
			GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
			break;
		case LED_PISCANDO:
			GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
			break;
		case LED_ACESO:
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			break;
	}

	switch(led_estado[1]){
		case LED_APAGADO:
			GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
			break;
		case LED_PISCANDO:
			GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
			break;
		case LED_ACESO:
			GpioDataRegs.GPASET.bit.GPIO11 = 1;
			break;
	}

	switch(led_estado[2]){
		case LED_APAGADO:
			GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
			break;
		case LED_PISCANDO:
			GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
			break;
		case LED_ACESO:
			GpioDataRegs.GPBSET.bit.GPIO34 = 1;
			break;
	}

	switch(led_estado[3]){
		case LED_APAGADO:
			GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
			break;
		case LED_PISCANDO:
			GpioDataRegs.GPBTOGGLE.bit.GPIO49 = 1;
			break;
		case LED_ACESO:
			GpioDataRegs.GPBSET.bit.GPIO49 = 1;
			break;
	}

}


