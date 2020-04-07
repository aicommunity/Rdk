#ifndef UServerControlH
#define UServerControlH

#include "../../Deploy/Include/rdk.h"
#include "UIVisualController.h"
#include "UServerTransportTcp.h"
//Проредить потом?
#include "URpcDispatcherQueues.h"
#include "URpcDispatcher.h"
#include "URpcDecoder.h"
#include "URpcDecoderInternal.h"
#include "URpcDecoderCommon.h"
#include "URpcCommand.h"
#include "URpcCommandInternal.h"

namespace RDK {

class UApplication;

class RDK_LIB_TYPE UServerControl: public UAppController
{
friend class UApplication;
protected: // Параметры
/// Флаг разрешения запуска сервера при старте
bool AutoStartFlag;

/// Имя сервера
std::string ServerName;

/// Идентификатор сервера
std::string ServerId;

/// Число шагов усреднения оценки производительности
int AverageIterations;

std::string MetaComponentName;

std::string MetaComponentStateName;

///Папка для сохранения отладочных данных
std::string DebugFolder;

///Полный путь на одну сессию
std::string DebugOutputPath;

protected: // Данные
/// Указатель на экземпляр приложения
UEPtr<UApplication> Application;

/// Указатель на экземпляр транспорта
UEPtr<UServerTransport> ServerTransport;

/// Указатель на экземпляр транспорта для приема и передачи HTTP
UEPtr<UServerTransport> ServerTransportHttp;

/// Диспетчер команд
UEPtr<URpcDispatcher> RpcDispatcher;

/// Флаг состояния инициализации
bool InitFlag;

protected: // Данные для оценки производительности и сохранения отладочной информации
/// Результаты измерений производительности, мс
std::vector<std::vector<RDK::ULongTime> > ModelPerformanceResults;
std::vector<std::vector<RDK::ULongTime> > TransportPerformanceResults;

// Индекс складывания данных в массив оценки производительности
int PerformancePushIndex;

std::vector<RDK::ULongTime> ModelAvg, TransportAvg;

///Данные производительности по каждому из каналов
//std::vector< std::vector< RDK::ULongTime > > perf_data;

///Усредненная производительность для построения графика
//std::vector< std::vector< RDK::ULongTime > > aver_perf_data;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UServerControl(void);
virtual ~UServerControl(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
/// Флаг разрешения запуска сервера при старте
bool GetAutoStartFlag(void) const;
bool SetAutoStartFlag(bool value);

/// Имя сервера
const std::string& GetServerName(void) const;
bool SetServerName(const std::string& value);

/// Идентификатор сервера
const std::string& GetServerId(void) const;
bool SetServerId(const std::string& value);

/// Число шагов усреднения оценки производительности
int GetAverageIterations(void) const;
bool SetAverageIterations(int value);

const std::string& GetMetaComponentName(void) const;
bool SetMetaComponentName(const std::string& value);

const std::string& GetMetaComponentStateName(void) const;
bool SetMetaComponentStateName(const std::string& value);

///Папка для сохранения отладочных данных
const std::string& GetDebugFolder(void) const;
bool SetDebugFolder(const std::string& value);

///Полный путь на одну сессию
const std::string& GetDebugOutputPath(void) const;
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Возвращает указатель на экземпляр приложения
UEPtr<UApplication> GetApplication(void);
bool SetApplication(UEPtr<UApplication> value);
// --------------------------
/// Возвращает указатель на экземпляр транспорта
UEPtr<UServerTransport> GetServerTransport(void);
bool SetServerTransport(UEPtr<UServerTransport> value);

UEPtr<UServerTransport> GetServerTransportHttp(void);
bool SetServerTransportHttp(UEPtr<UServerTransport> value);
// --------------------------

/// Предоставляет доступ к диспетчеру команд
virtual UEPtr<URpcDispatcher> GetRpcDispatcher(void);

/// Устанавливает новый диспетчер команд
/// Ответственность за освобождение памяти диспетчера лежит на вызывающей стороне
virtual bool SetRpcDispatcher(const UEPtr<URpcDispatcher> &value);

// --------------------------
// Данные для оценки производительности и сохранения отладочной информации
// --------------------------
/// Результаты измерений производительности, мс
const std::vector<std::vector<RDK::ULongTime> >& GetModelPerformanceResults(void) const;
const std::vector<std::vector<RDK::ULongTime> >& GetTransportPerformanceResults(void) const;

/// Средние значения затраченного времени на расчет модели и обмена
const std::vector<RDK::ULongTime>& GetModelAvg(void) const;
const std::vector<RDK::ULongTime>& GetTransportAvg(void) const;
// --------------------------


// --------------------------
// Методы управления
// --------------------------
/// Инициализация (выполняется первой)
virtual void Init(void);

/// Деинициализация (выполняется последней)
virtual void UnInit(void);

/// Проверка состояния инициализации
bool IsInit(void) const;
// --------------------------

// --------------------------
// Методы управления вещателями
// --------------------------
/// Регистрирует удаленный приемник метаданных
virtual int RegisterMetadataReceiver(const std::string &address, int port);

/// Удаляет удаленный приемник метаданных
virtual int UnRegisterMetadataReceiver(const std::string &address, int port);
// --------------------------

// --------------------------
// Общие методы управления контроллером
// --------------------------
// Сохраняет параметры интерфейса в xml
virtual void SaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void LoadParameters(RDK::USerStorageXML &xml);
// --------------------------

protected:
// --------------------------
// Вспомогательные методы
// --------------------------
/// Рассчитывает производительность сервера
/// и сохраняет на диск логи производительности
void CalculatePerformance(void);

// Метод, вызываемый после сброса модели
virtual void AfterReset(void);

// Метод, вызываемый после шага расчета
virtual void AfterCalculate(void);
// --------------------------

// --------------------------
// Вспомогательные методы (обработка взаимодействия с транспортом)
// --------------------------

public:
/// Отправляет ответ на команду
virtual void SendCommandResponse(UServerTransport *transport, std::string &client_address, RDK::UParamT &dest, std::vector<RDK::UParamT> &binary_data);
/// Отправляет ответ на команду
//virtual void ASendCommandResponse(RDK::UParamT &dest, std::vector<RDK::UParamT> &binary_data);

virtual void ProcessCommandQueue(UServerTransport *transport);

virtual void ProcessIncomingData(std::string &bind,UServerTransport *transport);

protected:


/// Кодирует строку в вектор
void ConvertStringToVector(const std::string &source, RDK::UParamT &dest);

/// Кодирует вектор в строку
void ConvertVectorToString(const RDK::UParamT &source, std::string &dest);

// --------------------------
/// Управление числом каналов
/// Выполнение вспомогательных методов
/// Вызывается из UApplication
// --------------------------
virtual bool SetNumChannels(int number);
virtual bool ASetNumChannels(int num);

virtual bool InsertChannel(int index);
virtual bool AInsertChannel(int index);

virtual bool DeleteChannel(int index);
virtual bool ADeleteChannel(int index);
// --------------------------
};

}

#endif
