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
/// Pointers to dynamic IO
//std::vector<UEPtr<UProperty<T,UDynamicNet<T>, ptPubInput> > > InputData;

/// Pointers to dynamic IO
//std::vector<UEPtr<UProperty<T,UDynamicNet<T>, ptPubOutput> > > OutputData;

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
