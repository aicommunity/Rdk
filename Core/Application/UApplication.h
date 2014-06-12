#ifndef UApplication_H
#define UApplication_H

#include "UAppCore.h"
#include "UProject.h"
#include "UIVisualController.h"
#include "URpcDispatcher.h"
#include "URpcDecoder.h"
#include "URpcDecoderInternal.h"
#include "URpcCommand.h"
#include "URpcCommandInternal.h"

#ifdef __BORLANDC__
#include "Bcb/Application.bcb.h"
#endif

namespace RDK {

class UApplication
{
protected: // Данные приложения
/// Рабочий каталог
std::string WorkDirectory;

protected: // Модули приложения
/// Диспетчер команд
UEPtr<URpcDispatcher> RpcDispatcher;

/// Активный проект
UEPtr<UProject> Project;

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
// --------------------------

// --------------------------
// Методы инициализации
// --------------------------
/// Предоставляет доступ к диспетчеру команд
virtual UEPtr<URpcDispatcher> GetRpcDispatcher(void);

/// Устанавливает новый диспетчер команд
/// Ответственность за освобождение памяти диспетчера лежит на вызывающей стороне
virtual bool SetRpcDispatcher(const UEPtr<URpcDispatcher> &value);

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
};

}

#endif

