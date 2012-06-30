#ifndef MYRDK_H
#define MYRDK_H

#include "../../Core/System/rdk_system.h"
#include "../../ThirdParty/ThirdParty.h"
#include "../../Core/Math/UMath.h"
#include "../../Core/Utilities/Utilities.h"
#include "../../Core/Serialize/Serialize.h"

#include "../../Core/Engine/UAEnvSupport.h"
#include "../../Core/Engine/UXMLEnvSerialize.h"
#include "../../Core/Engine/UBinaryEnvSerialize.h"
#include "../../Core/Graphics/UBitmap.h"
#include "../../Core/Graphics/UAGraphics.h"
#include "../../Core/Graphics/UGraphics.h"
#include "../../Core/Graphics/UGraphicsXMLSerialize.h"
#include "../../Core/Graphics/UGraphicsBinarySerialize.h"
#ifndef RDK_LIB_TYPE
#include "rdk_initlib.h"
#else
#include "rdk_init.h"
#endif

namespace RDK {

// Декодирует содержимое свойства/переменной состояния компонента
template<typename T>
T& DecodeParameterValue(const std::string &param_value, T &res)
{
 using namespace RDK::Serialize;

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
 using namespace RDK::Serialize;

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
 std::string param_value=Model_GetComponentParameterValue(comp_name.c_str(),param_name.c_str());

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
 std::string state_value=Model_GetComponentStateValue(comp_name.c_str(),state_name.c_str());

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


// Класс прототип-визуальных интерфейсов
class UIVisualController
{
public:
// Метод, вызываемый перед сбросом модели
virtual void BeforeReset(void)=0;

// Метод, вызываемый после сброса модели
virtual void AfterReset(void)=0;

// Метод, вызываемый перед шагом расчета
virtual void BeforeCalculate(void)=0;

// Метод, вызываемый после шага расчета
virtual void AfterCalculate(void)=0;

// Обновление интерфейса
virtual void UpdateInterface(void)=0;

// Возвращает уникальное имя интерфейса
virtual std::string GetName(void)=0;

// Сохраняет параметры интерфейса в xml
virtual void SaveParameters(RDK::Serialize::USerStorageXML &xml)=0;

// Загружает параметры интерфейса из xml
virtual void LoadParameters(RDK::Serialize::USerStorageXML &xml)=0;
};

// Класс хранилище-визуальных интерфейсов
class UIVisualControllerStorage
{
public:
// Список обработчиков, которые должны быть вызваны после расчета
static std::vector<RDK::UIVisualController*> InterfaceUpdaters;

public:
// Добавляет обработчик в список
static void AddInterface(RDK::UIVisualController *value);

// Удаляет обработчик из списка
static void DelInterface(RDK::UIVisualController *value);

// Метод, вызываемый перед сбросом модели
static void BeforeReset(void);

// Метод, вызываемый после сброса модели
static void AfterReset(void);

// Метод, вызываемый перед шагом расчета
static void BeforeCalculate(void);

// Метод, вызываемый после шага расчета
static void AfterCalculate(void);

// Обновление интерфейса
static void UpdateInterface(void);

// Сохраняет параметры интерфейса в xml
static void SaveParameters(RDK::Serialize::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
static void LoadParameters(RDK::Serialize::USerStorageXML &xml);
};

}
#endif

