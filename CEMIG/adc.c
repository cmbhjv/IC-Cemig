/*
 * adc.c
 *
 *  Created on: 01/04/2015
 *      Author: Joao
 */

#include "adc.h"

volatile Uint16 buffer_index = 0;

/**
 * Configura os registradores do ADC
 */
void configuraADC(void)
{

	EALLOW;
	SysCtrlRegs.HISPCP.all = ADC_MODCLK; //HSPCLK = 150MHz/(2*ADC_MODCLK) = 25MHz
	PieVectTable.ADCINT = &adc_isr;
	EDIS;

	InitAdc();

	// Configuraçoes especificas do ADC
	AdcRegs.ADCTRL1.bit.CPS = ADC_CPS;
	AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK; //Janela de aquisição - amostrar durante 15+1 ciclos
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 0x1; //single sequencer - até 16  amostragem em sequência

	PieCtrlRegs.PIEIER1.bit.INTx6 = 1; //habilita interrupção Grupo 1 - interrupcao 6 (ADC)
	IER |= M_INT1; // Enable CPU Interrupt 1

	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0; //interrupçao qdo seq 1 terminar
//	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1; //habilita interrupçao ADC
//	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; //Habilita disparo por PWM


	AdcRegs.ADCTRL3.bit.SMODE_SEL = 0x1; //dual sampling - adquirir canal A e B em paralelo
	AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS; //clk = 25mhz


	//Seleciona Canais de conversao
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0; //A0 e B0
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1; //A1 e B1
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2; //A2 e B2
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3; //A3 e B3
	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4; //A4 e B4

	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 4;   // 10 Conversoes por SOC (disparo) | 10 = (4+1)*2

	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 0x1; // resetar sequencia

	//AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 0x1; dispara 1 conversao manual
}

/**
 * Desabilita a aquisição de dados
 */
void adcDisable()
{
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 0; //(dis)enable interrupçao ADC
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 0; //(des)Habilita disparo por PWM
}

/**
 * Habilita a aquisição de dados
 */
void adcEnable()
{
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1; //Habilita interrupçao ADC
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; //Habilita disparo por PWM
}

void resetAmostras()
{
	buffer_index = 0;
}


/**
 * Rotina de interrupção ao final da aquisição das correntes
 */
__interrupt void adc_isr(void)
{

	/* Mapa: Variável - Periférico - Sinal
	 * ADCRESULT0 - A0 - Ias
	 * ADCRESULT2 - A1 - Ibs
	 * ADCRESULT4 - A2 - Ics
	 * ADCRESULT1 - B0 - Vab
	 * ADCRESULT3 - B1 - Vbc
	 * ADCRESULT5 - B2 - Vca
	 * ADCRESULT7 - B3 - Iap
	 * ADCRESULT9 - B4 - Ibp
	 * ADCRESULT6 - A3 - Icp
	 * ADCRESULT8 - A4 - Bat_out
	*/

	//Copia valores amostrados para buffer
	sinais_amostra[buffer_index].Iap = AdcMirror.ADCRESULT7;
	sinais_amostra[buffer_index].Ibp = AdcMirror.ADCRESULT9;
	sinais_amostra[buffer_index].Icp = AdcMirror.ADCRESULT6;
	sinais_amostra[buffer_index].Vab = AdcMirror.ADCRESULT1;
	sinais_amostra[buffer_index].Vbc = AdcMirror.ADCRESULT3;
	sinais_amostra[buffer_index].Vca = AdcMirror.ADCRESULT5;
	sinais_amostra[buffer_index].Ias = AdcMirror.ADCRESULT0;
	sinais_amostra[buffer_index].Ibs = AdcMirror.ADCRESULT2;
	sinais_amostra[buffer_index].Ics = AdcMirror.ADCRESULT4;
	global_flags.v_bat += AdcMirror.ADCRESULT8<<BUFFER_SIZE_BIT;


	//Copia Amostra/2^BUFFER_SIZE_BIT pro somatorio da media;
	sinais_MEDIA.Iap += sinais_amostra[buffer_index].Iap>>BUFFER_SIZE_BIT;
	sinais_MEDIA.Ibp += sinais_amostra[buffer_index].Ibp>>BUFFER_SIZE_BIT;
	sinais_MEDIA.Icp += sinais_amostra[buffer_index].Icp>>BUFFER_SIZE_BIT;
	sinais_MEDIA.Vab += sinais_amostra[buffer_index].Vab>>BUFFER_SIZE_BIT;
	sinais_MEDIA.Vbc += sinais_amostra[buffer_index].Vbc>>BUFFER_SIZE_BIT;
	sinais_MEDIA.Vca += sinais_amostra[buffer_index].Vca>>BUFFER_SIZE_BIT;
	sinais_MEDIA.Ias += sinais_amostra[buffer_index].Ias>>BUFFER_SIZE_BIT;
	sinais_MEDIA.Ibs += sinais_amostra[buffer_index].Ibs>>BUFFER_SIZE_BIT;
	sinais_MEDIA.Ics += sinais_amostra[buffer_index].Ics>>BUFFER_SIZE_BIT;


	if(buffer_index == BUFFER_SIZE-1)
	{
		global_flags.fim_ciclo = 1;
	}

	buffer_index = (buffer_index+1)%BUFFER_SIZE;

	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
}
