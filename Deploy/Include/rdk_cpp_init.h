#ifndef RDK_CPP_INIT_H
#define RDK_CPP_INIT_H

#include "rdk_init.h"
#include "rdk.h"
#include "rdk_rpc.h"

extern "C++"  {

// Возвращает ссылку на указатель управляющего ядра
RDK_LIB_TYPE RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UEngine> RDK_CALL GetEngine(int engine_index);

// Возвращает ссылку на указатель среды выполнения
RDK_LIB_TYPE RDK::UEPtr<RDK::UEnvironment>& RDK_CALL GetEnvironment(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UEnvironment> RDK_CALL GetEnvironment(int engine_index);

// Возвращает ссылку на указатель хранилища
RDK_LIB_TYPE RDK::UEPtr<RDK::UStorage>& RDK_CALL GetStorage(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UStorage> RDK_CALL GetStorage(int engine_index);

// Возвращает указатель на текущую модель
RDK_LIB_TYPE RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(int engine_index);

// --------------------------
// Методы доступа к каналам с блокировкой
// --------------------------
// Возвращает ссылку на указатель управляющего ядра
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(void);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(int engine_index);

// Возвращает ссылку на указатель среды выполнения
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(void);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(int engine_index);

// Возвращает ссылку на указатель хранилища
RDK_LIB_TYPE RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(void);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(int engine_index);

// Возвращает указатель на текущую модель
RDK_LIB_TYPE RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(void);
RDK_LIB_TYPE RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(int engine_index);
// --------------------------
}

namespace RDK {

// Возвращает указатель на текущую модель
template<class T>
RDK::UELockPtr<T> GetModelLock(void)
{
 RDK::UEPtr<T> p=dynamic_pointer_cast<T>(::GetModel());
 if(!p)
  return RDK::UELockPtr<T>();
 return RDK::UELockPtr<T>((UGenericMutex*)Engine_GetMutex(),p);
}

template<class T>
RDK::UELockPtr<T> GetModelLock(int engine_index)
{
 RDK::UEPtr<T> p=dynamic_pointer_cast<T>(::GetModel(engine_index));
 if(!p)
  return RDK::UELockPtr<T>();
 return RDK::UELockPtr<T>((UGenericMutex*)MEngine_GetMutex(engine_index),p);
}

template<class T>
RDK::UEPtr<T> GetModel(void)
{
 return dynamic_pointer_cast<T>(::GetModel());
}

template<class T>
RDK::UEPtr<T> GetModel(int engine_index)
{
 return dynamic_pointer_cast<T>(::GetModel(engine_index));
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
T& MReadPropertyValue(int engine_index, const std::string &comp_name, const std::string &property_name, T &res)
{
 const char *param_value=MModel_GetComponentPropertyValue(engine_index, comp_name.c_str(),property_name.c_str());
 if(!param_value)
  throw EEnginePropertyNotFound(comp_name, property_name);

 RDK::DecodePropertyValue(param_value,res);
 MEngine_FreeBufString(engine_index, param_value);
 return res;
}

template<typename T>
T ReadPropertyValue(const std::string &comp_name, const std::string &property_name)
{
 T res;
 return ReadPropertyValue(comp_name, property_name,res);
}

template<typename T>
T MReadPropertyValue(int engine_index, const std::string &comp_name, const std::string &property_name)
{
 T res;
 return MReadPropertyValue(engine_index, comp_name, property_name,res);
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

// Кодирует и записывает содержимое переменной состояния компонента
template<typename T>
void WriteStateValue(const std::string &comp_name, const std::string &param_name, const T &res)
{
 WritePropertyValue(comp_name.c_str(),param_name.c_str(),res);
}


}

#endif

