#ifndef RDK_CPP_INIT_H
#define RDK_CPP_INIT_H

#include "rdk_init.h"
#include "rdk.h"

extern "C++"  {
/*
template<typename T>
class UELockEnginePtr: public UELockPtr<T>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UELockEnginePtr(int channel_index=0)
 : UELockPtr<T>(MutexList)
{

}

UELockEnginePtr(UGenericMutex* mutex, T* pdata);
UELockEnginePtr(UGenericMutex* mutex, const UEPtr<T> &pdata);
UELockEnginePtr(const UELockPtr<T> &p);
virtual ~UELockEnginePtr(void);
// --------------------------

};
  */
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
// Декодирует содержимое свойства/переменной состояния компонента
template<typename T>
T& DecodeParameterValue(const std::string &param_value, T &res)
{
 using namespace RDK;

 USerStorageXML xml;
 if(param_value.size()>0 && param_value[0]=='<')
 {
  xml.Load(param_value,"");
 }
 else
 {
  xml.Create("1");
  xml.SelectRoot();
  xml.SetNodeText(param_value);
 }

 xml>>res;
 return res;
}

template<typename T>
T DecodeParameterValue(const std::string &param_value)
{
 T res;

 return DecodeParameterValue(param_value,res);
}

// Кодирует содержимое свойства/переменной состояния компонента
template<typename T>
std::string& EncodeParameterValue(const T &param_data, std::string& res)
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
T& ReadParameterValue(const std::string &comp_name, const std::string &param_name, T &res)
{
 const char *param_value=Model_GetComponentParameterValue(comp_name.c_str(),param_name.c_str());
 if(!param_value)
  throw int(1); // TODO здесь исключение

 return RDK::DecodeParameterValue(param_value,res);
}

template<typename T>
T ReadParameterValue(const std::string &comp_name, const std::string &param_name)
{
 T res;
 return ReadParameterValue(comp_name, param_name,res);
}

// Считывает и декодирует содержимое переменной состояния компонента
template<typename T>
T& ReadStateValue(const std::string &comp_name, const std::string &state_name, T &res)
{
 T v;
 const char* state_value=Model_GetComponentStateValue(comp_name.c_str(),state_name.c_str());

 if(!state_value)
  throw int(1); // TODO здесь исключение

 return RDK::DecodeParameterValue(state_value,res);
}

template<typename T>
T ReadStateValue(const std::string &comp_name, const std::string &state_name)
{
 T res;
 return ReadStateValue(comp_name, state_name,res);
}

// Кодирует и записывает содержимое свойства компонента
template<typename T>
void WriteParameterValue(const std::string &comp_name, const std::string &param_name, const T &res)
{
 std::string param_value;

 RDK::EncodeParameterValue(res,param_value);
 Model_SetComponentParameterValue(comp_name.c_str(),param_name.c_str(),param_value.c_str());
}

// Кодирует и записывает содержимое переменной состояния компонента
template<typename T>
void WriteStateValue(const std::string &comp_name, const std::string &param_name, const T &res)
{
 std::string param_value;
 RDK::EncodeParameterValue(res,param_value);
 Model_SetComponentStateValue(comp_name.c_str(),param_name.c_str(),param_value.c_str());
}

}

#endif

