#ifndef RDK_SYSTEM_H
#define RDK_SYSTEM_H   

#include <vector>
#include <string>
#include "USharedMemoryLoader.h"
#include "UGenericMutex.h"

#define ONETHOUSANDMILLISECONDS  .0000115740740740

namespace RDK {

extern "C++" {

/// ¬озвращает текущее врем€ в миллисекундах от некоторого фиксированного момента
/// (зависит от реализации)
RDK_LIB_TYPE unsigned long long GetCurrentStartupTime(void);

// «аписывает в seconds и useconds текущие значени€ секунд и микросекунд,
// прошедших с некоторого фиксированного момента
RDK_LIB_TYPE void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds);

/// ¬ычисл€ет разницу во времени в миллисекундах
RDK_LIB_TYPE unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2);

/// ¬озвращает локальное врем€ в дн€х (с точностью до миллисекунд) от начала времен
RDK_LIB_TYPE double GetVariantLocalTime(void);

/// ”сыпл€ет процесс на заданное число миллисекунд
RDK_LIB_TYPE void Sleep(int value);

/// —оздает каталог
/// ¬озвращает 0 в случае успеха или если каталог уже существует
/// 1 - если уже существует файл с таким именем
/// 2 - если такой путь не существует
/// 3 - если произошла друга€ ошибка
RDK_LIB_TYPE int CreateNewDirectory(const char* path);

/// ѕолучает список файлов или каталогов по заданному пути
RDK_LIB_TYPE int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results);

///  опирует файл
RDK_LIB_TYPE int RdkCopyFile(const std::string &source_file, const std::string &dest_file);

///  опирует каталог с содержимым
RDK_LIB_TYPE int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask);

/// ‘ункци€ осуществл€ет вывод в отладочный лог, если сборка в отладке
RDK_LIB_TYPE void RdkDebuggerMessage(const std::string &message);

}

}

#endif

