#ifndef RDK_CPP_INIT_CPP
#define RDK_CPP_INIT_CPP

#include "rdk_cpp_init.h"
#include "rdk_init.cpp"

// --------------------------
// Методы доступа к ядру без блокировки
// --------------------------
// Возвращает ссылку на указатель ядра
RDK::UEPtr<RDKDllManager> RDK_CALL GetCore(void)
{
 return &DllManager;
}

// Возвращает ссылку на указатель управляющего ядра
RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void)
{
 return DllManager.GetEngine();
}

RDK::UEPtr<RDK::UEngine> RDK_CALL GetEngine(int channel_index)
{
 return DllManager.GetEngine(channel_index);
}


// Возвращает ссылку на указатель среды выполнения
RDK::UEPtr<RDK::UEnvironment>& RDK_CALL GetEnvironment(void)
{
 return DllManager.GetEnvironment();
}

RDK::UEPtr<RDK::UEnvironment> RDK_CALL GetEnvironment(int channel_index)
{
 return DllManager.GetEnvironment(channel_index);
}

// Возвращает ссылку на указатель хранилища
RDK::UEPtr<RDK::UStorage>& RDK_CALL GetStorage(void)
{
 return DllManager.GetStorage();
}

RDK::UEPtr<RDK::UStorage> RDK_CALL GetStorage(int channel_index)
{
 return DllManager.GetStorage(channel_index);
}

// Возвращает указатель на текущую модель
RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(void)
{
 return DllManager.GetModel();
}

RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(int channel_index)
{
 return DllManager.GetModel(channel_index);
}
// --------------------------

// --------------------------
// Методы доступа к каналам с блокировкой
// --------------------------
// Возвращает ссылку на указатель ядра
RDK::UELockPtr<RDKDllManager> RDK_CALL GetCoreLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDKDllManager>(0,&DllManager);
#else
 return RDK::UELockPtr<RDKDllManager>(DllManager.GetGlobalMutex(),&DllManager);
#endif
}

// Возвращает ссылку на указатель управляющего ядра
RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(void)
{
 return DllManager.GetEngineLock(0);
}

RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(int channel_index)
{
 return DllManager.GetEngineLock(channel_index);
}

// Возвращает ссылку на указатель среды выполнения
RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(void)
{
 return DllManager.GetEnvironmentLock(0);
}

RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(int channel_index)
{
 return DllManager.GetEnvironmentLock(channel_index);
}

// Возвращает ссылку на указатель хранилища
RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(void)
{
 return DllManager.GetStorageLock(0);
}

RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(int channel_index)
{
 return DllManager.GetStorageLock(channel_index);
}

// Возвращает указатель на текущую модель
RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(void)
{
 return DllManager.GetModelLock(0);
}

RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(int channel_index)
{
 return DllManager.GetModelLock(channel_index);
}
// --------------------------

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EEnginePropertyNotFound::EEnginePropertyNotFound(const std::string &component_name, const std::string &property_name)
: ComponentName(component_name), PropertyName(property_name)
{

}

EEnginePropertyNotFound::~EEnginePropertyNotFound(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EEnginePropertyNotFound::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" ComponentName=")+ComponentName+
			std::string(" PropertyName=")+PropertyName;
}
// --------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EEnginePropertyDecodeLoadFail::EEnginePropertyDecodeLoadFail(const std::string &xml_data, const std::string &variable_type)
: XmlData(xml_data), VariableType(variable_type)
{

}

EEnginePropertyDecodeLoadFail::~EEnginePropertyDecodeLoadFail(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EEnginePropertyDecodeLoadFail::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" XmlData=")+XmlData+
			std::string(" VariableType=")+VariableType;
}
// --------------------------

}


#endif
