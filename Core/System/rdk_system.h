#ifndef RDK_SYSTEM_H
#define RDK_SYSTEM_H   

#include <vector>
#include <string>
#include <locale>
#include "USharedMemoryLoader.h"
#include "UGenericMutex.h"
#include "UDllLoader.h"

#define ONETHOUSANDMILLISECONDS  .0000115740740740

namespace RDK {

extern "C++" {

/// Возвращает текущее время в миллисекундах от некоторого фиксированного момента
/// (зависит от реализации)
RDK_LIB_TYPE unsigned long long GetCurrentStartupTime(void);

// Записывает в seconds и useconds текущие значения секунд и микросекунд,
// прошедших с некоторого фиксированного момента
RDK_LIB_TYPE void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds);

/// Вычисляет разницу во времени в миллисекундах
RDK_LIB_TYPE unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2);

/// Возвращает локальное время в днях (с точностью до миллисекунд) от начала времен
RDK_LIB_TYPE double GetVariantLocalTime(void);

/// Усыпляет процесс на заданное число миллисекунд
RDK_LIB_TYPE void Sleep(int value);

/// Создает каталог
/// Возвращает 0 в случае успеха или если каталог уже существует
/// 1 - если уже существует файл с таким именем
/// 2 - если такой путь не существует
/// 3 - если произошла другая ошибка
RDK_LIB_TYPE int CreateNewDirectory(const char* path);

/// Удаляет все файлы в директории
/// И саму директорию, если в ней были только файлы
RDK_LIB_TYPE int DeleteDirectory(const char* path);

/// Удаляет файл
RDK_LIB_TYPE int RemoveFile(const char* path);

/// Получает список файлов или каталогов по заданному пути
RDK_LIB_TYPE int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results);

/// Копирует файл
RDK_LIB_TYPE int RdkCopyFile(const std::string &source_file, const std::string &dest_file);

/// Перемещает файл
RDK_LIB_TYPE int RdkMoveFile(const std::string &source_file, const std::string &dest_file);

/// Копирует каталог с содержимым
RDK_LIB_TYPE int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask);

/// Функция осуществляет вывод в отладочный лог, если сборка в отладке
RDK_LIB_TYPE void RdkDebuggerMessage(const std::string &message);

/// Функция создает загрузчика динамических библиотек и вызывает для него Load(dll_name)
RDK_LIB_TYPE UDllLoader* UCreateAndLoadDllLoader(const std::string &dll_name);

/// Функция разрушения объекта загрузчика динамических бибилиотек, НЕ выгружает библиотеку
RDK_LIB_TYPE void UDestroyDllLoader(UDllLoader *handle);

/// Возвращает объем используемой приложением памяти
/// Если не удалось определить то возвращает false
RDK_LIB_TYPE bool ReadUsedMemoryInfo(unsigned long long &total_used_memory, unsigned long long &largest_free_block);

/// Преобразует широкую строку в обычную
/// 0 - означает кодировку по умолчанию
RDK_LIB_TYPE std::string& narrow(const std::wstring& wstr, std::string &result, unsigned codepage=0);

/// Deprecated
RDK_LIB_TYPE std::string& narrow(const std::wstring& wstr, const std::locale& loc, std::string &result);

/// Преобразует обычную строку в широкую
/// 0 - означает кодировку по умолчанию
RDK_LIB_TYPE std::wstring& widen(const std::string& str, std::wstring &result, unsigned codepage=0);

/// Deprecated
RDK_LIB_TYPE std::wstring& widen(const std::string& str, const std::locale& loc, std::wstring &result);

}

}

#endif

