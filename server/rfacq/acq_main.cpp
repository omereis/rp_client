/* Red Pitaya C API example Acquiring a signal from a buffer
* This application acquires a signal on a specific channel */

#ifndef __cplusplus
#define __cplusplus
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "red_pitaya.h"
//#include "rp.h"
#include "rfacq.h"

int main(int argc, char **argv){

        /* Print error, if rp_Init() function failed */
	if(rp_Init() != RP_OK){
		fprintf(stderr, "Rp api init failed!\n");
	}

        /*LOOB BACK FROM OUTPUT 2 - ONLY FOR TESTING*/
	rp_GenReset();
	rp_GenFreq(RP_CH_1, 20000.0);
	rp_GenAmp(RP_CH_1, 1.0);
	rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
	rp_GenOutEnable(RP_CH_1);

	uint32_t buff_size = 16384;
	float *buff = (float *)malloc(buff_size * sizeof(float));

	rp_AcqReset();

	RfAquisition (buff, buff_size);
/*
        rp_AcqSetDecimation(RP_DEC_8);
        rp_AcqSetTriggerLevel(RP_T_CH_1, 0.5); //Trig level is set in Volts while in SCPI
        rp_AcqSetTriggerDelay(0);

        // there is an option to select coupling when using SIGNALlab 250-12
        // rp_AcqSetAC_DC(RP_CH_1, RP_AC); // enables AC coupling on channel 1

        // by default LV level gain is selected
        rp_AcqSetGain(RP_CH_1, RP_LOW); // user can switch gain using this command

        rp_AcqStart();

        // After acquisition is started some time delay is needed in order to acquire fresh samples in to buffer
        //Here we have used time delay of one second but you can calculate exact value taking in to account buffer
        //length and smaling rate

        usleep(100);
        rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
        rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;

        while(1){
                rp_AcqGetTriggerState(&state);
                if(state == RP_TRIG_STATE_TRIGGERED){
                break;
                }
        }


        rp_AcqGetOldestDataV(RP_CH_1, &buff_size, buff);
*/
        int i;
        for(i = 0; i < buff_size; i++){
                printf("%f\n", buff[i]);
        }
        /* Releasing resources */
        free(buff);
        rp_Release();
        return 0;
}


