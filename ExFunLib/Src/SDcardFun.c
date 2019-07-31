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
	uint8_t res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 1);
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
	uint8_t res = f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, workBuffer, _MAX_SS);
	if( res == FR_OK ) return SDcard_success;
	if(res == FR_DISK_ERR) return SDcard_Dsk_Err;
	if(res == FR_NOT_READY) return SDcard_NotReady;
	else return SDcard_error;
}
SDcardFileStatus_Typedef SDcardTryOpen(SDcardFile_HandleTypeDef * scfhtd, const TCHAR * path)
{
	if((scfhtd->fileIsOpened == SDcard_FileOpen2write)||(scfhtd->fileIsOpened == SDcard_FileOpen2read)){
		SDcardCloseFile(scfhtd);
	}
	uint8_t res = f_open(&scfhtd->trgFile, path, FA_OPEN_EXISTING | FA_WRITE);
	f_close(&scfhtd->trgFile);
	if( res == FR_OK ) {
		return SDcard_success;
	}
	else	return SDcard_error;
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
SDcardFileStatus_Typedef SDcardSelfTest(SDcardFile_HandleTypeDef * scfhtd)
{
	FIL MyFile;
	FRESULT res;                                          /* FatFs function common result code */
  uint32_t byteswritten, bytesread;                     /* File write/read counts */
  uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
  uint8_t rtext[100];                                   /* File read buffer */
//	res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 1);
//	HAL_Delay(200);
  	res = (SDcardMount(scfhtd) == SDcard_Mounted) ? FR_OK : FR_DISK_ERR;

	if(res != FR_OK)	return SDcard_error; /* FatFs Initialization Error */
	/*##-3- Create a FAT file system (format) on the logical drive #########*/
	/* WARNING: Formatting the uSD card will delete all content on the device */
	// res = f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer));
	if(res != FR_OK)	return SDcard_error; /* FatFs Format Error */
  /*##-4- Create and Open a new text file object with write access #####*/
	res = f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK)	return SDcard_error;
	/*##-5- Write data to the text file ################################*/
	//uint8_t sdstate = BSP_SD_GetCardState();
	res = f_write(&MyFile, wtext,(UINT) sizeof(wtext), (void *)&byteswritten);
	if((byteswritten == 0) || (res != FR_OK)) return SDcard_error; /* 'STM32.TXT' file Write or EOF Error */
	/*##-6- Close the open text file #################################*/
	f_close(&MyFile);

	  /*##-7- Open the text file object with read access ###############*/
	  if(f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK) return SDcard_error; /* 'STM32.TXT' file Open for read Error */
	  /*##-8- Read data from the text file ###########################*/
	  res = f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);

	  if((bytesread == 0) || (res != FR_OK)) return SDcard_error; /* 'STM32.TXT' file Read or EOF Error */

	  /*##-9- Close the open text file #############################*/
	  f_close(&MyFile);

	  /*##-10- Compare read data with the expected data ############*/
	  if((bytesread != byteswritten))
	  {
		/* Read data is different from the expected data */
		return SDcard_error;
	  }
	  else
	  {
		/* Success of the demo: no error occurrence */
		return SDcard_success;
	  }
}
SDcardFileStatus_Typedef SDcardOpenDir(SDcardFile_HandleTypeDef * scfhtd, const TCHAR* path)
{
	if(f_opendir (&scfhtd->trgDir, path) == FR_OK)
		return SDcard_success;
	if(f_mkdir(path) != FR_OK)
		return SDcard_error;
	if(f_opendir (&scfhtd->trgDir, path) != FR_OK)
		return SDcard_error;
	return SDcard_success;
}
SDcardFileStatus_Typedef SDcardCloseDir(SDcardFile_HandleTypeDef * scfhtd)
{
	if(f_closedir (&scfhtd->trgDir) != FR_OK)
		return SDcard_error;
	else
		return SDcard_success;
}
SDcardFileStatus_Typedef SDcardFindFstFilInDir(SDcardFile_HandleTypeDef * scfhtd)
{
	if(f_readdir(&scfhtd->trgDir, &scfhtd->curFileInfo) != FR_OK)
		return SDcard_error;
	else
		return SDcard_success;
}
SDcardFileStatus_Typedef SDcardFindNxtFilInDir(SDcardFile_HandleTypeDef * scfhtd)
{
	if(f_findnext(&scfhtd->trgDir, &scfhtd->curFileInfo) != FR_OK)
			return SDcard_error;
	else
			return SDcard_success;
}
SDcardFileStatus_Typedef SDcardGetMountStatus(SDcardFile_HandleTypeDef * scfhtd)
{
	if(scfhtd->status == SDcard_Mounted) return SDcard_Mounted;
	else return SDcard_NotMount;
}

SDcardFileStatus_Typedef SDcardSetMountStatus(SDcardFile_HandleTypeDef * scfhtd)
{
	scfhtd->status = SDcard_Mounted;
	return SDcard_success;
}
