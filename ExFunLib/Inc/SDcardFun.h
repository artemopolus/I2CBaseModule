/*
 * SDcardFun.h
 *
 *  Created on: 9 июл. 2019 г.
 *      Author: Artem
 */

#ifndef EXFUNLIB_INC_SDCARDFUN_H_
#define EXFUNLIB_INC_SDCARDFUN_H_

#include "fatfs.h"
#include "stm32f7xx_hal.h"

typedef enum {
	SDcard_NotMount = 0x00U,
	SDcard_Mounted,
	SDcard_BUSY,
	SDcard_NotReady,
	SDcard_Dsk_Err,
	SDcard_success,
	SDcard_error,
	SDcard_FileNotOpened,
	SDcard_InputInvalid,
	SDcard_FileOpen2write,
	SDcard_FileOpen2read
}SDcardFileStatus_Typedef;

typedef struct {
	FATFS SDFatFs;
	char SDPath[4];
	uint8_t workBuffer[_MAX_SS];
	SDcardFileStatus_Typedef status;
	FIL trgFile;
	SDcardFileStatus_Typedef fileIsOpened;
}SDcardFile_HandleTypeDef;



SDcardFileStatus_Typedef SDcardMount(SDcardFile_HandleTypeDef * scfhtd);
SDcardFileStatus_Typedef GetSDcardStatus(SDcardFile_HandleTypeDef * scfhtd);
SDcardFileStatus_Typedef SDcardFormat(SDcardFile_HandleTypeDef * scfhtd);
SDcardFileStatus_Typedef SDcardOpenFile2write(SDcardFile_HandleTypeDef * scfhtd, const TCHAR * path);
SDcardFileStatus_Typedef SDcardOpenFile2read(SDcardFile_HandleTypeDef * scfhtd, const TCHAR * path);
SDcardFileStatus_Typedef SDcardWrite2file(SDcardFile_HandleTypeDef * scfhtd, const uint8_t * msg, const UINT msglen, UINT * getmsglen);
SDcardFileStatus_Typedef SDcardReadFile(SDcardFile_HandleTypeDef * scfhtd, uint8_t * msg, UINT msglen, UINT * getmsglen);
SDcardFileStatus_Typedef SDcardCloseFile(SDcardFile_HandleTypeDef * scfhtd);

#endif /* EXFUNLIB_INC_SDCARDFUN_H_ */
