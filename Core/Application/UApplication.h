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

#ifdef __BORLANDC__
#include "Bcb/Application.bcb.h"
#endif

namespace RDK {

class UEngineControl;
class UServerControl;

class RDK_LIB_TYPE UApplication: public UAppController
{
protected: // Данные приложения
/// Рабочий каталог
std::string WorkDirectory;

// Признак наличия открытого проекта
bool ProjectOpenFlag;

protected: // Модули приложения
/// Диспетчер команд
UEPtr<URpcDispatcher> RpcDispatcher;

/// Активный проект
UEPtr<UProject> Project;

/// Контроллер движка
UEPtr<UEngineControl> EngineControl;

/// Контроллер серверной части
UEPtr<UServerControl> ServerControl;

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
/// Рабочий каталог
const std::string& GetWorkDirectory(void) const;
bool SetWorkDirectory(const std::string& value);

// Признак наличия открытого проекта
bool GetProjectOpenFlag(void) const;
bool SetProjectOpenFlag(bool value);
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
virtual UEPtr<UProject> GetProject(void);

/// Устанавливает новый проект
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
virtual bool SetProject(const UEPtr<UProject> &value);

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
virtual bool CreateProject(const std::string &filename);

/// Открывает проект
virtual bool OpenProject(const std::string &filename);

/// Сохраняет проект
virtual bool SaveProject(void);
virtual bool SaveProjectAs(const std::string &filename);

/// Закрывает проект
virtual bool CloseProject(void);

/// Клонирует проект в новое расположение
virtual bool CloneProject(const std::string &filename);
// --------------------------

// --------------------------
// Методы управления движком
// --------------------------
/// Управление числом каналов
int GetNumEngines(void) const;
bool SetNumEngines(int num);
bool InsertEngine(int index);
bool DeleteEngine(int index);
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
/// Запускает аналитику выбранного канала, или всех, если engine_index == -1
virtual void StartEngine(int engine_index);

/// Останавливает аналитику выбранного канала, или всех, если engine_index == -1
virtual void PauseEngine(int engine_index);

/// Сбрасывает аналитику выбранного канала, или всех, если engine_index == -1
virtual void ResetEngine(int engine_index);

/// Делает шаг расчета выбранного канала, или всех, если engine_index == -1
virtual void StepEngine(int engine_index);
// --------------------------
};

}

#endif

