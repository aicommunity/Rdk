#ifndef RDK_SYSTEM_H
#define RDK_SYSTEM_H   

#include <vector>
#include <string>
#include "USharedMemoryLoader.h"
#include "UGenericMutex.h"


namespace RDK {

extern "C++" {

/// Возвращает текущее время в миллисекундах от некоторого фиксированного момента
/// (зависит от реализации)
RDK_LIB_TYPE unsigned long long GetCurrentStartupTime(void);

/// Вычисляет разницу во времени в миллисекундах
RDK_LIB_TYPE unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2);

/// Усыпляет процесс на заданное число миллисекунд
RDK_LIB_TYPE void Sleep(int value);

/// Создает каталог
/// Возвращает 0 в случае успеха или если каталог уже существует
/// 1 - если уже существует файл с таким именем
/// 2 - если такой путь не существует
/// 3 - если произошла другая ошибка
RDK_LIB_TYPE int CreateNewDirectory(const char* path);

/// Получает список файлов или каталогов по заданному пути
RDK_LIB_TYPE int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results);

/// Копирует файл
RDK_LIB_TYPE int RdkCopyFile(const std::string &source_file, const std::string &dest_file);

/// Копирует каталог с содержимым
RDK_LIB_TYPE int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask);


}

}

#endif

