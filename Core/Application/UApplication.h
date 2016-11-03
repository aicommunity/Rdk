#ifndef UApplication_H
#define UApplication_H

#include "UAppCore.h"
#include "UProject.h"
#include "UIVisualController.h"
#include "URpcDispatcherQueues.h"
#include "URpcDispatcher.h"
#include "URpcDecoder.h"
#include "URpcDecoderInternal.h"
#include "URpcDecoderCommon.h"
#include "URpcCommand.h"
#include "URpcCommandInternal.h"
#include "UEngineControl.h"
#include "UEngineControlThread.h"
#include "UEngineStateThread.h"
#include "UBroadcasterInterface.h"
#include "UServerControl.h"
#include "UChannelProfiler.h"

#ifdef __BORLANDC__
#include "Bcb/Application.bcb.h"
#endif

namespace RDK {

class UEngineControl;
class UServerControl;

class RDK_LIB_TYPE UApplication: public UAppController
{
protected: // Данные приложения
/// Имя файла приложения
std::string ApplicationFileName;

/// Рабочий каталог
std::string WorkDirectory;

// Признак наличия открытого проекта
bool ProjectOpenFlag;

// Путь до папки проекта
std::string ProjectPath;

// Имя файла проекта
std::string ProjectFileName;

/// Список последних открытых проектов
std::list<std::string> LastProjectsList;

/// Размер истории последних открытых проектов
int LastProjectsListMaxSize;

/// Каталог логов
//std::string LogDir;

/// Флаг включения отладочного режима логирования
//bool DebugMode;

protected: // Модули приложения
/// Диспетчер команд
UEPtr<URpcDispatcher> RpcDispatcher;

/// Активный проект
UEPtr<UProject> Project;

/// Контроллер движка
UEPtr<UEngineControl> EngineControl;

/// Контроллер серверной части
UEPtr<UServerControl> ServerControl;

protected: // Временные переменные
/// Заголовок приложения
std::string AppCaption;

// Файл настроек проекта
RDK::USerStorageXML ProjectXml;

// Файл настроек интефрейса
RDK::USerStorageXML InterfaceXml;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UApplication(void);
virtual ~UApplication(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Имя файла приложения
const std::string& GetApplicationFileName(void) const;
bool SetApplicationFileName(const std::string& value);

/// Рабочий каталог
const std::string& GetWorkDirectory(void) const;
bool SetWorkDirectory(const std::string& value);

// Признак наличия открытого проекта
bool GetProjectOpenFlag(void) const;
bool SetProjectOpenFlag(bool value);

// Путь до папки проекта
const std::string& GetProjectPath(void) const;
bool SetProjectPath(const std::string& value);

// Имя файла проекта
const std::string& GetProjectFileName(void) const;
bool SetProjectFileName(const std::string& value);

/// Список последних открытых проектов
const std::list<std::string>& GetLastProjectsList(void) const;
bool SetLastProjectsList(const std::list<std::string>& value);

/// Размер истории последних открытых проектов
int GetLastProjectsListMaxSize(void) const;
bool SetLastProjectsListMaxSize(int value);

/// Заголовок приложения
const std::string& GetAppCaption(void) const;

// Файл настроек проекта
const RDK::USerStorageXML& GetProjectXml(void) const;

// Файл настроек интефрейса
const RDK::USerStorageXML& GetInterfaceXml(void) const;

/// Каталог логов (глобальная настройка)
std::string GetLogDir(void) const;
bool SetLogDir(const std::string& value);

/// Флаг включения отладочного режима логирования
bool GetDebugMode(void) const;
bool SetDebugMode(bool value);

/// Текущий каталог логов (с учетом переопределения в проекте)
std::string CalcCurrentLogDir(void) const;
// --------------------------

// --------------------------
// Методы инициализации
// --------------------------
/// Предоставляет доступ к диспетчеру команд
virtual UEPtr<URpcDispatcher> GetRpcDispatcher(void);

/// Устанавливает новый диспетчер команд
/// Ответственность за освобождение памяти диспетчера лежит на вызывающей стороне
virtual bool SetRpcDispatcher(const UEPtr<URpcDispatcher> &value);

/// Предоставляет доступ к контроллеру движка
virtual UEPtr<UEngineControl> GetEngineControl(void);

/// Устанавливает новый контроллер движка
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
virtual bool SetEngineControl(const UEPtr<UEngineControl> &value);

/// Предоставляет доступ к проекту
//virtual UEPtr<UProject> GetProject(void);

/// Устанавливает новый проект
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
virtual bool SetProject(const UEPtr<UProject> &value);

/// Возвращает конфигурацию проекта
virtual const TProjectConfig& GetProjectConfig(void) const;

/// Устанавливает новую конфигурацию проекта
virtual bool SetProjectConfig(const TProjectConfig& value);

/// Предоставляет доступ к контроллеру серверной части
virtual UEPtr<UServerControl> GetServerControl(void) const;

/// Устанавливает новый контроллер сервера
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
virtual bool SetServerControl(const UEPtr<UServerControl> &value);

/// Инициализирует приложение
virtual bool Init(void);

/// Деинициализирует приложение
virtual bool UnInit(void);
// --------------------------

// --------------------------
// Методы управления проектом
// --------------------------
/// Создает проект (через сохранение и открытие)
virtual bool CreateProject(const std::string &file_name, RDK::TProjectConfig &project_config);

/// Открывает проект
virtual bool OpenProject(const std::string &filename);

/// Сохраняет проект
virtual bool SaveProject(void);
virtual bool SaveProjectAs(const std::string &filename);

/// Закрывает проект
virtual bool CloseProject(void);

/// Клонирует проект в новое расположение
virtual bool CloneProject(const std::string &filename);
virtual bool CloneProject(int source_id, int cloned_id);

virtual void ReloadParameters(void);

virtual bool CopyProject(const std::string &new_path);

/// Сохраняет только файл настроек проекта
virtual bool SaveProjectConfig(void);
// --------------------------

// --------------------------
// Методы управления движком
// --------------------------
/// Управление числом каналов
int GetNumChannels(void) const;
bool SetNumChannels(int num);
bool InsertChannel(int index);
bool DeleteChannel(int index);
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
virtual void StartChannel(int channel_index);

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
virtual void PauseChannel(int channel_index);

/// Сбрасывает аналитику выбранного канала, или всех, если channel_index == -1
virtual void ResetChannel(int channel_index);

/// Делает шаг расчета выбранного канала, или всех, если channel_index == -1
virtual void StepChannel(int channel_index);

/// Возвращает true если канал запущен
virtual bool IsChannelStarted(int channel_index);
// --------------------------

// --------------------------
// Методы загрузки сохранения данных в файл
// --------------------------
bool LoadModelFromFile(int channel_index, const std::string &file_name);
bool SaveModelToFile(int channel_index, const std::string &file_name);

bool LoadParametersFromFile(int channel_index, const std::string &file_name);
bool SaveParametersToFile(int channel_index, const std::string &file_name);

bool LoadStatesFromFile(int channel_index, const std::string &file_name);
bool SaveStatesToFile(int channel_index, const std::string &file_name);

bool LoadDescriptionFromFile(int channel_index, const std::string &file_name);
bool SaveDescriptionToFile(int channel_index, const std::string &file_name);

bool LoadClassesDescriptionsFromFile(const std::string &file_name);
bool SaveClassesDescriptionsToFile(const std::string &file_name);

bool LoadCommonClassesDescriptionsFromFile(const std::string &file_name);
bool SaveCommonClassesDescriptionsToFile(const std::string &file_name);

/// Загружает историю проектов из файла
void LoadProjectsHistory(void);

/// Сохраняет историю проектов в файл
void SaveProjectsHistory(void);
// --------------------------

protected:
// --------------------------
// Вспомогательные методы управления счетом
// --------------------------
/// Вычисляет заголовок приложения
void CalcAppCaption(void);

/// Обновляет состояние средств логгирования
void UpdateLoggers(void);

/// Загружает файл в строку
bool LoadFile(const std::string &file_name, std::string &buffer) const;

/// Сохраняет файл из строки
bool SaveFile(const std::string &file_name, const std::string &buffer) const;
// --------------------------
};

}

#endif

