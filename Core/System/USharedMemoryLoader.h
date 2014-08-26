#ifndef USHAREDMEMORYLOADER_H
#define USHAREDMEMORYLOADER_H

#include "../../Deploy/Include/initdll_defs.h"

#ifdef RDK_CALL
#define CALLING_CONVERSION RDK_CALL
#else
    #ifdef __BORLANDC__
        #define RDK_CALL __cdecl
    #else
        #ifdef __GNUC__
            #define RDK_CALL
        #else
            #define RDK_CALL __cdecl
        #endif
    #endif

#define CALLING_CONVERSION RDK_CALL
#endif

/// Возвращает число доступных каналов обмена
typedef int (CALLING_CONVERSION *PUsm_GetNumPipes)(void);

/// Задает число каналов обмена данными
/// Эта функция не должна вызываться во время обмена!
typedef void (CALLING_CONVERSION *PUsm_SetNumPipes)(int value);

/// Задает размер памяти канала 'pipe_index'
/// Также выбирает способ подключения 'mode', master (1) или slave (0)
/// (Ответственность за распределение ролей лежит на программисте)
/// Параметр 'pipe_uid' определяет уникальный строковой идентификатор общей области памяти для этого канала.
/// (Ответственность за уникальность идентификатора лежитна программисте)
/// Эта функция не должна вызываться во время обмена по выбранному каналу!
/// В случае подключения в режиме slave входное значение переменной 'pipe_byte_size', и в нее записывается
/// размер доступной памяти для этого канала
/// Возвращает 0 в случае успеха, иначе отрицательное число
typedef int (CALLING_CONVERSION *PUsm_InitPipe)(int pipe_index, int &pipe_byte_size, int mode, const char *pipe_uid);

/// Отключается от канала
typedef int (CALLING_CONVERSION *PUsm_UnInitPipe)(int pipe_index);

/// Проверяет, инициализирован ли канал
/// Sic! Возвращает 0 в случае, если канали инициализирован
/// или сообщение об ошибке < 0.
typedef int (CALLING_CONVERSION *PUsm_IsPipeInit)(int pipe_index);

/// Возвращает размер общей памяти канала
typedef int (CALLING_CONVERSION *PUsm_GetPipeSize)(int pipe_index);

/// Записывает данные в общую память, копируя их из буфера 'buffer'
/// Возвращает число записанных байт или отрицательное число в случае ошибки
typedef int (CALLING_CONVERSION *PUsm_WriteData)(int pipe_index, const char* buffer, int buffer_byte_size);

/// Читает данные из общей памяти, копируя их в буфер 'buffer', максимум buffer_byte_size
/// Память должна быть выделена
/// Возвращает число прочитанных байт или отрицательное число в случае ошибки
typedef int (CALLING_CONVERSION *PUsm_ReadData)(int pipe_index, char* buffer, int buffer_byte_size);

extern RDK_LIB_TYPE PUsm_GetNumPipes Usm_GetNumPipes;
extern RDK_LIB_TYPE PUsm_SetNumPipes Usm_SetNumPipes;
extern RDK_LIB_TYPE PUsm_InitPipe Usm_InitPipe;
extern RDK_LIB_TYPE PUsm_UnInitPipe Usm_UnInitPipe;
extern RDK_LIB_TYPE PUsm_IsPipeInit Usm_IsPipeInit;
extern RDK_LIB_TYPE PUsm_GetPipeSize Usm_GetPipeSize;
extern RDK_LIB_TYPE PUsm_WriteData Usm_WriteData;
extern RDK_LIB_TYPE PUsm_ReadData Usm_ReadData;

/// Загружает библиотеку
RDK_LIB_TYPE int LoadUSharedMemoryLibrary(const char *library_file_name);

/// Выгружает библиотеку
RDK_LIB_TYPE int UnLoadUSharedMemoryLibrary(void);

#endif // USHAREDMEMORYLOADER_H
