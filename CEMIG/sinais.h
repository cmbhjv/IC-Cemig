/*
 * sinais.h
 *
 *  Created on: 08/04/2015
 *      Author: Joao
 */

#ifndef SINAIS_H_
#define SINAIS_H_

#include "DSP28x_Project.h"
#include "parametros.h"



typedef struct SIGNAL_PACK
{
	Uint16 Iap;
	Uint16 Ibp;
	Uint16 Icp;
	Uint16 Vab;
	Uint16 Vbc;
	Uint16 Vca;
	Uint16 Ias;
	Uint16 Ibs;
	Uint16 Ics;

} SIGNAL_PACK;

typedef struct SIGNAL_PACK_RMS_SQ
{
	Uint32 Iap;
	Uint32 Ibp;
	Uint32 Icp;
	Uint32 Vab;
	Uint32 Vbc;
	Uint32 Vca;
	Uint32 Ias;
	Uint32 Ibs;
	Uint32 Ics;

} SIGNAL_PACK_RMS_SQ;

typedef struct SIGNAL_PACK_RMS
{
	double Iap;
	double Ibp;
	double Icp;
	double Vab;
	double Vbc;
	double Vca;
	double Ias;
	double Ibs;
	double Ics;

} SIGNAL_PACK_RMS;


typedef struct GLOBAL_FLAGS
{
	Uint16 fim_ciclo:1;
	Uint16 estado:2;
	Uint16 index_rms;
	Uint16 v_bat;
} GLOBAL_FLAGS;


//variaveis globais
extern volatile GLOBAL_FLAGS global_flags;
extern volatile SIGNAL_PACK sinais_amostra[BUFFER_SIZE];
extern volatile SIGNAL_PACK sinais_MEDIA;
extern volatile SIGNAL_PACK_RMS_SQ sinais_RMS_SQ;
extern volatile SIGNAL_PACK_RMS_SQ sinais_RMS_SQ_hist[RMS_HISTORICO_SIZE];
extern volatile SIGNAL_PACK_RMS sinais_RMS_medio;
extern volatile Uint16 v_bat;




//funcoes de operação de sinal
void copia_sinal(volatile SIGNAL_PACK_RMS_SQ*,volatile SIGNAL_PACK_RMS_SQ*);
void rms_sinal(volatile SIGNAL_PACK*,volatile SIGNAL_PACK*,volatile SIGNAL_PACK_RMS_SQ*);
void limpa_sinal(volatile SIGNAL_PACK*);
void limpa_sinal_RMS(volatile SIGNAL_PACK_RMS*);
void limpa_sinal_RMS_SQ(volatile SIGNAL_PACK_RMS_SQ*);
void init_variaveis();
//void atualiza_media_RMS();

#endif /* SINAIS_H_ */
