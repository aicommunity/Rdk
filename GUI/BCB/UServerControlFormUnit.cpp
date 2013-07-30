//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UServerControlFormUnit.h"
#include "rdk_cpp_initdll.h"
#include "TUBitmap.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUHttpServerUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUServerControlForm *UServerControlForm;

/// Стандартная функция, осуществляющую декодирование параметров запроса
int StandardCommandRequestDecoder(std::map<std::string,std::vector<char> > &source, std::map<std::string,std::vector<char> > &dest)
{
 dest=source;
 return 0;
}

/// Стандартная функция, осуществляющую кодирование параметров ответа
int StandardCommandResponseEncoder(const std::string &response_type, std::vector<char> &source, std::vector<char> &dest)
{
 dest=source;
 return 0;
}


//---------------------------------------------------------------------------
__fastcall TUServerControlForm::TUServerControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 MemStream=0;
 CommandRequestDecoder=StandardCommandRequestDecoder;
 CommandResponseEncoder=StandardCommandResponseEncoder;
}

// Функция, обрабатывающая команды управления сервером
void TUServerControlForm::ProcessControlCommand(const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data)
{
 std::map<std::string,std::vector<char> >::const_iterator I;
 int res=0;
 std::string response,temp;
 std::string name;

 response_type="text/plain";
 if(cmd_name == "SetNumEngines")
 {
  I=args.find("Num");
  int num_engines=0;
  if(I != args.end())
  {
   temp.assign(&I->second[0],I->second.size());
   num_engines=atoi(temp);
//  res=SetNumEngines();
  }
  else
   res=1;

  ntoa(res,response);
  ConvertStringToVector(response, response_data);
 }
 else
 if(cmd_name == "GetNumEngines")
 {
//  res=GetNumEngines();
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
 }

}

// Функция, обрабатывающая команды удаленного вызова процедур
void TUServerControlForm::ProcessRPCCommand(int channel, const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data)
{
 std::map<std::string,std::vector<char> >::const_iterator I;
 int res=0;
 std::string response,temp;
 std::string name;

 response_type="text/plain";
 if(cmd_name == "EngineInit")
 {
  I=args.find("PredefinedStructure");
  int predefined_structure=0;
  if(I != args.end())
  {
   temp.assign(&I->second[0],I->second.size());
   predefined_structure=atoi(temp);
  }

  res=EngineInit(predefined_structure);
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
 }
 else
 if(cmd_name == "EngineUnInit")
 {
  res=EngineUnInit();
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
 }
 else
 if(cmd_name == "Env_Reset")
 {
  name.assign(&args["Name"][0],args["Name"].size());
  res=Env_Reset(name.c_str());
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
 }
 else
 if(cmd_name == "Env_Calculate")
 {
  name.assign(&args["Name"][0],args["Name"].size());
  res=Env_Calculate(name.c_str());
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
 }
 else
 if(cmd_name == "Model_Create")
 {
  I=args.find("Class");
  if(I != args.end())
  {
   temp.assign(&I->second[0],I->second.size());
   res=Model_Create(temp.c_str());
  }
  else
   res=100;
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
 }
 else
 if(cmd_name == "Model_LoadComponent")
 {
  name.assign(&args["Name"][0],args["Name"].size());
  temp.assign(&args["Xml"][0],args["Xml"].size());

  res=Model_LoadComponent(name.c_str(),temp.c_str());
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
 }
 else
 if(cmd_name == "Model_LoadComponentParameters")
 {
  name.assign(&args["Name"][0],args["Name"].size());
  temp.assign(&args["Xml"][0],args["Xml"].size());

  res=Model_LoadComponentParameters(name.c_str(),temp.c_str());
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
 }
 else
 if(cmd_name == "Model_GetComponentParameters")
 {
  name.assign(&args["Name"][0],args["Name"].size());
  const char* value=Model_GetComponentParameters(name.c_str());
  if(value)
  {
   ConvertStringToVector(value, response_data);
   res=0;
  }
  else
  {
   res=1;
   response_data.clear();
  }
 }
 else
 if(cmd_name == "Model_GetComponentStates")
 {
  name.assign(&args["Name"][0],args["Name"].size());
  const char* value=Model_GetComponentState(name.c_str());
  if(value)
  {
   ConvertStringToVector(value, response_data);
   res=0;
  }
  else
  {
   res=1;
   response_data.clear();
  }
 }
 else
 if(cmd_name == "Model_GetComponentBitmapOutput")
 {
  name.assign(&args["Name"][0],args["Name"].size());
  temp.assign(&args["Index"][0],args["Index"].size());

  int output_number=atoi(temp);
  const RDK::UBitmap* value=(const RDK::UBitmap*)Model_GetComponentBitmapOutput(name.c_str(),output_number);
  if(value)
  {
   *value>>Bitmap;
   MemStream->Clear();
   Bitmap->SaveToStream(MemStream);
   MemStream->Position=0;
   response_data.resize(MemStream->Size);
   if(response_data.size()>0)
    MemStream->ReadBuffer(&response_data[0],MemStream->Size);

   response_type="image/bmp";
   res=0;
  }
  else
  {
   res=1;
   response_data.clear();
  }
 }
 else
 if(cmd_name == "Model_GetComponentBitmapInput")
 {
  name.assign(&args["Name"][0],args["Name"].size());
  temp.assign(&args["Index"][0],args["Index"].size());

  int output_number=atoi(temp);
  const RDK::UBitmap* value=(const RDK::UBitmap*)Model_GetComponentBitmapInput(name.c_str(),output_number);
  if(value)
  {
   *value>>Bitmap;
   MemStream->Clear();
   Bitmap->SaveToStream(MemStream);
   MemStream->Position=0;
   response_data.resize(MemStream->Size);
   if(response_data.size()>0)
	MemStream->ReadBuffer(&response_data[0],MemStream->Size);

   res=0;
  }
  else
  {
   response_data.clear();
   res=1;
  }
 }
 else
 if(cmd_name == "Model_SetComponentBitmapOutput")
 {
  name.assign(&args["Name"][0],args["Name"].size());
  temp.assign(&args["Index"][0],args["Index"].size());
  vector<char> &image_data=args["Image"];

  if(image_data.empty())
  {
   res=1;
   response_data.clear();
  }
  else
  {
   MemStream->Clear();
   if(image_data.size()>0)
	MemStream->Write(&image_data[0],image_data.size());
   MemStream->Position=0;

   Bitmap->LoadFromStream(MemStream);
   TempUBitmap<<Bitmap;
 //  TempUBitmap>>Image1->Picture->Bitmap;
 //  Image1->Repaint();

   int output_number=atoi(temp);

   Model_SetComponentBitmapOutput(name.c_str(),output_number,&TempUBitmap);
   ConvertStringToVector("0", response_data);
  }
 }
}


/// Кодирует строку в вектор
void TUServerControlForm::ConvertStringToVector(const std::string &source, std::vector<char> &dest)
{
 dest.resize(source.size());
 if(source.size()>0)
  memcpy(&dest[0],source.c_str(),source.size());
}



// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
void TUServerControlForm::AAfterReset(void)
{

}

// Обновление интерфейса
void TUServerControlForm::AUpdateInterface(void)
{

}

// Сохраняет параметры интерфейса в xml
void TUServerControlForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("ServerControlPort",StrToInt(ServerControlPortLabeledEdit->Text));
 xml.WriteInteger("NumberOfChannels",StrToInt(NumberOfChannelsLabeledEdit->Text));
}

// Загружает параметры интерфейса из xml
void TUServerControlForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 ServerControlPortLabeledEdit->Text=IntToStr(xml.ReadInteger("ServerControlPort",80));
 NumberOfChannelsLabeledEdit->Text=IntToStr(xml.ReadInteger("NumberOfChannelsPort",1));
}
// -----------------------------

//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext,
		  TIdHTTPRequestInfo *ARequestInfo, TIdHTTPResponseInfo *AResponseInfo)

{
 Mode=-1;

 if(ARequestInfo->Document != "/control.cgi")
 {
  AResponseInfo->ResponseNo=404;
  AResponseInfo->ResponseText="404 Not Found";
  AResponseInfo->ContentText="404 Not Found";
  return;
 }

 UHttpServerFrame->IdHTTPServerCommandGet(AContext, ARequestInfo, AResponseInfo);

 int decode_res=0;
 if(CommandRequestDecoder)
 {
  decode_res=CommandRequestDecoder(UHttpServerFrame->ParsedRequestArgs, DecodedRequest);
 }
 else
 {
  AResponseInfo->ResponseNo=404;
  AResponseInfo->ResponseText="Request decode fail";
  AResponseInfo->ContentText="Request decode fail";
  return;
 }

 std::map<std::string,std::vector<char> >::iterator I;

 int engine_index=0;
 I=DecodedRequest.find("Cmd");
 if(I != DecodedRequest.end())
 {
  TStream* file=0;
  if(I->second.size()>0)
   Command.assign(&I->second[0],I->second.size());
 }
 else
  Command.clear();

 I=DecodedRequest.find("Channel");
 if(I != DecodedRequest.end())
 {
  TStream* file=0;
  if(I->second.size()>0)
   ChannelIndex.assign(&I->second[0],I->second.size());
 }
 else
  ChannelIndex="0";

 switch(Mode)
 {
 case 0:
  ProcessControlCommand(Command, DecodedRequest, ResponseType, Response);
 break;

 case 1:
  ProcessRPCCommand(atoi(ChannelIndex), Command, DecodedRequest, ResponseType, Response);
 break;
 }

 int encode_res=0;
 if(CommandResponseEncoder)
 {
  encode_res=CommandResponseEncoder(ResponseType, Response, EncodedResponse);
 }
 else
 {
  AResponseInfo->ResponseNo=404;
  AResponseInfo->ResponseText="Response encode fail";
  AResponseInfo->ContentText="Response encode fail";
  return;
 }

 TMemoryStream *DataStream=new TMemoryStream;

 if(EncodedResponse.size()>0)
  DataStream->Write(&EncodedResponse[0],EncodedResponse.size());
 DataStream->Position=0;

 AResponseInfo->ContentType=ResponseType.c_str();
 AResponseInfo->ContentStream=DataStream;
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::FormCreate(TObject *Sender)
{
 MemStream=new TMemoryStream;
 Bitmap=new Graphics::TBitmap;
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::FormDestroy(TObject *Sender)
{
 if(MemStream)
  delete MemStream;

 if(Bitmap)
  delete Bitmap;
}
//---------------------------------------------------------------------------
