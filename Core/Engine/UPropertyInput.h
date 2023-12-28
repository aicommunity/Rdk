#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "UProperty.h"
#include "UConnector.h"

namespace RDK {


// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputCBase: public UCProperty<std::vector<T*>,OwnerT,type>
{
protected:
/// Временная переменная, использующаяся, если нет реального подключения
std::vector<T*> Local;

/// Указатель на подключенный выход
std::vector<UIPropertyOutput*> ConnectedOutput;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputCBase(const string &name, OwnerT * const owner, int input_type)
 : UCProperty<std::vector<T*>,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
}

~UPropertyInputCBase(void)
{
 for(size_t i=0;i<Local.size();i++)
  delete Local[i];
}
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает языковой тип хранимого свойства для одного элемента
virtual const type_info& GetElemLanguageType(void) const
{
 return typeid(T);
}

// Метод сравнивает тип этого свойства с другим свойством (по одному элементу)
virtual bool CompareElemLanguageType(const UIProperty &dt) const
{
 return GetElemLanguageType() == dt.GetElemLanguageType();
}

// Возвращает true если вход имеет подключение
bool IsConnected(void) const
{
 return !this->v.empty();
}

// Число указателей на данные
int GetNumPointers(void) const
{
 return int(this->v.size());
}

// Возвращает указатель на данные входа
void const * GetPointer(int index) const
{
 if(int(this->v.size())<=index)
 #if defined(__BORLANDC__) && !defined(__clang__)
//  return 0;
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),index);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),index);
 #endif
 return this->v[index];
}

// Устанавливает указатель на данные входа
bool SetPointer(int index, UIPropertyOutput* property)
{
 if(index<0)
  return false;

 if(int(this->v.size())<=index || (this->v.size() != Local.size()))
 {
  size_t new_size=index+1;
  this->v.resize(new_size);
  size_t old_size=Local.size();
  for(size_t i=new_size;i<old_size;i++)
   delete Local[i];
  Local.resize(new_size);
  ConnectedOutput.resize(new_size,0);

  for(size_t i=old_size;i<new_size;i++)
   Local[i]=new T;
 }
 this->v[index]=const_cast<T*>(&dynamic_cast<UVBaseDataProperty<T>*>(property)->GetData());
 ConnectedOutput.resize(this->v.size());
 ConnectedOutput[index]=property;
 return true;
}

/// Сбрасывает указатель на данные
bool ResetPointer(int index, UIPropertyOutput* property)
{
 if(int(this->v.size())>index && index >=0)
 {
  this->v.erase(this->v.begin()+index);
  if(int(Local.size())>index)
  {
   delete Local[index];
   Local.erase(Local.begin()+index);
  }
  if(int(this->ConnectedOutput.size())>index)
   this->ConnectedOutput.erase(ConnectedOutput.begin()+index);
  return true;
 }
 return false;
}

/// Удаляет все указатели
void ClearAllPointers(void)
{
 for(size_t i=0;i<Local.size();i++)
 {
  delete Local[i];
 }
 Local.clear();
 ConnectedOutput.clear();
 this->v.clear();
}

T* operator [] (int i)
{
 if(int(this->v.size())<=i)
 #if defined(__BORLANDC__) && !defined(__clang__)
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	0,int(this->v.size()),i);
 #endif

 return (this->v[i])?this->v[i]:Local[i];
}

const T* operator [] (int i) const
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return (this->v[i])?this->v[i]:Local[i];
}
// --------------------------

// --------------------------
// Методы управления входами
// --------------------------
/// Возвращает имя подключенного компонента
virtual UItem* GetItem(int index)
{
 if(int(this->ConnectedOutput.size())>index && index >=0)
 {
  return reinterpret_cast<UItem*>(ConnectedOutput[index]->GetOwner());
 }
 return 0;
}

/// Возвращает имя подключенного выхода
virtual std::string GetItemOutputName(int index) const
{
 if(int(this->ConnectedOutput.size())>index && index >=0)
 {
  return ConnectedOutput[index]->GetName();
 }
 return std::string();
}

/// Возвращает имя подключенного компонента
virtual std::string GetItemName(int index) const
{
 if(int(this->ConnectedOutput.size())>index && index >=0)
 {
  return ConnectedOutput[index]->GetOwner()->GetName();
 }
 return std::string();
}

/// Возвращает полное имя подключенного компонента
virtual std::string GetItemFullName(int index) const
{
 if(int(this->ConnectedOutput.size())>index && index >=0)
 {
  return ConnectedOutput[index]->GetOwner()->GetFullName();
 }
 return std::string();
}

protected:
/// Возвращает имя подключенного компонента
virtual UItem* GetItem(void)
{
 return 0;
}
// --------------------------

};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputCData: public UPropertyInputCBase<T,OwnerT,type>
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputCData(const string &name, OwnerT * const owner)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, ipRange | ipData)
{ }

/// Deprecated
UPropertyInputCData(const string &name, OwnerT * const owner, int index)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, ipRange | ipData)
{ }

// -----------------------------
};

}
#endif
