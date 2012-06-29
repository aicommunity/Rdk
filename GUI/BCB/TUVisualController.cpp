//---------------------------------------------------------------------------

#pragma hdrstop

#include "TUVisualController.h"
/*#pragma package(smart_init)

// --------------------------
// ������������ � �����������
// --------------------------
TUFormsSerialize::TUFormsSerialize(void)
{
 Xml=0;
}

TUFormsSerialize::~TUFormsSerialize(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���������� ���������� ����
RDK::Serialize::USerStorageXML* TUFormsSerialize::GetXml(void)
{
 return Xml;
}

void TUFormsSerialize::SetXml(RDK::Serialize::USerStorageXML *xml)
{
 Xml=xml;
}

// ��������� ����� � ������ �������������
void TUFormsSerialize::AddForm(const std::string &name, TForm *form)
{
 Forms[name]=form;
}

// ������� ����� �� ������ �������������
void TUFormsSerialize::DelForm(TForm *form)
{
 std::map<std::string, TForm* >::iterator I,J,K;

 for(I=Forms.begin(), J=Forms.end();I!=J;++I)
 {
  if(I->second == form)
  {
   K=++I;
   Forms.erase(I);
   I=K;
  }
 }
}

void TUFormsSerialize::DelForm(const std::string &name)
{
 std::map<std::string, TForm* >::iterator I=Forms.find(name);

 if(I != Forms.end())
  Forms.erase(I);
}

// ������� ��� ����� �� ������ �������������
void TUFormsSerialize::ClearForms(void)
{
 Forms.clear();
}

// --------------------------

// --------------------------
// ������ ������������
// --------------------------
// ��������� ��� ������ ���� � xml
void TUFormsSerialize::Save(void)
{
 std::map<std::string, TForm* >::iterator I,J;

 for(I=Forms.begin(), J=Forms.end();I!=J;++I)
 {

 }
}

// ��������� ��� ������ ���� �� xml
void TUFormsSerialize::Load(void)
{
 std::map<std::string, TForm* >::iterator I,J;


 for(I=Forms.begin(), J=Forms.end();I!=J;++I)
 {

 }

}


// ��������� xml � ����
void TUFormsSerialize::SaveXmlToFile(const String &filename)
{
 if(!Xml)
  return;

 TRichEdit *edit=new TRichEdit(Application->Handle);
 edit->Visible=false;
 edit->PlainText=true;
 std::string xmldata;
 Xml->Save(xmldata);
 edit->Text=xmldata.c_str();
 edit->Lines->SaveToFile(filename);
 delete edit;
}

// ��������� xml �� �����
void TUFormsSerialize::LoadXmlFromFile(const String &filename)
{
 if(!Xml)
  return;

 TRichEdit *edit=new TRichEdit(Application->Handle);
 edit->Visible=false;
 edit->PlainText=true;
 edit->Lines->LoadFromFile(filename);
 std::string xmldata=AnsiString(edit->Text).c_str();
 Xml->Load(xmldata,"");
 delete edit;
}
// --------------------------

// --------------------------
// ��������������� ������ ������������
// --------------------------
// ��������� ������ ����� � xml
void TUFormsSerialize::SaveForm(RDK::Serialize::USerStorageXML &xml, TForm *form)
{
 if(!form)
  return;

 xml.AddNode("Position");
 xml.SetNodeAttribute("Left",RDK::sntoa(form->Left));
 xml.SetNodeAttribute("Top",RDK::sntoa(form->Top));
 xml.SetNodeAttribute("Width",RDK::sntoa(form->Width));
 xml.SetNodeAttribute("Height",RDK::sntoa(form->Height));
 xml.SetNodeAttribute("Visible",RDK::sntoa(form->Visible));
 xml.SelectUp();
}

// ��������� ������ ����� �� xml
void TUFormsSerialize::LoadForm(RDK::Serialize::USerStorageXML &xml, TForm *form)
{
 if(!form)
  return;

 xml.SelectNode("Position");

 std::string value;
 value=xml.GetNodeAttribute("Left");
 if(!value.empty())
  form->Left=RDK::atoi(value);

 value=xml.GetNodeAttribute("Top");
 if(!value.empty())
  form->Top=RDK::atoi(value);

 value=xml.GetNodeAttribute("Width");
 if(!value.empty())
  form->Width=RDK::atoi(value);

 value=xml.GetNodeAttribute("Height");
 if(!value.empty())
  form->Height=RDK::atoi(value);

 value=xml.GetNodeAttribute("Visible");
 if(!value.empty())
  form->Visible=RDK::atoi(value);
 xml.SelectUp();
}
// --------------------------

//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TUVisualControllerFrame*)
{
  new TUVisualControllerFrame(NULL);
}

namespace TUVisualController
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TUVisualControllerFrame)};
     RegisterComponents("Frames", classes, 0);
  }
}         */
