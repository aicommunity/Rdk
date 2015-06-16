#ifndef UApplication_CPP
#define UApplication_CPP

#include "UApplication.h"

//#include "UAppCore.cpp"
//#include "UProject.cpp"
//#include "UIVisualController.cpp"
//#include "URpcDispatcherQueues.cpp"
//#include "URpcDispatcher.cpp"
//#include "URpcDecoder.cpp"
//#include "URpcDecoderInternal.cpp"
//#include "URpcCommand.cpp"
//#include "URpcCommandInternal.cpp"

              
#ifdef __BORLANDC__
//#include "Bcb/Application.bcb.cpp"
#endif

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UApplication::UApplication(void)
{

}

UApplication::~UApplication(void)
{

}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Рабочий каталог
const std::string& UApplication::GetWorkDirectory(void) const
{
 return WorkDirectory;
}

bool UApplication::SetWorkDirectory(const std::string& value)
{
 WorkDirectory=value;
 return true;
}
// --------------------------

// --------------------------
// Методы инициализации
// --------------------------
/// Предоставляет доступ к диспетчеру команд
UEPtr<URpcDispatcher> UApplication::GetRpcDispatcher(void)
{
 return RpcDispatcher;
}

/// Устанавливает новый диспетчер команд
/// Ответственность за освобождение памяти диспетчера лежит на вызывающей стороне
bool UApplication::SetRpcDispatcher(const UEPtr<URpcDispatcher> &value)
{
 RpcDispatcher=value;
 return true;
}

/// Инициализирует приложение
bool UApplication::Init(void)
{
 return true;
}

/// Деинициализирует приложение
bool UApplication::UnInit(void)
{
 return true;
}
// --------------------------

// --------------------------
// Методы управления проектом
// --------------------------
/// Создает проект (через сохранение и открытие)
bool UApplication::CreateProject(const std::string &filename)
{
 return true;
}

/// Открывает проект
bool UApplication::OpenProject(const std::string &filename)
{
 return true;
}

/// Сохраняет проект
bool UApplication::SaveProject(void)
{
 return true;
}

bool UApplication::SaveProjectAs(const std::string &filename)
{
 return true;
}

/// Закрывает проект
bool UApplication::CloseProject(void)
{
 return true;
}

/// Клонирует проект в новое расположение
bool UApplication::CloneProject(const std::string &filename)
{
 return true;
}
// --------------------------

}

#endif

