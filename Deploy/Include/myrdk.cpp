#ifndef MYRDK_CPP
#define MYRDK_CPP

#include "myrdk.h"

#include "../../ThirdParty/ThirdParty.cpp"
#include "../../Core/Math/UMath.cpp"
#include "../../Core/Utilities/Utilities.cpp"
#include "../../Core/Serialize/Serialize.cpp"

#include "../../Core/Engine/UEnvSupport.cpp"
#include "../../Core/Engine/UXMLEnvSerialize.cpp"
#include "../../Core/Engine/UBinaryEnvSerialize.cpp"
#include "../../Core/Graphics/UBitmap.cpp"
#include "../../Core/Graphics/UBitmapVector.cpp"
#include "../../Core/Graphics/UAGraphics.cpp"
#include "../../Core/Graphics/UGraphicsIO.cpp"
#include "../../Core/Graphics/UGraphics.cpp"
#include "../../Core/Graphics/UFont.cpp"
#include "../../Core/Graphics/UDrawEngine.cpp"
#include "../../Core/Graphics/UGraphicsXMLSerialize.cpp"
#include "../../Core/Graphics/UGraphicsBinarySerialize.cpp"

namespace RDK {

// ���������� ��������� �������
RDK::UBitmapFontCollection GlobalFonts;

// ������� ��������� ���������� �������
void ClearClobalFonts(void)
{
 GlobalFonts.DelAllFonts();
}

// ��������� ����� ���������� �����
bool AddGlobalFont(const std::string &font_file_name)
{
 RDK::UBitmapFont font;
 std::size_t dir_sep_pos=font_file_name.find_last_of("\\/");
 std::string font_name;
 if(dir_sep_pos != string::npos)
  font_name=font_file_name.substr(dir_sep_pos+1);
 else
  font_name=font_file_name;
 std::size_t _pos=font_name.find_first_of("_");
 std::size_t _pos2=font_name.find_first_not_of("0123456789",_pos+1);
 if(_pos != std::string::npos)
 {
  std::string font_string_size=font_name.substr(_pos+1,_pos2);
  int size=RDK::atoi(font_string_size);
  if(!font.LoadFromFile(font_name.substr(0,_pos),font_file_name,size))
   return false;
  return RDK::GlobalFonts.AddFont(font.GetName(),size,font);
 }
 return false;
}



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

// �����, ���������� ����� ������� ������
void UIVisualControllerStorage::BeforeReset(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->BeforeReset();
}

// �����, ���������� ����� ������ ������
void UIVisualControllerStorage::AfterReset(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->AfterReset();
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
   InterfaceUpdaters[i]->UpdateInterface(false);
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


