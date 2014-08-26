#ifndef USHAREDMEMORY_LOADER_QT_CPP
#define USHAREDMEMORY_LOADER_QT_CPP

#include "../USharedMemoryLoader.h"

PUsm_GetNumPipes Usm_GetNumPipes=0;
PUsm_SetNumPipes Usm_SetNumPipes=0;
PUsm_InitPipe Usm_InitPipe=0;
PUsm_WriteData Usm_WriteData=0;
PUsm_ReadData Usm_ReadData=0;
PUsm_UnInitPipe Usm_UnInitPipe=0;
PUsm_IsPipeInit Usm_IsPipeInit=0;
PUsm_GetPipeSize Usm_GetPipeSize=0;

/// ��������� ����������
int LoadUSharedMemoryLibrary(const char *library_file_name)
{        
/*
 USharedMemoryLibrary=LoadLibrary(library_file_name);
 if(!USharedMemoryLibrary)
  return 1;

 Usm_GetNumPipes=(PUsm_GetNumPipes)GetProcAddress(USharedMemoryLibrary,"GetNumPipes");
 if(!Usm_GetNumPipes)
  return 2;

 Usm_SetNumPipes=(PUsm_SetNumPipes)GetProcAddress(USharedMemoryLibrary,"SetNumPipes");
 if(!Usm_SetNumPipes)
  return 3;

 Usm_InitPipe=(PUsm_InitPipe)GetProcAddress(USharedMemoryLibrary,"InitPipe");
 if(!Usm_InitPipe)
  return 4;          
  
 Usm_UnInitPipe=(PUsm_UnInitPipe)GetProcAddress(USharedMemoryLibrary,"UnInitPipe");
 if(!Usm_UnInitPipe)
  return 5;          
  
 Usm_WriteData=(PUsm_WriteData)GetProcAddress(USharedMemoryLibrary,"WriteData");
 if(!Usm_WriteData)
  return 6;

 Usm_ReadData=(PUsm_ReadData)GetProcAddress(USharedMemoryLibrary,"ReadData");
 if(!Usm_ReadData)
  return 7;
     */
 return 100;
}


/// ��������� ����������
int UnLoadUSharedMemoryLibrary(void)
{                    
/*
 if(USharedMemoryLibrary)
 {
  Usm_GetNumPipes=0;
  Usm_SetNumPipes=0;
  Usm_InitPipe=0;
  Usm_WriteData=0;
  Usm_ReadData=0;
  FreeLibrary(USharedMemoryLibrary);
  USharedMemoryLibrary=0;
 }*/
 return 100;
}


#endif

