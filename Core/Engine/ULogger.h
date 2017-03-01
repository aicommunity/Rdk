#ifndef ULoggerH
#define ULoggerH

#include "../Utilities/USupport.h"
#include "../System/rdk_system.h"
#include "../System/UGenericMutex.h"
#include "UELockVar.h"
#include "UEPtr.h"

namespace RDK {

class RDK_LIB_TYPE ULogger
{
protected: // Параметры
/// Путь до папки с логами
RDK::UELockVar<std::string> LogDir;

/// Суффикс имени файла с логом
RDK::UELockVar<std::string> Suffix;

protected: // Данные
/// Файл для сохранения логов
RDK::UEPtr<std::ofstream> EventsLogFile;

/// Флаг, разрешающий запись в лог
/// сбрасывается, если невозможно создать катлог, или файл
bool LogEnabledFlag;

UGenericMutex* LogMutex;

public: // Конструкторы и деструкторы
ULogger(void);
virtual ~ULogger(void);

public: // Методы управления параметрами
/// Путь до папки с логами
std::string GetLogDir(void) const;
bool SetLogDir(const std::string &value);

/// Суффикс имени файла
std::string GetSuffix(void) const;
bool SetSuffix(const std::string &value);

public: // Методы записи логов
/// Инициализирует лог и создает новые файлы
int InitLog(void);

/// Сохраняет строку в лог
int WriteMessageToFile(const std::string &str);

/// Закрывает файлы с логами и удаляет связанные файловые переменные
int Clear(void);

/// Возвращает true если файл записи логов открыт
bool IsLogFileCreated(void) const;

};

}

#endif
