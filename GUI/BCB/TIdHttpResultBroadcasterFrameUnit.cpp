//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TIdHttpResultBroadcasterFrameUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "TUBitmap.h"
#include "TUHttpServerUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TIdHttpResultBroadcasterFrame *IdHttpResultBroadcasterFrame;

/// Экземпляр класса контроллера расчета
extern UEngineControlVcl RdkEngineControl;
//---------------------------------------------------------------------------
__fastcall TIdHttpResultBroadcasterFrame::TIdHttpResultBroadcasterFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 MemStream=new TMemoryStream;

 Bitmap=new TBitmap;
 ConnectionEstablishedFlag=false;

 LastSentTimeStamp=-1;
}

__fastcall TIdHttpResultBroadcasterFrame::~TIdHttpResultBroadcasterFrame(void)
{
 ConnectionEstablishedFlag=false;
 IdHTTP->Disconnect();
 if(MemStream)
  delete MemStream;

 if(Bitmap)
  delete Bitmap;
}

//---------------------------------------------------------------------------

// --------------------------
// Методы управления фреймом
// --------------------------
void TIdHttpResultBroadcasterFrame::ABeforeCalculate(void)
{

}

void TIdHttpResultBroadcasterFrame::AAfterCalculate(void)
{
 if(!EnableXmlTranslationCheckBox->Checked && !EnableImagesTranslationCheckBox->Checked)
  return;

 int channel_index=StrToInt(ChannelIndexLabeledEdit->Text);
 if(channel_index>GetNumEngines())
  return;

 if(LastSentTimeStamp == RdkEngineControl.GetEngineThread(channel_index)->GetLastCalculationExternalTime()*86400.0*1000.0)
  return;
 LastSentTimeStamp=RdkEngineControl.GetEngineThread(channel_index)->GetLastCalculationExternalTime()*86400.0*1000.0;

 String AUrl=ServerAddressLabeledEdit->Text;

 if(!ConnectionEstablishedFlag)
 {
  try
  {
   int i = LastDelimiter(':', AUrl);
   int j = LastDelimiter('/', AUrl);
   if(!i || !j)
	return;

   int port=StrToInt(AUrl.SubString(i + 1, j - i-1));
   String url=AUrl.SubString(1, i-1);
   IdHTTP->Connect(url,port);
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
   IdHTTP->Disconnect();
  }
 }


 TIdMultiPartFormDataStream* ASource=new TIdMultiPartFormDataStream;

// ASource->AddFormField("TimeStamp",IntToStr(LastSentTimeStamp));
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

   ASource->AddFormField("Response",Metadata.c_str(),"","text/plain");
  }
  MEngine_FreeBufString(channel_index,xml_data);
 }

 if(EnableImagesTranslationCheckBox->Checked)
 {
  const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentBitmapOutput(AnsiString(ImagesComponentNameLabeledEdit->Text).c_str(),
												AnsiString(ImagesComponentOutputNameLabeledEdit->Text).c_str());
  if(bmp)
  {
   *bmp>>Bitmap;
   MemStream->Clear();
   Bitmap->SaveToStream(MemStream);
   MemStream->Position=0;
   MemStream->Seek(__int64(0),soBeginning);
   ASource->AddObject("Image", "image/bmp", "", MemStream, ImagesComponentNameLabeledEdit->Text+"."+ImagesComponentOutputNameLabeledEdit->Text);
  }
 }

 if(EnableXmlTranslationCheckBox->Checked || EnableImagesTranslationCheckBox->Checked)
 {
  try
  {
   IdHTTP->Post(AUrl, ASource);
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
   delete ASource;
   throw;
  }
/*  catch (EIdNotASocket &ex)
  {
   int a=0;
  }*/
 }

 delete ASource;
}

void TIdHttpResultBroadcasterFrame::AUpdateInterface(void)
{

}

// Возврат интерфейса в исходное состояние
void TIdHttpResultBroadcasterFrame::AClearInterface(void)
{
}


// Сохраняет параметры интерфейса в xml
void TIdHttpResultBroadcasterFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("ServerAddress",AnsiString(ServerAddressLabeledEdit->Text).c_str());
 xml.WriteString("XmlComponentName",AnsiString(XmlComponentNameLabeledEdit->Text).c_str());
 xml.WriteString("XmlComponentStateName",AnsiString(XmlComponentStateNameLabeledEdit->Text).c_str());
 xml.WriteString("ImagesComponentName",AnsiString(ImagesComponentNameLabeledEdit->Text).c_str());
 xml.WriteString("ImagesComponentOutputName",AnsiString(ImagesComponentOutputNameLabeledEdit->Text).c_str());
 xml.WriteBool("EnableXmlTranslation",EnableXmlTranslationCheckBox->Checked);
 xml.WriteBool("EnableImagesTranslation",EnableImagesTranslationCheckBox->Checked);
 xml.WriteInteger("ChannelIndex",StrToInt(ChannelIndexLabeledEdit->Text));
}

// Загружает параметры интерфейса из xml
void TIdHttpResultBroadcasterFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 ServerAddressLabeledEdit->Text=xml.ReadString("ServerAddress","").c_str();
 XmlComponentNameLabeledEdit->Text=xml.ReadString("XmlComponentName","").c_str();
 XmlComponentStateNameLabeledEdit->Text=xml.ReadString("XmlComponentStateName","").c_str();
 ImagesComponentNameLabeledEdit->Text=xml.ReadString("ImagesComponentName","").c_str();
 ImagesComponentOutputNameLabeledEdit->Text=xml.ReadString("ImagesComponentOutputName","").c_str();
 EnableXmlTranslationCheckBox->Checked=xml.ReadBool("EnableXmlTranslation",false);
 EnableImagesTranslationCheckBox->Checked=xml.ReadBool("EnableImagesTranslation",false);
 ChannelIndexLabeledEdit->Text=IntToStr(xml.ReadInteger("ChannelIndex",0));

IdHTTP->ConnectTimeout=100;
IdHTTP->ReadTimeout=10;
}


// Создание копии этого компонента
TIdHttpResultBroadcasterFrame* TIdHttpResultBroadcasterFrame::New(TComponent *owner)
{
 return new TIdHttpResultBroadcasterFrame(owner);
}

// --------------------------
void __fastcall TIdHttpResultBroadcasterFrame::ConnectButtonClick(TObject *Sender)

{
 //
}
//---------------------------------------------------------------------------

void __fastcall TIdHttpResultBroadcasterFrame::DisconnectButtonClick(TObject *Sender)

{
 //
}
//---------------------------------------------------------------------------


void __fastcall TIdHttpResultBroadcasterFrame::IdHTTPConnected(TObject *Sender)
{
 ConnectionEstablishedFlag=true;
}
//---------------------------------------------------------------------------

void __fastcall TIdHttpResultBroadcasterFrame::IdHTTPDisconnected(TObject *Sender)

{
 ConnectionEstablishedFlag=false;
}
//---------------------------------------------------------------------------

