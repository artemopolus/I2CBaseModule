/*
 * SDcardFun.c
 *
 *  Created on: 9 июл. 2019 г.
 *      Author: Artem
 */

#include "SDcardFun.h"

SDcardFileStatus_Typedef SDcardMount(SDcardFile_HandleTypeDef * scfhtd)
{
	SDcardFileStatus_Typedef result = SDcard_NotMount;
	uint8_t res = f_mount(&scfhtd->SDFatFs, (TCHAR const*)scfhtd->SDPath, 1);
	HAL_Delay(200);
	if(res == FR_OK)
	{
		result = SDcard_Mounted;
	}
	scfhtd->status = result;
	scfhtd->fileIsOpened = SDcard_FileNotOpened;
	return result;
}
SDcardFileStatus_Typedef SDcardFormat(SDcardFile_HandleTypeDef * scfhtd)
{
	if(scfhtd->status == SDcard_NotMount)	return SDcard_NotMount;
	uint8_t res = f_mkfs((TCHAR const*)scfhtd->SDPath, FM_ANY, 0, scfhtd->workBuffer, _MAX_SS);
	if( res == FR_OK ) return SDcard_success;
	if(res == FR_DISK_ERR) return SDcard_Dsk_Err;
	if(res == FR_NOT_READY) return SDcard_NotReady;
	else return SDcard_error;
}
SDcardFileStatus_Typedef SDcardOpenFile2write(SDcardFile_HandleTypeDef * scfhtd, const TCHAR * path)
{
	if((scfhtd->fileIsOpened == SDcard_FileOpen2write)||(scfhtd->fileIsOpened == SDcard_FileOpen2read)){
		SDcardCloseFile(scfhtd);
	}
	if(scfhtd->status == SDcard_NotMount)	return SDcard_NotMount;
	uint8_t res = f_open(&scfhtd->trgFile, path, FA_CREATE_ALWAYS | FA_WRITE);
	if( res == FR_OK ) {
		scfhtd->fileIsOpened = SDcard_FileOpen2write;
		return SDcard_success;
	}
	if((res == FR_INVALID_NAME)||(res == FR_NO_PATH)) return SDcard_InputInvalid;
	else return SDcard_FileNotOpened;
}
SDcardFileStatus_Typedef SDcardOpenFile2read(SDcardFile_HandleTypeDef * scfhtd, const TCHAR * path)
{
	if((scfhtd->fileIsOpened == SDcard_FileOpen2write)||(scfhtd->fileIsOpened == SDcard_FileOpen2read)){
		SDcardCloseFile(scfhtd);
	}
	if(scfhtd->status == SDcard_NotMount)	return SDcard_NotMount;
	uint8_t res = f_open(&scfhtd->trgFile, path, FA_READ);
	if( res == FR_OK ) {
		scfhtd->fileIsOpened = SDcard_FileOpen2read;
		return SDcard_success;
	}
	if((res == FR_INVALID_NAME)||(res == FR_NO_PATH)||(res == FR_NO_FILE)) return SDcard_InputInvalid;
	else return SDcard_FileNotOpened;
}
SDcardFileStatus_Typedef SDcardWrite2file(SDcardFile_HandleTypeDef * scfhtd, const uint8_t * msg, const UINT msglen, UINT * getmsglen)
{
	if(scfhtd->fileIsOpened == SDcard_FileNotOpened) return SDcard_FileNotOpened;
	uint8_t res = f_write(&scfhtd->trgFile, msg,msglen, (void *)&getmsglen);
	if(res == FR_OK)	return SDcard_success;
	else return SDcard_error;
}
SDcardFileStatus_Typedef SDcardReadFile(SDcardFile_HandleTypeDef * scfhtd, uint8_t * msg, UINT msglen, UINT * getmsglen)
{
	if(scfhtd->fileIsOpened == SDcard_FileNotOpened) return SDcard_FileNotOpened;
	uint8_t res = f_read(&scfhtd->trgFile, msg,msglen, (UINT*)&getmsglen);
	if(res == FR_OK)	return SDcard_success;
	else return SDcard_error;
}
SDcardFileStatus_Typedef SDcardCloseFile(SDcardFile_HandleTypeDef * scfhtd)
{
	if(scfhtd->fileIsOpened != SDcard_FileNotOpened)
	{
	f_close(&scfhtd->trgFile);
	scfhtd->fileIsOpened = SDcard_FileNotOpened;
	}
	return SDcard_success;
}
