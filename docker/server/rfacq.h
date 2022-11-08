/* Red Pitaya C API example Acquiring a signal from a buffer
* This application acquires a signal on a specific channel */

#ifndef	_RDACQ_H
#define	_RDACQ_H

#ifndef __cplusplus
#define __cplusplus
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "red_pitaya.h"
//#include "rp.h"

int RfAquisition (float *buff, uint32_t buff_size);

#endif
