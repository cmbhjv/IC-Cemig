/*
 * main.c
 */
#include "DSP28x_Project.h"


#include "pwm.h"
#include "adc.h"
#include "io.h"
#include "sinais.h"
#include "math.h"

void avalia_medicoes(void);


int main(void) {

	// - Disables the watchdog
	// - Set the PLLCR for proper SYSCLKOUT frequency
	// - Set the pre-scaler for the high and low frequency peripheral clocks
	// - Enable the clocks to the peripherals
	InitSysCtrl();


	// Desabilita todas as interrupções
	DINT;
	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();
	InitCpuTimers();

	configuraPWM();
	configuraADC();
	configuraGPIO();

	//Inicializa todas as estruturas com 0;
	init_variaveis();

	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM

	led_estado[0] = LED_ACESO; //TODO Verificar se energizou (por um AD)

	//TODO Hibernar

	while(1){

		if(global_flags.fim_ciclo)//Ao final de cada periodo (60Hz)
		{
			adcDisable(); //para amostragem
			rms_sinal(sinais_amostra,&sinais_MEDIA,&sinais_RMS_SQ); //calcular valor RMS
			resetAmostras(); //Volta o contador de amostras para o començo
			adcEnable(); //continua amostragem
			limpa_sinal(&sinais_MEDIA); //limpar acumulador de média
			global_flags.fim_ciclo = 0;

			if(global_flags.v_bat <= V_BAT_ref)
			{
				pwmDisable();
				led_estado[0] = LED_PISCANDO;
				global_flags.v_bat =0;
			}


			if(global_flags.estado == ESTADO_RAMPA) //Rampa de Tensão
			{
				//Testar se há curto
				if(sinais_RMS_SQ.Iap>=I_CURTO || sinais_RMS_SQ.Ibp>=I_CURTO || sinais_RMS_SQ.Icp>=I_CURTO)
				{
					pwmDisable();
					led_estado[3] = LED_PISCANDO; // Alerta que transformador não deve ser religado
				}

				if(sinais_RMS_SQ.Vab < V_SET && divisor<1.0)  //Verifica se a tensao já está no nivel desejado
					divisor += PASSO_DIVISOR; //Incrementar Rampa de tensao
				else
					global_flags.estado = ESTADO_MED; // A tensão está no valor desejado, começar a registrar mediçoes

			}
			else if(global_flags.estado == ESTADO_MED)//Registrar valores RMS
			{
				if(global_flags.index_rms==RMS_HISTORICO_SIZE-1) // Há valores RMS suficientes no histórico
				{
					global_flags.estado = ESTADO_CALC; // Começar avaliação dos parametros adquiridos
					global_flags.fim_ciclo = 0; // Redundância
				}

				copia_sinal(&sinais_RMS_SQ,&sinais_RMS_SQ_hist[global_flags.index_rms]); //Registra o último valor RMS calculado no histórico
				global_flags.index_rms = (global_flags.index_rms+1)%RMS_HISTORICO_SIZE; //Incrementar índice do histórico
			}

		}
		else if(global_flags.estado == ESTADO_CALC) //calcular e analisar parâmetros
		{
			adcDisable(); // Parar de amostrar
#ifndef DEBUG //Nao parar de gerar PWM quando o programa estiver sendo debugado
			pwmDisable(); //Parar de gerar PWM
#endif
			avalia_medicoes();
			while(1);
		}
	};
}

/**
 * Calcula as relações entre corrente e tensão e avalia a dispersão e valor médio dessas.
 */
void avalia_medicoes()
{
	Uint16 i = 0;
	double relacoes[9]; //Relacoes entre correntes (Ns)
	double relacao_media = 0; // N médio
	double relacao_std=0; //Desvio padrao de N
	double aux = 0;


	for(i=0; i< RMS_HISTORICO_SIZE;i++) //Soma todos os valores RMS registrados.
	{
		sinais_RMS_medio.Iap += sqrt(sinais_RMS_SQ_hist[i].Iap);
		sinais_RMS_medio.Ias += sqrt(sinais_RMS_SQ_hist[i].Ias);
		sinais_RMS_medio.Ibp += sqrt(sinais_RMS_SQ_hist[i].Ibp);
		sinais_RMS_medio.Ibs += sqrt(sinais_RMS_SQ_hist[i].Ibs);
		sinais_RMS_medio.Icp += sqrt(sinais_RMS_SQ_hist[i].Icp);
		sinais_RMS_medio.Ics += sqrt(sinais_RMS_SQ_hist[i].Ics);
		sinais_RMS_medio.Vab += sqrt(sinais_RMS_SQ_hist[i].Vab);
		sinais_RMS_medio.Vbc += sqrt(sinais_RMS_SQ_hist[i].Vbc);
		sinais_RMS_medio.Vca += sqrt(sinais_RMS_SQ_hist[i].Vca);
	}

	//Calcula a média
	sinais_RMS_medio.Iap /= RMS_HISTORICO_SIZE;
	sinais_RMS_medio.Ias /= RMS_HISTORICO_SIZE;
	sinais_RMS_medio.Ibp /= RMS_HISTORICO_SIZE;
	sinais_RMS_medio.Ibs /= RMS_HISTORICO_SIZE;
	sinais_RMS_medio.Icp /= RMS_HISTORICO_SIZE;
	sinais_RMS_medio.Ics /= RMS_HISTORICO_SIZE;
	sinais_RMS_medio.Vab /= RMS_HISTORICO_SIZE;
	sinais_RMS_medio.Vbc /= RMS_HISTORICO_SIZE;
	sinais_RMS_medio.Vca /= RMS_HISTORICO_SIZE;

	//Calcula Ns
	relacoes[0] = sinais_RMS_medio.Ias/sinais_RMS_medio.Iap;
	relacao_media += relacoes[0];
	relacoes[1] = sinais_RMS_medio.Ibs/sinais_RMS_medio.Iap;
	relacao_media += relacoes[1];
	relacoes[2] = sinais_RMS_medio.Ics/sinais_RMS_medio.Iap;
	relacao_media += relacoes[2];
	relacoes[3] = sinais_RMS_medio.Ias/sinais_RMS_medio.Ibp;
	relacao_media += relacoes[3];
	relacoes[4] = sinais_RMS_medio.Ibs/sinais_RMS_medio.Ibp;
	relacao_media += relacoes[4];
	relacoes[5] = sinais_RMS_medio.Ics/sinais_RMS_medio.Ibp;
	relacao_media += relacoes[5];
	relacoes[6] = sinais_RMS_medio.Ias/sinais_RMS_medio.Icp;
	relacao_media += relacoes[6];
	relacoes[7] = sinais_RMS_medio.Ibs/sinais_RMS_medio.Icp;
	relacao_media += relacoes[7];
	relacoes[8] = sinais_RMS_medio.Ics/sinais_RMS_medio.Icp;
	relacao_media += relacoes[8];

	//Calcula N médio
	relacao_media /= 9;

	//Calcula desvio padrão
	for(i=0;i<9;i++)
	{
		aux = relacoes[i]-relacao_media;
		relacao_std += aux*aux;
	}
	relacao_std /= 9;
	relacao_std = sqrt(relacao_std);
	//

	led_estado[1] = LED_APAGADO; // Terminou-se a avaliação

	if((relacao_std*2)/relacao_media<=0.05)
		led_estado[2] = LED_PISCANDO; //Alerta: transformador OK
	else
		led_estado[3] = LED_PISCANDO; // Alerta: Transformador nao pode ser religado
}


