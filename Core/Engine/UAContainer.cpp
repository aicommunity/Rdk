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
#include "UAContainer.h"
#include "UAContainerStorage.h"
#include "UAConnector.h"
#include "UAItem.h"
#include "UANet.h"
#include "../Serialize/Serialize.h"
#include "../Math/MUBinarySerialize.h"
#include "../Math/MUXMLSerialize.h"
#include "UAContainerStorage.h"

namespace RDK {

NameT ForbiddenName="";

/* *********************************************************************** */
/* *********************************************************************** */
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAContainerVector::UAContainerVector(void)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;
}

UAContainerVector::UAContainerVector(const UAContainerVector &copy)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;

 *this=copy;
}

UAContainerVector::~UAContainerVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Возвращает размер массива
int UAContainerVector::GetSize(void) const
{
 return Size;
}

// Изменяет размер массива с сохранением прежних данных
void UAContainerVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  PUAContainer *newbuffer=new PUAContainer[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  for(int i=Size;i<newsize;i++)
   newbuffer[i]=0;

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Очищает массив
void UAContainerVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// Возвращает указатель на начало данных
PUAContainer* UAContainerVector::GetBuffer(void)
{
 return Buffer;
}

// Добавляет элемент в конец массива
// Возвращает индекс элемента
int UAContainerVector::Add(PUAContainer value)
{
 Resize(Size+1);
 Buffer[Size-1]=value;
 return Size-1;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UAContainerVector& UAContainerVector::operator = (const UAContainerVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  *Buffer[i]=*copy.Buffer[i];

 return *this;
}

// Оператор доступа
PUAContainer& UAContainerVector::operator [] (int index)
{
 return Buffer[index];
}

UAContainer& UAContainerVector::operator () (int index)
{
 return *Buffer[index];
}
// --------------------------

/* *********************************************************************** */
/* *********************************************************************** */


/* *************************************************************************** */
// Class UAContainer
/* *************************************************************************** */
// --------------------------
// Глобальные свойства
// --------------------------
// Текущее время модели
ULongTime UAContainer::Time=0;
double UAContainer::DoubleTime=0;
// --------------------------

// --------------------------
// Методы управления глобальными свойствами
// --------------------------
// Возвращает текущее время модели
const ULongTime& UAContainer::GetTime(void)
{
 return Time;
}

const double& UAContainer::GetDoubleTime(void)
{
 return DoubleTime;
}

// Устанавливает текущее время модели
bool UAContainer::SetTime(ULongTime value)
{
 if(Time == value)
  return true;

 Time=value;
 DoubleTime=Time/1000000.0;
 return true;
}
// --------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAContainer::UAContainer(void)
{
 // Указатель на 0-й элемент вектора компонент
 PComponents=0;

 // Количество компонент в векторе компонент
 NumComponents=0;

 LastId=0;

 AddLookupProperty("Id",new UVProperty<UId,UAContainer>(this,&UAContainer::SetId,&UAContainer::GetId));
 AddLookupProperty("Name",new UVProperty<NameT,UAContainer>(this,&UAContainer::SetName,&UAContainer::GetName));
 AddLookupProperty("TimeStep",new UVProperty<UTime,UAContainer>(this,&UAContainer::SetTimeStep,&UAContainer::GetTimeStep));
 AddLookupProperty("Activity",new UVProperty<bool,UAContainer>(this,&UAContainer::SetActivity,&UAContainer::GetActivity));
 AddLookupProperty("Coord",new UVProperty<RDK::MVector<double>,UAContainer>(this,&UAContainer::SetCoord,&UAContainer::GetCoord));
}

UAContainer::~UAContainer(void)
{
 ClearLookupPropertyTable();
 ClearLookupStateTable();
 DelAllComponents();
 UnLinkAllControllers();

 if(!BreakOwner())
  return;// Заглушка! Неустранимая ошибка, требуется исключение

 UAContainerStorage *cstorage=dynamic_cast<UAContainerStorage*>(Storage);
 if(cstorage)
  {
   cstorage->PopObject(this);
  }
}
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Проверяет, является ли объект owner
// владельцем этого объекта на каком-либо уровне иерархии
bool UAContainer::CheckOwner(const UAContainer *owner) const
{
 if(Owner == 0 && Owner != owner)
  return false;

 if(Owner == owner)
  return true;

 return static_cast<UAContainer*>(Owner)->CheckOwner(owner);
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

 static_cast<UAContainer*>(Owner)->GetFullId(buffer);
 buffer.Add(Id);

 return buffer;
}

// Возвращает  'длинный' Id объекта
// (исключая имя владельца 'mainowner').
// Метод возвращает пустой вектор, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии.
ULongId& UAContainer::GetLongId(const UAContainer *mainowner, ULongId &buffer) const
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

 if(static_cast<UAContainer*>(Owner)->GetLongId(mainowner,buffer).GetSize() == 0)
  return buffer;

 buffer.Add(Id);

 return buffer;
}
                 /*
// Возвращает последний использованный Id компонент
UId UAContainer::GetLastId(void) const
{
 return LastId;
}                  */
// --------------------------

// --------------------------
// Методы управления свойствами
// --------------------------
// Координата компонента в пространстве сети
RDK::MVector<double> UAContainer::GetCoord(void) const
{
 return Coord;
}

bool UAContainer::SetCoord(RDK::MVector<double> value)
{
 if(Coord == value)
  return true;

 Coord=value;
 return true;
}

// Удаляет владельца объекта
bool UAContainer::BreakOwner(void)
{
 if(Owner)
  return static_cast<UAContainer*>(Owner)->DelComponent(this,false);

 return true;
}

// Устанавливает указатель на главного владельца этим объектом
// Указатель устанавливается на число уровней дочерних компонент
// 'levels'. Если levels < 0 то устанавливается компонентам на всех уровнях
bool UAContainer::SetMainOwner(RDK::UAComponent* const mainowner)
{
 return RDK::UAComponent::SetMainOwner(mainowner);
}

bool UAContainer::SetMainOwner(RDK::UAComponent* const mainowner, int levels)
{
 if(MainOwner == mainowner && !levels)
  return true;

 MainOwner=mainowner;

 if(!levels)
  return true;

 for(int i=0;i<NumComponents;i++)
 {
  // Устанавливаем главного владельца только тем дочерним компонентам
  // у которых он еще не задан
  if(PComponents[i]->GetMainOwner() == 0)
  {
   if(levels<0)
   {
    if(!PComponents[i]->SetMainOwner(static_cast<UAContainer*>(MainOwner),levels))
     return false;
   }
   else
    if(!PComponents[i]->SetMainOwner(static_cast<UAContainer*>(MainOwner),levels-1))
     return false;
  }
 }

 return true;
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
 if(Name == name)
  return true;

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
 unsigned long int k=2;
 char buffer[20];

 while(true)
  {
   sprintf(buffer,"%i",k);
   if(prefix.size()>0)
    namebuffer=prefix+buffer;
   else
    namebuffer=NameT("n")+buffer;
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
   if(!dynamic_cast<UAContainer*>(GetOwner())->CheckName(name))
    return false;

   if(!dynamic_cast<UAContainer*>(GetOwner())->ModifyLookupComponent(Name, name))
    return false;
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

 dynamic_cast<const UAContainer*>(GetOwner())->GetFullName(buffer);
 buffer+='.';
 buffer+=Name;

 return buffer;
}

// Возвращает  'длинное' имени объекта
// (исключая имя владельца 'mainowner').
// Метод возвращает пустую строку, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии.
NameT& UAContainer::GetLongName(const UAContainer *mainowner, NameT &buffer) const
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

 if(dynamic_cast<UAContainer*>(GetOwner())->GetLongName(mainowner,buffer) == ForbiddenName)
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
 map<NameT,UId>::const_iterator I=CompsLookupTable.begin(),
                                 J=CompsLookupTable.end();
 while(I != J)
  {
   if(I->second == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// Возвращает Id дочернего компонента по его имени
const UId& UAContainer::GetComponentId(const NameT &name) const
{
 map<NameT,UId>::const_iterator I=CompsLookupTable.find(name);
 if(I == CompsLookupTable.end())
  return ForbiddenId;
 else return I->second;
}

// Возвращает имя параметра по его Id
const NameT& UAContainer::GetPropertyName(const UId &id) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                         J=PropertiesLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// Возвращает Id параметра по его имени
const UId& UAContainer::GetPropertyId(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);
 if(I == PropertiesLookupTable.end())
  return ForbiddenId;
 else return I->second.Id;
}

// Возвращает полное имя параметра без префикса RDK, и суффикса '*'
NameT UAContainer::GetPropertyLongName(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);
 if(I == PropertiesLookupTable.end())
  return NameT();
 else return GetPropertyLongName(*(I->second.Property));
}

NameT UAContainer::GetPropertyLongName(const UId &id) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                                 J=PropertiesLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return GetPropertyLongName(*(I->second.Property));
   ++I;
  }
 return NameT();
}


// Возвращает имя переменной состояния по его Id
const NameT& UAContainer::GetStateName(const UId &id) const
{
 VariableMapCIteratorT I=StateLookupTable.begin(),
                                 J=StateLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// Возвращает Id переменной состояния по его имени
const UId& UAContainer::GetStateId(const NameT &name) const
{
 VariableMapCIteratorT I=StateLookupTable.find(name);
 if(I == StateLookupTable.end())
  return ForbiddenId;
 else return I->second.Id;
}

// Возвращает полное имя переменной состояния без префикса NMSDK, и суффикса '*'
NameT UAContainer::GetStateLongName(const NameT &name) const
{
 VariableMapCIteratorT I=StateLookupTable.find(name);
 if(I == StateLookupTable.end())
  return NameT();
 else return GetStateLongName(*(I->second.Property));
}

NameT UAContainer::GetStateLongName(const UId &id) const
{
 VariableMapCIteratorT I=StateLookupTable.begin(),
                                 J=StateLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return GetStateLongName(*(I->second.Property));
   ++I;
  }
 return NameT();
}


// Возвращает имя локального указателя по его Id
const NameT& UAContainer::GetPointerName(const UId &id) const
{
 PointerMapCIteratorT I=PointerLookupTable.begin(),
                                 J=PointerLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// Возвращает Id локального указателя по его имени
const UId& UAContainer::GetPointerId(const NameT &name) const
{
 PointerMapCIteratorT I=PointerLookupTable.find(name);
 if(I == PointerLookupTable.end())
  return ForbiddenId;
 else return I->second.Id;
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
  OwnerTimeStep=static_cast<UAContainer*>(Owner)->TimeStep;
 else
  OwnerTimeStep=timestep;

 // Обращение ко всем компонентам объекта
 for(int i=0;i<NumComponents;i++)
  PComponents[i]->OwnerTimeStep=timestep;

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
 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Activity=activity;

 if(activity)
  return Reset();

 Activity=activity;

 return true;
}

UId UAContainer::GetClass(void) const
{
 return RDK::UAComponent::GetClass();
}

bool UAContainer::SetClass(UId value)
{
 return RDK::UAComponent::SetClass(value);
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

 if(Owner != 0)
  {
   if(!static_cast<UAContainer*>(Owner)->CheckId(id))
    return false;

   static_cast<UAContainer*>(Owner)->SetLookupComponent(Name, id);
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
UAContainer* UAContainer::Alloc(UAContainerStorage *stor, bool copystate)
{
 UAContainer* copy;
 UAContainerStorage *storage=(stor!=0)?stor:static_cast<UAContainerStorage*>(Storage);

 if(storage)
 {
  copy=static_cast<UAContainer*>(storage->TakeObject(Class,this));
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
bool UAContainer::Copy(UAContainer *target, UAContainerStorage *stor, bool copystate) const
{
 bool res=CopyProperties(target);
 if(!res)
  return false;

 if(!target->Build())
  return false;

 if(copystate)
 {
  res=CopyState(target);
  if(!res)
   return false;
 }

 res=CopyComponents(target,stor);
 if(!res)
  return false;

 return true;
}

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UAContainer::Free(void)
{
 if(Storage)
 {
  if(!BreakOwner())
   return; // Заглушка, здесь должно быть исключение!!
//   throw; // Заглушка, здесь должно быть исключение!!
  Storage->ReturnObject(this);
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

 for(int i=0;i<NumComponents;i++)
  res+=PComponents[i]->GetNumAllComponents();

 return res;
}


// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool UAContainer::CheckComponentType(const UAContainer* comp) const
{
 return false;
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому Id 'id'
// Если id == ForbiddenId то возвращает указатель на этот компонент
UAContainer* UAContainer::GetComponent(const UId &id) const
{
 if(id == ForbiddenId)
  return 0;

 for(int i=0;i<NumComponents;i++)
  if(id == PComponents[i]->Id)
   return PComponents[i];

 return 0;
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому имени 'name'
UAContainer* UAContainer::GetComponent(const NameT &name) const
{
 return GetComponent(GetComponentId(name));
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по ДЛИННОМУ Id 'id'.
// Если id[0] == ForbiddenId или Id имеет нулевой размер,
// то возвращает указатель на этот компонент
UAContainer* UAContainer::GetComponentL(const ULongId &id) const
{
 UAContainer* comp;

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
UAContainer* UAContainer::GetComponentL(const NameT &name) const
{
 UAContainer* comp;
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
UAContainer* UAContainer::GetComponentByIndex(int index) const
{
 if(index >= NumComponents)
  return 0;

 return PComponents[index];
}

// Добавляет дочерний компонент в этот объект
// Возвращает его Id или ForbiddenId если добавление неудачно
// Может быть передан указатель на локальную переменную
UId UAContainer::AddComponent(UAContainer* comp, UIPointer* pointer)
{
// NameT namebuffer;

 if(!comp)
  return ForbiddenId;

 if(comp->GetOwner() == this)
  return comp->Id;

 if(comp->GetOwner())
  return ForbiddenId;

 if(!CheckComponentType(comp))
  return ForbiddenId;

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

 comp->SetId(id);

 comp->SetOwner(this);


 // Добавляем компонент в таблицу соответствий владельца
 SetLookupComponent(comp->Name, comp->Id);

 // Добавление в базу компонент
 AddComponentTable(comp,pointer);

 comp->OwnerTimeStep=TimeStep;

 if(MainOwner)
  comp->SetMainOwner(MainOwner);

 if(!AAddComponent(comp,pointer))
  {
   // Откат
   // Удаляем компонент из таблицы соответствий владельца
   DelLookupComponent(comp->Name);

   // Удаление из базы компонент
   DelComponentTable(comp);

   comp->Owner=0;

   return ForbiddenId;
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
bool UAContainer::DelComponent(const UId &id, bool canfree)
{
 UAContainer *comp=GetComponent(id);

 if(!comp)
  return false;

 return DelComponent(comp, canfree);
}

// Принудительно удаляет все дочерние компоненты
void UAContainer::DelAllComponents(void)
{
 while(NumComponents)
  DelComponent(PComponents[NumComponents-1],true);
/*
 for(int i=0;i<NumComponents;i++)
 {
  PComponents[i]->Owner=0;
  PComponents[i]->Free();
 }

 Components.Resize(0);
 PComponents=0;
 NumComponents=0;
// CompsLookupTable.clear();
*/
}

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Таким образом 'name' - должно быть коротким именем
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
bool UAContainer::DelComponent(const NameT &name, bool canfree)
{
 return DelComponent(GetComponentId(name),canfree);
}

// Возвращает список имен и Id компонент, содержащихся непосредственно
// в этом объекте
// Память должна быть выделена
void UAContainer::GetComponentsList(UId *buffer) const
{
 UAContainer **pcomps=PComponents;
 for(int i=0;i<NumComponents;i++,buffer++,pcomps++)
  *buffer=(*pcomps)->Id;
}

// Копирует все компоненты этого объекта в объект 'comp', если возможно.
bool UAContainer::CopyComponents(UAContainer* comp, UAContainerStorage* stor) const
{
 UAContainer* bufcomp;

 if(!comp)
  return false;

 // Удаляем лишние компоненты из 'comp'
 comp->DelAllComponents();

 UAContainer ** pcomponents=0;
 PointerMapCIteratorT I;
 PointerMapIteratorT J;
 if(NumComponents>0)
  pcomponents=&PComponents[0];

 for(int i=0;i<NumComponents;i++,pcomponents++)
  {
   bufcomp=(*pcomponents)->Alloc(stor);
   UIPointer *pointer=0;
   I=FindLookupPointer(*pcomponents);
   if(I != PointerLookupTable.end())
   {
    J=comp->PointerLookupTable.find(I->first);
    if(J != comp->PointerLookupTable.end())
     pointer=J->second.Pointer;
   }

   if(comp->AddComponent(bufcomp,pointer) == ForbiddenId)
    return false;

   bufcomp->SetId((*pcomponents)->Id);
   comp->SetLookupComponent(bufcomp->GetName(), bufcomp->GetId());
  }
 return true;
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
   map<UId,UIPointer*>::const_iterator I=FindLookupPointer(PComponents[i]);
   if(I != PointerLookupTable.end())
   {
    map<UId,UIPointer*>::iterator J=comp->PointerLookupTable.find(I->first);
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

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Возвращает значение параметра по Id 'id'
UVariableData* UAContainer::GetProperty(const UId &id, UVariableData *values) const
{
 VariableMapCIteratorT I,J;

 I=PropertiesLookupTable.begin();
 J=PropertiesLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Save(values);
   return values;
  }

  ++I;
 }

 return values;
}

// Возвращает значение параметра по имени 'name'
UVariableData* UAContainer::GetProperty(const NameT &name, UVariableData *values) const
{
 return GetProperty(GetPropertyId(name),values);
}

// Устанавливает значение параметра по Id 'id'
bool UAContainer::SetProperty(const UId &id, UVariableData *values)
{
 VariableMapCIteratorT I,J;

 I=PropertiesLookupTable.begin();
 J=PropertiesLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   return I->second.Property->Load(values);
  }

  ++I;
 }

 return false;
}

// Устанавливает значение параметра по имени 'name'
bool UAContainer::SetProperty(const NameT &name, UVariableData *values)
{
 return SetProperty(GetPropertyId(name),values);
}

const UAContainer::VariableMapT& UAContainer::GetPropertiesList(void) const
{
 return PropertiesLookupTable;
}

// Копирует все параметры этого объекта в объект 'comp', если возможно.
bool UAContainer::CopyProperties(UAContainer* comp) const
{
 bool key=true;

 if(!comp)
  return false;

 UAContainer* ccomp=static_cast<UAContainer*>(comp);

 Serialize::USerStorageBinary databuffer;
  VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                                        J=PropertiesLookupTable.end();
  while(I != J)
  {
   databuffer.clear();
   key &= ccomp->SetProperty(I->second.Id,GetProperty(I->second.Id,&databuffer));
   ++I;
  }
 return key;
}

// Ищет имя свойства по указателю на него
const NameT& UAContainer::FindPropertyName(const UIProperty *prop) const
{
  VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                        J=PropertiesLookupTable.end();
  while(I != J)
  {
   if(I->second.Property == prop)
    return I->first;
   ++I;
  }
 return ForbiddenName;
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
                            int sublevel, UAContainer *ownerlevel)
{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UAContainer *cont=GetComponentByIndex(i);
  if(dynamic_cast<UAConnector*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:this,id);
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
                            int sublevel, UAContainer *ownerlevel)

{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UAContainer *cont=GetComponentByIndex(i);
  if(dynamic_cast<UAItem*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:this,id);
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
                            int sublevel, UAContainer *ownerlevel)
{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UAContainer *cont=GetComponentByIndex(i);
  if(dynamic_cast<UANet*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:this,id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetNetsList(buffer,(sublevel<0)?sublevel:sublevel-1,ownerlevel);
 }

 return buffer;
}
// ----------------------


// --------------------------
// Методы доступа к переменным состояния
// --------------------------
// Возвращает значение переменной состояния по Id 'id'
UVariableData* UAContainer::GetState(const UId &id, UVariableData *values) const
{
 VariableMapCIteratorT I,J;

 I=StateLookupTable.begin();
 J=StateLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Save(values);
   return values;
  }

  ++I;
 }

 return values;
}

// Возвращает значение переменной состояния по имени 'name'
UVariableData* UAContainer::GetState(const NameT &name, UVariableData *values) const
{
 return GetState(GetStateId(name),values);
}

// Устанавливает значение переменной состояния по Id 'id'
bool UAContainer::SetState(const UId &id, UVariableData *values)
{
 VariableMapCIteratorT I,J;

 I=StateLookupTable.begin();
 J=StateLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   return I->second.Property->Load(values);
  }

  ++I;
 }

 return false;
}

// Устанавливает значение переменной состояния по имени 'name'
bool UAContainer::SetState(const NameT &name, UVariableData *values)
{
 return SetState(GetStateId(name),values);
}

// Возвращает список имен и Id переменных состояния, содержащихся непосредственно
// в этом объекте
const UAContainer::VariableMapT& UAContainer::GetStateList(void) const
{
 return StateLookupTable;
}

// Ищет имя свойства по указателю на него
const NameT& UAContainer::FindStateName(const UIProperty *prop) const
{
  VariableMapCIteratorT I=StateLookupTable.begin(),
                        J=StateLookupTable.end();
  while(I != J)
  {
   if(I->second.Property == prop)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// Копирует все переменные состояния этого объекта в объект 'comp', если возможно.
bool UAContainer::CopyState(UAContainer* comp) const
{
 VariableMapCIteratorT I=StateLookupTable.begin(),
                       J=StateLookupTable.end();
 bool key=true;

 if(!comp)
  return false;

 Serialize::USerStorageBinary serstorage;
 while(I != J)
  {
   if(!comp->SetState(I->second.Id,GetState(I->second.Id,&serstorage)))
    key=false;
   ++I;
  }
 return key;
}
// --------------------------

// --------------------------
// Методы управления локальными указателями
// --------------------------
// Устанавливает дочерний компонент 'id' в качестве заданного класса локальных указателей
// 'pointerid'
bool UAContainer::SetComponentAs(const UId &id, const UId &pointerid)
{
 UAContainer* cont=GetComponent(id);

 if(!cont)
  return false;

 PointerMapIteratorT I=PointerLookupTable.end();;
 PointerMapIteratorT K=PointerLookupTable.end();;
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
 UAContainer* cont=GetComponent(name);

 if(!cont)
  return false;

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
 UAContainer* cont=GetComponent(id);

 if(!cont)
  return false;

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
 UAContainer* cont=GetComponent(name);

 if(!cont)
  return false;

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
 UAContainer* cont=GetComponent(id);

 if(!cont)
  return false;

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
 UAContainer* cont=GetComponent(name);

 if(!cont)
  return false;

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
  UAContainer *cont=J->second.Pointer->Get();

  while(cont)
  {
   DelComponent(cont,canfree);
   cont=J->second.Pointer->Get();
  }
 }
}
// --------------------------

// --------------------------
// Методы управления текущим состоянием
// --------------------------
/*
// Сохраняет объект в контейнер сериализации
bool UAContainer::Save(UVariableData *values)
{
 NameT name=Name;

 comp->SetName(name);

 values->

 map<NameT,NVariable>::iterator I,J;
 I=ParamsLookupTable.begin();
 J=ParamsLookupTable.end();

 NameT str,str2,paramname;
 while(I != J)
 {
  paramname=GetParameterLongName(*(I->second.Property));
//  paramname+="::";
//  paramname+=I->first;
  stringstream stream(ios::out);
  I->second.Property->Save(stream);
  (*comp)()[paramname][I->first]=stream.str();
  ++I;
 }

 buffer.AddSorted(comp);

 for(int i=0;i<NumComponents;i++)
  if(!PComponents[i]->Save(*comp))
   return false;

 return true;
}

// Загружает объект из контейнера сериализации
// Для создания компонент используется
// хранилище этого объекта или storage, если он не 0
// Если никакое хранилище не задано, то не делает ничего
// Если флаг paramsonly == true то только обновляет данные параметров
// для компонента и всех его уже существующих дочерних компонент
bool UAContainer::Load(UVariableData *values, UAContainerStorage *storage, bool paramsonly)
{
}
            */
/* // Сохраняет объект в дерево
bool UAContainer::Save(RDK::UClassRegistry &buffer)
{
 RDK::UClassRegistry *comp=new RDK::UClassRegistry;

 NameT name=Name;

 if(Storage)
 {
  UId classid=Storage->FindClass(this);
  if(classid != ForbiddenId)
  {
   name+=": ";
   name+=static_cast<UAContainerStorage*>(Storage)->GetClassName(classid);
  }

 }

 comp->SetName(name);

 map<NameT,NVariable>::iterator I,J;
 I=ParamsLookupTable.begin();
 J=ParamsLookupTable.end();

 NameT str,str2,paramname;
 while(I != J)
 {
  paramname=GetParameterLongName(*(I->second.Property));
//  paramname+="::";
//  paramname+=I->first;
  stringstream stream(ios::out);
  I->second.Property->Save(stream);
  (*comp)()[paramname][I->first]=stream.str();
  ++I;
 }

 buffer.AddSorted(comp);

 for(int i=0;i<NumComponents;i++)
  if(!PComponents[i]->Save(*comp))
   return false;

 return true;
}

// Загружает объект из дерева
// Для создания компонент используется
// хранилище этого объекта или storage, если он не 0
// Если никакое хранилище не задано, то не делает ничего
bool UAContainer::Load(RDK::UClassRegistry &buffer, UAContainerStorage *storage, bool paramsonly)
{
 // Текущая версия грузит только параметры

 if(!Storage && !storage && paramsonly == false)
  return false;

 UAContainerStorage* stor=(storage != 0)?storage:static_cast<UAContainerStorage*>(Storage);

 UAContainer *comp=0;

 // Восстанавливаем имя
 NameT name=buffer.GetName();
 int name_i=name.find_last_of(":");
 if(name_i == NameT::npos)
  Name=name;
 else
  Name=name.substr(0,name_i);

 RDK::UClassRegDataIterator I,J;
 RDK::URegDataIterator rI,rJ;
 map<NameT,NVariable>::iterator K;

 // Восстанавливаем параметры
 I=buffer().begin();
 J=buffer().end();
 while(I != J)
 {
  rI=I->second.begin();
  rJ=I->second.end();
  while(rI != rJ)
  {
   K=ParamsLookupTable.find(rI->first);
   if(K != ParamsLookupTable.end())
   {
    stringstream stream(buffer()[I->first][rI->first],ios::in);
    K->second.Property->Load(stream);
   }
   ++rI;
  }

  ++I;
 }

 // Восстанавливаем структуру дочерних компонент если необходимо
 vector<NameT> treebuf;
 vector<NameT> treeclassbuf;
 vector<NameT>::iterator tI,tJ,tK;
 buffer.GetSubTrees(treebuf);
 treeclassbuf.resize(treebuf.size());
 for(int i=0;i<treebuf.size();i++)
 {
  name_i=treebuf[i].find_last_of(":");
  if(name_i != NameT::npos)
  {
   treeclassbuf[i]=treebuf[i].substr(name_i+2);
   treebuf[i]=treebuf[i].substr(0,name_i);
  }
 }

 if(!paramsonly)
 {
  vector<UId> deletelist;

  for(int i=0;i<NumComponents;i++)
  {
   tI=find(treebuf.begin(),treebuf.end(),PComponents[i]->Name);
   if(tI == treebuf.end())
    deletelist.push_back(PComponents[i]->Id);
   else
   {
    tJ=treeclassbuf.begin();
    for(tK=treebuf.begin();tK!=tI;++tK,++tJ);
    PComponents[i]->Load(buffer[*tI+string(": ")+*tJ],storage,paramsonly);
    treebuf.erase(tI); treeclassbuf.erase(tJ);
   }
  }

  // Удаляем компоненты которых более нет в списке
  for(int i=0;i<deletelist.size();i++)
   DelComponent(deletelist[i]);

  // Добавляем новые компоненты
  for(int i=0;i<treebuf.size();i++)
  {
   if(treeclassbuf[i] != "")
   {
    comp=stor->TakeObject(treeclassbuf[i]);
    if(comp)
    {
     if(!AddComponent(comp))
      delete comp;
     else
      comp->Load(buffer[treebuf[i]+string(": ")+treeclassbuf[i]],storage,paramsonly);
    }
   }
  }
 }
 else
 {
  // Загрузка для компонент
  for(int i=0;i<treebuf.size();i++)
  {
   RDK::UClassRegistry &temp=buffer[treebuf[i]+string(": ")+treeclassbuf[i]];
   name=temp.GetName();
   name_i=name.find_last_of(":");
   if(name_i != NameT::npos)
    name=name.substr(0,name_i);
   comp=GetComponent(name);
   if(comp)
    comp->Load(temp,storage,paramsonly);
  }
 }

 return true;
}

// Сохраняет состояние объекта в дерево
bool UAContainer::SaveState(RDK::UClassRegistry &buffer)
{
 RDK::UClassRegistry *comp=new RDK::UClassRegistry;

 NameT name=Name;

 if(Storage)
 {
//  UId classid=Storage->FindClass(this);
//  if(classid != ForbiddenId)
  if(Class != ForbiddenId)
  {
   name+=": ";
   name+=static_cast<UAContainerStorage*>(Storage)->GetClassName(Class);//Storage->GetClassName(classid);
  }

 }

 comp->SetName(name);

 map<NameT,NVariable>::iterator I,J;
 I=StateLookupTable.begin();
 J=StateLookupTable.end();

 NameT str,str2,statename;
 while(I != J)
 {
  statename=GetStateLongName(*(I->second.Property));
//  paramname+="::";
//  paramname+=I->first;
  stringstream stream(ios::out);
  I->second.Property->Save(stream);
  (*comp)()[statename][I->first]=stream.str();
//  (*comp)()[statename][I->first].assign(1500,'a');
  ++I;
 }

 buffer.AddSorted(comp);

 for(int i=0;i<NumComponents;i++)
  if(!PComponents[i]->SaveState(*comp))
   return false;

 return true;
}

// Загружает состояние объекта из дерева
// Если какие-то из дочерних компонент не созданы, то их состояние не восстанавливается
// однако метод возвращает true.
// Если для каких-то дочерних компонент данные состояния отсутствуют, то
// метод возвращает false
bool UAContainer::LoadState(RDK::UClassRegistry &buffer)
{
 UAContainer *comp=0;

 // Восстанавливаем имя
 NameT name=buffer.GetName();
 int name_i=name.find_last_of(":");
 if(name_i != NameT::npos)
  name=name.substr(0,name_i);

 if(Name != name)
  return false;

 RDK::UClassRegDataIterator I,J;
 RDK::URegDataIterator rI,rJ;
 map<NameT,NVariable>::iterator K;

 I=buffer().begin();
 J=buffer().end();
 while(I != J)
 {
  rI=I->second.begin();
  rJ=I->second.end();
  while(rI != rJ)
  {
   K=StateLookupTable.find(rI->first);
   if(K != StateLookupTable.end())
   {
    stringstream stream(buffer()[I->first][rI->first],ios::in);
    K->second.Property->Load(stream);
   }
   ++rI;
  }

  ++I;
 }

 // Восстанавливаем структуру дочерних компонент если необходимо
 vector<NameT> treebuf;
 vector<NameT>::iterator tI;
 buffer.GetSubTrees(treebuf);

 for(int i=0;i<treebuf.size();i++)
 {
  comp=GetComponent(buffer[treebuf[i]].GetName());
  if(comp)
   comp->LoadState(buffer[treebuf[i]]);
 }

 return true;
}              */
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UAContainer::Default(void)
{
 Ready=false;
 for(int i=0;i<NumComponents;i++)
  if(!PComponents[i]->Default())
   return false;

 // Если существует прообраз в хранилище, то берем настройки параметров
 // из прообраза
 UAContainer *original=0;
 if(Storage)
  original=static_cast<UAContainer*>(Storage->GetClass(Class));

 SetTimeStep(2000);

 if(original && original != this)
 {
  NameT name=Name;
  bool activity=Activity;
  original->CopyProperties(this);
  SetName(name);
  SetActivity(activity);
 }

 return ADefault();//Build();
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
  if(!PComponents[i]->Build())
   return false;

 if(ABuild())
 {
  Ready=true;
  return Reset();
 }

 Ready=false;
 return false;
}

// Сброс процесса счета.
bool UAContainer::Reset(void)
{
 if(!Ready)
  return Build();

 bool key=true;
 for(int i=0;i<NumComponents;i++)
  if(!PComponents[i]->Reset())
  {
   key=false;
   break;
  }

 if(!key)
  return false;

 if(!AReset())
  return false;

 CalcCounter=0;
 return true;
}

// Выполняет расчет этого объекта
bool UAContainer::Calculate(void)
{
 if(!Activity)
  return true;

 if(!Ready && !Build())
  return false;

// bool key=true;
 UAContainer **comps=PComponents;
 for(int i=0;i<NumComponents;++i)
  if(!(*comps++)->Calculate())
   return false;
//  {
//   key=false;
//   break;
//  }
// if(!key)
//  return false;

 if(!Owner)
  return ACalculate();

 if(TimeStep == OwnerTimeStep)
  return ACalculate();
 else
 if(TimeStep < OwnerTimeStep)
  {
   --CalcCounter;
   if(CalcCounter <= 0)
    {
	 CalcCounter=OwnerTimeStep/TimeStep;
     return ACalculate();
    }
  }
 else
 if(TimeStep > OwnerTimeStep)
  {
   for(UTime i=TimeStep/OwnerTimeStep;i>=0;--i)
    if(!ACalculate())
     return false;
  }
// if(!ACalculate())
//  return false;

 // Обрабатываем контроллеры
 int numcontrollers=Controllers.size();

 if(numcontrollers)
 {
  UController** controllers=&Controllers[0];
  for(int i=0;i<numcontrollers;i++,controllers++)
  {
   (*controllers)->Update();
  }
 }
 return true;
}
// --------------------------


// --------------------------
// Скрытые методы управления таблицей соответсвий компонент
// --------------------------
// Обновляет таблицу соответствий компонент заменяя 'oldname'
// имя компонента на 'newname'
bool UAContainer::ModifyLookupComponent(const NameT &oldname,
                                        const NameT newname)
{
 UId id;

 map<NameT,UId>::iterator I=CompsLookupTable.find(oldname);
 if(I == CompsLookupTable.end())
  return false;

 id=I->second;
 CompsLookupTable.erase(I);
 CompsLookupTable[newname]=id;

 return true;
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
 map<NameT,UId>::iterator I=CompsLookupTable.find(name);

 if(I == CompsLookupTable.end())
  return;

 CompsLookupTable.erase(name);
}
// --------------------------

// --------------------------
// Скрытые методы управления параметрами
// --------------------------
// Добавляет параметр с именем 'name' в таблицу соотвествий
// параметров и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
UId UAContainer::AddLookupProperty(const NameT &name, UIProperty *property, bool delenable)
{
 if(!property)
  return ForbiddenId;

 VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 UVariable P(1,property);
 P.DelEnable=delenable;

 if(PropertiesLookupTable.find(name) != J)
  return ForbiddenId;

 while(I != J)
  {
   if(P.Id <= I->second.Id)
    P.Id=I->second.Id+1;
   ++I;
  }

 PropertiesLookupTable.insert(make_pair(name,P));

 return P.Id;
}

// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
bool UAContainer::DelLookupProperty(const NameT &name)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  return false;

 if(I->second.DelEnable)
  delete I->second.Property;
 PropertiesLookupTable.erase(I);
 return true;
}

// Удаляет всю таблицу соответствий
void UAContainer::ClearLookupPropertyTable(void)
{
  VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
  while(I != J)
  {
   if(I->second.Property && I->second.DelEnable)
	delete I->second.Property;

   ++I;
  }
 PropertiesLookupTable.clear();
}

// Возвращает полное имя параметра без префикса RDK, и суффикса '*'
NameT UAContainer::GetPropertyLongName(const UIProperty &property) const
{
 NameT name=property.GetOwnerName();

 size_t i=name.find("RDK::");
 if(i == NameT::npos)
  i=0;
 else
  i=5;

 size_t j=name.find_last_of("*");
 if(j == NameT::npos)
  j=name.size()-i;
 else
  j-=i+1;

 return name.substr(i,j);
}
// --------------------------


// --------------------------
// Методы управления контроллерами интерфейса
// Удаление контроллеров лежит на вызывающем модуле
// --------------------------
// Добавляет новый контроллер
bool UAContainer::AddController(UController *controller, bool forchilds)
{
 if(!controller)
  return false;

 if(CheckController(controller))
  return true;

 Controllers.push_back(controller);
 if(forchilds)
  for(int i=0;i<NumComponents;i++)
  {
   PComponents[i]->AddController(controller,forchilds);
  }
 return true;
}

// Удаляет контроллер из списка
bool UAContainer::DelController(UController *controller, bool forchilds)
{
 vector<UController*>::iterator I=find(Controllers.begin(),Controllers.end(),controller);

 if(I != Controllers.end())
  Controllers.erase(I);

 if(forchilds)
  for(int i=0;i<NumComponents;i++)
  {
   PComponents[i]->DelController(controller,forchilds);
  }

 return true;
}

// Удаляет все контроллеры
bool UAContainer::DelAllControllers(bool forchilds)
{
 Controllers.clear();
 if(forchilds)
  for(int i=0;i<NumComponents;i++)
  {
   PComponents[i]->DelAllControllers(forchilds);
  }
 return true;
}

// Инициирует отключение всех контроллеров
bool UAContainer::UnLinkAllControllers(bool forchilds)
{
 while(Controllers.begin() != Controllers.end())
  Controllers.front()->UnLink(this);

 if(forchilds)
  for(int i=0;i<NumComponents;i++)
  {
   PComponents[i]->UnLinkAllControllers(forchilds);
  }

 return true;
}


// Проверяет, существует ли контроллер в списке
bool UAContainer::CheckController(UController *controller) const
{
 if(find(Controllers.begin(),Controllers.end(),controller) != Controllers.end())
  return true;
 return false;
}

// Возвращает число контроллеров
bool UAContainer::GetNumControllers(void) const
{
 return Controllers.size();
}

// Возвращает контроллер по индексу
UController* UAContainer::GetController(int index)
{
 return Controllers[index];
}
// --------------------------

// --------------------------
// Скрытые методы управления состоянием
// --------------------------
// Добавляет переменную состояния с именем 'name' в таблицу соотвествий
// параметров и назначает ей корректный индекс
// Должна вызываться в конструкторах классов
UId UAContainer::AddLookupState(const NameT &name,UIProperty *property, bool delenable)
{
 if(!property)
  return ForbiddenId;

 VariableMapIteratorT I=StateLookupTable.begin(),
					  J=StateLookupTable.end();
 UVariable P(1,property);
 P.DelEnable=delenable;

 if(StateLookupTable.find(name) != J)
  return ForbiddenId;

 while(I != J)
  {
   if(P.Id <= I->second.Id)
    P.Id=I->second.Id+1;
   ++I;
  }

 StateLookupTable.insert(make_pair(name,P));

 return P.Id;
}

// Удаляет переменную состояния с именем 'name' из таблицы соотвествий
bool UAContainer::DelLookupState(const NameT &name)
{
 VariableMapIteratorT I=StateLookupTable.find(name);

 if(I == StateLookupTable.end())
  return false;

 if(I->second.DelEnable)
  delete I->second.Property;
 StateLookupTable.erase(I);
 return true;
}

// Удаляет всю таблицу соответствий
void UAContainer::ClearLookupStateTable(void)
{
  VariableMapIteratorT I=StateLookupTable.begin(),
                      J=StateLookupTable.end();
  while(I != J)
  {
   if(I->second.Property && I->second.DelEnable)
    delete I->second.Property;

   ++I;
  }
 StateLookupTable.clear();
}

// Возвращает полное имя переменной состояния без префикса NMSDK, и суффикса '*'
NameT UAContainer::GetStateLongName(const UIProperty &property) const
{
 NameT name=property.GetOwnerName();

 size_t i=name.find("RDK::");
 if(i == NameT::npos)
  i=0;
 else
  i=5;

 size_t j=name.find_last_of("*");
 if(j == NameT::npos)
  j=name.size()-i;
 else
  j-=i+1;

 return name.substr(i,j);
}
// --------------------------

// --------------------------
// Скрытые методы управления локальными указателями
// --------------------------
// Добавляет указатель в таблицу соотвествий
// Должна вызываться в конструкторах классов
UId UAContainer::AddLookupPointer(const NameT &name, UIPointer *pointer)
{
 if(!pointer)
  return ForbiddenId;

 PointerMapIteratorT I=PointerLookupTable.begin(),
                      J=PointerLookupTable.end();
 UPVariable P(1,pointer);

 if(PointerLookupTable.find(name) != J)
  return ForbiddenId;

 while(I != J)
  {
   if(P.Id <= I->second.Id)
    P.Id=I->second.Id+1;
   ++I;
  }

 PointerLookupTable.insert(make_pair(name,P));

 return P.Id;
}

// Удаляет указатель с ID 'id' из таблицы соотвествий
bool UAContainer::DelLookupPointer(const NameT &name)
{
 PointerMapIteratorT I=PointerLookupTable.find(name);

 if(I == PointerLookupTable.end())
  return false;

 delete I->second.Pointer;
 PointerLookupTable.erase(I);
 return true;
}

// Возвращает полное имя указателя без префикса NMSDK, и суффикса '*'
NameT UAContainer::GetPointerLongName(const UIPointer &pointer) const
{
/*
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

 return name.substr(i,j);*/
 return "";
}

// Осуществляет поиск в таблице указателя, соответствующего заданному источнику
UAContainer::PointerMapCIteratorT UAContainer::FindLookupPointer(const UAContainer *source) const
{
 PointerMapCIteratorT I=PointerLookupTable.begin(),
                      J=PointerLookupTable.end();

 while(I != J)
  {
   if(I->second.Pointer && I->second.Pointer->Find(source)>=0)
    break;
   ++I;
  }

 return I;
}
// --------------------------


// --------------------------
// Скрытые методы управления таблицей компонент
// --------------------------
// Добавляет компонент 'comp' в таблицу компонент
void UAContainer::AddComponentTable(UAContainer *comp, UIPointer* pointer)
{
 Components.Add(comp);
 PComponents=&Components[0];
 NumComponents=Components.GetSize();

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

// Удаляет компонент 'comp' из таблицы компонент
void UAContainer::DelComponentTable(UAContainer *comp)
{
 int i;

 if(PComponents[NumComponents-1]==comp)
  Components.Resize(NumComponents-1);
 else
  {
   for(i=0;i<NumComponents;i++)
    if(PComponents[i] == comp)
     break;

   if(i>=NumComponents)
    return;

   memmove(PComponents+i,PComponents+i+1,(NumComponents-i-1)*sizeof(UAContainer*));
   Components.Resize(NumComponents-1);
  }

 NumComponents=Components.GetSize();
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
bool UAContainer::DelComponent(UAContainer* comp, bool canfree)
{
 if(!ADelComponent(comp))
  return false;

 if(comp->GetMainOwner() == MainOwner)
  comp->SetMainOwner(0);

 // Удаление из базы компонент
 // Удаляем компонент из таблицы соответствий владельца
 DelLookupComponent(comp->Name);

 // Удаление из базы компонент
 DelComponentTable(comp);

 comp->Owner=0;

 if(canfree)
  comp->Free();

 if(!NumComponents)
  LastId=0;
 return true;
}


// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool UAContainer::AAddComponent(UAContainer* comp, UIPointer* pointer)
{
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool UAContainer::ADelComponent(UAContainer* comp)
{
 return true;
}
// --------------------------

/* *************************************************************************** */


}

#endif


