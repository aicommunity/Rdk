//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TIdTcpResultBroadcasterFrameUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "myrdk.h"
#include "rdk_initdll.h"
#include "TUBitmap.h"
#include "TUHttpServerUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TIdTcpResultBroadcasterFrame *IdTcpResultBroadcasterFrame;
//---------------------------------------------------------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TTcpResultBroadcasterThread::TTcpResultBroadcasterThread(TIdTcpResultBroadcasterFrame * frame, bool CreateSuspended)
: TResultBroadcasterThread(CreateSuspended), Frame(frame)
{
 IdTCPClient=new TIdTCPClient(Frame);
 IdTCPClient->OnConnected=IdTCPConnected;
 IdTCPClient->OnDisconnected=IdTCPDisconnected;
}

__fastcall TTcpResultBroadcasterThread::~TTcpResultBroadcasterThread(void)
{
 SetSendEnableFlag(false);
 Terminate();
 WaitForSingleObject(SendNotInProgressEvent,100);

 Disconnect();
 if(IdTCPClient)
 {
  delete IdTCPClient;
  IdTCPClient=0;
 }
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
std::string TTcpResultBroadcasterThread::GetAddress(void) const
{
 return Address;
}

int TTcpResultBroadcasterThread::GetPort(void) const
{
 return Port;
}

bool TTcpResultBroadcasterThread::Init(const std::string &address, int port)
{
 if(!IdTCPClient)
  return false;

 if(WaitForSingleObject(SendNotInProgressEvent,30) == WAIT_TIMEOUT)
  return false;

 if(IdTCPClient->Host == address.c_str() && IdTCPClient->Port == port)
  return true;
 Disconnect();
 Address=address;
 Port=port;

 return true;
}

void TTcpResultBroadcasterThread::Connect(void)
{
 if(!IdTCPClient)
  return;
  try
  {
   IdTCPClient->Host=Address.Get().c_str();
   IdTCPClient->Port=Port;
   IdTCPClient->Connect();
  }
  catch (EIdConnectTimeout &ex)
  {
   return;
  }
  catch (EIdAlreadyConnected &ex)
  {
   ConnectionEstablishedFlag=true;
  }
  catch (EIdSocketError &ex)
  {
   ConnectionEstablishedFlag=false;
   IdTCPClient->Disconnect();
  }
}

void TTcpResultBroadcasterThread::Disconnect(void)
{
 if(!IdTCPClient)
  return;
 if(IdTCPClient)
 {
  try
  {
   IdTCPClient->Disconnect();
   ConnectionEstablishedFlag=false;
  }
  catch(EIdNotConnected &ex)
  {

  }
 }
}



bool __fastcall TTcpResultBroadcasterThread::ASend(void)
{
 if(!ConnectionEstablishedFlag)
 {
  Connect();
  if(!ConnectionEstablishedFlag)
   return false;
 }

  try
  {
   TIdBytes Buf;
   Buf.set_length(SendBuffer.size());
   memcpy(&Buf[0],&SendBuffer[0],SendBuffer.size());
   if(IdTCPClient->IOHandler)
   {
	IdTCPClient->IOHandler->WriteDirect(Buf,Buf.Length,0);
	Engine_LogMessage(RDK_EX_DEBUG, (std::string("Metadata sent to ")+Address.Get()+std::string(":")+RDK::sntoa(Port)+std::string(": ")+RDK::sntoa(Buf.Length)+std::string(" bytes")).c_str());
   }
  }
  catch (EIdConnectTimeout &ex)
  {
   ConnectionEstablishedFlag=false;
  }
  catch (EIdReadTimeout &ex)
  {
   ConnectionEstablishedFlag=false;
  }
  catch (EIdSocketError &ex)
  {
   ConnectionEstablishedFlag=false;
//   IdHTTP->Disconnect();
  }
  catch(...)
  {
   throw;
  }

 return true;
}

void __fastcall TTcpResultBroadcasterThread::IdTCPConnected(TObject *Sender)
{
 ConnectionEstablishedFlag=true;
}

void __fastcall TTcpResultBroadcasterThread::IdTCPDisconnected(TObject *Sender)
{
 ConnectionEstablishedFlag=false;
}
// --------------------------



__fastcall TIdTcpResultBroadcasterFrame::TIdTcpResultBroadcasterFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 MemStream=new TMemoryStream;

 Thread=new TTcpResultBroadcasterThread(this, false);

 Bitmap=new TBitmap;
 ConnectionEstablishedFlag=false;

 LastSentTimeStamp=-1;
}

__fastcall TIdTcpResultBroadcasterFrame::~TIdTcpResultBroadcasterFrame(void)
{
 if(Thread)
 {
  Thread->Terminate();
  WaitForSingleObject(Thread->SendNotInProgressEvent,100);

  delete Thread;
  Thread=0;
 }

 ConnectionEstablishedFlag=false;
 if(MemStream)
  delete MemStream;

 if(Bitmap)
  delete Bitmap;
 try{
 IdTCPClient->Disconnect();
 }
 catch(EIdNotConnected &ex)
 {

 }
}

//---------------------------------------------------------------------------

// --------------------------
// Методы управления фреймом
// --------------------------
/// Инициализация канала связи в соответствии с настройками
bool TIdTcpResultBroadcasterFrame::Init(void)
{
 if(!Thread)
  return false;

 String AUrl=ServerAddressLabeledEdit->Text;

 int i = LastDelimiter(':', AUrl);
 int j = AUrl.Length();
 if(!i || !j)
  return false;

 int port=StrToInt(AUrl.SubString(i + 1, j - i));
 std::string url=AnsiString(AUrl.SubString(1, i-1)).c_str();
 Thread->Init(url,port);
 return true;
}

bool TIdTcpResultBroadcasterFrame::UnInit(void)
{
 if(!Thread)
  return true;

 Thread->Disconnect();
 return true;

}


/// Функция добавления метаданных в очередь на отправку в соответствии с настройками
bool TIdTcpResultBroadcasterFrame::AddMetadata(int channel_index, RDK::ULongTime time_stamp)
{
 if(!EnableXmlTranslationCheckBox->Checked)
  return true;

 if(!Thread)
  return false;

 return Thread->AddMetadataSafe(channel_index,time_stamp,AnsiString(XmlComponentNameLabeledEdit->Text).c_str(),
   AnsiString(XmlComponentStateNameLabeledEdit->Text).c_str());
}


void TIdTcpResultBroadcasterFrame::ABeforeCalculate(void)
{

}

void TIdTcpResultBroadcasterFrame::AAfterCalculate(void)
{
/*
 if(!EnableXmlTranslationCheckBox->Checked)
  return;

 int channel_index=StrToInt(ChannelIndexLabeledEdit->Text);
 if(channel_index>GetNumEngines())
  return;

 if(LastSentTimeStamp == UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[channel_index])
  return;
 LastSentTimeStamp=UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[channel_index];

 String AUrl=ServerAddressLabeledEdit->Text;

 if(!ConnectionEstablishedFlag)
 {
  try
  {
   int i = LastDelimiter(':', AUrl);
   int j = AUrl.Length();
   if(!i || !j)
	return;

   int port=StrToInt(AUrl.SubString(i + 1, j - i));
   String url=AUrl.SubString(1, i-1);
   IdTCPClient->Host=url;
   IdTCPClient->Port=port;
   IdTCPClient->Connect();
  }
  catch (EIdConnectTimeout &ex)
  {
   return;
  }
  catch (EIdAlreadyConnected &ex)
  {
   ConnectionEstablishedFlag=true;
  }
  catch (EIdSocketError &ex)
  {
   ConnectionEstablishedFlag=false;
   IdTCPClient->Disconnect();
  }
 }

 if(EnableXmlTranslationCheckBox->Checked)
 {
  const char* xml_data=0;
  if(XmlComponentStateNameLabeledEdit->Text.Length()>0)
   xml_data=MModel_GetComponentStateValue(channel_index, AnsiString(XmlComponentNameLabeledEdit->Text).c_str(),
												AnsiString(XmlComponentStateNameLabeledEdit->Text).c_str());
  else
   xml_data=MModel_GetComponentState(channel_index, AnsiString(XmlComponentNameLabeledEdit->Text).c_str());

  if(xml_data)
  {
   EncodeMetaPackage(xml_data, LastSentTimeStamp, channel_index, Metadata);
  }

  try
  {
   TIdBytes Buf;
   Buf.set_length(Metadata.size());
   memcpy(&Buf[0],&Metadata[0],Metadata.size());
   if(IdTCPClient->IOHandler)
    IdTCPClient->IOHandler->WriteDirect(Buf,Buf.Length,0);
   //Post(AUrl, ASource);
  }
  catch (EIdConnectTimeout &ex)
  {
   ConnectionEstablishedFlag=false;
  }
  catch (EIdReadTimeout &ex)
  {
   ConnectionEstablishedFlag=false;
  }
  catch (EIdSocketError &ex)
  {
   ConnectionEstablishedFlag=false;
//   IdHTTP->Disconnect();
  }
  catch(...)
  {
   throw;
  }

 }
 */
}

void TIdTcpResultBroadcasterFrame::AUpdateInterface(void)
{

}

// Возврат интерфейса в исходное состояние
void TIdTcpResultBroadcasterFrame::AClearInterface(void)
{
 UnInit();
}

// Сохраняет параметры интерфейса в xml
void TIdTcpResultBroadcasterFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("ServerAddress",AnsiString(ServerAddressLabeledEdit->Text).c_str());
 xml.WriteString("XmlComponentName",AnsiString(XmlComponentNameLabeledEdit->Text).c_str());
 xml.WriteString("XmlComponentStateName",AnsiString(XmlComponentStateNameLabeledEdit->Text).c_str());
 xml.WriteBool("EnableXmlTranslation",EnableXmlTranslationCheckBox->Checked);
// xml.WriteInteger("ChannelIndex",StrToInt(ChannelIndexLabeledEdit->Text));
}

// Загружает параметры интерфейса из xml
void TIdTcpResultBroadcasterFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 ServerAddressLabeledEdit->Text=xml.ReadString("ServerAddress","").c_str();
 XmlComponentNameLabeledEdit->Text=xml.ReadString("XmlComponentName","").c_str();
 XmlComponentStateNameLabeledEdit->Text=xml.ReadString("XmlComponentStateName","").c_str();
 EnableXmlTranslationCheckBox->Checked=xml.ReadBool("EnableXmlTranslation",false);
// ChannelIndexLabeledEdit->Text=IntToStr(xml.ReadInteger("ChannelIndex",0));
 Init();
}


// Создание копии этого компонента
TIdTcpResultBroadcasterFrame* TIdTcpResultBroadcasterFrame::New(TComponent *owner)
{
 return new TIdTcpResultBroadcasterFrame(owner);
}

// --------------------------
void __fastcall TIdTcpResultBroadcasterFrame::ConnectButtonClick(TObject *Sender)

{
 Init();
}
//---------------------------------------------------------------------------

void __fastcall TIdTcpResultBroadcasterFrame::DisconnectButtonClick(TObject *Sender)
{
 if(Thread)
 {
  ServerAddressLabeledEdit->Text=Thread->GetAddress().c_str();
  ServerAddressLabeledEdit->Text+=":";
  ServerAddressLabeledEdit->Text=IntToStr(Thread->GetPort());
 }
}
//---------------------------------------------------------------------------




void __fastcall TIdTcpResultBroadcasterFrame::EnableXmlTranslationCheckBoxClick(TObject *Sender)

{
 if(Thread)
 {
  if(EnableXmlTranslationCheckBox->Checked)
   Thread->SetSendEnableFlag(true);
  else
   Thread->SetSendEnableFlag(false);
 }
}
//---------------------------------------------------------------------------




