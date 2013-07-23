//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TIdHttpResultBroadcasterFrameUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "myrdk.h"
#include "rdk_initdll.h"
#include "TUBitmap.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TIdHttpResultBroadcasterFrame *IdHttpResultBroadcasterFrame;
//---------------------------------------------------------------------------
__fastcall TIdHttpResultBroadcasterFrame::TIdHttpResultBroadcasterFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 MemStream=new TMemoryStream;

 Bitmap=new TBitmap;
}

__fastcall TIdHttpResultBroadcasterFrame::~TIdHttpResultBroadcasterFrame(void)
{
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
 String AUrl=ServerAddressLabeledEdit->Text;
 TIdMultiPartFormDataStream* ASource=new TIdMultiPartFormDataStream;

 ASource->AddFormField("TimeStamp",IntToStr(UEngineMonitorForm->EngineMonitorFrame->ServerTimeStamp));
 if(EnableXmlTranslationCheckBox->Checked)
 {
  const char* xml_data=Model_GetComponentStateValue(AnsiString(XmlComponentNameLabeledEdit->Text).c_str(),
												AnsiString(XmlComponentStateNameLabeledEdit->Text).c_str());
  if(xml_data)
   ASource->AddFormField("Response",xml_data,"","text/plain");
 }

 if(EnableImagesTranslationCheckBox->Checked)
 {
  const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentBitmapOutput(AnsiString(ImagesComponentNameLabeledEdit->Text).c_str(),
												StrToInt(ImagesComponentOutputNameLabeledEdit->Text));
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

// System::Classes::TStream* AResponseContent;
 IdHTTP->Post(AUrl, ASource);

 delete ASource;
}

void TIdHttpResultBroadcasterFrame::AUpdateInterface(void)
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
