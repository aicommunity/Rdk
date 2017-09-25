#ifndef RDK_CPP_INIT_H
#define RDK_CPP_INIT_H

#include "rdk_init.h"
#include "rdk.h"
#include "rdk_rpc.h"
#include "rdk_engine_support.h"

namespace RDK {

// --------------------------
// Методы доступа к ядру без блокировки
// --------------------------
// Возвращает ссылку на указатель ядра
RDK_LIB_TYPE RDK::UEPtr<URdkCoreManager> RDK_CALL GetCore(void);

// Возвращает указатель на логгер
RDK_LIB_TYPE RDK::UEPtr<RDK::ULoggerEnv> RDK_CALL GetLogger(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::ULoggerEnv> RDK_CALL GetLogger(int channel_index);

// Возвращает ссылку на указатель управляющего ядра
RDK_LIB_TYPE RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UEngine> RDK_CALL GetEngine(int channel_index);

// Возвращает ссылку на указатель среды выполнения
RDK_LIB_TYPE RDK::UEPtr<RDK::UEnvironment>& RDK_CALL GetEnvironment(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UEnvironment> RDK_CALL GetEnvironment(int channel_index);

// Возвращает ссылку на указатель хранилища
RDK_LIB_TYPE RDK::UEPtr<RDK::UStorage>& RDK_CALL GetStorage(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UStorage> RDK_CALL GetStorage(int channel_index);

// Возвращает указатель на текущую модель
RDK_LIB_TYPE RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(int channel_index);
// --------------------------

// --------------------------
// Методы доступа к ядру с блокировкой
// --------------------------
// Возвращает ссылку на указатель ядра
RDK_LIB_TYPE RDK::UELockPtr<URdkCoreManager> RDK_CALL GetCoreLock(void);

// Возвращает ссылку на указатель управляющего ядра
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(void);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(int channel_index);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLockTimeout(unsigned timeout);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLockTimeout(int channel_index, unsigned timeout);

// Возвращает ссылку на указатель среды выполнения
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(void);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(int channel_index);

// Возвращает ссылку на указатель хранилища
RDK_LIB_TYPE RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(void);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(int channel_index);

// Возвращает указатель на текущую модель
RDK_LIB_TYPE RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(void);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(int channel_index);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLockTimeout(unsigned timeout);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLockTimeout(int channel_index, unsigned timeout);
// --------------------------

// --------------------------
// Методы доступа к щрифтам
// --------------------------
/// Возвращает ссылку на шрифты
/// (не потокобезопасно!)
RDK::UBitmapFontCollection& GetFonts(void);
// --------------------------

/// Возвращает RDK_UNHANDLED_EXCEPTION если не удалось записать данные исключения
/// иначе возвращает RDK_EXCEPTION_CATCHED
RDK_LIB_TYPE int RDK_CALL ProcessException(int channel_index, const UException &ex);


/// Записывает в отладочную консоль сообщение, если result != RDK_SUCCESS
/// работает только если включен отладочный режим
RDK_LIB_TYPE void RDK_CALL AssertDebugger(int result, const char* function, const char* file, int line);

/// Записывает в лог сообщение, если result != RDK_SUCCESS
/// Также записывает его в отладочную консоль если включен отладочный режим
RDK_LIB_TYPE void RDK_CALL AssertLog(int result, const char* function, const char* file, int line);

template<class T>
RDK::UELockPtr<T> GetEngineLock(int channel_index)
{
 return RdkCoreManager.GetEngineLock<T>(channel_index);
}

template<class T>
RDK::UELockPtr<T> GetEngineLockTimeout(int channel_index, unsigned timeout)
{
 return RdkCoreManager.GetEngineLockTimeout<T>(channel_index,timeout);
}

// Возвращает указатель на текущую модель
template<class T>
RDK::UELockPtr<T> GetModelLock(void)
{
 return RdkCoreManager.GetModelLock<T>(RdkCoreManager.GetSelectedChannelIndex());
}

template<class T>
RDK::UELockPtr<T> GetModelLock(int channel_index)
{
 return RdkCoreManager.GetModelLock<T>(channel_index);
}

template<class T>
RDK::UELockPtr<T> GetModelLockTimeout(int channel_index, unsigned timeout)
{
 return RdkCoreManager.GetModelLockTimeout<T>(channel_index,timeout);
}

template<class T>
RDK::UEPtr<T> GetModel(void)
{
 return dynamic_pointer_cast<T>(GetModel());
}

template<class T>
RDK::UEPtr<T> GetModel(int channel_index)
{
 return dynamic_pointer_cast<T>(GetModel(channel_index));
}

// Исключения
/// Исключение - свойство не найдено
struct RDK_LIB_TYPE EEnginePropertyNotFound: public EError
{
/// Имя компонента
std::string ComponentName;

/// Имя свойства
std::string PropertyName;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EEnginePropertyNotFound(const std::string &component_name, const std::string &property_name);
virtual ~EEnginePropertyNotFound(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

struct RDK_LIB_TYPE EEnginePropertyDecodeLoadFail: public EError
{
/// Имя компонента
std::string XmlData;

/// Имя свойства
std::string VariableType;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EEnginePropertyDecodeLoadFail(const std::string &xml_data, const std::string &variable_type);
virtual ~EEnginePropertyDecodeLoadFail(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Декодирует содержимое свойства/переменной состояния компонента
template<typename T>
T& DecodePropertyValue(const std::string &param_value, T &res)
{
 using namespace RDK;

 USerStorageXML xml;
 if(param_value.size()>0 && param_value[0]=='<')
 {
  if(!xml.Load(param_value,""))
   throw EEnginePropertyDecodeLoadFail(param_value, typeid(res).name());
 }
 else
 {
  xml.Create("1");
  xml.SelectRoot();
  if(!xml.SetNodeText(param_value))
   throw EEnginePropertyDecodeLoadFail(param_value, typeid(res).name());
 }

 xml>>res;
 return res;
}

template<typename T>
T DecodePropertyValue(const std::string &param_value)
{
 T res;

 return DecodePropertyValue<T>(param_value,res);
}

// Кодирует содержимое свойства/переменной состояния компонента
template<typename T>
std::string& EncodePropertyValue(const T &param_data, std::string& res)
{
 using namespace RDK;

 USerStorageXML xml;
 xml.Create("1");
 xml.SelectRoot();
 xml<<param_data;
 if(xml.GetNumNodes()>0)
  xml.Save(res);
 else
  res=xml.GetNodeText();
 return res;
}

// Считывает и декодирует содержимое свойства компонента
template<typename T>
T& ReadPropertyValue(const std::string &comp_name, const std::string &property_name, T &res)
{
 const char *param_value=Model_GetComponentPropertyValue(comp_name.c_str(),property_name.c_str());
 if(!param_value)
  throw EEnginePropertyNotFound(comp_name, property_name);


 RDK::DecodePropertyValue(param_value,res);
 Engine_FreeBufString(param_value);
 return res;
}

// Считывает и декодирует содержимое свойства компонента
template<typename T>
T& MReadPropertyValue(int channel_index, const std::string &comp_name, const std::string &property_name, T &res)
{
 const char *param_value=MModel_GetComponentPropertyValue(channel_index, comp_name.c_str(),property_name.c_str());
 if(!param_value)
  throw EEnginePropertyNotFound(comp_name, property_name);

 RDK::DecodePropertyValue(param_value,res);
 MEngine_FreeBufString(channel_index, param_value);
 return res;
}

template<typename T>
T ReadPropertyValue(const std::string &comp_name, const std::string &property_name)
{
 T res;
 return ReadPropertyValue(comp_name, property_name,res);
}

template<typename T>
T MReadPropertyValue(int channel_index, const std::string &comp_name, const std::string &property_name)
{
 T res;
 return MReadPropertyValue(channel_index, comp_name, property_name,res);
}

// Считывает и декодирует содержимое параметра компонента
template<typename T>
T& ReadParameterValue(const std::string &comp_name, const std::string &param_name, T &res)
{
 return ReadPropertyValue(comp_name,param_name,res);
}

template<typename T>
T ReadParameterValue(const std::string &comp_name, const std::string &param_name)
{
 return ReadPropertyValue<T>(comp_name,param_name);
}

// Считывает и декодирует содержимое переменной состояния компонента
template<typename T>
T& ReadStateValue(const std::string &comp_name, const std::string &state_name, T &res)
{
 return ReadPropertyValue(comp_name,state_name,res);
}

template<typename T>
T ReadStateValue(const std::string &comp_name, const std::string &state_name)
{
 T res;
 return ReadPropertyValue(comp_name,state_name,res);
}

// Кодирует и записывает содержимое свойства компонента
template<typename T>
void WritePropertyValue(const std::string &comp_name, const std::string &property_name, const T &res)
{
 std::string property_value;

 RDK::EncodePropertyValue(res,property_value);
 Model_SetComponentPropertyValue(comp_name.c_str(),property_name.c_str(),property_value.c_str());
}

// Кодирует и записывает содержимое свойства компонента
template<typename T>
void WriteParameterValue(const std::string &comp_name, const std::string &param_name, const T &res)
{
 WritePropertyValue(comp_name.c_str(),param_name.c_str(),res);
}

// Кодирует и записывает содержимое свойства компонента
template<typename T>
void MWritePropertyValue(int engine_index, const std::string &comp_name, const std::string &property_name, const T &res)
{
 const char *param_value=MModel_GetComponentPropertyValue(engine_index, comp_name.c_str(),property_name.c_str());
 if(!param_value)
  throw EEnginePropertyNotFound(comp_name, property_name);
 MEngine_FreeBufString(engine_index, param_value);

 std::string property_value;
 RDK::EncodePropertyValue(res,property_value);
 MModel_SetComponentPropertyValue(engine_index, comp_name.c_str(),property_name.c_str(), property_value.c_str());
}

// Кодирует и записывает содержимое свойства компонента
template<typename T>
void MWriteParameterValue(int engine_index, const std::string &comp_name, const std::string &property_name, const T &res)
{
	MWritePropertyValue(engine_index, comp_name, property_name, res);
}

// Кодирует и записывает содержимое переменной состояния компонента
template<typename T>
void WriteStateValue(const std::string &comp_name, const std::string &param_name, const T &res)
{
 WritePropertyValue(comp_name.c_str(),param_name.c_str(),res);
}


}

#endif

