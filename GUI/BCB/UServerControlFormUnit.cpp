//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UServerControlFormUnit.h"
#include "UGEngineControlFormUnit.h"
#ifdef RDK_VIDEO
#include "TVideoGrabberControlFormUnit.h"
#endif
#include "UImagesFormUnit.h"
#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#ifdef RDK_VIDEO
#include "VideoOutputFormUnit.h"
#endif
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
bool TUServerControlForm::ProcessControlCommand(const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data)
{
 std::map<std::string,std::vector<char> >::const_iterator I;
 int res=0;
 std::string response,temp;
 std::string name;

 response_type="text/plain";
 if(cmd_name == "SetNumChannels")
 {
  int num_engines=0;
  res=DecodeParamAsInteger("Num",args,num_engines);
  if(res == 0)
  {
   res=SetNumChannels(num_engines);
  }

  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "GetNumChannels")
 {
  res=GetNumChannels();
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "GetChannelVideoSource")
 {
  int channel_id=0;
  res=DecodeParamAsInteger("Channel",args,channel_id);
  if(res == 0)
  {
   res=GetChannelVideoSource(channel_id);
  }
  else
   res=-1;

  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "SetChannelVideoSource")
 {
  int channel_id=0, mode=-1;
  res=0;
  res|=DecodeParamAsInteger("Channel",args,channel_id);
  res|=DecodeParamAsInteger("Mode",args,mode);
  if(res == 0)
  {
   res=SetChannelVideoSource(channel_id,mode);
  }

  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "ResetChannel")
 {
  int channel_id=0;
  res=0;
  res|=DecodeParamAsInteger("Channel",args,channel_id);
  if(res == 0)
  {
   res=ResetChannel(channel_id);
  }

  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "StartChannel")
 {
  int channel_id=0;
  res=0;
  res|=DecodeParamAsInteger("Channel",args,channel_id);
  if(res == 0)
  {
   res=StartChannel(channel_id);
  }

  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "StopChannel")
 {
  int channel_id=0;
  res=0;
  res|=DecodeParamAsInteger("Channel",args,channel_id);
  if(res == 0)
  {
   res=StopChannel(channel_id);
  }

  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "LoadProject")
 {
  int channel_id=0;
  std::string file_name;
  res=0;
  res|=DecodeParamAsInteger("Channel",args,channel_id);
  ConvertVectorToString(args["FileName"],file_name);
  if(res == 0 && !file_name.empty())
  {
   res=LoadProject(channel_id,file_name);
  }

  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }

 return false;
}

// Функция, обрабатывающая команды удаленного вызова процедур
bool TUServerControlForm::ProcessRPCCommand(int channel, const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data)
{
 std::map<std::string,std::vector<char> >::const_iterator I;
 int res=0;
 std::string response,temp;
 std::string name;

 response_type="text/plain";
 if(cmd_name == "EngineInit")
 {
  int predefined_structure=0;
  res=DecodeParamAsInteger("PredefinedStructure",args,predefined_structure);

  res=EngineInit(predefined_structure);
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "EngineUnInit")
 {
  res=EngineUnInit();
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "Env_Reset")
 {
  ConvertVectorToString(args["Name"], name);
  res=Env_Reset(name.c_str());
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "Env_Calculate")
 {
  ConvertVectorToString(args["Name"], name);
  res=Env_Calculate(name.c_str());
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "Model_Create")
 {
  I=args.find("Class");
  if(I != args.end())
  {
   ConvertVectorToString(I->second, temp);
   res=Model_Create(temp.c_str());
  }
  else
   res=100;
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "Model_LoadComponent")
 {
  ConvertVectorToString(args["Name"], name);
  ConvertVectorToString(args["Xml"], temp);

  res=Model_LoadComponent(name.c_str(),temp.c_str());
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "Model_LoadComponentParameters")
 {
  ConvertVectorToString(args["Name"], name);
  ConvertVectorToString(args["Xml"], temp);

  res=Model_LoadComponentParameters(name.c_str(),temp.c_str());
  ntoa(res,response);
  ConvertStringToVector(response, response_data);
  return true;
 }
 else
 if(cmd_name == "Model_GetComponentParameters")
 {
  ConvertVectorToString(args["Name"], name);
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
  return true;
 }
 else
 if(cmd_name == "Model_GetComponentStates")
 {
  ConvertVectorToString(args["Name"], name);
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
  return true;
 }
 else
 if(cmd_name == "Model_GetComponentBitmapOutput")
 {
  ConvertVectorToString(args["Name"], name);
  int output_number=0;
  res=DecodeParamAsInteger("Index",args,output_number);

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
  return true;
 }
 else
 if(cmd_name == "Model_GetComponentBitmapInput")
 {
  ConvertVectorToString(args["Name"], name);
  int output_number=0;
  res=DecodeParamAsInteger("Index",args,output_number);

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
  return true;
 }
 else
 if(cmd_name == "Model_SetComponentBitmapOutput")
 {
  ConvertVectorToString(args["Name"], name);
  int output_number=0;
  res=DecodeParamAsInteger("Index",args,output_number);

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

   Model_SetComponentBitmapOutput(name.c_str(),output_number,&TempUBitmap);
   ConvertStringToVector("0", response_data);
  }
  return true;
 }

 return false;
}


/// Кодирует строку в вектор
void TUServerControlForm::ConvertStringToVector(const std::string &source, std::vector<char> &dest)
{
 dest.resize(source.size());
 if(source.size()>0)
  memcpy(&dest[0],source.c_str(),source.size());
}

/// Кодирует вектор в строку
void TUServerControlForm::ConvertVectorToString(const std::vector<char> &source, std::string &dest)
{
 dest.resize(source.size());
 if(source.size()>0)
  memcpy(&dest[0],&source[0],source.size());
}


/// Декодирует параметр массива команды с именем 'param_name' в целое число
/// и записывает его в value
/// Возвращает 0 в случае успеха
int TUServerControlForm::DecodeParamAsInteger(const std::string &param_name, const std::map<std::string,std::vector<char> > &args, int &value)
{
 std::map<std::string,std::vector<char> >::const_iterator I;
 I=args.find(param_name);
 if(I != args.end() && I->second.size()>0)
 {
  std::string temp;
  temp.assign(&I->second[0],I->second.size());
  value=atoi(temp);
  return 0;
 }
 return 1;
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
 ServerControlPortLabeledEdit->Text=IntToStr(UHttpServerFrame->GetListenPort());
 NumberOfChannelsLabeledEdit->Text=IntToStr(GetNumChannels());

 ChannelNamesStringGrid->RowCount=ChannelNames.size()+1;
 ChannelNamesStringGrid->ColWidths[0]=20;
 ChannelNamesStringGrid->ColWidths[1]=ChannelNamesStringGrid->Width-ChannelNamesStringGrid->ColWidths[0]-20;
 ChannelNamesStringGrid->Cells[0][0]="Channel #";
 ChannelNamesStringGrid->Cells[1][0]="Channel Name";
 for(int i=0;i<ChannelNames.size();i++)
 {
  ChannelNamesStringGrid->Cells[0][i+1]=IntToStr(i);
  ChannelNamesStringGrid->Cells[1][i+1]=ChannelNames[i].c_str();
 }
}

// Сохраняет параметры интерфейса в xml
void TUServerControlForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("ServerControlPort", UHttpServerFrame->GetListenPort());
 xml.WriteInteger("NumberOfChannels",GetNumChannels());
 xml.WriteInteger("AutoStartFlag",AutoStartFlag);
 for(size_t i=0;i<ChannelNames.size();i++)
 {
  xml.WriteString(std::string("ChannelName_")+RDK::sntoa(i),ChannelNames[i]);
 }
}

// Загружает параметры интерфейса из xml
void TUServerControlForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 UHttpServerFrame->SetListenPort(xml.ReadInteger("ServerControlPort",80));
 SetNumChannels(xml.ReadInteger("NumberOfChannels",1));
 for(size_t i=0;i<ChannelNames.size();i++)
 {
  SetChannelName(i,xml.ReadString(std::string("ChannelName_")+RDK::sntoa(i),RDK::sntoa(i)));
 }

 AutoStartFlag=xml.ReadInteger("AutoStartFlag",true);

 if(AutoStartFlag)
  ServerStartButtonClick(this);
}
// -----------------------------


// -----------------------------
// Обработчики команд сервера
// -----------------------------
/// Возвращает число каналов
int TUServerControlForm::GetNumChannels(void) const
{
 return ChannelNames.size();
}

/// Устанавливает число каналов
/// также выставляет число источников видео
int TUServerControlForm::SetNumChannels(int value)
{
 // Здесь установка числа каналов
 int num=GetNumChannels();
 if(num == value)
  return 0;

 if(value<=0)
  return 1;

 if(VideoOutputForm->GetNumSources()<value)
 {
  for(int i=VideoOutputForm->GetNumSources();i<value;i++)
  {
   VideoOutputForm->AddSource();
   VideoOutputForm->GetVideoOutputFrame(i)->MyVideoGrabberControlForm->VideoGrabberControlFrame->PipeUidEdit->Text=(std::string("USharedMemory")+RDK::sntoa(i)).c_str();
   VideoOutputForm->GetVideoOutputFrame(i)->MyVideoGrabberControlForm->VideoGrabberControlFrame->PipeIndexEdit->Text=IntToStr(i);
   VideoOutputForm->GetVideoOutputFrame(i)->PipeIndex=i;
  }
  VideoOutputForm->UpdateInterface();
 }
 else
 {
  while(VideoOutputForm->GetNumSources()>value)
  {
   VideoOutputForm->DelSource(VideoOutputForm->GetNumSources()-1);
  }
 }

 ChannelNames.resize(value);
 for(size_t i=0;i<ChannelNames.size();i++)
 {
  if(ChannelNames[i].empty())
   SetChannelName(i,RDK::sntoa(i));
 }


 return 0;
}

/// Возвращает тип источника видео для канала
/// в соответствии с режимами VideoOutputFrame
int TUServerControlForm::GetChannelVideoSource(int channel_id)
{
 TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_id);
 if(!frame)
  return -1;

 return frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GetMode();
}

/// Задает источник видео для канала
/// в соответствии с режимами VideoOutputFrame
int TUServerControlForm::SetChannelVideoSource(int channel_id, int source_mode)
{
 int num=GetNumChannels();
 if(channel_id<0)
 {
  for(int i=0;i<num;i++)
  {
   TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(i);
   if(!frame)
	return 1;

   frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(source_mode);
  }
 }
 else
 {
  TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_id);
  if(!frame)
   return 1;

  frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(source_mode);
 }
 return 0;
}


/// Возвращает имя канала
const std::string TUServerControlForm::GetChannelName(int channel)
{
 if(channel<0 || channel>=int(ChannelNames.size()))
  return std::string("");

 return ChannelNames[channel];
}

/// Устанавливает имя канала
bool TUServerControlForm::SetChannelName(int channel, const std::string& name)
{
 if(channel<0 || channel>=int(ChannelNames.size()))
  return false;

 if(ChannelNames[channel] == name)
  return true;

 if(find(ChannelNames.begin(),ChannelNames.end(),name) != ChannelNames.end())
  return false;

 ChannelNames[channel]=name;
 return true;
}


/// Сбрасывает аналитику выбранного канала в исходное состояние
/// или всех каналов, если channel_id<0
int TUServerControlForm::ResetChannel(int channel_id)
{
 UEngineMonitorForm->EngineMonitorFrame->Reset1Click(this);
 return 0;
}

/// Запускает выбранный канал
/// или все каналы, если channel_id<0
int TUServerControlForm::StartChannel(int channel_id)
{
 UEngineMonitorForm->EngineMonitorFrame->Start1Click(this);
 return 0;
}

/// Останавливает выбранный канал
/// или все каналы, если channel_id<0
int TUServerControlForm::StopChannel(int channel_id)
{
 UEngineMonitorForm->EngineMonitorFrame->Pause1Click(this);
 return 0;
}

/// Загружает проект аналитики для канала
/// или загружает проект для всех каналов, если channel_id<0
int TUServerControlForm::LoadProject(int channel_id, const std::string &project_file_name)
{
 UGEngineControlForm->OpenProject(project_file_name.c_str());
 return 0;
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

 bool is_processed=ProcessControlCommand(Command, DecodedRequest, ResponseType, Response);

 if(!is_processed)
  is_processed=ProcessRPCCommand(atoi(ChannelIndex), Command, DecodedRequest, ResponseType, Response);

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
 UGEngineControlForm->SpecialForms["TUServerControlForm"]=this;
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::FormDestroy(TObject *Sender)
{
// UGEngineControlForm->SpecialForms.erase("TUServerControlForm");

 if(MemStream)
  delete MemStream;

 if(Bitmap)
  delete Bitmap;
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ServerStartButtonClick(TObject *Sender)
{
 UHttpServerFrame->ServerListenOn();
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ServerStopButtonClick(TObject *Sender)
{
 UHttpServerFrame->ServerListenOff();
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ReturnOptionsButtonClick(TObject *Sender)
{
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::ApplyOptionsButtonClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 UHttpServerFrame->SetListenPort(StrToInt(ServerControlPortLabeledEdit->Text));

 SetNumChannels(StrToInt(NumberOfChannelsLabeledEdit->Text));

 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ChannelNamesStringGridKeyDown(TObject *Sender,
          WORD &Key, TShiftState Shift)
{
 if(UpdateInterfaceFlag)
  return;

 if(Key == VK_RETURN)
 {
  SetChannelName(ChannelNamesStringGrid->Row-1,AnsiString(ChannelNamesStringGrid->Cells[1][ChannelNamesStringGrid->Row]).c_str());
  UpdateInterface(true);
 }
}
//---------------------------------------------------------------------------
