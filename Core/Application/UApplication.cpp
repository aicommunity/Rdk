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
 Name="Application";
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

// Признак наличия открытого проекта
bool UApplication::GetProjectOpenFlag(void) const
{
 return ProjectOpenFlag;
}

bool UApplication::SetProjectOpenFlag(bool value)
{
 ProjectOpenFlag=value;
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
 if(RpcDispatcher == value)
  return true;

 if(RpcDispatcher)
  RpcDispatcher->SetApplication(0);
 RpcDispatcher=value;
 RpcDispatcher->SetApplication(this);
 return true;
}

/// Предоставляет доступ к контроллеру движка
UEPtr<UEngineControl> UApplication::GetEngineControl(void)
{
 return EngineControl;
}

/// Устанавливает новый контроллер движка
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
bool UApplication::SetEngineControl(const UEPtr<UEngineControl> &value)
{
 if(EngineControl == value)
  return true;

 if(EngineControl)
  EngineControl->PauseEngine(-1);
 EngineControl=value;
 return true;
}

/// Предоставляет доступ к проекту
UEPtr<UProject> UApplication::GetProject(void)
{
 return Project;
}

/// Устанавливает новый проект
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
bool UApplication::SetProject(const UEPtr<UProject> &value)
{
 if(Project == value)
  return true;

 // TODO: Здесь какие-то завершающие действия со старым проектом.
 Project=value;
 return true;
}

/// Предоставляет доступ к контроллеру серверной части
UEPtr<UServerControl> UApplication::GetServerControl(void) const
{
 return ServerControl;
}

/// Устанавливает новый контроллер сервера
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
bool UApplication::SetServerControl(const UEPtr<UServerControl> &value)
{
 if(ServerControl == value)
  return true;

 // TODO: Здесь какие-то завершающие действия со старым сервером
 if(ServerControl)
  ServerControl->SetApplication(0);
 ServerControl=value;
 ServerControl->SetApplication(this);
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
 if(EngineControl)
  EngineControl->PauseEngine(-1);
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


// --------------------------
// Методы управления движком
// --------------------------
/// Управление числом каналов
int UApplication::GetNumEngines(void) const
{
 return EngineControl->GetNumEngines();
}

bool UApplication::SetNumEngines(int num)
{
 if(!EngineControl->SetNumEngines(num))
  return false;

 if(!ServerControl->SetNumEngines(num))
  return false;

 return true;
}

bool UApplication::InsertEngine(int index)
{
 if(!EngineControl->InsertEngine(index))
  return false;

 if(!ServerControl->InsertEngine(index))
  return false;

 return true;
}

bool UApplication::DeleteEngine(int index)
{
 if(!EngineControl->DeleteEngine(index))
  return false;

 if(!ServerControl->DeleteEngine(index))
  return false;

 return true;
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
/// Запускает аналитику выбранного канала, или всех, если engine_index == -1
void UApplication::StartEngine(int engine_index)
{
 EngineControl->StartEngine(engine_index);
}

/// Останавливает аналитику выбранного канала, или всех, если engine_index == -1
void UApplication::PauseEngine(int engine_index)
{
 EngineControl->PauseEngine(engine_index);
}

/// Сбрасывает аналитику выбранного канала, или всех, если engine_index == -1
void UApplication::ResetEngine(int engine_index)
{
 EngineControl->ResetEngine(engine_index);
}

/// Делает шаг расчета выбранного канала, или всех, если engine_index == -1
void UApplication::StepEngine(int engine_index)
{
 EngineControl->StepEngine(engine_index);
}
// --------------------------


}

#endif

