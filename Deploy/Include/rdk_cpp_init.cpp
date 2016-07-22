#ifndef RDK_CPP_INIT_CPP
#define RDK_CPP_INIT_CPP

#include "rdk_cpp_init.h"
#include "rdk_init.cpp"

// --------------------------
// Методы доступа к ядру без блокировки
// --------------------------
// Возвращает ссылку на указатель ядра
RDK::UEPtr<URdkCoreManager> RDK_CALL GetCore(void)
{
 return &RdkCoreManager;
}

// Возвращает указатель на логгер
RDK::UEPtr<RDK::ULoggerEnv> RDK_CALL GetLogger(void)
{
 return RdkCoreManager.GetLogger();
}

RDK::UEPtr<RDK::ULoggerEnv> RDK_CALL GetLogger(int channel_index)
{
 return RdkCoreManager.GetLogger(channel_index);
}

// Возвращает ссылку на указатель управляющего ядра
RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void)
{
 return RdkCoreManager.GetEngine();
}

RDK::UEPtr<RDK::UEngine> RDK_CALL GetEngine(int channel_index)
{
 return RdkCoreManager.GetEngine(channel_index);
}


// Возвращает ссылку на указатель среды выполнения
RDK::UEPtr<RDK::UEnvironment>& RDK_CALL GetEnvironment(void)
{
 return RdkCoreManager.GetEnvironment();
}

RDK::UEPtr<RDK::UEnvironment> RDK_CALL GetEnvironment(int channel_index)
{
 return RdkCoreManager.GetEnvironment(channel_index);
}

// Возвращает ссылку на указатель хранилища
RDK::UEPtr<RDK::UStorage>& RDK_CALL GetStorage(void)
{
 return RdkCoreManager.GetStorage();
}

RDK::UEPtr<RDK::UStorage> RDK_CALL GetStorage(int channel_index)
{
 return RdkCoreManager.GetStorage(channel_index);
}

// Возвращает указатель на текущую модель
RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(void)
{
 return RdkCoreManager.GetModel();
}

RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(int channel_index)
{
 return RdkCoreManager.GetModel(channel_index);
}
// --------------------------

// --------------------------
// Методы доступа к каналам с блокировкой
// --------------------------
// Возвращает ссылку на указатель ядра
RDK::UELockPtr<URdkCoreManager> RDK_CALL GetCoreLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<URdkCoreManager>(0,&RdkCoreManager);
#else
 return RDK::UELockPtr<URdkCoreManager>(RdkCoreManager.GetGlobalMutex(),&RdkCoreManager);
#endif
}

// Возвращает ссылку на указатель управляющего ядра
RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(void)
{
 return RdkCoreManager.GetEngineLock(0);
}

RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(int channel_index)
{
 return RdkCoreManager.GetEngineLock(channel_index);
}

// Возвращает ссылку на указатель среды выполнения
RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(void)
{
 return RdkCoreManager.GetEnvironmentLock(0);
}

RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(int channel_index)
{
 return RdkCoreManager.GetEnvironmentLock(channel_index);
}

// Возвращает ссылку на указатель хранилища
RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(void)
{
 return RdkCoreManager.GetStorageLock(0);
}

RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(int channel_index)
{
 return RdkCoreManager.GetStorageLock(channel_index);
}

// Возвращает указатель на текущую модель
RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(void)
{
 return RdkCoreManager.GetModelLock(0);
}

RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(int channel_index)
{
 return RdkCoreManager.GetModelLock(channel_index);
}
// --------------------------


// --------------------------
// Методы доступа к щрифтам
// --------------------------
/// Возвращает ссылку на шрифты
/// (не потокобезопасно!)
RDK::UBitmapFontCollection& GetFonts(void)
{
 return RdkCoreManager.GetFonts();
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
