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
#include "UAContainer.h"
#include "UAContainerStorage.h"
#include "UAConnector.h"
#include "UAItem.h"
#include "UANet.h"
#include "../Serialize/Serialize.h"
#include "../Math/MUBinarySerialize.h"
#include "../Math/MUXMLSerialize.h"
#include "UAContainerStorage.h"
#include "UContainerDescription.h"

namespace RDK {

NameT ForbiddenName="";


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
// Class UAContainer
/* *************************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAContainer::UAContainer(void)
 : PComponents(0), NumComponents(0), Id(0), LastId(0)
{
 AddLookupProperty("Id",new UVProperty<UId,UAContainer>(this,&UAContainer::SetId,&UAContainer::GetId));
 AddLookupProperty("Name",new UVProperty<NameT,UAContainer>(this,&UAContainer::SetName,&UAContainer::GetName));
 AddLookupProperty("TimeStep",new UVProperty<UTime,UAContainer>(this,&UAContainer::SetTimeStep,&UAContainer::GetTimeStep));
 AddLookupProperty("Activity",new UVProperty<bool,UAContainer>(this,&UAContainer::SetActivity,&UAContainer::GetActivity));
 AddLookupProperty("Coord",new UVProperty<RDK::MVector<double,3>,UAContainer>(this,&UAContainer::SetCoord,&UAContainer::GetCoord));
}

UAContainer::~UAContainer(void)
{
 DelAllComponents();
 UnLinkAllControllers();

 BreakOwner();

 UEPtr<UAContainerStorage> cstorage=dynamic_pointer_cast<UAContainerStorage>(Storage);
 if(cstorage)
  {
   cstorage->PopObject(UEPtr<UAContainer>(this));
  }
}
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта
UEPtr<UAContainer> UAContainer::GetOwner(void) const
{
 return dynamic_pointer_cast<UAContainer>(Owner);
}

// Возвращает указатель на главного владельца этим объектом
UEPtr<UAContainer> UAContainer::GetMainOwner(void) const
{
 return dynamic_pointer_cast<UAContainer>(MainOwner);
}

// Возвращает хранилище компонент этого объекта
UEPtr<UAContainerStorage> const UAContainer::GetStorage(void) const
{
 return dynamic_pointer_cast<UAContainerStorage>(Storage);
}

// Проверяет, является ли объект owner
// владельцем этого объекта на каком-либо уровне иерархии
bool UAContainer::CheckOwner(UEPtr<UAContainer> owner) const
{
 if(Owner == 0 && Owner != owner)
  return false;

 if(Owner == owner)
  return true;

 return GetOwner()->CheckOwner(owner);
}

// Возвращает полный Id объекта
// (включая Id всех владельцев).
ULongId& UAContainer::GetFullId(ULongId &buffer) const
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

// Возвращает  'длинный' Id объекта
// (исключая имя владельца 'mainowner').
// Метод возвращает пустой вектор, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии.
ULongId& UAContainer::GetLongId(UEPtr<UAContainer> mainowner, ULongId &buffer) const
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

// Промежуточный вариант одноименного метода, возвращающего длинное имя
std::string& UAContainer::GetLongId(UEPtr<UAContainer> mainowner, std::string &buffer) const
{
 return GetLongName(mainowner,buffer);
}

// Возвращает true если передаваемый идентификатор объекта корректен, в противном случае возвращает false
bool UAContainer::CheckLongId(const ULongId &id) const
{
 if(id.GetSize() == 0 || id[0] == ForbiddenId)
  return false;

 return true;
}

// Промежуточный вариант одноименного метода, обрабатывающего длинное имя
bool UAContainer::CheckLongId(const std::string &id) const
{
 if(id.size() == 0)
  return false;

 return true;
}
// --------------------------

// --------------------------
// Методы управления свойствами
// --------------------------
// Координата компонента в пространстве сети
RDK::MVector<double,3> UAContainer::GetCoord(void) const
{
 return Coord;
}

bool UAContainer::SetCoord(RDK::MVector<double,3> value)
{
 if(Coord != value)
  Coord=value;

 return true;
}


// Время, затраченное на обработку объекта
// (без учета времени обсчета дочерних объектов) (мс)
long long UAContainer::GetStepDuration(void) const
{
 long long res=0;
 for(int i=0;i<NumComponents;i++)
  res+=PComponents[i]->GetFullStepDuration();

 return StepDuration-res;
}

// Время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
long long UAContainer::GetFullStepDuration(void) const
{
 return StepDuration;
}

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double UAContainer::GetInstantPerformance(void) const
{
 return ((GetFullStepDuration()*TimeStep)/1000.0);
}

// Удаляет владельца объекта
void UAContainer::BreakOwner(void)
{
 UEPtr<UAContainer> owner=GetOwner();
 if(owner)
  owner->DelComponent(this,false);
}

// Устанавливает указатель на главного владельца этим объектом
// Указатель устанавливается на число уровней дочерних компонент
// 'levels'. Если levels < 0 то устанавливается компонентам на всех уровнях
void UAContainer::SetMainOwner(UEPtr<UAComponent> mainowner)
{
 UAComponent::SetMainOwner(mainowner);
}

void UAContainer::SetMainOwner(UEPtr<UAComponent> mainowner, int levels)
{
 if(MainOwner == mainowner && !levels)
  return;

 MainOwner=mainowner;

 if(!levels)
  return;

 UEPtr<UAContainer>* comps=PComponents;
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
bool UAContainer::CheckId(const UId &id)
{
 return (id>LastId)?true:false;
}

// Проверяет предлагаемое имя 'name' на уникальность в рамках
// данного объекта.
bool UAContainer::CheckName(const NameT &name)
{
// if(Name == name)
//  return true;

// if(!GetOwner())
//  return true;

 if(CompsLookupTable.find(name) == CompsLookupTable.end())
  return true;

 return false;
}

// Генерирует уникальный Id.
UId UAContainer::GenerateId(void)
{
 return LastId+1;
}

#pragma warning (disable : 4996)
// Генерирует имя уникальное в компонентах этого объекта
NameT& UAContainer::GenerateName(const NameT &prefix, NameT &namebuffer)
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
#pragma warning (default : 4996)

// Устанавливает имя объекта.
const NameT& UAContainer::GetName(void) const
{
 return Name;
}

bool UAContainer::SetName(const NameT &name)
{
 if(Name == name)
  return true;

 if(name.empty())
  return false;

  if(GetOwner() != 0)
  {
   if(!GetOwner()->CheckName(name))
	throw new EComponentNameAlreadyExist(name);

   GetOwner()->ModifyLookupComponent(Name, name);
  }
 Name=name;
 return true;
}

// Возвращает полное имя объекта
// (включая имена всех владельцев).
NameT& UAContainer::GetFullName(NameT &buffer) const
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

// Возвращает  'длинное' имени объекта
// (исключая имя владельца 'mainowner').
// Метод возвращает пустую строку, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии.
NameT& UAContainer::GetLongName(const UEPtr<UAContainer> mainowner, NameT &buffer) const
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
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает имя дочернего компонента по его Id
const NameT& UAContainer::GetComponentName(const UId &id) const
{
 for(std::map<NameT,UId>::const_iterator I=CompsLookupTable.begin(),
								 J=CompsLookupTable.end(); I!=J; ++I)
 {
  if(I->second == id)
   return I->first;
 }
 throw new EComponentIdNotExist(id);
}

// Возвращает Id дочернего компонента по его имени
const UId& UAContainer::GetComponentId(const NameT &name) const
{
 std::map<NameT,UId>::const_iterator I=CompsLookupTable.find(name);
 if(I == CompsLookupTable.end())
  throw new EComponentNameNotExist(name);

 return I->second;
}

// Возвращает имя локального указателя по его Id
const NameT& UAContainer::GetPointerName(const UId &id) const
{
 for(PointerMapCIteratorT I=PointerLookupTable.begin(),
								 J=PointerLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Id == id)
   return I->first;
 }
 throw new EPointerIdNotExist(id);
}

// Возвращает Id локального указателя по его имени
const UId& UAContainer::GetPointerId(const NameT &name) const
{
 PointerMapCIteratorT I=PointerLookupTable.find(name);
 if(I == PointerLookupTable.end())
  throw new EPointerNameNotExist(name);

 return I->second.Id;
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает величину шага интегрирования
UTime UAContainer::GetTimeStep(void) const
{
 return TimeStep;
}

bool UAContainer::SetTimeStep(UTime timestep)
{
 if(timestep <= 0)
  return false;

 TimeStep=timestep;

 if(Owner)
  OwnerTimeStep=GetOwner()->TimeStep;
 else
  OwnerTimeStep=timestep;

 // Обращение ко всем компонентам объекта
 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->OwnerTimeStep=timestep;

 return true;
}

// Устанавливает величину шага интегрирования компоненту и всем его дочерним компонентам
bool UAContainer::SetGlobalTimeStep(UTime timestep)
{
 if(!SetTimeStep(timestep))
  return false;

 // Обращение ко всем компонентам объекта
 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(!(*comps)->SetGlobalTimeStep(timestep))
   return false;

 return true;
}



// Устанавливает флаг активности объекта
bool UAContainer::GetActivity(void) const
{
 return Activity;
}

bool UAContainer::SetActivity(bool activity)
{
 if(Activity == activity)
  return true;

 Activity=true;
 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->Activity=activity;

 if(activity)
  return Reset(); // !!! Заглушка. Возможно это не нужно!

 Activity=activity;

 return true;
}

// Id объекта
const UId& UAContainer::GetId(void) const
{
 return Id;
}

bool UAContainer::SetId(const UId &id)
{
 if(Id == id)
  return true;

 if(id == ForbiddenId)
  return true;// Заглушка!! Это хак! throw new EForbiddenId(id);

 if(id < 0)
  throw new EInvalidId(id);


 if(Owner != 0)
  {
   if(!GetOwner()->CheckId(id))
	throw new EComponentIdAlreadyExist(id);

   GetOwner()->SetLookupComponent(Name, id);
  }
 Id=id;
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Создает копию этого объекта с сохранением всех компонент
// и значений параметров.
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
UEPtr<UAContainer> UAContainer::Alloc(UEPtr<UAContainerStorage> stor, bool copystate)
{
 UEPtr<UAContainer> copy;
 UEPtr<UAContainerStorage> storage=(stor!=0)?stor:GetStorage();

 if(storage)
 {
  copy=dynamic_pointer_cast<UAContainer>(storage->TakeObject(Class,this));
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
bool UAContainer::Copy(UEPtr<UAContainer> target, UEPtr<UAContainerStorage> stor, bool copystate) const
{
 CopyProperties(target);
 target->Build();

 if(copystate)
  CopyState(target);

 CopyComponents(target,stor);
 return true;
}

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UAContainer::Free(void)
{
 while(NumComponents)
  PComponents[0]->Free();

 if(Storage)
 {
  BreakOwner();
  GetStorage()->ReturnObject(this);
 }
 else
  delete this;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Возвращает число дочерних компонент
int UAContainer::GetNumComponents(void) const
{
 return NumComponents;
}


// Возвращает полное число дочерних компонент
// (включая все компоненты дочерних компонент)
int UAContainer::GetNumAllComponents(void) const
{
 int res=NumComponents;

 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  res+=(*comps)->GetNumAllComponents();

 return res;
}


// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool UAContainer::CheckComponentType(UEPtr<UAContainer> comp) const
{
 return false;
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому Id 'id'
// Если id == ForbiddenId то возвращает указатель на этот компонент
UEPtr<UAContainer> UAContainer::GetComponent(const UId &id) const
{
 if(id == ForbiddenId)
  throw new EComponentIdNotExist(id);

 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(id == (*comps)->Id)
   return *comps;

 throw new EComponentIdNotExist(id);
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому имени 'name'
UEPtr<UAContainer> UAContainer::GetComponent(const NameT &name) const
{
 return GetComponent(GetComponentId(name));
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по ДЛИННОМУ Id 'id'.
// Если id[0] == ForbiddenId или Id имеет нулевой размер,
// то возвращает указатель на этот компонент
UEPtr<UAContainer> UAContainer::GetComponentL(const ULongId &id) const
{
 UEPtr<UAContainer> comp;

 if(id.GetSize() == 0)
  return 0;

 comp=GetComponent(id[0]);
 for(int i=1;i<id.GetSize();i++)
  {
   if(!comp)
    return 0;
   comp=comp->GetComponent(id[i]);
  }
 return comp;
}


// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по ДЛИННОМУ имени 'name'
UEPtr<UAContainer> UAContainer::GetComponentL(const NameT &name) const
{
 UEPtr<UAContainer> comp;
 NameT::size_type pi,pj;

 pi=name.find_first_of('.');
 if(pi == NameT::npos)
  return GetComponent(name);

 comp=GetComponent(name.substr(0,pi));
 while(pi != name.size())
  {
   if(!comp)
    return 0;
   pj=pi+1;
   pi=name.find_first_of('.',pj);
   if(pi == NameT::npos)
    pi=name.size();
   comp=comp->GetComponent(name.substr(pj,pi-pj));
  }
 return comp;
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по порядковому индеку в списке компонент
// Метод возвращает 0, если индекс выходит за границы массива
UEPtr<UAContainer> UAContainer::GetComponentByIndex(int index) const
{
 return Components[index];
}

// Добавляет дочерний компонент в этот объект
// Возвращает его Id или ForbiddenId если добавление неудачно
// Может быть передан указатель на локальную переменную
void UAContainer::BeforeAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 ABeforeAddComponent(comp,pointer);
}

void UAContainer::AfterAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 AAfterAddComponent(comp,pointer);
}

UId UAContainer::AddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 if(comp->GetOwner() == this)
  return comp->Id;

 if(comp->GetOwner())
  throw new EAddComponentAlreadyHaveOwner(comp->Id);

 if(!CheckComponentType(comp))
  throw new EAddComponentHaveInvalidType(comp->Id);

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
  throw EComponentIdAlreadyExist(id);

 comp->SetId(id);
 comp->SetOwner(this);

 // Добавляем компонент в таблицу соответствий владельца
 SetLookupComponent(comp->Name, comp->Id);

 // Добавление в базу компонент
 AddComponentTable(comp,pointer);

 comp->OwnerTimeStep=TimeStep;

 if(MainOwner)
  comp->SetMainOwner(MainOwner);

 try{
  AAddComponent(comp,pointer);
  comp->SharesInit();
  AfterAddComponent(comp,pointer);
 }
 catch(Exception *exception)
 {
  // Откат
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
void UAContainer::DelComponent(const UId &id, bool canfree)
{
 UEPtr<UAContainer> comp=GetComponent(id);

 DelComponent(comp, canfree);
}

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Таким образом 'name' - должно быть коротким именем
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
void UAContainer::DelComponent(const NameT &name, bool canfree)
{
 DelComponent(GetComponentId(name),canfree);
}

// Принудительно удаляет все дочерние компоненты
void UAContainer::DelAllComponents(void)
{
 while(NumComponents)
  DelComponent(PComponents[NumComponents-1],true);
}

// Возвращает список имен и Id компонент, содержащихся непосредственно
// в этом объекте
// Память должна быть выделена
void UAContainer::GetComponentsList(std::vector<UId> &buffer) const
{
 UEPtr<UAContainer> *pcomps=PComponents;
 buffer.resize(0);
 buffer.reserve(NumComponents);
 for(int i=0;i<NumComponents;i++,pcomps++)
  buffer.push_back((*pcomps)->Id);
}

// Копирует все компоненты этого объекта в объект 'comp', если возможно.
void UAContainer::CopyComponents(UEPtr<UAContainer> comp, UEPtr<UAContainerStorage> stor) const
{
 UEPtr<UAContainer> bufcomp;

 // Удаляем лишние компоненты из 'comp'
 comp->DelAllComponents();

 UEPtr<UAContainer> * pcomponents=0;
 PointerMapCIteratorT I;
 PointerMapIteratorT J;
 if(NumComponents>0)
  pcomponents=&PComponents[0];

 for(int i=0;i<NumComponents;i++,pcomponents++)
  {
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
// --------------------------


// ----------------------
// Методы управления коммуникационными компонентами
// ----------------------
// Возвращает список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
ULongIdVector& UAContainer::GetConnectorsList(ULongIdVector &buffer,
                            int sublevel, UEPtr<UAContainer> ownerlevel)
{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UAContainer> cont=GetComponentByIndex(i);
  UEPtr<UAConnector> temp;
  temp=dynamic_pointer_cast<UAConnector>(cont);
  if(temp)
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UAContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetConnectorsList(buffer,(sublevel<0)?sublevel:sublevel-1,ownerlevel);
 }

 return buffer;
}

// Возвращает список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
ULongIdVector& UAContainer::GetItemsList(ULongIdVector &buffer,
							int sublevel, UEPtr<UAContainer> ownerlevel)

{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UAContainer> cont=GetComponentByIndex(i);
  UEPtr<UAItem> temp;
  temp=dynamic_pointer_cast<UAItem>(cont);
  if(temp)
//  if(dynamic_cast<UAItem*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UAContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetItemsList(buffer,(sublevel<0)?sublevel:sublevel-1,ownerlevel);
 }

 return buffer;
}

// Возвращает список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
ULongIdVector& UAContainer::GetNetsList(ULongIdVector &buffer,
                            int sublevel, UEPtr<UAContainer> ownerlevel)
{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UAContainer> cont=GetComponentByIndex(i);
  UEPtr<UANet> temp;
  temp=dynamic_pointer_cast<UANet>(cont);
  if(temp)
//  if(dynamic_cast<UANet*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UAContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetNetsList(buffer,(sublevel<0)?sublevel:sublevel-1,ownerlevel);
 }

 return buffer;
}
// ----------------------


// --------------------------
// Методы управления локальными указателями
// --------------------------
// Устанавливает дочерний компонент 'id' в качестве заданного класса локальных указателей
// 'pointerid'
bool UAContainer::SetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UAContainer> cont=GetComponent(id);

 PointerMapIteratorT I=PointerLookupTable.end();
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
bool UAContainer::SetComponentAs(const NameT &name,const NameT &pointername)
{
 UEPtr<UAContainer> cont=GetComponent(name);

 PointerMapIteratorT I=PointerLookupTable.end();;
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
bool UAContainer::ResetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UAContainer> cont=GetComponent(id);

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
bool UAContainer::ResetComponentAs(const NameT &name,const NameT &pointername)
{
 UEPtr<UAContainer> cont=GetComponent(name);

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
bool UAContainer::ResetComponentAll(const UId &id)
{
 UEPtr<UAContainer> cont=GetComponent(id);

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
bool UAContainer::ResetComponentAll(const NameT &name)
{
 UEPtr<UAContainer> cont=GetComponent(name);

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
void UAContainer::DelAllComponentsAs(const NameT &pointername, bool canfree)
{
 PointerMapIteratorT J=PointerLookupTable.find(pointername);

 if(J != PointerLookupTable.end())
 {
  UEPtr<UAContainer> cont=J->second.Pointer->Get();

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
void UAContainer::SharesInit(void)
{
 ASharesInit();
}

// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
void UAContainer::SharesUnInit(void)
{
 ASharesUnInit();
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UAContainer::Default(void)
{
 Ready=false;
 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Default();

 // Если существует прообраз в хранилище, то берем настройки параметров
 // из прообраза
 UEPtr<UAContainer> original;
 if(Storage)
  original=dynamic_pointer_cast<UAContainer>(GetStorage()->GetClass(Class));

 SetTimeStep(2000);

 if(original && original != this)
 {
  NameT name=Name;
  bool activity=Activity;
  original->CopyProperties(this);
  SetName(name);
  SetActivity(activity);
 }

 return ADefault();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UAContainer::Build(void)
{
 if(Ready)
  return true;

 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Build();

 ABuild();
 Ready=true;
 Reset();

 return true;
}

// Сброс процесса счета.
bool UAContainer::Reset(void)
{
 Build();

 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Reset();

 AReset();

 CalcCounter=0;
 SkipComponentCalculation=false;
 ComponentReCalculation=false;
 return true;
}

// Выполняет расчет этого объекта
bool UAContainer::Calculate(void)
{
 if(!Activity)
  return true;

 Build();

 long long tempstepduration=GetCurrentStartupTime();
 UEPtr<UAContainer> *comps=PComponents;
 int i=0;
 while((i<NumComponents) & !SkipComponentCalculation)
 {
  (*comps)->Calculate();
  if(ComponentReCalculation)
  {
   ComponentReCalculation=false;
   i=0; comps=PComponents;
  }
  else
  {
   ++i,++comps;
  }
 }

 SkipComponentCalculation=false;
 ComponentReCalculation=false;

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
   for(UTime i=TimeStep/OwnerTimeStep;i>=0;--i)
	ACalculate();
  }

 UpdateMainOwner();
 StepDuration=CalcDiffTime(GetCurrentStartupTime(),tempstepduration);
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
 return true;
}

// Обновляет состояние MainOwner после расчета этого объекта
void UAContainer::UpdateMainOwner(void)
{
 if(!MainOwner)
  return;

 return AUpdateMainOwner();
}

// Обычно вызывается дочерним компонентом и прерывает обсчет цепочки дочерних
// компонент на этом шаге счета
void UAContainer::ForceSkipComponentCalculation(void)
{
 SkipComponentCalculation=true;
}

// Обычно вызывается дочерним компонентом и требует перерасчет цепочки дочерних
// компонент на этом шаге счета сначала
void UAContainer::ForceComponentReCalculation(void)
{
 ComponentReCalculation=true;
}
// --------------------------


// --------------------------
// Скрытые методы управления таблицей соответсвий компонент
// --------------------------
// Обновляет таблицу соответствий компонент заменяя 'oldname'
// имя компонента на 'newname'
void UAContainer::ModifyLookupComponent(const NameT &oldname,
										const NameT newname)
{
 UId id;

 std::map<NameT,UId>::iterator I=CompsLookupTable.find(oldname);
 if(I == CompsLookupTable.end())
  throw new EComponentNameNotExist(oldname);

 id=I->second;
 CompsLookupTable.erase(I);
 CompsLookupTable[newname]=id;
}

// Обновляет таблицу соответствий компонент устанавливая Id 'id'
// для компонента с именем 'name'
void UAContainer::SetLookupComponent(const NameT &name, const UId &id)
{
 CompsLookupTable[name]=id;
 if(LastId<id)
  LastId=id;
}

// Обновляет таблицу соответствий компонент удаляя запись
// компонента с именем 'name'
void UAContainer::DelLookupComponent(const NameT &name)
{
 std::map<NameT,UId>::iterator I=CompsLookupTable.find(name);

 if(I == CompsLookupTable.end())
  throw new EComponentNameNotExist(name);
 CompsLookupTable.erase(name);
}
// --------------------------

// --------------------------
// Методы управления контроллерами интерфейса
// Удаление контроллеров лежит на вызывающем модуле
// --------------------------
// Добавляет новый контроллер
void UAContainer::AddController(UEPtr<UController> controller, bool forchilds)
{
 if(CheckController(controller))
  return;

 Controllers.push_back(controller);
 if(forchilds)
 {
  UEPtr<UAContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->AddController(controller,forchilds);
 }
}

// Удаляет контроллер из списка
void UAContainer::DelController(UEPtr<UController> controller, bool forchilds)
{
 vector<UEPtr<UController> >::iterator I=find(Controllers.begin(),Controllers.end(),controller);

 if(I != Controllers.end())
  Controllers.erase(I);

 if(forchilds)
 {
  UEPtr<UAContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->DelController(controller,forchilds);
 }
}

// Удаляет все контроллеры
void UAContainer::DelAllControllers(bool forchilds)
{
 Controllers.clear();
 if(forchilds)
 {
  UEPtr<UAContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->DelAllControllers(forchilds);
 }
}

// Инициирует отключение всех контроллеров
void UAContainer::UnLinkAllControllers(bool forchilds)
{
 while(Controllers.begin() != Controllers.end())
  Controllers.front()->UnLink(this);

 if(forchilds)
 {
  UEPtr<UAContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->UnLinkAllControllers(forchilds);
 }
}


// Проверяет, существует ли контроллер в списке
bool UAContainer::CheckController(UEPtr<UController> controller) const
{
 if(find(Controllers.begin(),Controllers.end(),controller) != Controllers.end())
  return true;
 return false;
}

// Возвращает число контроллеров
size_t UAContainer::GetNumControllers(void) const
{
 return Controllers.size();
}

// Возвращает контроллер по индексу
UEPtr<UController> UAContainer::GetController(int index)
{
 return Controllers[index];
}
// --------------------------

// --------------------------
// Скрытые методы управления локальными указателями
// --------------------------
// Добавляет указатель в таблицу соотвествий
// Должна вызываться в конструкторах классов
UId UAContainer::AddLookupPointer(const NameT &name, UEPtr<UIPointer> pointer)
{
 UPVariable P(1,pointer);

 if(PointerLookupTable.find(name) != PointerLookupTable.end())
  throw EPointerNameAlreadyExist(name);

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
void UAContainer::DelLookupPointer(const NameT &name)
{
 PointerMapIteratorT I=PointerLookupTable.find(name);

 if(I == PointerLookupTable.end())
  throw new EPointerNameNotExist(name);

 delete I->second.Pointer;
 PointerLookupTable.erase(I);
}
/*
// Возвращает полное имя указателя без префикса NMSDK, и суффикса '*'
NameT UAContainer::GetPointerLongName(const UIPointer &pointer) const
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
UAContainer::PointerMapCIteratorT UAContainer::FindLookupPointer(UEPtr<UAContainer> source) const
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
void UAContainer::ASharesInit(void)
{
}

// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
void UAContainer::ASharesUnInit(void)
{
}
// --------------------------

// --------------------------
// Скрытые методы управления таблицей компонент
// --------------------------
// Добавляет компонент 'comp' в таблицу компонент
void UAContainer::AddComponentTable(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
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

void UAContainer::DelComponentTable(UEPtr<UAContainer> comp)
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

   memmove(PComponents+i,PComponents+i+1,(NumComponents-i-1)*sizeof(UEPtr<UAContainer>));
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
// Удаляет компонент comp
// Метод предполагает, что компонент принадлежит объекту
void UAContainer::BeforeDelComponent(UEPtr<UAContainer> comp, bool canfree)
{
 ABeforeDelComponent(comp,canfree);
}

void UAContainer::AfterDelComponent(UEPtr<UAContainer> comp, bool canfree)
{
 AAfterDelComponent(comp,canfree);
}

void UAContainer::DelComponent(UEPtr<UAContainer> comp, bool canfree)
{
 BeforeDelComponent(comp,canfree);
 SharesUnInit();
 ADelComponent(comp);

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
}


// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
void UAContainer::ABeforeAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{

}

void UAContainer::AAfterAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{

}

bool UAContainer::AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
void UAContainer::ABeforeDelComponent(UEPtr<UAContainer> comp, bool canfree)
{

}

void UAContainer::AAfterDelComponent(UEPtr<UAContainer> comp, bool canfree)
{

}

bool UAContainer::ADelComponent(UEPtr<UAContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Обновляет состояние MainOwner после расчета этого объекта
void UAContainer::AUpdateMainOwner(void)
{
}
// --------------------------

/* *************************************************************************** */
/* Классы исключений */
// class EIContainer
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAContainer::EIContainer::EIContainer(void)
{

}

UAContainer::EIContainer::EIContainer(const UAContainer *cont)
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


UAContainer::EIContainer::EIContainer(const EIContainer &copy)
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

UAContainer::EIContainer::~EIContainer(void)
{

}
// --------------------------


// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UAContainer::EIContainer::CreateLogMessage(void) const
{
 string result;//=UException::CreateLogMessage();

// EIContainer *iexception=dynamic_cast<EIContainer*>(exception);

// if(iexception)
 {
  // Короткое имя компонента в котором сгенерировано исключение
  result+=" Name=";
  result+=Name;

  // Короткий идентификатор компонента в котором сгенерировано исключение
//  result+=" Id=";
//  result+=iexception->Id;

  // Полное имя владельца компонента в котором сгенерировано исключение
  result+=" OwnerName=";
  result+=OwnerName;

  // Полный идентификатор владельца компонента в котором сгенерировано исключение
//  result+=" OwnerId=";
//  result+=iexception->OwnerId;

  if(MainOwnerName != OwnerName && MainOwnerName.size()>0)
  {
   // Полное имя главного владельца компонента в котором сгенерировано исключение
   result+=" MainOwnerName=";
   result+=MainOwnerName;
  }

  // Полный идентификатор главного владельца компонента в котором сгенерировано исключение
//  result+=" MainOwnerId=";
//  result+=iexception->MainOwnerId;
 }


 return result;
}
// --------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAContainer::EComponentCalculate::EComponentCalculate(void)
{

}

UAContainer::EComponentCalculate::EComponentCalculate(const UAContainer *cont, const UAContainer *subcont)
 : EError(), EIContainer(cont)
{
 if(!subcont)
  return;

 // Короткое имя компонента в котором сгенерировано исключение
 SubName=subcont->GetName();

 // Короткий идентификатор компонента в котором сгенерировано исключение
 SubId=subcont->GetId();
}


UAContainer::EComponentCalculate::EComponentCalculate(const EComponentCalculate &copy)
 : EError(copy), EIContainer(copy)
{
 // Короткое имя компонента в котором сгенерировано исключение
 SubName=copy.SubName;

 // Короткий идентификатор компонента в котором сгенерировано исключение
 SubId=copy.SubId;
}

UAContainer::EComponentCalculate::~EComponentCalculate(void)
{

}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UAContainer::EComponentCalculate::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+EIContainer::CreateLogMessage();
}
// --------------------------




}

#endif


