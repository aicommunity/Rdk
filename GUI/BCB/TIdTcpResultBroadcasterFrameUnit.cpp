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
__fastcall TIdTcpResultBroadcasterFrame::TIdTcpResultBroadcasterFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 MemStream=new TMemoryStream;

 Bitmap=new TBitmap;
 ConnectionEstablishedFlag=false;

 LastSentTimeStamp=-1;
}

__fastcall TIdTcpResultBroadcasterFrame::~TIdTcpResultBroadcasterFrame(void)
{
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
void TIdTcpResultBroadcasterFrame::ABeforeCalculate(void)
{

}

void TIdTcpResultBroadcasterFrame::AAfterCalculate(void)
{
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
/*  catch (EIdNotASocket &ex)
  {
   int a=0;
  }*/
 }

}

void TIdTcpResultBroadcasterFrame::AUpdateInterface(void)
{

}

// Сохраняет параметры интерфейса в xml
void TIdTcpResultBroadcasterFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("ServerAddress",AnsiString(ServerAddressLabeledEdit->Text).c_str());
 xml.WriteString("XmlComponentName",AnsiString(XmlComponentNameLabeledEdit->Text).c_str());
 xml.WriteString("XmlComponentStateName",AnsiString(XmlComponentStateNameLabeledEdit->Text).c_str());
 xml.WriteBool("EnableXmlTranslation",EnableXmlTranslationCheckBox->Checked);
 xml.WriteInteger("ChannelIndex",StrToInt(ChannelIndexLabeledEdit->Text));
}

// Загружает параметры интерфейса из xml
void TIdTcpResultBroadcasterFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 ServerAddressLabeledEdit->Text=xml.ReadString("ServerAddress","").c_str();
 XmlComponentNameLabeledEdit->Text=xml.ReadString("XmlComponentName","").c_str();
 XmlComponentStateNameLabeledEdit->Text=xml.ReadString("XmlComponentStateName","").c_str();
 EnableXmlTranslationCheckBox->Checked=xml.ReadBool("EnableXmlTranslation",false);
 ChannelIndexLabeledEdit->Text=IntToStr(xml.ReadInteger("ChannelIndex",0));
}


// Создание копии этого компонента
TIdTcpResultBroadcasterFrame* TIdTcpResultBroadcasterFrame::New(TComponent *owner)
{
 return new TIdTcpResultBroadcasterFrame(owner);
}

// --------------------------
void __fastcall TIdTcpResultBroadcasterFrame::ConnectButtonClick(TObject *Sender)

{
 //
}
//---------------------------------------------------------------------------

void __fastcall TIdTcpResultBroadcasterFrame::DisconnectButtonClick(TObject *Sender)

{
 //
}
//---------------------------------------------------------------------------


void __fastcall TIdTcpResultBroadcasterFrame::IdHTTPConnected(TObject *Sender)
{
 ConnectionEstablishedFlag=true;
}
//---------------------------------------------------------------------------

void __fastcall TIdTcpResultBroadcasterFrame::IdHTTPDisconnected(TObject *Sender)

{
 ConnectionEstablishedFlag=false;
}
//---------------------------------------------------------------------------

