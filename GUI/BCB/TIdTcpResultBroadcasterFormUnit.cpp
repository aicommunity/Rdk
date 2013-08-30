//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TIdTcpResultBroadcasterFormUnit.h"
#include "UGEngineControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TIdTcpResultBroadcasterForm *IdTcpResultBroadcasterForm;
//---------------------------------------------------------------------------
__fastcall TIdTcpResultBroadcasterForm::TIdTcpResultBroadcasterForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

/// ������� ���������� ���������� � ������� �� �������� � ������������ � �����������
bool TIdTcpResultBroadcasterForm::AddMetadata(int channel_index, long long time_stamp)
{
 bool res=true;
 for(size_t i=0;i<Broadcasters.size();i++)
 {
  res&=Broadcasters[i]->AddMetadata(channel_index, time_stamp);
 }
 return res;
}

/// ���������� ��������� �������� ����������
bool TIdTcpResultBroadcasterForm::SendMetadata(void)
{
 for(size_t i=0;i<Broadcasters.size();i++)
  SetEvent(Broadcasters[i]->Thread->SendEnable);

 return true;
}


// �����, ���������� ����� ����� �������
void TIdTcpResultBroadcasterForm::ABeforeCalculate(void)
{
}

// ��������� ���������
void TIdTcpResultBroadcasterForm::AUpdateInterface(void)
{
}

// ��������� ��������� ���������� � xml
void TIdTcpResultBroadcasterForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("NumBroadcasters",GetNumBroadcasters());
}

// ��������� ��������� ���������� �� xml
void TIdTcpResultBroadcasterForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 int num=xml.ReadInteger("NumBroadcasters",1);
 ClearBroadcasters();
 for(int i=0;i<num;i++)
  AddBroadcaster();
 UpdateInterface();
}

// �������� ����� ����� ����������
TIdTcpResultBroadcasterForm* TIdTcpResultBroadcasterForm::New(TComponent *owner)
{
 return new TIdTcpResultBroadcasterForm(owner);
}

// ����� ���������
int TIdTcpResultBroadcasterForm::GetNumBroadcasters(void) const
{
 return PageControl->PageCount;
}

// ��������� ����� ��������
void TIdTcpResultBroadcasterForm::AddBroadcaster(void)
{
 TTabSheet *sheet=new TTabSheet(PageControl);
 sheet->PageControl=PageControl;
 sheet->PageIndex=PageControl->PageCount-1;
 Broadcasters.resize(PageControl->PageCount);

 size_t index=Broadcasters.size()-1;

 Broadcasters[index]=new TIdTcpResultBroadcasterFrame(0);
 Broadcasters[index]->Name=Broadcasters[index]->Name+String("_")+PageControl->PageCount;

 InsertComponent(Broadcasters[index]);
 Broadcasters[index]->Parent=sheet;
 Broadcasters[index]->Align=alClient;
 PageControl->Pages[index]->Caption=IntToStr(int(index));
}

// ������� ��������
void TIdTcpResultBroadcasterForm::DelBroadcaster(int index)
{
 if(index<0 || index >=int(Broadcasters.size()))
  return;

 delete Broadcasters[index];
 Broadcasters.erase(Broadcasters.begin()+index);
 delete PageControl->Pages[index];
}

// ������� ��� ��������
void TIdTcpResultBroadcasterForm::ClearBroadcasters(void)
{
 for(size_t i=0;i<Broadcasters.size();i++)
 {
  delete Broadcasters[i];
  delete PageControl->Pages[0];
 }

 Broadcasters.clear();
}

// ���������� ������ �������� ��������� (����������) ��������
int TIdTcpResultBroadcasterForm::GetActiveBroadcaster(void) const
{
 return PageControl->ActivePageIndex;
}

// ���������� ����� ��������
TIdTcpResultBroadcasterFrame* TIdTcpResultBroadcasterForm::GetBroadcasterFrame(int index)
{
 if(index<0 || index >=int(Broadcasters.size()))
  return 0;

 return Broadcasters[index];
}

// ���������� ����� ��������� (����������) ��������
TIdTcpResultBroadcasterFrame* TIdTcpResultBroadcasterForm::GetActiveBroadcasterFrame(void)
{
 if(GetActiveBroadcaster()>=0)
  return Broadcasters[GetActiveBroadcaster()];

 return 0;
}

// ���������� ��������, ��� ���, ���� index == -1
void TIdTcpResultBroadcasterForm::Connect(int index)
{
 if(index>=0 && index<GetNumBroadcasters())
  Broadcasters[index]->ConnectButtonClick(this);
 else
 {
  for(int i=0;i<GetNumBroadcasters();i++)
   Broadcasters[i]->ConnectButtonClick(this);
 }
}

// ��������� ��������, ��� ���, ���� index == -1
void TIdTcpResultBroadcasterForm::Disconnect(int index)
{
 if(index>=0 && index<GetNumBroadcasters())
  Broadcasters[index]->DisconnectButtonClick(this);
 else
 {
  for(int i=0;i<GetNumBroadcasters();i++)
   Broadcasters[i]->DisconnectButtonClick(this);
 }
}

//---------------------------------------------------------------------------
void __fastcall TIdTcpResultBroadcasterForm::Add1Click(TObject *Sender)
{
 AddBroadcaster();
}
//---------------------------------------------------------------------------
void __fastcall TIdTcpResultBroadcasterForm::Del1Click(TObject *Sender)
{
 if(GetNumBroadcasters()<=0)
  return;

 DelBroadcaster(PageControl->ActivePageIndex);
}
//---------------------------------------------------------------------------
void __fastcall TIdTcpResultBroadcasterForm::ConnectAll1Click(TObject *Sender)
{
 Connect();
}
//---------------------------------------------------------------------------
void __fastcall TIdTcpResultBroadcasterForm::DisconnectAll1Click(TObject *Sender)
{
 Disconnect();
}
//---------------------------------------------------------------------------

void __fastcall TIdTcpResultBroadcasterForm::FormDestroy(TObject *Sender)
{
// UGEngineControlForm->SpecialForms.erase("TIdHttpResultBroadcasterForm");
}
//---------------------------------------------------------------------------

void __fastcall TIdTcpResultBroadcasterForm::FormCreate(TObject *Sender)
{
 UGEngineControlForm->SpecialForms["TIdTcpResultBroadcasterForm"]=this;
 UGEngineControlForm->AddBroadcasterMenu(HttpBroadcaster1,PopupMenu1);
}
//---------------------------------------------------------------------------

void __fastcall TIdTcpResultBroadcasterForm::HttpBroadcaster1Click(TObject *Sender)

{
 UGEngineControlForm->AddSpecialFormPage("TIdTcpResultBroadcasterForm");
 Show();
}
//---------------------------------------------------------------------------

