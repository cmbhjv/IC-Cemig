/*
 * sinais.c
 *
 *  Created on: 08/04/2015
 *      Author: Joao
 */

#include "sinais.h"
//#pragma DATA_SECTION(sinais_buffer,"DMARAML7");
volatile SIGNAL_PACK sinais_amostra[BUFFER_SIZE];
volatile SIGNAL_PACK sinais_MEDIA;

volatile SIGNAL_PACK_RMS_SQ sinais_RMS_SQ;
volatile SIGNAL_PACK_RMS_SQ sinais_RMS_SQ_hist[RMS_HISTORICO_SIZE];

volatile SIGNAL_PACK_RMS sinais_RMS_medio;

volatile GLOBAL_FLAGS global_flags;

void init_variaveis()
{
	Uint16 i;

	limpa_sinal(&sinais_MEDIA);
	for(i = 0;i<BUFFER_SIZE;i++)
		limpa_sinal(&sinais_amostra[i]);


	limpa_sinal_RMS_SQ(&sinais_RMS_SQ);

	for(i = 0;i<RMS_HISTORICO_SIZE;i++)
		limpa_sinal_RMS_SQ(&sinais_RMS_SQ_hist[i]);

	limpa_sinal_RMS(&sinais_RMS_medio);

	global_flags.fim_ciclo = 0;
	global_flags.index_rms = 0;
	global_flags.estado = 0;
	global_flags.v_bat = 0;
}


void limpa_sinal(volatile SIGNAL_PACK* dest)
{
	dest->Iap=0;
	dest->Ias=0;
	dest->Ibp=0;
	dest->Ibs=0;
	dest->Icp=0;
	dest->Ics=0;
	dest->Vab=0;
	dest->Vbc=0;
	dest->Vca=0;
}

void limpa_sinal_RMS_SQ(volatile SIGNAL_PACK_RMS_SQ* dest)
{
	dest->Iap=0;
	dest->Ias=0;
	dest->Ibp=0;
	dest->Ibs=0;
	dest->Icp=0;
	dest->Ics=0;
	dest->Vab=0;
	dest->Vbc=0;
	dest->Vca=0;
}

void limpa_sinal_RMS(volatile SIGNAL_PACK_RMS* dest)
{
	dest->Iap=0;
	dest->Ias=0;
	dest->Ibp=0;
	dest->Ibs=0;
	dest->Icp=0;
	dest->Ics=0;
	dest->Vab=0;
	dest->Vbc=0;
	dest->Vca=0;
}

void copia_sinal(volatile SIGNAL_PACK_RMS_SQ* src,volatile SIGNAL_PACK_RMS_SQ* dest)
{
	dest->Iap = src->Iap;
	dest->Ias = src->Ias;
	dest->Ibp = src->Ibp;
	dest->Ibs = src->Ibs;
	dest->Icp = src->Icp;
	dest->Ics = src->Ics;
	dest->Vab = src->Vab;
	dest->Vbc = src->Vbc;
	dest->Vca = src->Vca;
}

void rms_sinal(volatile SIGNAL_PACK* buffer,volatile SIGNAL_PACK* media,volatile SIGNAL_PACK_RMS_SQ* dest)
{
	Uint16 count;
	long aux;

	limpa_sinal_RMS_SQ(dest);

	for(count = 0; count<BUFFER_SIZE;count++) //165 ciclos/loop
	{
		aux = (long)buffer[count].Iap - media->Iap;
		dest->Iap += (Uint32)  aux*aux;
		aux = (long)buffer[count].Ias - media->Ias;
		dest->Ias += (Uint32) aux*aux;
		aux = (long)buffer[count].Ibp - media->Ibp;
		dest->Ibp += (Uint32) aux*aux;
		aux = (long)buffer[count].Ibs - media->Ibs;
		dest->Ibs += (Uint32) aux*aux;
		aux = (long)buffer[count].Icp - media->Icp;
		dest->Icp += (Uint32) aux*aux;
		aux = (long)buffer[count].Ics - media->Ics;
		dest->Ics += (Uint32) aux*aux;
		aux = (long)buffer[count].Vab - media->Vab;
		dest->Vab += (Uint32) aux*aux;
		aux = (long)buffer[count].Vbc - media->Vbc;
		dest->Vbc += (Uint32) aux*aux;
		aux = (long)buffer[count].Vca - media->Vca;
		dest->Vca += (Uint32) aux*aux;
	}

	//Divide 1/N
	dest->Iap >>= BUFFER_SIZE_BIT;
	dest->Ias >>= BUFFER_SIZE_BIT;
	dest->Ibp >>= BUFFER_SIZE_BIT;
	dest->Ibs >>= BUFFER_SIZE_BIT;
	dest->Icp >>= BUFFER_SIZE_BIT;
	dest->Ics >>= BUFFER_SIZE_BIT;
	dest->Vab >>= BUFFER_SIZE_BIT;
	dest->Vbc >>= BUFFER_SIZE_BIT;
	dest->Vca >>= BUFFER_SIZE_BIT;
}
