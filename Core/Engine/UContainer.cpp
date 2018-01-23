/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAContainerCPP
#define UAContainerCPP

#include <algorithm>
#include <string.h>
#include <cstdio>
#include "UContainer.h"
#include "UStorage.h"
#include "UConnector.h"
#include "UItem.h"
#include "UNet.h"
#include "../Serialize/Serialize.h"
#include "../Math/MUBinarySerialize.h"
#include "../Math/MUXMLSerialize.h"
#include "UStorage.h"
#include "UContainerDescription.h"
#include "UEnvironment.h"
#include "../../Deploy/Include/rdk_exceptions.h"
#include "UEnvException.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPVariable::UPVariable(void)
{
 Id=ForbiddenId;
}

UPVariable::UPVariable(UId id, UEPtr<UIPointer> prop)
 : Id(id), Pointer(prop)
{

}

UPVariable::~UPVariable(void)
{

}
// --------------------------


/* *************************************************************************** */
// Class UContainer
/* *************************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UContainer::UContainer(void)
 : Id(0), Activity(false), Coord(0), PComponents(0), NumComponents(0), LastId(0),
   MemoryMonitor("MemoryMonitor",this,&UContainer::SetMemoryMonitor)
{
 AddLookupProperty("Id",ptParameter | pgSystem,new UVProperty<UId,UContainer>(this,&UContainer::SetId,&UContainer::GetId));
 AddLookupProperty("Name",ptParameter | pgSystem,new UVProperty<NameT,UContainer>(this,&UContainer::SetName,&UContainer::GetName));
 AddLookupProperty("TimeStep",ptParameter | pgSystem,new UVProperty<UTime,UContainer>(this,&UContainer::SetTimeStep,&UContainer::GetTimeStep));
 AddLookupProperty("Activity",ptParameter | pgPublic,new UVProperty<bool,UContainer>(this,&UContainer::SetActivity,&UContainer::GetActivity));
 AddLookupProperty("Coord",ptParameter | pgPublic,new UVProperty<RDK::MVector<double,3>,UContainer>(this,&UContainer::SetCoord,&UContainer::GetCoord));
 AddLookupProperty("MaxCalculationDuration",ptParameter | pgPublic,new UVProperty<long long,UContainer>(this,&UContainer::SetMaxCalculationDuration,&UContainer::GetMaxCalculationDuration));
 AddLookupProperty("CalculationDurationThreshold",ptParameter | pgPublic,new UVProperty<long long,UContainer>(this,&UContainer::SetCalculationDurationThreshold,&UContainer::GetCalculationDurationThreshold));
 AddLookupProperty("DebugSysEventsMask",ptParameter | pgPublic,new UVProperty<unsigned int,UContainer>(this,&UContainer::SetDebugSysEventsMask,&UContainer::GetDebugSysEventsMask));

 InitFlag=false;

 CalculationDurationThreshold= -1;
 MaxCalculationDuration = -1;
 MovingFlag=false;
}

UContainer::~UContainer(void)
{
 if(GetStaticFlag() && Owner)
  GetOwner()->DelStaticComponent(this);
 DelAllComponentsRaw();
 DelAllStaticComponents();


 UnLinkAllControllers();

 BreakOwner();

 if(Storage)
  Storage->PopObject(UEPtr<UContainer>(this));
}
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта
UEPtr<UContainer> UContainer::GetOwner(void) const
{
 return dynamic_pointer_cast<UContainer>(Owner);
}

// Возвращает указатель на главного владельца этим объектом
UEPtr<UContainer> UContainer::GetMainOwner(void) const
{
 return dynamic_pointer_cast<UContainer>(MainOwner);
}

// Возвращает хранилище компонент этого объекта
UEPtr<UStorage> const UContainer::GetStorage(void) const
{
 return Storage;
}

// Проверяет, является ли объект owner
// владельцем этого объекта на каком-либо уровне иерархии
bool UContainer::CheckOwner(UEPtr<UContainer> owner) const
{
 if(Owner == 0 && Owner != owner)
  return false;

 if(Owner == owner)
  return true;

 return GetOwner()->CheckOwner(owner);
}

// Возвращает полный Id объекта
// (включая Id всех владельцев).
ULongId& UContainer::GetFullId(ULongId &buffer) const
{
 if(Owner == 0)
  {
   buffer.Add(Id);
   return buffer;
  }

 GetOwner()->GetFullId(buffer);
 buffer.Add(Id);

 return buffer;
}

ULongId UContainer::GetFullId(void) const
{
 ULongId id;

 return GetFullId(id);
}


// Возвращает  'длинный' Id объекта
// (исключая имя владельца 'mainowner').
// Метод возвращает пустой вектор, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии.
ULongId& UContainer::GetLongId(UEPtr<UContainer> mainowner, ULongId &buffer) const
{
 if(Owner == 0 && Owner != mainowner)
  {
   buffer.Resize(0);
   return buffer;
  }

 if(Owner == mainowner)
  {
   buffer.Resize(0);
   buffer.Add(Id);
   return buffer;
  }

 if(GetOwner()->GetLongId(mainowner,buffer).GetSize() == 0)
  return buffer;

 buffer.Add(Id);

 return buffer;
}

ULongId UContainer::GetLongId(UEPtr<UContainer> mainowner) const
{
 ULongId id;

 return GetLongId(mainowner,id);
}


// Промежуточный вариант одноименного метода, возвращающего длинное имя
std::string& UContainer::GetLongId(UEPtr<UContainer> mainowner, std::string &buffer) const
{
 return GetLongName(mainowner,buffer);
}

// Возвращает true если передаваемый идентификатор объекта корректен, в противном случае возвращает false
bool UContainer::CheckLongId(const ULongId &id) const
{
 if(id.GetSize() == 0 || id[0] == ForbiddenId)
  return false;

 return true;
}

// Промежуточный вариант одноименного метода, обрабатывающего длинное имя
bool UContainer::CheckLongId(const std::string &id) const
{
 if(id.size() == 0)
  return false;

 return true;
}

// Управление средой выполнения этого объекта
bool UContainer::SetEnvironment(UEPtr<UEnvironment> environment)
{
 if(!UComponent::SetEnvironment(environment))
  return false;

 bool res=true;

 for(int i=0;i<NumComponents;i++)
  res&=PComponents[i]->SetEnvironment(environment);

 return res;
}

// Указатель на логгер
bool UContainer::SetLogger(UEPtr<ULoggerEnv> logger)
{
 if(!UComponent::SetLogger(logger))
  return false;

 bool res=true;

 for(int i=0;i<NumComponents;i++)
  res&=PComponents[i]->SetLogger(logger);

 return res;
}

// Вызов обработчика исключений среды
void UContainer::ProcessException(UException &exception)
{
 if(Logger)
  Logger->ProcessException(exception);
 else
  throw exception;
}


// Вызов обработчика исключений среды для простой записи данных в лог
void UContainer::LogMessage(int msg_level, const std::string &line, int error_event_number)
{
 if(Logger)
 {
  Logger->LogMessageEx(msg_level, GetFullName(), line, error_event_number);
 }
}

void UContainer::LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number)
{
 if(Logger)
 {
  Logger->LogMessageEx(msg_level, GetFullName(), method_name, line, error_event_number);
 }
}

void UContainer::LogMessageEx(int msg_level, const std::string &line, int error_event_number)
{
 if(Logger)
 {
  std::string full_name;
  GetFullName(full_name);
  Logger->LogMessageEx(msg_level, full_name, full_name+std::string(" - ")+line, error_event_number);
 }
}

void UContainer::LogMessageEx(int msg_level, const std::string &method_name, const std::string &line, int error_event_number)
{
 if(Logger)
 {
  std::string full_name;
  GetFullName(full_name);
  Logger->LogMessageEx(msg_level, full_name, method_name, full_name+std::string(" - ")+line, error_event_number);
 }
}

void UContainer::LogMessageEx(int msg_level, const std::string &method_name, int line, int error_event_number)
{
 std::string str_line = sntoa(line);
 LogMessageEx(msg_level, method_name, str_line, error_event_number);
}

void UContainer::LogDebugSysMessage(unsigned long long debug_sys_msg_type, unsigned long long modifier)
{
 if(Logger && Logger->GetDebugMode() && (Logger->GetDebugSysEventsMask() & (debug_sys_msg_type & DebugSysEventsMask)))
 {
  std::string prefix;
  switch(debug_sys_msg_type)
  {
  case RDK_SYS_DEBUG_CALC:
   prefix="Calculate: ";
  break;

  case RDK_SYS_DEBUG_RESET:
   prefix="Reset: ";
  break;
  }

  std::string suffix;
  switch(modifier)
  {
  case RDK_SYS_MESSAGE_ENTER:
   suffix="Enter";
  break;

  case RDK_SYS_MESSAGE_EXIT_OK:
   suffix="Exit: OK";
  break;

  case RDK_SYS_MESSAGE_EXIT_ININIT_FAIL:
   suffix="Exit: InInit == false";
  break;

  case RDK_SYS_MESSAGE_NEW_CALC_ITERATION:
   suffix="<======== NEW ITERATION ========>";
  break;
  }

  LogMessageEx(RDK_EX_DEBUG, prefix+suffix);
 }
}

/// Логирует свойства при входе в расчет (входы, параметры, состояния)
void UContainer::LogPropertiesBeforeCalc(void)
{
 if(Logger && Logger->GetDebugMode() && (Logger->GetDebugSysEventsMask() & (RDK_SYS_DEBUG_PROPERTIES & DebugSysEventsMask)))
 {
  std::string log_message;

  if(PropertiesForDetailedLog.empty())
  {
   VariableMapCIteratorT I=PropertiesLookupTable.begin(),J=PropertiesLookupTable.end();
   for(; I != J; ++I)
   {
	if(I->second.GetPropertyType() & ptInput)
	 if(PreparePropertyLogString(I->second, ptInput, log_message))
	  LogMessageEx(RDK_EX_DEBUG, log_message);
   }

   I=PropertiesLookupTable.begin();
   for(; I != J; ++I)
   {
	if(I->second.GetPropertyGroup() != pgPublic)
	 continue;

	if(I->second.GetPropertyType() & ptParameter)
	 if(PreparePropertyLogString(I->second, ptParameter, log_message))
	  LogMessageEx(RDK_EX_DEBUG, log_message);
   }
  }
  else
  {
   for(size_t i=0;i<PropertiesForDetailedLog.size();i++)
   {
	VariableMapCIteratorT I=PropertiesLookupTable.find(PropertiesForDetailedLog[i]);
	if(PreparePropertyLogString(I->second, ptInput | ptParameter, log_message))
 	 LogMessageEx(RDK_EX_DEBUG, log_message);
   }
  }
 }
}

/// Логирует свойства при выходе из расчета (выходы)
void UContainer::LogPropertiesAfterCalc(void)
{
 if(Logger && Logger->GetDebugMode() && (Logger->GetDebugSysEventsMask() & (RDK_SYS_DEBUG_PROPERTIES & DebugSysEventsMask)))
 {
  std::string log_message;
  if(PropertiesForDetailedLog.empty())
  {
   VariableMapCIteratorT I=PropertiesLookupTable.begin(),J=PropertiesLookupTable.end();
   for(; I != J; ++I)
   {
	if(I->second.GetPropertyType() & ptOutput)
	 if(PreparePropertyLogString(I->second, ptOutput, log_message))
	  LogMessageEx(RDK_EX_DEBUG, log_message);
   }

   I=PropertiesLookupTable.begin();
   for(; I != J; ++I)
   {
	if(I->second.GetPropertyGroup() != pgPublic)
	 continue;

	if(I->second.GetPropertyType() & ptState && !(I->second.GetPropertyType() & ptOutput)  && !(I->second.GetPropertyType() & ptInput))
	 if(PreparePropertyLogString(I->second, ptState, log_message))
	  LogMessageEx(RDK_EX_DEBUG, log_message);
   }

  }
  else
  {
   for(size_t i=0;i<PropertiesForDetailedLog.size();i++)
   {
	VariableMapCIteratorT I=PropertiesLookupTable.find(PropertiesForDetailedLog[i]);
	if(PreparePropertyLogString(I->second, ptOutput | ptState, log_message))
	 LogMessageEx(RDK_EX_DEBUG, log_message);
   }
  }
 }
}

/// Возвращает состояние флага режима отладки
bool UContainer::CheckDebugMode(void) const
{
 if(Logger)
 {
  return Logger->GetDebugMode();
 }
 return false;
}


/// Формирует список свйоств для детального лога из строки
/// Разделитель - запятая
void UContainer::SetPropertiesForDetailedLog(const std::string &str)
{
 PropertiesForDetailedLog.clear();
 RDK::separatestring(str, PropertiesForDetailedLog, ',');
}
// --------------------------

// --------------------------
// Методы управления свойствами
// --------------------------
// Координата компонента в пространстве сети
const RDK::MVector<double,3>& UContainer::GetCoord(void) const
{
 return Coord;
}

bool UContainer::SetCoord(const RDK::MVector<double,3> &value)
{
 if(Coord != value)
  Coord=value;

 return true;
}


// Время, затраченное на обработку объекта
// (без учета времени обсчета дочерних объектов) (мс)
unsigned long long UContainer::GetStepDuration(void) const
{
 unsigned long long res=0;
 for(int i=0;i<NumComponents;i++)
  res+=PComponents[i]->GetFullStepDuration();

 return StepDuration-res;
}

// Время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
unsigned long long UContainer::GetFullStepDuration(void) const
{
 return StepDuration;
}

// Время, прошедшее между двумя последними итерациями счета
unsigned long long UContainer::GetInterstepsInterval(void) const
{
 return InterstepsInterval;
}

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double UContainer::GetInstantPerformance(void) const
{
 return ((GetFullStepDuration()*TimeStep)/1000.0);
}

// Удаляет владельца объекта
void UContainer::BreakOwner(void)
{
 UEPtr<UContainer> owner=GetOwner();
 if(owner)
  owner->DelComponent(this,false);
}

// Устанавливает указатель на главного владельца этим объектом
// Указатель устанавливается на число уровней дочерних компонент
// 'levels'. Если levels < 0 то устанавливается компонентам на всех уровнях
void UContainer::SetMainOwner(UEPtr<UComponent> mainowner)
{
 UComponent::SetMainOwner(mainowner);
}

void UContainer::SetMainOwner(UEPtr<UComponent> mainowner, int levels)
{
 if(MainOwner == mainowner && !levels)
  return;

 MainOwner=mainowner;

 if(!levels)
  return;

 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++, comps++)
 {
  // Устанавливаем главного владельца только тем дочерним компонентам
  // у которых он еще не задан
  if((*comps)->GetMainOwner() == 0)
  {
   if(levels<0)
    (*comps)->SetMainOwner(MainOwner,levels);
   else
    (*comps)->SetMainOwner(MainOwner,levels-1);
  }
 }
}

// Проверяет предлагаемый Id 'id' на уникальность в рамках данного, объекта.
bool UContainer::CheckId(const UId &id)
{
 return (id>LastId)?true:false;
}

bool UContainer::CheckComponent(const NameT &name)
{
  return !CheckName(name);
}

bool UContainer::CheckComponentL(const NameT &name)
{
 if(GetComponentL(name,true) != 0)
  return true;
 return false;
}

// Проверяет предлагаемое имя 'name' на уникальность в рамках
// данного объекта.
bool UContainer::CheckName(const NameT &name)
{
 if(CompsLookupTable.find(name) == CompsLookupTable.end())
  return true;

 return false;
}

// Проверяет предлагаемое имя 'name' на синтаксическую корректность
bool UContainer::ValidateName(const NameT &name)
{
 if(name.empty())
  return false;
 if(name.find_first_of(" ") != std::string::npos)
  return false;
 std::locale loc;
 if(!std::isalpha(*name.begin(),loc))
  return false;
 for (std::string::const_iterator it=name.begin(); it!=name.end(); ++it)
  {
	if (!std::isalnum(*it,loc) && *it != '_' && *it != '-')
	 return false;
  }
 return true;
}

// Генерирует уникальный Id.
UId UContainer::GenerateId(void)
{
 return LastId+1;
}

#ifdef __BORLANDC__
#pragma warning (disable : 4996)
#endif
// Генерирует имя уникальное в компонентах этого объекта
NameT& UContainer::GenerateName(const NameT &prefix, NameT &namebuffer)
{
 int k=2;
 //char buffer[20];

 while(true)
  {
   std::stringstream stream;
   stream<<k;

//   std::sprintf(buffer,"%i",k);
   //stream.str();
   if(prefix.size()>0)
    namebuffer=prefix+stream.str();
   else
    namebuffer=NameT("n")+stream.str();
   if(CheckName(namebuffer))
    break;

   k++;

   if(k == 0) // Заглушка!! Должно быть исключение - ресурсы исчерпаны
    return namebuffer;
  }

 return namebuffer;
}
#ifdef __BORLANDC__
#pragma warning (default : 4996)
#endif

// Устанавливает имя объекта.
const NameT& UContainer::GetName(void) const
{
 return Name;
}

bool UContainer::SetName(const NameT &name)
{
 if(Name == name)
  return true;

 if(name.empty())
  return false;

 if(!ValidateName(name))
  RDK_THROW(EComponentNameInvalid(name));

  if(GetOwner() != 0)
  {
   if(!GetOwner()->CheckName(name))
	RDK_THROW(EComponentNameAlreadyExist(name));

   GetOwner()->ModifyLookupComponent(Name, name);
  }
 Name=name;
 return true;
}

// Возвращает полное имя объекта
// (включая имена всех владельцев).
NameT& UContainer::GetFullName(NameT &buffer) const
{
 if(!GetOwner())
  {
   buffer+=Name;
   return buffer;
  }

 GetOwner()->GetFullName(buffer);
 buffer+='.';
 buffer+=Name;

 return buffer;
}

NameT UContainer::GetFullName(void) const
{
 std::string buf;
 return GetFullName(buf);
}

// Возвращает  'длинное' имени объекта
// (исключая имя владельца 'mainowner').
// Метод возвращает пустую строку, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии.
NameT& UContainer::GetLongName(const UEPtr<UContainer> &mainowner, NameT &buffer) const
{
 if(!GetOwner() && GetOwner() != mainowner)
  {
   buffer.clear();
   return buffer;
  }

 if(GetOwner() == mainowner)
  {
   buffer=Name;
   return buffer;
  }

 if(GetOwner()->GetLongName(mainowner,buffer) == ForbiddenName)
  {
   buffer.clear();
   return buffer;
  }

 buffer+='.';
 buffer+=Name;

 return buffer;
}

NameT UContainer::GetLongName(const UEPtr<UContainer> &mainowner) const
{
 std::string buf;
 return GetLongName(mainowner,buf);
}
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает имя дочернего компонента по его Id
const NameT& UContainer::GetComponentName(const UId &id) const
{
 for(std::map<NameT,UId>::const_iterator I=CompsLookupTable.begin(),
								 J=CompsLookupTable.end(); I!=J; ++I)
 {
  if(I->second == id)
   return I->first;
 }
 RDK_THROW(EComponentIdNotExist(id));
}

// Возвращает Id дочернего компонента по его имени
const UId& UContainer::GetComponentId(const NameT &name, bool nothrow) const
{
 std::map<NameT,UId>::const_iterator I=CompsLookupTable.find(name);
 if(I == CompsLookupTable.end())
 {
  if(nothrow)
   return ForbiddenId;
  RDK_THROW(EComponentNameNotExist(name));
 }

 return I->second;
}

// Возвращает имя локального указателя по его Id
const NameT& UContainer::GetPointerName(const UId &id) const
{
 for(PointerMapCIteratorT I=PointerLookupTable.begin(),
                                 J=PointerLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Id == id)
   return I->first;
 }
 RDK_THROW(EPointerIdNotExist(id));
}

// Возвращает Id локального указателя по его имени
const UId& UContainer::GetPointerId(const NameT &name) const
{
 PointerMapCIteratorT I=PointerLookupTable.find(name);
 if(I == PointerLookupTable.end())
  RDK_THROW(EPointerNameNotExist(name));

 return I->second.Id;
}

// Осуществляет поиск всех компонент по заданному имени класса
// и возвращает вектор длинных имен компонент либо пустой вектор
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
const vector<UEPtr<UContainer> >& UContainer::GetComponentsByClassName(const NameT &name, vector<UEPtr<UContainer> > &buffer, bool find_all)
{
 size_t numComp=GetNumComponents();
 UEPtr<UContainer> comp;

 switch(find_all)
 {
  case false:
   for(size_t i=0; i<numComp; i++)
   {
	comp=GetComponentByIndex(i);
	if( comp->GetCompClassName() == name )
	{
	 buffer.push_back(comp);
	}
   }
   break;

  case true:
   for(size_t i=0; i<numComp; i++)
   {
	comp=GetComponentByIndex(i);
	comp->GetComponentsByClassName(name, buffer, true);
	if( comp->GetCompClassName() == name )
	{
	 buffer.push_back(comp);
	}
   }
   break;
 }

 return buffer;
}

// Осуществляет поиск всех компонент по заданному имени класса
// и возвращает вектор длинных имен компонент либо пустой вектор
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
const vector<NameT>& UContainer::GetComponentsNameByClassName(const NameT &name, vector<NameT> &buffer, bool find_all)
{
 vector<UEPtr<UContainer> > components;
 string compName;
 GetComponentsByClassName(name, components, find_all);
 size_t numComp=components.size();

 for(size_t i=0; i<numComp; i++)
 {
  compName=components[i]->GetLongName(this, compName);
  buffer.push_back(compName);
 }

 return buffer;
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает величину шага интегрирования
const UTime& UContainer::GetTimeStep(void) const
{
 return TimeStep;
}

bool UContainer::SetTimeStep(const UTime &timestep)
{
 if(timestep <= 0)
  return false;

 TimeStep=timestep;

 if(Owner)
  OwnerTimeStep=GetOwner()->TimeStep;
 else
  OwnerTimeStep=timestep;

 // Обращение ко всем компонентам объекта
 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->OwnerTimeStep=timestep;

 return true;
}

// Устанавливает величину шага интегрирования компоненту и всем его дочерним компонентам
bool UContainer::SetGlobalTimeStep(UTime timestep)
{
 if(!SetTimeStep(timestep))
  return false;

 // Обращение ко всем компонентам объекта
 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(!(*comps)->SetGlobalTimeStep(timestep))
   return false;

 return true;
}



// Устанавливает флаг активности объекта
const bool& UContainer::GetActivity(void) const
{
 return Activity;
}

bool UContainer::SetActivity(const bool &activity)
{
 if(Activity == activity)
  return true;

 Activity=true;
 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->SetActivity(activity);

// if(activity)
//  return Reset(); // !!! Заглушка. Возможно это не нужно!

 Activity=activity;
 StepDuration=0;
 InterstepsInterval=0;

 if(!Activity)
  return Reset();

 return true;
}

// Id объекта
const UId& UContainer::GetId(void) const
{
 return Id;
}

bool UContainer::SetId(const UId &id)
{
 if(Id == id)
  return true;

 if(id == ForbiddenId)
  return true;// Заглушка!! Это хак! throwEForbiddenId(id);

 if(id < 0)
  RDK_THROW(EInvalidId(id));


 if(Owner != 0)
  {
   if(!GetOwner()->CheckId(id))
    RDK_THROW(EComponentIdAlreadyExist(id));

   GetOwner()->SetLookupComponent(Name, id);
  }
 Id=id;
 return true;
}

/// Максимально допустимое время расчета компонента вместе с дочерними компонентами
/// в миллисекундах.
/// Если время расчета превышено, то расчет последующих дочерних компонент
/// не выполняется
/// Если значение параметра <0, то нет ограничений
const long long& UContainer::GetMaxCalculationDuration(void) const
{
 return MaxCalculationDuration;
}

bool UContainer::SetMaxCalculationDuration(const long long &value)
{
 if(MaxCalculationDuration == value)
  return true;

 MaxCalculationDuration=value;
 return true;
}

/// Время расчета компонента вместе с дочерними компонентами
/// в миллисекундах, по превышении которого выдается предупреждающее сообщение в лог.
/// Если значение параметра <0, то нет ограничений
const long long& UContainer::GetCalculationDurationThreshold(void) const
{
 return CalculationDurationThreshold;
}

bool UContainer::SetCalculationDurationThreshold(const long long& value)
{
 if(CalculationDurationThreshold == value)
  return true;

 CalculationDurationThreshold=value;
 return true;
}


/// Флаги переопределения настроек вывода детальной отладочной информации
const unsigned int& UContainer::GetDebugSysEventsMask(void) const
{
 return DebugSysEventsMask;
}

bool UContainer::SetDebugSysEventsMask(const unsigned int &value)
{
 if(DebugSysEventsMask == value)
  return true;
 DebugSysEventsMask=value;
 return true;
}

bool UContainer::SetMemoryMonitor(const bool &value)
{
 if(value == true && Logger)
  Logger->LogMessageEx(RDK_EX_WARNING, __FUNCTION__, GetFullName()+": Memory monitor enabled. Performance warning.");
 else
 {
  MemoryUsageDiff=0;
  MaxMemoryBlockDiff=0;
 }
 return true;
}


/// Объем потребленной памяти за шаг расчета.
/// Может быть отрицательрным если память освобождалась.
/// Актуально если включен флаг MemoryMonitor
long long UContainer::GetMemoryUsageDiff(void) const
{
 return MemoryUsageDiff;
}

/// Изменение максимально длинного куска доступной памяти после шага расчета
/// Может быть отрицательрным если кусок увеличился.
/// Актуально если включен флаг MemoryMonitor
long long UContainer::GetMaxMemoryBlockDiff(void) const
{
 return MaxMemoryBlockDiff;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Создает копию этого объекта с сохранением всех компонент
// и значений параметров.
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
UEPtr<UContainer> UContainer::Alloc(UEPtr<UStorage> stor, bool copystate)
{
 UEPtr<UContainer> copy;
 UEPtr<UStorage> storage=(stor!=0)?stor:GetStorage();

 if(storage)
 {
  copy=dynamic_pointer_cast<UContainer>(storage->TakeObject(Class,this));
 }
 else
 {
  copy=New();
  Copy(copy,stor,copystate);
 }

 return copy;
}

// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
bool UContainer::Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor, bool copystate) const
{
 CopyProperties(target, ptParameter);
 target->Build();

 if(copystate)
  CopyProperties(target, ptState);

 CopyComponents(target,stor);
 return true;
}

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UContainer::Free(void)
{
 while(NumComponents)
  PComponents[0]->Free();

 if(Storage)
 {
  BreakOwner();
  if(!StaticFlag)
   GetStorage()->ReturnObject(this);
 }
 else
  UComponent::Free();
}

/// Осуществляет обновление внутренних данных компонента, обеспечивающих его целостность
void UContainer::AUpdateInternalData(void)
{
 std::map<UEPtr<UContainer>, NameT>::iterator I=StaticComponents.begin();
 for(;I!=StaticComponents.end();++I)
 {
  UpdateStaticComponent(I->second,I->first);
 }
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Возвращает число дочерних компонент
int UContainer::GetNumComponents(void) const
{
 return NumComponents;
}


// Возвращает полное число дочерних компонент
// (включая все компоненты дочерних компонент)
int UContainer::GetNumAllComponents(void) const
{
 int res=NumComponents;

 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  res+=(*comps)->GetNumAllComponents();

 return res;
}


// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool UContainer::CheckComponentType(UEPtr<UContainer> comp) const
{
 return false;
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому Id 'id'
// Если id == ForbiddenId то возвращает указатель на этот компонент
UEPtr<UContainer> UContainer::GetComponent(const UId &id, bool nothrow) const
{
 if(id == ForbiddenId)
 {
  if(nothrow)
   return 0;
  RDK_THROW(EComponentIdNotExist(id));
 }

 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(id == (*comps)->Id)
   return *comps;

 if(nothrow)
  return 0;
 RDK_THROW(EComponentIdNotExist(id));
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому имени 'name'
UEPtr<UContainer> UContainer::GetComponent(const NameT &name, bool nothrow) const
{
 return GetComponent(GetComponentId(name,nothrow),nothrow);
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по ДЛИННОМУ Id 'id'.
// Если id[0] == ForbiddenId или Id имеет нулевой размер,
// то возвращает указатель на этот компонент
UEPtr<UContainer> UContainer::GetComponentL(const ULongId &id, bool nothrow) const
{
 UEPtr<UContainer> comp;

 if(id.GetSize() == 0)
  return 0;

 comp=GetComponent(id[0],nothrow);
 for(int i=1;i<id.GetSize();i++)
  {
   if(!comp)
	return 0;
   comp=comp->GetComponent(id[i],nothrow);
  }
 return comp;
}


// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по ДЛИННОМУ имени 'name'
UEPtr<UContainer> UContainer::GetComponentL(const NameT &name, bool nothrow) const
{
 UEPtr<UContainer> comp;
 NameT::size_type pi,pj;

 pi=name.find_first_of('.');
 if(pi == NameT::npos)
  return GetComponent(name,nothrow);

 comp=GetComponent(name.substr(0,pi),nothrow);
 while(pi != name.size())
  {
   if(!comp)
	return 0;
   pj=pi+1;
   pi=name.find_first_of('.',pj);
   if(pi == NameT::npos)
	pi=name.size();
   comp=comp->GetComponent(name.substr(pj,pi-pj),nothrow);
  }
 return comp;
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по порядковому индеку в списке компонент
// Метод возвращает 0, если индекс выходит за границы массива
UEPtr<UContainer> UContainer::GetComponentByIndex(int index) const
{
 return Components[index];
}

// Добавляет дочерний компонент в этот объект
// Возвращает его Id или ForbiddenId если добавление неудачно
// Может быть передан указатель на локальную переменную
void UContainer::BeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 ABeforeAddComponent(comp,pointer);
}

void UContainer::AfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 AAfterAddComponent(comp,pointer);
}

UId UContainer::AddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 if(comp->GetOwner() == this)
  return comp->Id;

 if(comp->GetOwner())
  RDK_THROW(EAddComponentAlreadyHaveOwner(comp->Id));

 if(!CheckComponentType(comp))
  RDK_THROW(EAddComponentHaveInvalidType(comp->Id));

 BeforeAddComponent(comp,pointer);

 NameT namebuffer;

 if(!CheckName(comp->Name))
  comp->SetName(GenerateName(comp->Name,namebuffer));
 UId id=GenerateId();

 bool res=true;
 int i=0;
 for(i=0;i<NumComponents;i++)
  if(PComponents[i]->Id == id)
  {
   res=false;
   break;
  }

 if(!res)
  RDK_THROW(EComponentIdAlreadyExist(id));

 comp->SetLogger(Logger);
 comp->SetId(id);
 comp->SetOwner(this);

 // Добавляем компонент в таблицу соответствий владельца
 SetLookupComponent(comp->Name, comp->Id);

 // Добавление в базу компонент
 AddComponentTable(comp,pointer);

 comp->OwnerTimeStep=TimeStep;

 if(MainOwner)
  comp->SetMainOwner(MainOwner);

 comp->SetEnvironment(Environment);

 try{
  AAddComponent(comp,pointer);
  comp->SharesInit();
  AfterAddComponent(comp,pointer);
 }
 catch(UException &exception)
 {
  // Откат
  BeforeDelComponent(comp);
  comp->SharesUnInit();
  // Удаляем компонент из таблицы соответствий владельца
  DelLookupComponent(comp->Name);

  // Удаление из базы компонент
  DelComponentTable(comp);

  comp->Owner=0;
  throw;
 }

 return comp->Id;
}

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Таким образом 'id' - должно быть коротким Id
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
/*void UContainer::DelComponent(const UId &id, bool canfree)
{
 UEPtr<UContainer> comp=GetComponent(id);

 DelComponent(comp, canfree);
} */

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
void UContainer::DelComponent(const NameT &name, bool canfree)
{
 UEPtr<UContainer> comp=GetComponentL(name,true);
 if(comp && comp->GetOwner())
  comp->GetOwner()->DelComponent(comp,canfree);
}

// Принудительно удаляет все дочерние компоненты
void UContainer::DelAllComponents(void)
{
 DelAllComponentsRaw();
 UpdateInternalData();
}

void UContainer::DelAllComponentsRaw(void)
{
 while(NumComponents)
  DelComponent(PComponents[NumComponents-1],true);
}

/// Добавляет компонент как статическую переменную задавая ему имя класса 'classname'
/// и имя 'name'
void UContainer::AddStaticComponent(const NameT &classname, const NameT &name, UEPtr<UContainer> comp)
{
 comp->SetStaticFlag(true);
 comp->SetName(name);
 StaticComponents[comp]=classname;
}

/// Удаляет компонент как статическую переменную
void UContainer::DelStaticComponent(UEPtr<UContainer> comp)
{
 std::map<UEPtr<UContainer>, NameT>::iterator I=StaticComponents.find(comp);
 if(I != StaticComponents.end())
  StaticComponents.erase(I);
}

/// Удаляет компонент как статическую переменную
void UContainer::DelAllStaticComponents(void)
{
 StaticComponents.clear();
}

/// Возвращает указатель на static компонент
/// с классом 'classname' и именем 'name'
UEPtr<UContainer> UContainer::FindStaticComponent(const NameT &classname, const NameT &name) const
{
 std::map<UEPtr<UContainer>, NameT>::const_iterator I=StaticComponents.begin();
 for(;I!=StaticComponents.end();++I)
 {
  if(I->second == classname && I->first->GetName() == name)
   return I->first;
 }

 return 0;
}

/// Перемещает компоненту в другой компонент
/// Если comp не принадлежит этому компоненту, или target имеет отличный от
/// этого компонента storage, или target не может принять в себя компонент
/// то возвращает false и не делает ничего
bool UContainer::MoveComponent(UEPtr<UContainer> comp, UEPtr<UContainer> target)
{
 if(!comp || !target)
  return false;

 if(comp->GetOwner() != this)
  return false;

 if(target->GetStorage() != GetStorage())
  return false;

 if(!target->CheckComponentType(comp))
  return false;

 comp->MovingFlag=true;
 try
 {
  DelComponent(comp,false);
  if(target->AddComponent(comp) == ForbiddenId)
   return false;
 }
 catch(...)
 {
  comp->MovingFlag=false;
  throw;
 }

 return true;
}

// Возвращает список имен и Id компонент, содержащихся непосредственно
// в этом объекте
// Память должна быть выделена
void UContainer::GetComponentsList(std::vector<UId> &buffer) const
{
 UEPtr<UContainer> *pcomps=PComponents;
 buffer.resize(0);
 buffer.reserve(NumComponents);
 for(int i=0;i<NumComponents;i++,pcomps++)
  buffer.push_back((*pcomps)->Id);
}

void UContainer::GetComponentsList(vector<NameT> &buffer) const
{
 UEPtr<UContainer> *pcomps=PComponents;
 buffer.resize(0);
 buffer.reserve(NumComponents);
 for(int i=0;i<NumComponents;i++,pcomps++)
  buffer.push_back((*pcomps)->Name);
}

// Копирует все компоненты этого объекта в объект 'comp', если возможно.
void UContainer::CopyComponents(UEPtr<UContainer> comp, UEPtr<UStorage> stor) const
{
 UEPtr<UContainer> bufcomp;

 // Удаляем лишние компоненты из 'comp'
 comp->DelAllComponents();


 UEPtr<UContainer> * pcomponents=0;
 PointerMapCIteratorT I;
 PointerMapIteratorT J;
 if(NumComponents>0)
  pcomponents=&PComponents[0];

 for(int i=0;i<NumComponents;i++,pcomponents++)
  {
   if((*pcomponents)->GetStaticFlag())
    continue;
   bufcomp=(*pcomponents)->Alloc(stor);
   UEPtr<UIPointer> pointer=0;
   I=FindLookupPointer(*pcomponents);
   if(I != PointerLookupTable.end())
   {
    J=comp->PointerLookupTable.find(I->first);
    if(J != comp->PointerLookupTable.end())
     pointer=J->second.Pointer;
   }

   comp->AddComponent(bufcomp,pointer);
   bufcomp->SetId((*pcomponents)->Id);
   comp->SetLookupComponent(bufcomp->GetName(), bufcomp->GetId());
  }
 /*
 // Удаляем лишние компоненты из 'comp'
 for(int i=0;i<comp->GetNumComponents();i++)
 {
  bufcomp=comp->GetComponentByIndex(i);
  if(!GetComponent(bufcomp->Id))
   if(!comp->DelComponent(bufcomp,true))
    return false;
 }

 for(int i=0;i<NumComponents;i++)
  {
   bufcomp=comp->GetComponent(PComponents[i]->Id());
   if(bufcomp)
	if(!comp->DelComponent(bufcomp->Id()))
     return false;

   bufcomp=PComponents[i]->Alloc(PComponents[i]->Name(),stor);
   UIPointer *pointer=0;
   std::map<UId,UIPointer*>::const_iterator I=FindLookupPointer(PComponents[i]);
   if(I != PointerLookupTable.end())
   {
    std::map<UId,UIPointer*>::iterator J=comp->PointerLookupTable.find(I->first);
    if(J != comp->PointerLookupTable.end())
     pointer=J->second;
   }

   if(comp->AddComponent(bufcomp,pointer) == ForbiddenId)
    return false;

   bufcomp->Id=PComponents[i]->Id;
   comp->SetLookupComponent(bufcomp->Name(), bufcomp->Id());
  }
 return true;    */
}


// Перемещает компонент с текущим индексом index или именем 'name' вверх или
// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
// на эту границу
bool UContainer::ChangeComponentPosition(int index, int step)
{
 if(index<0 || index >= NumComponents)
  return false;

 if(step == 0)
  return true;

 int result=index+step;
 if(result<0)
  result=0;
 if(result>=NumComponents)
  result=NumComponents-1;

 UEPtr<UContainer> comp=PComponents[index];
 if(result>index)
 {
  for(int i=index;i<=result;i++)
   PComponents[i]=PComponents[i+1];
  PComponents[result]=comp;
 }
 else
 {
  for(int i=index;i>=result;i--)
   PComponents[i]=PComponents[i-1]; // TODO: при result==0 происходит обращение по индексу -1
  PComponents[result]=comp;
 }

 return true;
}

bool UContainer::ChangeComponentPosition(const NameT &name, int step)
{
 for(int i=0;i<NumComponents;i++)
  if(PComponents[i]->GetName() == name)
   return ChangeComponentPosition(i,step);

 return false;
}

// Устанавливает компонент с текущим индексом index или именем 'name' на
// заданную позицию
// Применяется для изменения порядка расчета компонент
bool UContainer::SetComponentPosition(int index, int new_position)
{
 int step=new_position-index;
 return ChangeComponentPosition(index, step);
}

bool UContainer::SetComponentPosition(const NameT &name, int new_position)
{
 for(int i=0;i<NumComponents;i++)
  if(PComponents[i]->GetName() == name)
   return SetComponentPosition(i,new_position);

 return false;
}

/// Флаг, выставляемый на время перемещения компонента
bool UContainer::IsMoving(void) const
{
 return MovingFlag;
}
// --------------------------


// ----------------------
// Методы управления коммуникационными компонентами
// ----------------------
// Возвращает список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
ULongIdVector& UContainer::GetConnectorsList(ULongIdVector &buffer,
							int sublevel, UEPtr<UContainer> ownerlevel)
{
 ULongId id;

 if(sublevel == -2)
 {
  id.Resize(0);
  this->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
  buffer.Add(id);
 }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UContainer> cont=GetComponentByIndex(i);
  UEPtr<UConnector> temp;
  temp=dynamic_pointer_cast<UConnector>(cont);
  if(temp)
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetConnectorsList(buffer,(sublevel<0)?-1:sublevel-1,ownerlevel);
 }

 return buffer;
}

// Возвращает список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
ULongIdVector& UContainer::GetItemsList(ULongIdVector &buffer,
                            int sublevel, UEPtr<UContainer> ownerlevel)

{
 ULongId id;

 if(sublevel == -2)
 {
  id.Resize(0);
  this->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
  buffer.Add(id);
 }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UContainer> cont=GetComponentByIndex(i);
  UEPtr<UItem> temp;
  temp=dynamic_pointer_cast<UItem>(cont);
  if(temp)
//  if(dynamic_cast<UItem*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetItemsList(buffer,(sublevel<0)?-1:sublevel-1,ownerlevel);
 }

 return buffer;
}

// Возвращает список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
ULongIdVector& UContainer::GetNetsList(ULongIdVector &buffer,
                            int sublevel, UEPtr<UContainer> ownerlevel)
{
 ULongId id;

 if(sublevel == -2)
 {
  id.Resize(0);
  this->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
  buffer.Add(id);
 }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UContainer> cont=GetComponentByIndex(i);
  UEPtr<UNet> temp;
  temp=dynamic_pointer_cast<UNet>(cont);
  if(temp)
//  if(dynamic_cast<UNet*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetNetsList(buffer,(sublevel<0)?-1:sublevel-1,ownerlevel);
 }

 return buffer;
}
// ----------------------


// --------------------------
// Методы управления локальными указателями
// --------------------------
// Устанавливает дочерний компонент 'id' в качестве заданного класса локальных указателей
// 'pointerid'
bool UContainer::SetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UContainer> cont=GetComponent(id);

 PointerMapIteratorT K=PointerLookupTable.end();
 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   if(J->second.Id == pointerid)
    return true;
  }

  if(J->second.Id == pointerid)
   K=J;

  ++J;
 }

 if(K != PointerLookupTable.end() && K->second.Pointer)
 {
  K->second.Pointer->Set(cont);
  return true;
 }

 return false;
}

// Устанавливает дочерний компонент 'name' в качестве заданного класса локальных указателей
// 'pointername'
bool UContainer::SetComponentAs(const NameT &name,const NameT &pointername)
{
 UEPtr<UContainer> cont=GetComponent(name);

 PointerMapIteratorT K=PointerLookupTable.end();;
 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   if(J->first == pointername)
    return true;
  }

  if(J->first == pointername)
   K=J;

  ++J;
 }

 if(K != PointerLookupTable.end() && K->second.Pointer)
 {
  K->second.Pointer->Set(cont);
  return true;
 }

 return false;
}

// Сбрасывает отношение дочерниего компонента 'id' к заданному классу локальных указателей
// 'pointerid'
bool UContainer::ResetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UContainer> cont=GetComponent(id);

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Id == pointerid && J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   J->second.Pointer->Del(cont);
   return true;
  }

  ++J;
 }

 return true;
}

// Сбрасывает отношение дочерниего компонента 'name' к заданному классу локальных указателей
// 'pointername'
bool UContainer::ResetComponentAs(const NameT &name,const NameT &pointername)
{
 UEPtr<UContainer> cont=GetComponent(name);

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->first == pointername && J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   J->second.Pointer->Del(cont);
   return true;
  }

  ++J;
 }

 return true;
}

// Сбрасывает отношение дочернего компонента 'id' ко всем классам локальных указателей
bool UContainer::ResetComponentAll(const UId &id)
{
 UEPtr<UContainer> cont=GetComponent(id);

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   J->second.Pointer->Del(cont);
  }

  ++J;
 }

 return true;
}

// Сбрасывает отношение дочернего компонента 'name' ко всем классам локальных указателей
bool UContainer::ResetComponentAll(const NameT &name)
{
 UEPtr<UContainer> cont=GetComponent(name);

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   J->second.Pointer->Del(cont);
  }

  ++J;
 }

 return true;
}

// Удаляет все компоненты относящиеся к заданному классу локальных указателей
void UContainer::DelAllComponentsAs(const NameT &pointername, bool canfree)
{
 PointerMapIteratorT J=PointerLookupTable.find(pointername);

 if(J != PointerLookupTable.end())
 {
  UEPtr<UContainer> cont=J->second.Pointer->Get();

  while(cont)
  {
   DelComponent(cont,canfree);
   cont=J->second.Pointer->Get();
  }
 }
}
// --------------------------


// --------------------------
// Методы управления общими (shared) переменными
// --------------------------
// Метод инициализации общих переменных. Вызывается автоматически при добавлении
// объекта владельцу
void UContainer::SharesInit(void)
{
 if(!ShareLookupTable.empty())
 {
  ShareMapIteratorT I=ShareLookupTable.begin();
  ShareMapIteratorT J=ShareLookupTable.end();
  for(;I != J;++I)
   I->second->Init(MainOwner);
 }
 ASharesInit();
}

// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
void UContainer::SharesUnInit(void)
{
 ASharesUnInit();
 if(!ShareLookupTable.empty())
 {
  ShareMapIteratorT I=ShareLookupTable.begin();
  ShareMapIteratorT J=ShareLookupTable.end();
  for(;I != J;++I)
   I->second->UnInit();
 }
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UContainer::Default(void)
{
 RDK_SYS_TRY
 {
  try
  {
   BeforeDefault();
   MemoryMonitor=false;
   Ready=false;
   for(int i=0;i<NumComponents;i++)
	PComponents[i]->Default();

   // Если существует прообраз в хранилище, то берем настройки параметров
   // из прообраза
   UEPtr<UContainer> original;
   if(Storage)
	original=dynamic_pointer_cast<UContainer>(GetStorage()->GetClass(Class));

   SetTimeStep(2000);
   SetMaxCalculationDuration(-1);
   SetCalculationDurationThreshold(-1);
   SetDebugSysEventsMask(0xFFFFFFFF);


   if(original && original != this)
   {
	NameT name=Name;
	bool activity=Activity;
	original->CopyProperties(this,ptParameter);
	SetName(name);
	SetActivity(activity);
   }

   if(!ADefault())
	return false;
   AfterDefault();
  }
  catch(UException &exception)
  {
   throw;
  }
  catch(std::exception &exception)
  {
   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return true;
}

/// Метод сброса параметров на значения по умолчанию
/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
bool UContainer::DefaultAll(UContainer* cont, bool subcomps)
{
 if(!cont)
  return false;

 RDK_SYS_TRY
 {
  try
  {
   if(!cont->Default())
	return false;

   bool res=true;
   if(subcomps)
   {
	for(int i=0;i<cont->GetNumComponents();i++)
	 res &= DefaultAll(cont->GetComponentByIndex(i),subcomps);
   }
  }
  catch(UException &exception)
  {
   throw;
  }
  catch(std::exception &exception)
  {
   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
  }
 RDK_SYS_CATCH
 {
  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UContainer::Build(void)
{
 if(Ready)
  return true;

 RDK_SYS_TRY
 {
  try
  {
   BeforeBuild();

   for(int i=0;i<NumComponents;i++)
	PComponents[i]->Build();

   ABuild();
   Ready=true;
// Reset();

   AfterBuild();
  }
  catch(UException &exception)
  {
   throw;
  }
  catch(std::exception &exception)
  {
   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
  }
 RDK_SYS_CATCH
 {
  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return true;
}

// Сброс процесса счета.
bool UContainer::Reset(void)
{
 RDK_SYS_TRY
 {
  try
  {
   LogDebugSysMessage(RDK_SYS_DEBUG_RESET, RDK_SYS_MESSAGE_ENTER);
   Build();
   SharesInit(); // TODO: Костыль. Без него в некоторых компонентах (не ясно каких)
   // шары остаются не инициализированных. возможно причина в статических дочерних компонентах в них

   // Init(); // Заглушка
   BeforeReset();

   if(!IsInit())
   {
	LogDebugSysMessage(RDK_SYS_DEBUG_RESET, RDK_SYS_MESSAGE_EXIT_ININIT_FAIL);
	return true; // TODO //false;
   }

   for(int i=0;i<NumComponents;i++)
	PComponents[i]->Reset();

   AReset();


   MemoryUsageDiff=0;
   MaxMemoryBlockDiff=0;

   CalcCounter=0;
   SkipComponentCalculation=false;
   ComponentReCalculation=false;
   LastCalcTime=0;
   InterstepsInterval=0;
   StepDuration=0;
   AfterReset();
   LogDebugSysMessage(RDK_SYS_DEBUG_RESET, RDK_SYS_MESSAGE_EXIT_OK);
  }
  catch(UException &exception)
  {
   throw;
  }
  catch(std::exception &exception)
  {
   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return true;
}

// Выполняет расчет этого объекта
bool UContainer::Calculate(void)
{
 if(!Activity)
  return true;
 int i=0;
 RDK_SYS_TRY
 {
  try
  {
   Init(); // Заглушка

   if(!Owner)
   {
	LogDebugSysMessage(RDK_SYS_DEBUG_CALC, RDK_SYS_MESSAGE_NEW_CALC_ITERATION);
   }

   LogDebugSysMessage(RDK_SYS_DEBUG_CALC, RDK_SYS_MESSAGE_ENTER);
   if(!IsInit())
   {
	LogDebugSysMessage(RDK_SYS_DEBUG_CALC, RDK_SYS_MESSAGE_EXIT_ININIT_FAIL);
	return false;
   }

   unsigned long long tempstepduration=StartCalcTime=GetCurrentStartupTime();
   InterstepsInterval=(LastCalcTime>0)?CalcDiffTime(tempstepduration,LastCalcTime):0;
   LastCalcTime=tempstepduration;

   Build();

   unsigned long long total_used_memory_before(0);
   unsigned long long largest_free_block_before(0);
   if(MemoryMonitor)
	ReadUsedMemoryInfo(total_used_memory_before, largest_free_block_before);

   BeforeCalculate();

   UEPtr<UContainer> *comps=PComponents;
   while((i<NumComponents) && !SkipComponentCalculation)
   {
	if((*comps)->GetStaticFlag())
	{
	 ++i,++comps;
	 continue;
	}
	(*comps)->Calculate();
	if(ComponentReCalculation)
	{
	 ComponentReCalculation=false;
	 std::string temp;
	 LogMessage(RDK_EX_DEBUG, string("Components recaltulation after ")+(*comps)->GetFullName(temp));
	 i=0; comps=PComponents;
	}
	else
	{
	 unsigned long long calc_duration=CalcDiffTime(GetCurrentStartupTime(),StartCalcTime);
	 if((MaxCalculationDuration >= 0) && (calc_duration > ULongTime(MaxCalculationDuration)))
	 {
	  ForceSkipComponentCalculation();
	  std::string temp;
	  LogMessage(RDK_EX_WARNING, string("CalcTime[")+sntoa(calc_duration)+std::string("]>MaxCalculationDuration[")+sntoa(MaxCalculationDuration)+("] after ")+(*comps)->GetFullName(temp));
     }
	 ++i,++comps;
	}
   }

   SkipComponentCalculation=false;
   ComponentReCalculation=false;

   LogPropertiesBeforeCalc();

   unsigned long long acalc_start_time=GetCurrentStartupTime();
   if(!Owner)
   {
	ACalculate();
   }
   else
   if(TimeStep == OwnerTimeStep)
   {
	ACalculate();
   }
   else
   if(TimeStep < OwnerTimeStep)
   {
	--CalcCounter;
	if(CalcCounter <= 0)
	{
	 CalcCounter=OwnerTimeStep/TimeStep;
	 ACalculate();
	}
   }
   else
   if(TimeStep > OwnerTimeStep)
   {
	for(int i=int(TimeStep/OwnerTimeStep);i>=0;--i)
	 ACalculate();
   }
   unsigned long long calc_duration=CalcDiffTime(GetCurrentStartupTime(),acalc_start_time);

   LogPropertiesAfterCalc();

   UpdateMainOwner();
   InterstepsInterval-=StepDuration;


   if((MaxCalculationDuration >= 0) && (calc_duration > ULongTime(MaxCalculationDuration)))
   {
	if(Owner)
	{
	 GetOwner()->ForceSkipComponentCalculation();
	}
    LogMessage(RDK_EX_WARNING, string("ACalculate CalcTime[")+sntoa(calc_duration)+std::string("]>MaxCalculationDuration[")+sntoa(MaxCalculationDuration)+"]");
   }

   StepDuration=CalcDiffTime(GetCurrentStartupTime(),tempstepduration);

   if((CalculationDurationThreshold >= 0) && (StepDuration > ULongTime(CalculationDurationThreshold)))
   {
	std::string temp;
    LogMessageEx(RDK_EX_WARNING, string("Performance warning: StepDuration>")+RDK::sntoa(CalculationDurationThreshold)+" ms");
   }

   // Обрабатываем контроллеры
   int numcontrollers=Controllers.size();

   if(numcontrollers)
   {
	UEPtr<UController>* controllers=&Controllers[0];
	for(int i=0;i<numcontrollers;i++,controllers++)
	{
	 (*controllers)->Update();
	}
   }
   AfterCalculate();
   unsigned long long total_used_memory_after(0);
   unsigned long long largest_free_block_after(0);
   if(MemoryMonitor && ReadUsedMemoryInfo(total_used_memory_after, largest_free_block_after))
   {
	MemoryUsageDiff=total_used_memory_before-total_used_memory_after;
	MaxMemoryBlockDiff=largest_free_block_before-largest_free_block_after;
	if(MemoryUsageDiff > 0 && MaxMemoryBlockDiff > 0)
	 Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, GetFullName()+std::string(" eats ")+sntoa(MemoryUsageDiff)+std::string(" bytes of RAM. Largest RAM block has been decreased to ")+sntoa(MaxMemoryBlockDiff)+" bytes");
	else
	if(MemoryUsageDiff > 0)
	 Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, GetFullName()+std::string(" eats ")+sntoa(MemoryUsageDiff)+std::string(" bytes of RAM."));
	else
	if(MaxMemoryBlockDiff > 0)
	 Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, GetFullName()+std::string(" Largest RAM block has been decreased to ")+sntoa(MaxMemoryBlockDiff)+" bytes");
   }
   LogDebugSysMessage(RDK_SYS_DEBUG_CALC, RDK_SYS_MESSAGE_EXIT_OK);
  }
  catch(UException &exception)
  {
   throw;
  }
  catch(std::exception &exception)
  {
   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return true;
}


// Выполняет начальную инициализацию этого объекта
void UContainer::Init(void)
{
 if(!Activity)
  return;

 RDK_SYS_TRY
 {
  try
  {
   if(IsInit())
	return;

   for(int i=0;i<NumComponents;i++)
	PComponents[i]->Init();

   AInit();
   InitFlag=true;
   Reset();
  }
  catch(UException &exception)
  {
   throw;
  }
  catch(std::exception &exception)
  {
   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
}

// Выполняет деинициализацию этого объекта
void UContainer::UnInit(void)
{
 if(!IsInit())
  return;

 RDK_SYS_TRY
 {
  try
  {
   AUnInit();

   for(int i=0;i<NumComponents;i++)
	PComponents[i]->UnInit();
  }
  catch(UException &exception)
  {
   throw;
  }
  catch(std::exception &exception)
  {
   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 InitFlag=false;
}
// Обновляет состояние MainOwner после расчета этого объекта
void UContainer::UpdateMainOwner(void)
{
 if(!MainOwner)
  return;

 return AUpdateMainOwner();
}

// Обычно вызывается дочерним компонентом и прерывает обсчет цепочки дочерних
// компонент на этом шаге счета
void UContainer::ForceSkipComponentCalculation(void)
{
 SkipComponentCalculation=true;
}

// Обычно вызывается дочерним компонентом и требует перерасчет цепочки дочерних
// компонент на этом шаге счета сначала
void UContainer::ForceComponentReCalculation(void)
{
 ComponentReCalculation=true;
}

/// Проверяет текущую длительность расчета этого компонента
/// и если она превышает MaxCalculationDuration и MaxCalculationDuration>=0
/// то прерывает обсчет остальной цепочки дочерних компонент
bool UContainer::CheckDurationAndSkipComponentCalculation(void)
{
 if((MaxCalculationDuration >= 0) && (CalcDiffTime(GetCurrentStartupTime(),StartCalcTime) > ULongTime(MaxCalculationDuration)))
 {
  GetOwner()->ForceSkipComponentCalculation();
  return true;
 }
 return false;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
void UContainer::AInit(void)
{
}

void UContainer::AUnInit(void)
{
}
// --------------------------

// --------------------------
// Скрытые методы управления таблицей соответсвий компонент
// --------------------------
// Обновляет таблицу соответствий компонент заменяя 'oldname'
// имя компонента на 'newname'
void UContainer::ModifyLookupComponent(const NameT &oldname,
                                        const NameT &newname)
{
 UId id;

 std::map<NameT,UId>::iterator I=CompsLookupTable.find(oldname);
 if(I == CompsLookupTable.end())
  RDK_THROW(EComponentNameNotExist(oldname));

 id=I->second;
 CompsLookupTable.erase(I);
 CompsLookupTable[newname]=id;
}

// Обновляет таблицу соответствий компонент устанавливая Id 'id'
// для компонента с именем 'name'
void UContainer::SetLookupComponent(const NameT &name, const UId &id)
{
 CompsLookupTable[name]=id;
 if(LastId<id)
  LastId=id;
}

// Обновляет таблицу соответствий компонент удаляя запись
// компонента с именем 'name'
void UContainer::DelLookupComponent(const NameT &name)
{
 std::map<NameT,UId>::iterator I=CompsLookupTable.find(name);

 if(I == CompsLookupTable.end())
  RDK_THROW(EComponentNameNotExist(name));
 CompsLookupTable.erase(name);
}
// --------------------------

// --------------------------
// Методы управления контроллерами интерфейса
// Удаление контроллеров лежит на вызывающем модуле
// --------------------------
// Добавляет новый контроллер
void UContainer::AddController(UEPtr<UController> controller, bool forchilds)
{
 if(CheckController(controller))
  return;

 Controllers.push_back(controller);
 if(forchilds)
 {
  UEPtr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->AddController(controller,forchilds);
 }
}

// Удаляет контроллер из списка
void UContainer::DelController(UEPtr<UController> controller, bool forchilds)
{
 vector<UEPtr<UController> >::iterator I=find(Controllers.begin(),Controllers.end(),controller);

 if(I != Controllers.end())
  Controllers.erase(I);

 if(forchilds)
 {
  UEPtr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->DelController(controller,forchilds);
 }
}

// Удаляет все контроллеры
void UContainer::DelAllControllers(bool forchilds)
{
 Controllers.clear();
 if(forchilds)
 {
  UEPtr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->DelAllControllers(forchilds);
 }
}

// Инициирует отключение всех контроллеров
void UContainer::UnLinkAllControllers(bool forchilds)
{
 while(Controllers.begin() != Controllers.end())
  Controllers.front()->UnLink(this);

 if(forchilds)
 {
  UEPtr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->UnLinkAllControllers(forchilds);
 }
}


// Проверяет, существует ли контроллер в списке
bool UContainer::CheckController(UEPtr<UController> controller) const
{
 if(find(Controllers.begin(),Controllers.end(),controller) != Controllers.end())
  return true;
 return false;
}

// Возвращает число контроллеров
size_t UContainer::GetNumControllers(void) const
{
 return Controllers.size();
}

// Возвращает контроллер по индексу
UEPtr<UController> UContainer::GetController(int index)
{
 return Controllers[index];
}
// --------------------------

// --------------------------
// Скрытые методы управления локальными указателями
// --------------------------
// Добавляет указатель в таблицу соотвествий
// Должна вызываться в конструкторах классов
UId UContainer::AddLookupPointer(const NameT &name, UEPtr<UIPointer> pointer)
{
 UPVariable P(1,pointer);

 if(PointerLookupTable.find(name) != PointerLookupTable.end())
  RDK_THROW(EPointerNameAlreadyExist(name));

 for(PointerMapIteratorT I=PointerLookupTable.begin(),
                      J=PointerLookupTable.end(); I!=J; ++I)
 {
  if(P.Id <= I->second.Id)
   P.Id=I->second.Id+1;
 }

 PointerLookupTable.insert(make_pair(name,P));
 return P.Id;
}

// Удаляет указатель с ID 'id' из таблицы соотвествий
void UContainer::DelLookupPointer(const NameT &name)
{
 PointerMapIteratorT I=PointerLookupTable.find(name);

 if(I == PointerLookupTable.end())
  RDK_THROW(EPointerNameNotExist(name));

 delete I->second.Pointer;
 PointerLookupTable.erase(I);
}
/*
// Возвращает полное имя указателя без префикса RDK, и суффикса '*'
NameT UContainer::GetPointerLongName(const UIPointer &pointer) const
{

 NameT name=pointer.GetOwnerName();

 int i=name.find("RDK::");
 if(i == NameT::npos)
  i=0;
 else
  i=7;

 int j=name.find_last_of("*");
 if(j == NameT::npos)
  j=name.size()-i;
 else
  j-=i+1;

 return name.substr(i,j);
 return "";
}                        */

// Осуществляет поиск в таблице указателя, соответствующего заданному источнику
UContainer::PointerMapCIteratorT UContainer::FindLookupPointer(UEPtr<UContainer> source) const
{
 for(PointerMapCIteratorT I=PointerLookupTable.begin(),
                      J=PointerLookupTable.end(); I!=J; ++I)
 {
   if(I->second.Pointer && I->second.Pointer->Find(source)>=0)
    return I;
 }

 return PointerLookupTable.end();
}
// --------------------------

// --------------------------
// Скрытые методы управления общими (shared) переменными
// --------------------------
// Метод инициализации общих переменных. Вызывается автоматически при добавлении
// объекта владельцу
void UContainer::ASharesInit(void)
{
}

// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
void UContainer::ASharesUnInit(void)
{
}
// --------------------------

// --------------------------
// Скрытые методы управления таблицей компонент
// --------------------------
// Добавляет компонент 'comp' в таблицу компонент
void UContainer::AddComponentTable(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 Components.push_back(comp);
 PComponents=&Components[0];
 NumComponents=Components.size();

 if(pointer)
  pointer->Set(comp);
 else
 {
  PointerMapCIteratorT I=FindLookupPointer(comp);
  if(I != PointerLookupTable.end())
  {
   I->second.Pointer->Del(comp);
  }
 }
}

void UContainer::DelComponentTable(UEPtr<UContainer> comp)
{
 int i;

 if(NumComponents)
 {
  if(PComponents[NumComponents-1]==comp)
   Components.resize(NumComponents-1);
  else
  {
   for(i=0;i<NumComponents;i++)
    if(PComponents[i] == comp)
     break;

   if(i>=NumComponents)
    return;

   memmove(PComponents+i,PComponents+i+1,(NumComponents-i-1)*sizeof(UEPtr<UContainer>));
   Components.resize(NumComponents-1);
  }
 }

 NumComponents=Components.size();
 if(NumComponents>0)
  PComponents=&Components[0];
 else
  PComponents=0;

 PointerMapCIteratorT I=FindLookupPointer(comp);
 if(I != PointerLookupTable.end())
 {
  I->second.Pointer->Del(comp);
 }
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
/// Производит необходимые операции по добавлению статического компонента
UId UContainer::UpdateStaticComponent(const NameT &classname, UEPtr<UContainer> comp)
{
 comp->SetLogger(GetLogger());
 comp->SetStorage(GetStorage());
 comp->SetEnvironment(GetEnvironment());
 if(GetStorage())
 {
  comp->SetClass(GetStorage()->FindClassId(classname));

  for(int i=0;i<NumComponents;i++)
   if(PComponents[i] == comp)
	return PComponents[i]->GetId();
  return AddComponent(comp);
 }
 return ForbiddenId;
}

// Удаляет компонент comp
// Метод предполагает, что компонент принадлежит объекту
void UContainer::BeforeDelComponent(UEPtr<UContainer> comp, bool canfree)
{
 ABeforeDelComponent(comp,canfree);
}

void UContainer::AfterDelComponent(UEPtr<UContainer> comp, bool canfree)
{
 AAfterDelComponent(comp,canfree);
}

void UContainer::DelComponent(UEPtr<UContainer> comp, bool canfree)
{
 BeforeDelComponent(comp,canfree);
 SharesUnInit();
 ADelComponent(comp);

 comp->SetEnvironment(0);

 if(comp->GetMainOwner() == MainOwner)
  comp->SetMainOwner(0);

 // Удаление из базы компонент
 // Удаляем компонент из таблицы соответствий владельца
 DelLookupComponent(comp->Name);

 // Удаление из базы компонент
 DelComponentTable(comp);

 comp->Owner=0;

 AfterDelComponent(comp,canfree);

 if(canfree)
  comp->Free();

 if(!NumComponents)
  LastId=0;
 comp->SetLogger(0);
}


// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
void UContainer::ABeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

}

void UContainer::AAfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

}

bool UContainer::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
void UContainer::ABeforeDelComponent(UEPtr<UContainer> comp, bool canfree)
{

}

void UContainer::AAfterDelComponent(UEPtr<UContainer> comp, bool canfree)
{

}

bool UContainer::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Обновляет состояние MainOwner после расчета этого объекта
void UContainer::AUpdateMainOwner(void)
{
}
// --------------------------

/* *************************************************************************** */
/* Классы исключений */
// class EIContainer
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UContainer::EIContainer::EIContainer(void)
{
}

UContainer::EIContainer::EIContainer(const UContainer *cont)
{
 if(!cont)
  return;

 // Короткое имя компонента в котором сгенерировано исключение
 Name=cont->GetName();

 // Короткий идентификатор компонента в котором сгенерировано исключение
 Id=cont->GetId();

 // Полное имя владельца компонента в котором сгенерировано исключение
 if(cont->GetOwner())
 {
  cont->GetOwner()->GetFullName(OwnerName);

  // Полный идентификатор владельца компонента в котором сгенерировано исключение
  OwnerId=cont->GetOwner()->GetFullId();
 }

 if(cont->GetMainOwner())
 {
  // Полное имя главного владельца компонента в котором сгенерировано исключение
  cont->GetMainOwner()->GetFullName(MainOwnerName);

  // Полный идентификатор главного владельца компонента в котором сгенерировано исключение
  MainOwnerId=cont->GetMainOwner()->GetFullId();
 }
}


UContainer::EIContainer::EIContainer(const EIContainer &copy)
{
 // Короткое имя компонента в котором сгенерировано исключение
 Name=copy.Name;

 // Короткий идентификатор компонента в котором сгенерировано исключение
 Id=copy.Id;

 // Полное имя владельца компонента в котором сгенерировано исключение
 OwnerName=copy.OwnerName;

 // Полный идентификатор владельца компонента в котором сгенерировано исключение
 OwnerId=copy.OwnerId;

 // Полное имя главного владельца компонента в котором сгенерировано исключение
 MainOwnerName=copy.MainOwnerName;

 // Полный идентификатор главного владельца компонента в котором сгенерировано исключение
 MainOwnerId=copy.MainOwnerId;
}

UContainer::EIContainer::~EIContainer(void)
{

}
// --------------------------


// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UContainer::EIContainer::CreateLogMessage(void) const
{
 string result;

 if(OwnerName.size()>0)
 {
  // Полное имя компонента в котором сгенерировано исключение
  result+=" Component=";
  result+=OwnerName;
  result+=".";
  result+=Name;
 }
 else
 if(Name.size()>0)
 {
  // Короткое имя компонента в котором сгенерировано исключение
  result+=" Component=";
  result+=Name;
 }
/*
 if(MainOwnerName != OwnerName && MainOwnerName.size()>0)
 {
  // Полное имя главного владельца компонента в котором сгенерировано исключение
  result+=" MainOwnerName=";
  result+=MainOwnerName;
 }
  */
  // Полный идентификатор главного владельца компонента в котором сгенерировано исключение
//  result+=" MainOwnerId=";
//  result+=iexception->MainOwnerId;


 return result;
}
// --------------------------

// Интерфейсный класс для обработки ошибок счета компонент
//class EICalculateContainer: public EIContainer
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UContainer::EICalculateContainer::EICalculateContainer(void)
{

}

UContainer::EICalculateContainer::EICalculateContainer(const UContainer *cont, const UContainer *subcont)
: EIContainer(cont)
{
 if(!subcont)
  return;

 // Короткое имя компонента в котором сгенерировано исключение
 SubName=subcont->GetName();

 // Короткий идентификатор компонента в котором сгенерировано исключение
 SubId=subcont->GetId();
}

UContainer::EICalculateContainer::EICalculateContainer(const EICalculateContainer &copy)
 :EIContainer(copy)
{
 SubName=copy.Name;
 SubId=copy.SubId;
}

UContainer::EICalculateContainer::~EICalculateContainer(void)
{

}
 // --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UContainer::EICalculateContainer::CreateLogMessage(void) const
{
 string result=UContainer::EIContainer::CreateLogMessage();
 if(SubName.size()>0)
 {
  result+=" SubName=";
  result+=SubName;
 }
 return result;
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
UContainer::EComponentSystemException::EComponentSystemException(void)
{

}

UContainer::EComponentSystemException::EComponentSystemException(const UContainer *cont, const UContainer *subcont, const std::string &info)
 : ESystemException(info), EICalculateContainer(cont,subcont)
{
}


UContainer::EComponentSystemException::EComponentSystemException(const EComponentSystemException &copy)
 : ESystemException(copy), EICalculateContainer(copy)
{
}

UContainer::EComponentSystemException::~EComponentSystemException(void) throw()
{

}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UContainer::EComponentSystemException::CreateLogMessage(void) const
{
 return ESystemException::CreateLogMessage()+EICalculateContainer::CreateLogMessage();
}
// --------------------------


/// Функция подготавливает строку для логирования
bool PreparePropertyLogString(const UVariable& variable, unsigned int expected_type, std::string &result)
{
 USerStorageXML xml;
 std::string str_type;
 unsigned int type=variable.GetPropertyType();

 if(!(type & expected_type))
  return false;

 if(type & ptInput)
  str_type="Input ";
 else
 if(type & ptOutput)
  str_type="Output ";
 else
 if(type & ptParameter)
  str_type="Parameter ";
 else
 if(type & ptState)
  str_type="State ";

 if((type & ptInput) && variable.Property->GetName().find("DataInput") != string::npos)
  return false;

 if((type & ptOutput) && variable.Property->GetName().find("DataOutput") != string::npos)
  return false;

 std::string line=str_type+variable.Property->GetName();
 result=line;

 if(type & ptInput && !variable.Property->IsConnected())
 {
  result=line+"[<Disconnected>]";
 }
 else
 {
  if(type & ptInput)
  {
   line+=std::string("[")+variable.Property->GetItemFullName()+std::string(":")+variable.Property->GetItemOutputName()+"]";

  }
  line+=" = ";

  try
  {
   variable.Property->Save(&xml,true);
   std::string str_data=xml.GetNodeText();
   if(str_data.empty())
   {
	xml.Save(str_data);
	line+="\n";
   }
   result=line+str_data;
  }
  catch(UIProperty::EPropertyZeroPtr &ex)
  {
   result=line+"[<Disconnected>]";
  }
 }
 return true;
}



}

#endif


