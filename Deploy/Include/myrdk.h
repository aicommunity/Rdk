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

namespace RDK {

// Декодирует содержимое свойства/переменной состояния компонента
template<typename T>
T& DecodeParameterValue(const std::string &param_value, T &res)
{
 using namespace RDK::Serialize;

 USerStorageXML xml;
 xml.Create("1");
 xml.SelectRoot();
 xml.SetNodeText(param_value);

 xml>>res;
 return res;
}

template<typename T>
T DecodeParameterValue(const std::string &param_value)
{
 T res;

 return DecodeParameterValue(param_value,res);
}

// Считывает и декодирует содержимое свойства компонента
template<typename T>
T& ReadPropertyValue(const std::string &comp_name, const std::string &param_name, T &res)
{
 std::string param_value=Model_GetComponentPropertyValue(comp_name.c_str(),param_name.c_str());

 return RDK::DecodeParameterValue(param_value,res);
}

template<typename T>
T ReadPropertyValue(const std::string &comp_name, const std::string &param_name)
{
 T res;
 return ReadPropertyValue(comp_name, param_name,res);
}

// Считывает и декодирует содержимое переменной состояния компонента
template<typename T>
T& ReadStateValue(const std::string &comp_name, const std::string &param_name, T &res)
{
 std::string param_value=Model_GetComponentStateValue(comp_name.c_str(),param_name.c_str());

 return RDK::DecodeParameterValue(param_value,res);
}

template<typename T>
T ReadStateValue(const std::string &comp_name, const std::string &param_name)
{
 T res;
 return ReadStateValue(comp_name, param_name,res);
}

// Класс прототип-визуальных интерфейсов
class IVisualInterface
{
public:
// Метод, вызываемый перед шагом расчета
virtual void BeforeCalculate(void)=0;

// Метод, вызываемый после шага расчета
virtual void AfterCalculate(void)=0;

// Обновление интерфейса
virtual void UpdateInterface(void)=0;
};

}
#endif

