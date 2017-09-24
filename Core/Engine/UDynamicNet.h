//---------------------------------------------------------------------------

#ifndef UDynamicNetH
#define UDynamicNetH
#include "UNet.h"
//---------------------------------------------------------------------------
namespace RDK {

template<class T>
class UDynamicNet: public UNet
{
public: // Parameters
/// Number of dynamic inputs
UProperty<int,UDynamicNet<T>, ptPubParameter> NumInputs;

/// Number of dynamic outputs
UProperty<int,UDynamicNet<T>, ptPubParameter> NumOutputs;

protected:
/// Pointers to dynamic IO
std::vector<UEPtr<UProperty<T,UDynamicNet<T>, ptPubInput> > > InputData;

/// Pointers to dynamic IO
std::vector<UEPtr<UProperty<T,UDynamicNet<T>, ptPubOutput> > > OutputData;

public:
// ------------------
// Constructors & destructors
// ------------------
UDynamicNet(void);
virtual ~UDynamicNet(void);
// ------------------

// ------------------
// Parameters setters
// ------------------
/// NumInputs setter
bool SetNumInputs(const int &value);

/// NumOutputs setter
bool SetNumOutputs(const int &value);
// ------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает вектор выходных данных
inline const T& GetOutputData(int index) const;

// Возвращает указатель на вектор входов InputData по указателю на item
// Возвращает 0 если citem == 0 или не найден в списке подключений
const T& GetInputData(const UEPtr<UDynamicNet<T> > &citem) const;

// Возвращает указатель на вектор входов InputData по индексу
// Проверяет индекс на корректность и возвращает 0, если такого входа нет фактически
const T& GetInputData(size_t index) const;
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool Default(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool Build(void);

// Сброс процесса счета без потери настроек
virtual bool Reset(void);
// --------------------------
};


// ------------------
// Constructors & destructors
// ------------------
template<class T>
UDynamicNet<T>::UDynamicNet(void)
 : NumInputs("NumInputs",this,&UDynamicNet<T>::SetNumInputs),
   NumOutputs("NumOutputs",this,&UDynamicNet<T>::SetNumOutputs)
{

}

template<class T>
UDynamicNet<T>::~UDynamicNet(void)
{

}
// ------------------

// ------------------
// Parameters setters
// ------------------
/// NumInputs setter
template<class T>
bool UDynamicNet<T>::SetNumInputs(const int &value)
{
 Ready=false;
 return true;
}

/// NumOutputs setter
template<class T>
bool UDynamicNet<T>::SetNumOutputs(const int &value)
{
 Ready=false;
 return true;
}
// ------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает вектор выходных данных
template<class T>
const T& UDynamicNet<T>::GetOutputData(int index) const
{
 return **OutputData[index];
};

// Возвращает указатель на вектор входов InputData по указателю на item
// Возвращает 0 если citem == 0 или не найден в списке подключений
template<class T>
const T& UDynamicNet<T>::GetInputData(const UEPtr<UDynamicNet<T> > &citem) const
{
 if(!citem)
  RDK_RAW_THROW(EInputIndexNotExist(-1));

 for(size_t i=0;i<InputData.size();i++)
 {
  if(!InputData[i]->IsConnected())
   continue;

  for(j=0;j<InputData[i]->GetNumConnections();j++)
   if(InputData[i]->GetConnectedProperty(j)->GetOwner() == citem)
	return *InputData[i];
 }
 RDK_RAW_THROW(EInputIndexNotExist(-1));
}

// Возвращает указатель на вектор входов InputData по индексу
// Проверяет индекс на корректность и возвращает 0, если такого входа нет фактически
template<class T>
const T& UDynamicNet<T>::GetInputData(size_t index) const
{
 return **InputData[index];
}
// ----------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
template<class T>
bool UDynamicNet<T>::Default(void)
{
 NumInputs=1;
 NumOutputs=1;
 return UNet::Default();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
template<class T>
bool UDynamicNet<T>::Build(void)
{
 while(int(InputData.size()) < NumInputs)
 {
  UEPtr<UProperty<T,UDynamicNet<T>,ptPubInput> > prop;
  prop=new UProperty<T,UDynamicNet<T>,ptPubInput>(std::string("DataInput")+sntoa(InputData.size()),this,0,true);
  InputData.push_back(prop);
 }

 while(int(InputData.size()) > NumInputs)
 {
  UEPtr<UProperty<T,UDynamicNet<T>,ptPubInput> > prop=InputData.back();
  delete prop;
  InputData.resize(InputData.size()-1);
 }

 while(int(OutputData.size()) < NumOutputs)
 {
  UEPtr<UProperty<T,UDynamicNet<T>,ptPubOutput> > prop;
  prop=new UProperty<T,UDynamicNet<T>,ptPubOutput>(std::string("DataOutput")+sntoa(OutputData.size()),this,0,true);
  OutputData.push_back(prop);
 }

 while(int(OutputData.size()) > NumOutputs)
 {
  UEPtr<UProperty<T,UDynamicNet<T>,ptPubOutput> > prop=OutputData.back();
  delete prop;
  OutputData.resize(OutputData.size()-1);
 }

 return UNet::Build();
}

// Сброс процесса счета без потери настроек
template<class T>
bool UDynamicNet<T>::Reset(void)
{
 return UNet::Reset();
}
// --------------------------

}
#endif
