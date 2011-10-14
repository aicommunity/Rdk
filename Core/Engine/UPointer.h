/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UPointerH
#define UPointerH

#include <cstdlib>
#include <string>
#include <memory.h>
#include "../Utilities/USupport.h"
#include "UAContainer.h"

namespace RDK {

class UAContainer;


// Указатель на локальный экземпляр компонента в классе
// Для удобства использования
template<typename T, class OwnerT>
class UEPointer: public UPointer<T>, public UIPointer
{
//friend class OwnerT;
protected: // Атрибуты
// Исходный контейнер
T* Source;

// Владелец указателя
OwnerT* Owner;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEPointer(OwnerT * const owner, const string &name)
 : Owner(owner)
{ Source=0; reinterpret_cast<UAContainer* const>(Owner)->AddLookupPointer(name,this); };
// --------------------------

UAContainer* const Get(void) const
{ return Source; };

virtual void Del(UAContainer* source)
{
 Source=0;
}

// Проверяет, существует ли такой указатель в этом классе
// Возвращает 0 если да, и <0 если нет
virtual int Find(const UAContainer * cont) const
{ return (cont == Source)?0:-1; };

virtual void Set(UAContainer* source)
{
 Source=static_cast<T*>(source);
// RDK::UEPointer<T,OwnerT>::operator = (Source);
};

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UEPointer<T,OwnerT>& operator = (UAContainer* pdata)
{
 UIPointer::operator = (pdata);
 return *this;
};
// --------------------------
};


// Указатель на массив локальных экземпляров компонент в классе
// Для удобства использования
template<typename T, class OwnerT>
class UCPointer: public UIPointer
{
//friend class OwnerT;
protected: // Атрибуты
// Владелец указателя
OwnerT* Owner;

// Массив указателей
T** Sources;

// Размер массива
size_t Size;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCPointer(OwnerT * const owner, const std::string &name)
 : Owner(owner)
{ Size=0; Sources=0; reinterpret_cast<UAContainer* const>(Owner)->AddLookupPointer(name,this); };
virtual ~UCPointer(void)
{
 if(Sources)
 {
  delete[] Sources;
  Sources=0;
  Size=0;
 }
};
// --------------------------

UAContainer* const Get(void) const
{ return *Sources; };

UAContainer* const Get(size_t index) const
{ return Sources[index]; };

virtual void Set(UAContainer* source)
{
 if(Find(source) >=0)
  return;

 T** sources=new T*[Size+1];
 memcpy(sources,Sources,sizeof(T*)*Size);
 delete []Sources;
 Sources=sources;
 Sources[Size]=static_cast<T*>(source);
 ++Size;
}

virtual void Del(UAContainer* source)
{
 int index=Find(source);

 if(index<0)
  return;

 T** sources=new T*[Size-1];
 memcpy(sources,Sources,sizeof(T*)*index);
 memcpy(sources+index,Sources,sizeof(T*)*(Size-index-1));
 delete []Sources;
 Sources=sources;
 --Size;
}

// Проверяет, существует ли такой указатель в этом классе
// Возвращает 0 если да, и <0 если нет
virtual int Find(const UAContainer * cont) const
{
 T** sources=Sources;
 for(size_t i=0;i<Size;i++,sources++)
  if(*sources == cont)
   return i;

 return -1;
};

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UEPointer<T,OwnerT>& operator = (UAContainer* pdata)
{
 UIPointer::operator = (pdata);
 return *this;
};
// --------------------------
};
}
#endif
