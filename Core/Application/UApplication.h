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
#include "UServerTransport.h"
#include "UProjectDeployer.h"
#include "UChannelProfiler.h"
#include "UTestManager.h"

#ifdef __BORLANDC__
#include "Bcb/Application.bcb.h"
#endif

namespace RDK {

class UEngineControl;
class UServerControl;

struct StandartXMLInCatalog
{
/// Имя xml файла
std::string XMLName;
/// Имя xml файла
std::string XMLDescription;
};

class RDK_LIB_TYPE UApplication: public UAppController
{
protected: // Данные приложения
/// Имя файла приложения
std::string ApplicationFileName;

/// Рабочий каталог
std::string WorkDirectory;

/// Относительный путь до папки с хранилищем конфигураций (обычно /Bin/Configs)
std::string ConfigsMainPath;

/// Относительный путь до папки с библиотеками (в данном пути сформируется две папки - MockLibs, RTlibs)
std::string LibrariesPath;

/// Относительный путь до папки с описаниями классов
std::string ClDescPath;

/// Относительный путь до папки с хранилищем проектов (пока обычно /Database)
std::string DatabaseMainPath;

/// Абсолютный путь до папки монтирования хранилища
std::string StorageMountPoint;

/// Относительный путь до папки с хранилищем моделей (обычно /Bin/Models)
std::string ModelsMainPath;

/// Имя пользователя
std::string UserName;

/// Id пользователя
int UserId;

/// Признак наличия открытого проекта
bool ProjectOpenFlag;

/// Путь до папки проекта
std::string ProjectPath;

/// Имя файла проекта
std::string ProjectFileName;

/// Флаг принудительного сохранения конфигураций в старом формате
bool UseNewXmlFormatProjectFile;

/// Флаг включения нового представления файловой структуры конфигурации
/// (только при сохранении данных конфигурации в новом формате)
bool UseNewProjectFilesStructure;

/// Список последних открытых проектов
std::list<std::string> LastProjectsList;

/// Размер истории последних открытых проектов
int LastProjectsListMaxSize;

/// Флаг, выставляется если включен режим тестирования
bool TestMode;

/// Имя файла с описанием тестов
std::string TestsDescriptionFileName;

/// Признак требования завершить работу приложения после тестирования
bool CloseAfterTest;

/// Флаг инцициализации
bool AppIsInit;

///Список имен и описаний xml файлов из папки с хранилищем моделей (обычно /Bin/Models)
std::list<StandartXMLInCatalog> xmlInCatalog;

/// Способ сборки хранилища:
// 1 -  обычная сборка. Сборка статических библиотек, затем динамических. Все компоненты рабочие (не заглушки)
// 2 -  поочередная сборка. Сборка статических, далее библиотек-заглушек, затем динамических.
//      Все компоненты, которые смогли собраться рабочими - рабочие.
//      Остальные (которые есть в библиотеках-заглушка, но нет сейчас в хранилище) - заглушки.
// 3 -  сборка только заглушек. Сборка библиотек-заглушек, затем динамических. Все компоненты заглушки (не рабочие)
int StorageBuildMode;

/// Фиксированный путь до логов
/// Используется вместо системного пути если задан
std::string FixedLogPath;

/// Режим записи логов
/// 0 - запись по умолчанию (логи создаются заново при каждом вызове ResetChannel или StartChannel в папке конфигурации)
/// 1 - файл лога создается заново только при открытии каждой новой конфигурации. В папке конфигурации
/// 2 - файл лога создается заново только при открытии каждой новой конфигурации. В системной папке
/// 3 - файл лога создается единожды на весь период работы приложения в системной папке
int LogCreationMode;

/// Уровень сообщения в логгере при появлении которого осуществляется автоматический останов расчета
int CalcStopLogLevel;

protected: // Модули приложения
/// Диспетчер команд
UEPtr<URpcDispatcher> RpcDispatcher;

/// Активный проект
UEPtr<UProject> Project;

/// Контроллер движка
UEPtr<UEngineControl> EngineControl;

/// Контроллер серверной части
UEPtr<UServerControl> ServerControl;

/// Менеджер тестов
UEPtr<UTestManager> TestManager;

/// Деплоер проекта
UEPtr<UProjectDeployer> ProjectDeployer;

protected: // Временные переменные
/// Заголовок приложения
std::string AppCaption;

/// Файл настроек проекта
RDK::USerStorageXML ProjectXml;

/// Файл истории проекта
RDK::USerStorageXML HistoryXml;

/// Файл настроек интефрейса
RDK::USerStorageXML InterfaceXml;

/// Переменная для сохранения данных
std::string SaveBuffer;

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

/// Относительный путь до папки с хранилищем конфигураций (обычно /Bin/Configs)
const std::string& GetConfigsMainPath(void) const;
bool SetConfigsMainPath(const std::string &value);

const std::string& GetDatabaseMainPath(void) const;
bool SetDatabaseMainPath(const std::string &value);

const std::string& GetStorageMountPoint(void) const;
bool SetStorageMountPoint(const std::string &value);

/// Относительный путь до папки с библиотеками (в данном пути сформируется две папки - MockLibs, RTLibs)
const std::string& GetLibrariesPath(void) const;
bool SetLibrariesPath(const std::string &value);

/// Относительный путь до папки с описаниями классов
const std::string& GetClDescPath(void) const;
bool SetClDescPath(const std::string &value);

/// Относительный путь до папки с хранилищем моделей (обычно /Bin/Models)
const std::string& GetModelsMainPath(void) const;
bool SetModelsMainPath(const std::string &value);

/// Имя пользователя
const std::string& GetUserName(void) const;
bool SetUserName(const std::string &value);

/// Id пользователя
int GetUserId(void) const;
bool SetUserId(int value);


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

/// Флаг принудительного сохранения конфигураций в старом формате
bool IsUseNewXmlFormatProjectFile(void) const;
bool ChangeUseNewXmlFormatProjectFile(bool value);

/// Флаг включения нового представления файловой структуры конфигурации
/// (только при сохранении данных конфигурации в новом формате)
bool IsUseNewProjectFilesStructure(void) const;
bool ChangeUseNewProjectFilesStructure(bool value);

///Список имен и описаний xml файлов из папки с хранилищем моделей (обычно /Bin/Models)
const std::list<StandartXMLInCatalog>& GetStandartXMLInCatalog(void) const;
bool SetStandartXMLInCatalog(void);

/// Фиксированный путь до логов
const std::string& GetFixedLogPath(void) const;
bool SetFixedLogPath(const std::string& value);

/// Режим записи логов
/// 0 - запись по умолчанию (логи создаются заново при каждом вызове ResetChannel или StartChannel в папке конфигурации)
/// 1 - файл лога создается заново только при открытии каждой новой конфигурации. В папке конфигурации
/// 2 - файл лога создается заново только при открытии каждой новой конфигурации. В системной папке
/// 3 - файл лога создается единожды на весь период работы приложения в системной папке
int GetLogCreationMode(void) const;
bool SetLogCreationMode(int mode);

/// Уровень сообщения в логгере при появлении которого осуществляется автоматический останов расчета
int GetCalcStopLogLevel(void) const;
bool SetCalcStopLogLevel(int log_level);

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

/// Флаг, выставляется если включен режим тестирования
bool IsTestMode(void) const;

/// Имя файла с описанием тестов
const std::string& GetTestsDescriptionFileName(void) const;
void SetTestsDescriptionFileName(const std::string& value);

/// Признак требования завершить работу приложения после тестирования
bool IsCloseAfterTest(void) const;

/// Приложение инициализированно
bool IsInit(void) const;

/// Установка необходимого режима сборки
void SetStorageBuildMode(int mode);

/// Получение текущего режима сборки
int GetStorageBuildMode();
// --------------------------
/// Создание библиотек-заглушек из статических библиотек с сохранением файлов
void CreateSaveMockLibs();

// --------------------------
// Методы инициализации
// --------------------------
/// Предоставляет доступ к диспетчеру команд
//virtual UEPtr<URpcDispatcher> GetRpcDispatcher(void);

/// Устанавливает новый диспетчер команд
/// Ответственность за освобождение памяти диспетчера лежит на вызывающей стороне
//virtual bool SetRpcDispatcher(const UEPtr<URpcDispatcher> &value);

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

/// Менеджер тестов
/// Ответственность за освобождение памяти менеджера лежит на вызывающей стороне
UEPtr<UTestManager> GetTestManager(void);
virtual bool SetTestManager(const UEPtr<UTestManager> &value);

/// Деплоер проекта (под кончретную задачу)
UEPtr<UProjectDeployer> GetProjectDeployer(void);
virtual bool SetProjectDeployer(const UEPtr<UProjectDeployer> &value);

/// Инициализирует приложение
virtual bool Init(void);

/// Деинициализирует приложение
virtual bool UnInit(void);

/// Проводит тестирование приложения, если менеджер тестов инициализирован и
/// тестовый режим включен
/// Возвращает код ошибки тестирования.
/// Если exit_request == true,
/// то по завершении метода приложение должно быть закрыто с возвращенным кодом ошибки
virtual int Test(bool &exit_request);

/// Осуществляет парсинг командной строки и соответствующую настройку приложение
void ProcessCommandLineArgs(std::vector<std::string> commandLineArgs);

#ifndef __BORLANDC__
/// Осуществляет парсинг командной строки и соответствующую настройку приложение
void ProcessCommandLineArgs(int argc, char **argv);
#endif
// --------------------------

// --------------------------
// Методы управления проектом
// --------------------------
/// Создает проект (через сохранение и открытие)
virtual bool CreateProject(const std::string &file_name, RDK::TProjectConfig &project_config);
virtual bool CreateProject(const std::string &file_name, const std::string &model_classname);

/// Обновляет проект по новой конфигурации
virtual bool UpdateProject(RDK::TProjectConfig &project_config);

/// Открывает проект
virtual bool OpenProject(const std::string &filename);

/// Сохраняет проект
virtual bool SaveProject(void);
virtual bool SaveProjectAs(const std::string &filename);

/// Закрывает проект
virtual bool CloseProject(void);

/// Клонирует проект в новое расположение
virtual bool CloneProject(const std::string &filename);

/// Переименовывает папку проекта
virtual bool RenameProject(const std::string &filename);

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

/// Клонирует канал source_id в cloned_id
virtual bool CloneChannel(int source_id, int cloned_id);
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

/// Проверяет состояние расчета по id канала
virtual UEngineControl::UCalcState CheckCalcState(int channel_id) const;
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
/// Сохраняет точки в истории изменений конфигурации
bool FixSavePoint(USerStorageXML &xml);

/// Включает и выключает тестовый режим
void ChangeTestModeState(bool state);

/// Инициализация парсера командной строки
void InitCmdParser(void);

/// Вычисляет заголовок приложения
void CalcAppCaption(void);

/// Обновляет состояние средств логгирования
void UpdateLoggers(void);

/// Сохраняет файл из строки, через временный файл. Делает n_pass попыток сохранить с чтением результата и сразвнением с оригиналом.
/// Если сохранение не удалось, то старый файл остается как был.
/// Если сохранение удалось, то временный файл заменяет старый
bool SaveFileSafe(const std::string &file_name, const std::string &buffer, const std::string &temp_file_name, int n_pass=3);
// --------------------------
};



}

#endif

