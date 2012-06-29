#ifndef MYRDK_CPP
#define MYRDK_CPP

#include "myrdk.h"

#include "../../ThirdParty/ThirdParty.cpp"
#include "../../Core/Math/UMath.cpp"
#include "../../Core/Utilities/Utilities.cpp"
#include "../../Core/Serialize/Serialize.cpp"

#include "../../Core/Engine/UAEnvSupport.cpp"
#include "../../Core/Engine/UXMLEnvSerialize.cpp"
#include "../../Core/Engine/UBinaryEnvSerialize.cpp"
#include "../../Core/Graphics/UBitmap.cpp"
#include "../../Core/Graphics/UAGraphics.cpp"
#include "../../Core/Graphics/UGraphics.cpp"
#include "../../Core/Graphics/UGraphicsXMLSerialize.cpp"
#include "../../Core/Graphics/UGraphicsBinarySerialize.cpp"

namespace RDK {


// ����� ���������-���������� �����������
// ������ ������������, ������� ������ ���� ������� ����� �������
std::vector<RDK::UIVisualController*> UIVisualControllerStorage::InterfaceUpdaters;

// ��������� ���������� � ������
void UIVisualControllerStorage::AddInterface(RDK::UIVisualController *value)
{
 std::vector<RDK::UIVisualController*>::iterator I=find(InterfaceUpdaters.begin(),InterfaceUpdaters.end(),value);
 if(I == InterfaceUpdaters.end())
  InterfaceUpdaters.push_back(value);
}

// ������� ���������� �� ������
void UIVisualControllerStorage::DelInterface(RDK::UIVisualController *value)
{
 std::vector<RDK::UIVisualController*>::iterator I=find(InterfaceUpdaters.begin(),InterfaceUpdaters.end(),value);
 if(I != InterfaceUpdaters.end())
  InterfaceUpdaters.erase(I);
}

// �����, ���������� ����� ����� �������
void UIVisualControllerStorage::BeforeCalculate(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->BeforeCalculate();
}

// �����, ���������� ����� ���� �������
void UIVisualControllerStorage::AfterCalculate(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->AfterCalculate();
}

// ���������� ����������
void UIVisualControllerStorage::UpdateInterface(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->UpdateInterface();
}

// ��������� ��������� ���������� � xml
void UIVisualControllerStorage::SaveParameters(RDK::Serialize::USerStorageXML &xml)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->SaveParameters(xml);
}

// ��������� ��������� ���������� �� xml
void UIVisualControllerStorage::LoadParameters(RDK::Serialize::USerStorageXML &xml)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->LoadParameters(xml);
}


}
#endif


