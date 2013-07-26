#ifndef USHAREDMEMORY_H
#define USHAREDMEMORY_H

#include <QtCore/qglobal.h>

#if defined(USHAREDMEMORY_LIBRARY)
#  define USHAREDMEMORYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define USHAREDMEMORYSHARED_EXPORT Q_DECL_IMPORT
#endif

#define CALLING_CONVERSION __cdecl

extern "C" {

/// Возвращает число доступных каналов обмена
USHAREDMEMORYSHARED_EXPORT int CALLING_CONVERSION GetNumPipes(void);

/// Задает число каналов обмена данными
/// Эта функция не должна вызываться во время обмена!
USHAREDMEMORYSHARED_EXPORT void CALLING_CONVERSION SetNumPipes(int value);

/// Задает размер памяти канала 'pipe_index'
/// Также выбирает способ подключения 'mode', master (1) или slave (0)
/// (Ответственность за распределение ролей лежит на программисте)
/// Параметр 'pipe_uid' определяет уникальный строковой идентификатор общей области памяти для этого канала.
/// (Ответственность за уникальность идентификатора лежитна программисте)
/// Эта функция не должна вызываться во время обмена по выбранному каналу!
/// В случае подключения в режиме slave входное значение переменной 'pipe_byte_size', и в нее записывается
/// размер доступной памяти для этого канала
/// Возвращает 0 в случае успеха, иначе отрицательное число
USHAREDMEMORYSHARED_EXPORT int CALLING_CONVERSION InitPipe(int pipe_index, int &pipe_byte_size, int mode, const char *pipe_uid);

/// Отключается от канала
USHAREDMEMORYSHARED_EXPORT int CALLING_CONVERSION UnInitPipe(int pipe_index);

/// Проверяет, инициализирован ли канал
/// Sic! Возвращает 0 в случае, если канали инициализирован
/// или сообщение об ошибке < 0.
USHAREDMEMORYSHARED_EXPORT int CALLING_CONVERSION IsPipeInit(int pipe_index);

/// Возвращает размер общей памяти канала
USHAREDMEMORYSHARED_EXPORT int CALLING_CONVERSION GetPipeSize(int pipe_index);

/// Записывает данные в общую память, копируя их из буфера 'buffer'
/// Возвращает число записанных байт или отрицательное число в случае ошибки
USHAREDMEMORYSHARED_EXPORT int CALLING_CONVERSION WriteData(int pipe_index, const char* buffer, int buffer_byte_size);

/// Читает данные из общей памяти, копируя их в буфер 'buffer', максимум buffer_byte_size
/// Память должна быть выделена
/// Возвращает число прочитанных байт или отрицательное число в случае ошибки
USHAREDMEMORYSHARED_EXPORT int CALLING_CONVERSION ReadData(int pipe_index, char* buffer, int buffer_byte_size);

}

#endif // USHAREDMEMORY_H
