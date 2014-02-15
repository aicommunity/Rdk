//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "../../Deploy/PtzLib/ptzlib_initdll.h"
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
#include "TIdTcpResultBroadcasterFormUnit.h"
#include "rdk_cpp_initdll.h"
#include "rdk_rpc.cpp"
#include "TUBitmap.h"
//#include "../../Core/Graphics/Libraries/Hardware/PtzRpc.cpp"
#ifdef DVA
#include "TGeViScopeResultBroadcasterFormUnit.h"
#endif
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUHttpServerUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUServerControlForm *UServerControlForm;

/// ����������� �������, �������������� ������������� ���������� �������
int StandardCommandRequestDecoder(UServerCommand &source, UServerCommand &dest)
{
 dest=source;
 return 0;
}

/// ����������� �������, �������������� ����������� ���������� ������
int StandardCommandResponseEncoder(const std::string &response_type, UParamT &source, UParamT &dest)
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
 AverageIterations=4;

 Clients=0;
// Clients=new TThreadList;

 CriticalSection=new TCriticalSection;
}

__fastcall TUServerControlForm::~TUServerControlForm(void)
{
 delete CriticalSection;
/* if(Clients)
 {

  delete Clients;
 }*/
}


const char* TUServerControlForm::ControlRemoteCall(const char *request, int &return_value)
{
 return_value=2001;

 RDK::USerStorageXML xml,xml_data;

 xml.Load(request,"RpcRequest");

 int engine_index=-1;
 std::string cmd;

 ExtractChannel(xml,engine_index);
 if(!ExtractCmd(xml,cmd) || cmd.empty())
 {
  return_value=2000;
  return 0;
 }

 ControlResponseString.clear();
 RDK::USerStorageXML result;

 result.Create("RpcResponse");
 result.WriteString("Id", xml.ReadString("Id",""));
 if(cmd == "RpcPing")
 {
  return_value=0;
 }
 else
 if(cmd == "SetNumChannels")
 {
  int num_engines=xml.ReadInteger("NumChannels",GetNumEngines());
  UEngineMonitorForm->EngineMonitorFrame->SetNumChannels(num_engines);
  return_value=SetNumChannels(num_engines);
 }
 else
 if(cmd == "GetNumChannels")
 {
  ControlResponseString=RDK::sntoa(GetNumChannels());
  return_value=0;
 }
 else
 if(cmd == "GetServerName")
 {
  ControlResponseString=ServerName;
  return_value=0;
 }
 else
 if(cmd == "GetServerId")
 {
  ControlResponseString=ServerId;
  return_value=0;
 }
 else
 if(cmd == "GetChannelName")
 {
  ControlResponseString=GetChannelName(engine_index);
  return_value=0;
 }
 else
 if(cmd == "SetChannelName")
 {
  std::string ch_name=xml.ReadString("Name","");
  if(!ch_name.empty())
   if(SetChannelName(engine_index,ch_name))
	return_value=0;
   else
	return_value=5005;
 }
 else
 if(cmd == "GetChannelVideoSource")
 {
#ifdef RDK_VIDEO
 TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(engine_index);
 if(!frame)
  return_value=1;

  result.WriteString("Mode",RDK::sntoa(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GetMode()));
  result.SelectNodeForce("Data");
  result.WriteString("Mode",RDK::sntoa(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GetMode()));
  if(GetChannelVideoSource(engine_index) == 3)
  {
   result.WriteString("IPCameraUrl",AnsiString(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->IPCameraUrlEdit->Text).c_str());
   result.WriteString("IPCameraUsername",AnsiString(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->IPCameraUserNameEdit->Text).c_str());
   result.WriteString("IPCameraPassword",AnsiString(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->IPCameraUserPasswordEdit->Text).c_str());
  }
#ifdef DVA
  else
  if(GetChannelVideoSource(engine_index) == 7)
  {
   result.WriteString("ServerUrl",AnsiString(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViCameraUrlEdit->Text).c_str());
   result.WriteString("Username",AnsiString(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViCameraUserNameEdit->Text).c_str());
   result.WriteString("Password",AnsiString(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViCameraUserPasswordEdit->Text).c_str());
   result.WriteString("MediaChannel",AnsiString(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViCameraMediaChannelEdit->Text).c_str());
   result.WriteString("Fps",AnsiString(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViFPSLabeledEdit->Text).c_str());
   result.SelectNodeForce("GeViAvaliableMediaChannels");
   std::vector<int> media_channels;
   media_channels.resize(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViComboBox->Items->Count);
   for(int i=0;i<frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViComboBox->Items->Count;i++)
	media_channels[i]=StrToInt(frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViComboBox->Items->Strings[i]);
   result<<media_channels;
   result.SelectUp();
  }
#endif
  result.SelectUp();
  return_value=0;
#else
  return_value=1;
#endif
 }
 else
 if(cmd == "SetChannelVideoSource")
 {
#ifdef RDK_VIDEO
  TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(engine_index);
  if(!frame)
   return_value=1;
  int mode=xml.ReadInteger("Mode",5);
  if(!xml.SelectNode("Data"))
  {
   return_value=1;
  }
  else
  {
   mode=xml.ReadInteger("Mode",mode);
#ifdef DVA
   if(mode == 7)
   {
	frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViCameraUrlEdit->Text=xml.ReadString("ServerUrl","").c_str();
	frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViCameraUserNameEdit->Text=xml.ReadString("Username","").c_str();
	frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViCameraUserPasswordEdit->Text=xml.ReadString("Password","").c_str();
	frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViCameraMediaChannelEdit->Text=xml.ReadString("MediaChannel","").c_str();
	frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GeViFPSLabeledEdit->Text=xml.ReadString("Fps","").c_str();
   }
#endif

   xml.SelectUp();
   return_value=SetChannelVideoSource(engine_index,mode);
  }
#endif
 }
 else
 if(cmd == "GetChannelBroacaster")
 {
  int type=xml.ReadInteger("BroadcasterType",0);
  result.SelectNodeForce("Data");
#ifdef DVA
  if(type == 1000)
  {
	TGeViScopeResultBroadcasterFrame *frame=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(engine_index);
	if(frame)
	{
	 result.WriteString("ChannelIndex",AnsiString(frame->ChannelIndexLabeledEdit->Text).c_str());
	 result.WriteString("ServerUrl",AnsiString(frame->ServerAddressLabeledEdit->Text).c_str());
	 result.WriteString("Username",AnsiString(frame->UsernameLabeledEdit->Text).c_str());
	 result.WriteString("Password",AnsiString(frame->PasswordLabeledEdit->Text).c_str());
	 result.WriteString("MediaChannel",AnsiString(frame->MediaChannelLabeledEdit->Text).c_str());
	 result.WriteString("XmlComponentName",AnsiString(frame->XmlComponentNameLabeledEdit->Text).c_str());
	 result.WriteString("XmlComponentStateName",AnsiString(frame->XmlComponentStateNameLabeledEdit->Text).c_str());
	 result.WriteBool("EnableXmlTranslationFlag",frame->EnableXmlTranslationCheckBox->Checked);
	}
  }
#endif
  result.SelectUp();
  return_value=0;
 }
 else
 if(cmd == "SetChannelBroadcaster")
 {
  int type=xml.ReadInteger("BroadcasterType",0);
  result.WriteInteger("BroadcasterType",type);
  if(!xml.SelectNode("Data"))
  {
   return_value=1;
  }
  else
  {
#ifdef DVA
  if(type == 1000)
  {
	TGeViScopeResultBroadcasterFrame *frame=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(engine_index);
	if(frame)
	{
	 frame->DisconnectButtonClick(this);
	 frame->ChannelIndexLabeledEdit->Text=xml.ReadString("ChannelIndex","0").c_str();
	 frame->ServerAddressLabeledEdit->Text=xml.ReadString("ServerUrl","").c_str();
	 frame->UsernameLabeledEdit->Text=xml.ReadString("Username","").c_str();
	 frame->PasswordLabeledEdit->Text=xml.ReadString("Password","").c_str();
	 frame->MediaChannelLabeledEdit->Text=xml.ReadString("MediaChannel","0").c_str();
	 frame->XmlComponentNameLabeledEdit->Text=xml.ReadString("XmlComponentName","").c_str();
	 frame->XmlComponentStateNameLabeledEdit->Text=xml.ReadString("XmlComponentStateName","").c_str();
	 frame->EnableXmlTranslationCheckBox->Checked=xml.ReadBool("EnableXmlTranslationFlag",true);
	 frame->ConnectButtonClick(this);
	}
  }
#endif
   result.SelectUp();
  }
  return_value=0;
 }
 else
 if(cmd == "ResetChannel")
 {
  return_value=ResetChannel(engine_index);
 }
 else
 if(cmd == "StartChannel")
 {
  return_value=StartChannel(engine_index);
 }
 else
 if(cmd == "StopChannel")
 {
  return_value=StopChannel(engine_index);
 }
 else
 if(cmd == "RegisterMetadataReceiver")
 {
  string address=xml.ReadString("Address","");
  int port=xml.ReadInteger("Port",8888);
  return_value=RegisterMetadataReceiver(address, port,
		MetaComponentName, MetaComponentStateName);
 }
 else
 if(cmd == "UnRegisterMetadataReceiver")
 {
  string address=xml.ReadString("Address","");
  int port=xml.ReadInteger("Port",port);
  return_value=UnRegisterMetadataReceiver(address, port);
 }
 else
 if(cmd == "LoadProject")
 {
  std::string file_name=xml.ReadString("FileName","");
  if(!file_name.empty())
  {
   return_value=LoadProject(engine_index,file_name);
  }
 }
 else
 if(cmd == "ReadImageFromVideoSource")
 {
#ifdef RDK_VIDEO
  int video_source_index=engine_index;
  TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(engine_index);
  if(!frame)
   return_value=2;

  return_value=0;
#else
   return_value=1;
#endif
 }
 else
  return_value=2001;

 result.WriteString("Data",ControlResponseString);
 result.WriteInteger("Res",return_value);
 result.Save(ControlResponseString);

 return ControlResponseString.c_str();
}

// �������, �������������� ������� ���������� ��������
bool TUServerControlForm::ProcessControlCommand(const UServerCommand &args, std::string &response_type, UParamT &response_data)
{
// UServerCommand::const_iterator I;
 std::string request;

 response_type="text/plain";
/*
 I=args.find("Request");
 if(I == args.end())
 {
  ConvertStringToVector("RPC: Request not found", response_data);
  return true;
 }
  */
 ConvertVectorToString(args.second, request);
 int response_status=0;
 const char* response=ControlRemoteCall(request.c_str(), response_status);

 if(response_status == 2001)
  return false;

 if(response)
  ConvertStringToVector(response, response_data);
 else
  ConvertStringToVector(RDK::sntoa(response_status), response_data);

 return true;
}

bool TUServerControlForm::ProcessRPCCommand(const UServerCommand &args, std::string &response_type, UParamT &response_data)
{
 std::string request;

 response_type="text/plain";

 ConvertVectorToString(args.second, request);
 int response_status=0;
 const char* response=RemoteCall(request.c_str(), response_status);

 if(response_status == 2001)
  return false;

 if(response)
  ConvertStringToVector(response, response_data);
 else
  ConvertStringToVector(RDK::sntoa(response_status), response_data);

 return true;
}

bool TUServerControlForm::ProcessPtzCommand(const UServerCommand &args, std::string &response_type, UParamT &response_data)
{
// UServerCommand::const_iterator I;
 std::string request;

 response_type="text/plain";
				/*
 I=args.find("Request");
 if(I == args.end())
 {
  ConvertStringToVector("RPC: Request not found", response_data);
  return true;
 }

 ConvertVectorToString(I->second, request);*/
 ConvertVectorToString(args.second, request);
 int response_status=0;
 const char* response=PtzRemoteCall(request.c_str(), response_status);

 if(response_status == 2001)
  return false;

 if(response)
  ConvertStringToVector(response, response_data);
 else
  ConvertStringToVector(RDK::sntoa(response_status), response_data);

 return true;
}


/// �������� ������ � ������
void TUServerControlForm::ConvertStringToVector(const std::string &source, UParamT &dest)
{
 dest.resize(source.size());
 if(source.size()>0)
  memcpy(&dest[0],source.c_str(),source.size());
}

/// �������� ������ � ������
void TUServerControlForm::ConvertVectorToString(const UParamT &source, std::string &dest)
{
 dest.resize(source.size());
 if(source.size()>0)
  memcpy(&dest[0],&source[0],source.size());
}


/// ���������� �������� ������� ������� � ������ 'param_name' � ����� �����
/// � ���������� ��� � value
/// ���������� 0 � ������ ������
/*int TUServerControlForm::DecodeParamAsInteger(const std::string &param_name, const UServerCommand &args, int &value)
{
 UServerCommand::const_iterator I;
 I=args.find(param_name);
 if(I != args.end() && I->second.size()>0)
 {
  std::string temp;
  temp.assign(&I->second[0],I->second.size());
  value=atoi(temp);
  return 0;
 }
 return 1;
} */


/// ���������� ����� �� �������
void TUServerControlForm::SendCommandResponse(const std::string &client_binding, UParamT &dest)
{
 UTransferPacket packet;

 packet.SetNumParams(1);
 packet.SetParamSize(0,dest.size());
 packet.SetParam(0,dest);
 RDK::UParamT buffer;
 packet.Save(buffer);

 if(IdTCPServer->Active && !dest.empty())
 {
  TByteDynArray arr;
  arr.set_length(buffer.size());
  memcpy(&arr[0],&buffer[0],buffer.size());
  try
  {
   TList *list=IdTCPServer->Contexts->LockList();
   std::string current_bind;
   for(int i=0;i<list->Count;i++)
   {
	TIdContext *context=static_cast<TIdContext*>(list->Items[i]);
	current_bind=AnsiString(context->Binding->PeerIP).c_str();
	current_bind+=":";
	current_bind+=RDK::sntoa(context->Binding->PeerPort);

	if(current_bind == client_binding)
	{
	 context->Connection->IOHandler->Write(arr, arr.get_length());
	 break;
	}
   }

   IdTCPServer->Contexts->UnlockList();
  }
  catch (...)
  {
   IdTCPServer->Contexts->UnlockList();
   throw;
  }
 }
}

/// ���������� ��������� �� ������ � ����� �� �������
/// 0 - ����������� ������
/// 1 - ������� �� ��������
void TUServerControlForm::SendCommandError(const std::string &client_binding, int request_id, int error_code)
{
 RDK::USerStorageXML result;

 result.Create("RpcResponse");
 result.WriteString("Id", sntoa(request_id));
 result.WriteString("Error",sntoa(error_code));
 result.Save(ControlResponseString);
 UParamT error_response;
 ConvertStringToVector(ControlResponseString, error_response);
 SendCommandResponse(client_binding, error_response);
}


// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
void TUServerControlForm::AAfterReset(void)
{
 PerformancePushIndex=0;
 if(AverageIterations<=0)
  AverageIterations=4;
 ModelPerformanceResults.resize(AverageIterations);
 TransportPerformanceResults.resize(AverageIterations);
 for(size_t i=0;i<ModelPerformanceResults.size();i++)
 {
  ModelPerformanceResults[i].clear();
  TransportPerformanceResults[i].clear();
 }
}

// �����, ���������� ����� ���� �������
void TUServerControlForm::AAfterCalculate(void)
{
 if(PerformancePushIndex>=int(ModelPerformanceResults.size()))
  PerformancePushIndex=0;

 if(ModelPerformanceResults.size() == 0)
  AAfterReset();
 ModelPerformanceResults[PerformancePushIndex].assign(GetNumChannels(),0);
 TransportPerformanceResults[PerformancePushIndex].assign(GetNumChannels(),0);
 for(size_t i=0;i<ModelPerformanceResults[PerformancePushIndex].size();i++)
 {
  if(!MIsEngineInit(i) || !MModel_Check(i))
   continue;
  long long model_time=MModel_GetFullStepDuration(i,"");
  long long ext_gui=MModel_GetInterstepsInterval(i,"");
  ModelPerformanceResults[PerformancePushIndex][i]=model_time;
  TransportPerformanceResults[PerformancePushIndex][i]=ext_gui;
 }
 ++PerformancePushIndex;
 if(PerformancePushIndex>=int(ModelPerformanceResults.size()))
  PerformancePushIndex=0;
}

// ���������� ����������
void TUServerControlForm::AUpdateInterface(void)
{
 std::vector<long long> model_avg,transport_avg;
 model_avg.assign(GetNumChannels(),0);
 transport_avg.assign(GetNumChannels(),0);
 int sum_number=0;
 for(size_t i=0;i<ModelPerformanceResults.size();i++)
 {
  for(size_t j=0;j<ModelPerformanceResults[i].size();j++)
  {
   model_avg[j]+=ModelPerformanceResults[i][j];
   transport_avg[j]+=TransportPerformanceResults[i][j];
  }
  if(ModelPerformanceResults[i].size()>0)
   ++sum_number;
 }

 if(sum_number>0)
  for(size_t j=0;j<model_avg.size();j++)
  {
   model_avg[j]/=sum_number;
   transport_avg[j]/=sum_number;
  }

 PerformanceChart->Series[0]->Clear();
 PerformanceChart->Series[1]->Clear();
 PerformanceChart->Series[2]->Clear();
 for(size_t i=0;i<model_avg.size();i++)
 {
  PerformanceChart->Series[0]->AddY(model_avg[i]);
  PerformanceChart->Series[1]->AddY(transport_avg[i]);
  PerformanceChart->Series[2]->AddY(model_avg[i]+transport_avg[i]);
 }


 ChannelNamesStringGrid->RowCount=ChannelNames.size()+1;
 if(ChannelNamesStringGrid->RowCount>1)
  ChannelNamesStringGrid->FixedRows=1;

 ChannelNamesStringGrid->Cells[0][0]="#";
 ChannelNamesStringGrid->Cells[1][0]="Channel Name";
 ChannelNamesStringGrid->ColWidths[0]=25;
 ChannelNamesStringGrid->ColWidths[1]=ChannelNamesStringGrid->Width-ChannelNamesStringGrid->ColWidths[0]-25;
 for(int i=0;i<ChannelNames.size();i++)
 {
  ChannelNamesStringGrid->Cells[0][i+1]=StrToInt(i);
  ChannelNamesStringGrid->Cells[1][i+1]=ChannelNames[i].c_str();
 }

 NumberOfChannelsLabeledEdit->Text=IntToStr(GetNumEngines());
 ServerNameLabeledEdit->Text=ServerName.c_str();
 ServerIdLabeledEdit->Text=ServerId.c_str();
 ServerControlPortLabeledEdit->Text=IdTCPServer->Bindings->Items[0]->Port;
 BindingAddressLabeledEdit->Text=IdTCPServer->Bindings->Items[0]->IP;

 MetadataComponentNameLabeledEdit->Text=MetaComponentName.c_str();
 MetadataComponentStateNameLabeledEdit->Text=MetaComponentStateName.c_str();
}

// ������� ���������� � �������� ���������
void TUServerControlForm::AClearInterface(void)
{
 ServerStopButtonClick(this);
}


// ��������� ��������� ���������� � xml
void TUServerControlForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("AverageIterations",AverageIterations);
// xml.WriteInteger("ServerControlPort", UHttpServerFrame->GetListenPort());
// xml.WriteInteger("ServerControlPort", StrToInt(TcpServer->LocalPort));
 xml.WriteInteger("ServerControlPort", StrToInt(IdTCPServer->Bindings->Items[0]->Port));
 xml.WriteString("ServerControlAddress", AnsiString(IdTCPServer->Bindings->Items[0]->IP).c_str());

 xml.WriteInteger("NumberOfChannels",GetNumChannels());
 xml.WriteInteger("AutoStartFlag",AutoStartFlag);
 xml.WriteString("ServerName",ServerName);
 xml.WriteString("ServerId",ServerId);

 xml.WriteString("MetadataComponentName",MetaComponentName);
 xml.WriteString("MetadataComponentStateName",MetaComponentStateName);

 for(size_t i=0;i<ChannelNames.size();i++)
 {
  xml.WriteString(std::string("ChannelName_")+RDK::sntoa(i),ChannelNames[i]);
 }
}

// ��������� ��������� ���������� �� xml
void TUServerControlForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 AverageIterations=xml.ReadInteger("AverageIterations",AverageIterations);
// UHttpServerFrame->SetListenPort(xml.ReadInteger("ServerControlPort",80));
// TcpServer->LocalPort=xml.ReadInteger("ServerControlPort",80);
 IdTCPServer->Bindings->Items[0]->Port=xml.ReadInteger("ServerControlPort",80);
 IdTCPServer->Bindings->Items[0]->IP=xml.ReadString("ServerControlAddress", "127.0.0.1").c_str();

 MetaComponentName=xml.ReadString("MetadataComponentName","");
 MetaComponentStateName=xml.ReadString("MetadataComponentStateName","");

 SetNumChannels(GetNumEngines());
 for(size_t i=0;i<ChannelNames.size();i++)
 {
  SetChannelName(i,xml.ReadString(std::string("ChannelName_")+RDK::sntoa(i),RDK::sntoa(i)));
 }

 AutoStartFlag=xml.ReadInteger("AutoStartFlag",true);
 ServerName=xml.ReadString("ServerName","Server");
 ServerId=xml.ReadString("ServerId","Server");

 UpdateInterface(true);

 if(AutoStartFlag)
  ServerStartButtonClick(this);
}
// -----------------------------


// -----------------------------
// ����������� ������ �������
// -----------------------------
/// ���������� ����� �������
int TUServerControlForm::GetNumChannels(void) const
{
 return GetNumEngines();
}

/// ������������� ����� �������
/// ����� ���������� ����� ���������� �����
int TUServerControlForm::SetNumChannels(int value)
{
 // ����� ��������� ����� �������
// int num=GetNumEngines();
// if(num == value)
//  return 0;

 if(value<=0)
  return 1;

// int selected=GetSelectedEngineIndex();
 for(int i=0;i<value;i++)
 {
  if(GetNumEngines()<=i)
   break;

  if(!MIsEngineInit(i) || !MModel_Check(i))
  {
   UGEngineControlForm->CloneProject(0, i);
  }
 }

#ifdef RDK_VIDEO
 if(UGEngineControlForm->ProjectMode == 1)
 {
  if(VideoOutputForm->GetNumSources()<value)
  {
   for(int i=VideoOutputForm->GetNumSources();i<value;i++)
   {
	VideoOutputForm->AddSource();
	VideoOutputForm->GetVideoOutputFrame(i)->MyVideoGrabberControlForm->VideoGrabberControlFrame->PipeUidEdit->Text=(std::string("USharedMemory")+RDK::sntoa(i)).c_str();
	VideoOutputForm->GetVideoOutputFrame(i)->MyVideoGrabberControlForm->VideoGrabberControlFrame->PipeIndexEdit->Text=IntToStr(i);
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

  if(IdTcpResultBroadcasterForm->GetNumBroadcasters()<value)
  {
   for(int i=IdTcpResultBroadcasterForm->GetNumBroadcasters();i<value;i++)
   {
	IdTcpResultBroadcasterForm->AddBroadcaster();

	TIdTcpResultBroadcasterFrame *frame=IdTcpResultBroadcasterForm->GetBroadcasterFrame(i);
	if(frame)
	{
	 frame->ChannelIndexLabeledEdit->Text=IntToStr(i);
	 frame->ServerAddressLabeledEdit->Text=IdTcpResultBroadcasterForm->GetBroadcasterFrame(0)->ServerAddressLabeledEdit->Text;
	 frame->XmlComponentNameLabeledEdit->Text=IdTcpResultBroadcasterForm->GetBroadcasterFrame(0)->XmlComponentNameLabeledEdit->Text;
	 frame->XmlComponentStateNameLabeledEdit->Text=IdTcpResultBroadcasterForm->GetBroadcasterFrame(0)->XmlComponentStateNameLabeledEdit->Text;
	 frame->EnableXmlTranslationCheckBox->Checked=IdTcpResultBroadcasterForm->GetBroadcasterFrame(0)->EnableXmlTranslationCheckBox->Checked;
	}
   }
   IdTcpResultBroadcasterForm->UpdateInterface();
  }
  else
  {
   while(IdTcpResultBroadcasterForm->GetNumBroadcasters()>value)
   {
	IdTcpResultBroadcasterForm->DelBroadcaster(IdTcpResultBroadcasterForm->GetNumBroadcasters()-1);
   }
  }
 }
#endif

#ifdef DVA
  if(GeViScopeResultBroadcasterForm->GetNumBroadcasters()<value)
  {
   for(int i=GeViScopeResultBroadcasterForm->GetNumBroadcasters();i<value;i++)
   {
	GeViScopeResultBroadcasterForm->AddBroadcaster();

	TGeViScopeResultBroadcasterFrame *frame=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(i);
	if(frame)
	{
	 frame->ChannelIndexLabeledEdit->Text=IntToStr(i);
	 frame->ServerAddressLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->ServerAddressLabeledEdit->Text;
	 frame->UsernameLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->UsernameLabeledEdit->Text;
	 frame->PasswordLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->PasswordLabeledEdit->Text;
	 frame->MediaChannelLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->MediaChannelLabeledEdit->Text;
	 frame->XmlComponentNameLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->XmlComponentNameLabeledEdit->Text;
	 frame->XmlComponentStateNameLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->XmlComponentStateNameLabeledEdit->Text;
	 frame->EnableXmlTranslationCheckBox->Checked=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->EnableXmlTranslationCheckBox->Checked;
	}
   }
   GeViScopeResultBroadcasterForm->UpdateInterface();
  }
  else
  {
   while(GeViScopeResultBroadcasterForm->GetNumBroadcasters()>value)
   {
	GeViScopeResultBroadcasterForm->DelBroadcaster(GeViScopeResultBroadcasterForm->GetNumBroadcasters()-1);
   }
  }
#endif

 ChannelNames.resize(value);
 for(size_t i=0;i<ChannelNames.size();i++)
 {
  if(ChannelNames[i].empty())
   SetChannelName(i,RDK::sntoa(i));
 }

 //UEngineMonitorForm->EngineMonitorFrame->SetNumChannels(value);

 AAfterReset();
 RDK::UIVisualControllerStorage::UpdateInterface();
 return 0;
}

/// ���������� ��� ��������� ����� ��� ������
/// � ������������ � �������� VideoOutputFrame
int TUServerControlForm::GetChannelVideoSource(int channel_id)
{
#ifdef RDK_VIDEO
 TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_id);
 if(!frame)
  return -1;

 return frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GetMode();
#else
 return -1;
#endif
}

/// ������ �������� ����� ��� ������
/// � ������������ � �������� VideoOutputFrame
int TUServerControlForm::SetChannelVideoSource(int channel_id, int source_mode)
{
#ifdef RDK_VIDEO
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
#else
 return 1000;
#endif
}


/// ���������� ��� ������
const std::string TUServerControlForm::GetChannelName(int channel)
{
 if(channel<0 || channel>=int(ChannelNames.size()))
  return std::string("");

 return ChannelNames[channel];
}

/// ������������� ��� ������
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


/// ���������� ��������� ���������� ������ � �������� ���������
/// ��� ���� �������, ���� channel_id<0
int TUServerControlForm::ResetChannel(int channel_id)
{
 UGEngineControlForm->Reset1Click(this);
 return 0;
}

/// ��������� ��������� �����
/// ��� ��� ������, ���� channel_id<0
int TUServerControlForm::StartChannel(int channel_id)
{
 UGEngineControlForm->Start1Click(this);
 return 0;
}

/// ������������� ��������� �����
/// ��� ��� ������, ���� channel_id<0
int TUServerControlForm::StopChannel(int channel_id)
{
 UGEngineControlForm->Pause1Click(this);
 return 0;
}


/// ������������ ��������� �������� ����������
int TUServerControlForm::RegisterMetadataReceiver(const std::string &address, int port,
		const std::string &component_name, const std::string &component_state)
{
 UnRegisterMetadataReceiver(address, port);
 TIdTcpResultBroadcasterFrame *broadcaster=IdTcpResultBroadcasterForm->FindBroadcasterFrame(address,port);
 if(broadcaster)
 {

 }
 else
 {
  IdTcpResultBroadcasterForm->AddBroadcaster();
  broadcaster=IdTcpResultBroadcasterForm->GetBroadcasterFrame(IdTcpResultBroadcasterForm->GetNumBroadcasters()-1);

  if(broadcaster)
  {
   string bind2=address;
   bind2+=":";
   bind2+=sntoa(port);
   broadcaster->ServerAddressLabeledEdit->Text=bind2.c_str();
   broadcaster->XmlComponentNameLabeledEdit->Text=component_name.c_str();//"Pipeline1.AggrPureOutput";
   broadcaster->XmlComponentStateNameLabeledEdit->Text=component_state.c_str();
   broadcaster->EnableXmlTranslationCheckBox->Checked=true;
   broadcaster->ConnectButtonClick(this);
   Engine_LogMessage(RDK_EX_INFO, (std::string("Metadata receiver registered: ")+bind2).c_str());
  }
 }

 return 0;
}

/// ������� ��������� �������� ����������
int TUServerControlForm::UnRegisterMetadataReceiver(const std::string &address, int port)
{
 TIdTcpResultBroadcasterFrame *broadcaster=IdTcpResultBroadcasterForm->FindBroadcasterFrame(address,port);
 if(broadcaster)
 {
  broadcaster->EnableXmlTranslationCheckBox->Checked=false;
 // broadcaster->ConnectButtonClick(this);
  int index=IdTcpResultBroadcasterForm->GetBroadcasterFrameIndex(broadcaster);
  if(index >= 0)
  {
   IdTcpResultBroadcasterForm->DelBroadcaster(index);
  }
  Engine_LogMessage(RDK_EX_INFO, (std::string("Metadata receiver unregistered: ")+address+string(":")+sntoa(port)).c_str());
 }

 return 0;
}

/// ��������� ������ ��������� ��� ������
/// ��� ��������� ������ ��� ���� �������, ���� channel_id<0
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
/* Mode=-1;

 if(ARequestInfo->Document != "/control.cgi")
 {
  AResponseInfo->ResponseNo=404;
  AResponseInfo->ResponseText="404 Not Found";
  AResponseInfo->ContentText="404 Not Found";
  return;
 }

 UHttpServerFrame->IdHTTPServerCommandGet(AContext, ARequestInfo, AResponseInfo);

 //int decode_res=0;
 if(CommandRequestDecoder)
 {
  CommandRequestDecoder(UHttpServerFrame->ParsedRequestArgs, DecodedRequest);
 }
 else
 {
  AResponseInfo->ResponseNo=404;
  AResponseInfo->ResponseText="Request decode fail";
  AResponseInfo->ContentText="Request decode fail";
  return;
 }

 if(WaitForSingleObject(CommandQueueUnlockEvent,1000) == WAIT_TIMEOUT)
 {
  AResponseInfo->ResponseNo=404;
  AResponseInfo->ResponseText="Request decode fail";
  AResponseInfo->ContentText="Request decode fail";
  return;
 }
 ResetEvent(CommandQueueUnlockEvent);
 CommandQueue.push_back(DecodedRequest);
 SetEvent(CommandQueueUnlockEvent);

 AResponseInfo->ContentText="Command added to queue";
 TMemoryStream *DataStream=new TMemoryStream;

 if(EncodedResponse.size()>0)
  DataStream->Write(&EncodedResponse[0],EncodedResponse.size());
 DataStream->Position=0;

 AResponseInfo->ContentType=ResponseType.c_str();
 AResponseInfo->ContentStream=DataStream;*/
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::FormCreate(TObject *Sender)
{
 CommandQueueUnlockEvent=CreateEvent(0,TRUE,TRUE,0);
 MemStream=new TMemoryStream;
 Bitmap=new Graphics::TBitmap;
 UGEngineControlForm->SpecialForms["TUServerControlForm"]=this;
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::FormDestroy(TObject *Sender)
{
 ServerStopButtonClick(Sender);
// UGEngineControlForm->SpecialForms.erase("TUServerControlForm");

 if(MemStream)
  delete MemStream;

 if(Bitmap)
  delete Bitmap;

 CloseHandle(CommandQueueUnlockEvent);
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ServerStartButtonClick(TObject *Sender)
{
 ServerRestartTimer->Enabled=true;
 try
 {
  //UHttpServerFrame->ServerListenOn();
  IdTCPServer->Active=true;
 }
 catch(EIdSocketError &ex)
 {
  Engine_LogMessage(RDK_EX_ERROR, AnsiString(ex.ToString()).c_str());
 }
 catch(EIdCouldNotBindSocket &ex)
 {
  Engine_LogMessage(RDK_EX_ERROR, AnsiString(ex.ToString()).c_str());
 }

// TcpServer->Active=true;
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ServerStopButtonClick(TObject *Sender)
{
 ServerRestartTimer->Enabled=false;

 TList *list=IdTCPServer->Contexts->LockList();
 for(int i=0;i<list->Count;i++)
 {
	TIdContext *context=static_cast<TIdContext*>(list->Items[i]);
	context->Connection->Disconnect();
 }

   IdTCPServer->Contexts->UnlockList();

 IdTCPServer->Active=false;
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ReturnOptionsButtonClick(TObject *Sender)
{
 UpdateInterface(true);
 PageControlChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::ApplyOptionsButtonClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 int new_num_channels=StrToInt(NumberOfChannelsLabeledEdit->Text);
 if(new_num_channels < 1)
  return;

 if(new_num_channels != GetNumEngines())
 {
  UGEngineControlForm->Pause1Click(Sender);

  UEngineMonitorForm->EngineMonitorFrame->SetNumChannels(new_num_channels);
  SetNumChannels(GetNumEngines());
 }

// UHttpServerFrame->SetListenPort(StrToInt(ServerControlPortLabeledEdit->Text));

 int new_port=StrToInt(ServerControlPortLabeledEdit->Text);
 String new_address=BindingAddressLabeledEdit->Text;
 if(StrToInt(IdTCPServer->Bindings->Items[0]->Port) != new_port || IdTCPServer->Bindings->Items[0]->IP != new_address)
 {
  IdTCPServer->Active=false;
  IdTCPServer->Bindings->Items[0]->Port=new_port;
  IdTCPServer->Bindings->Items[0]->IP=new_address;
  if(AutoStartFlag)
   ServerStartButtonClick(Sender);
 }

 ServerName=AnsiString(ServerNameLabeledEdit->Text).c_str();
 ServerId=AnsiString(ServerIdLabeledEdit->Text).c_str();
 MetaComponentName=AnsiString(MetadataComponentNameLabeledEdit->Text).c_str();
 MetaComponentStateName=AnsiString(MetadataComponentStateNameLabeledEdit->Text).c_str();

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
void __fastcall TUServerControlForm::PageControlChange(TObject *Sender)
{
// ServerControlPortLabeledEdit->Text=IntToStr(UHttpServerFrame->GetListenPort());
 ServerControlPortLabeledEdit->Text=IdTCPServer->Bindings->Items[0]->Port;//TcpServer->LocalPort;
 NumberOfChannelsLabeledEdit->Text=IntToStr(GetNumChannels());

 ChannelNamesStringGrid->RowCount=ChannelNames.size()+1;
 ChannelNamesStringGrid->ColWidths[0]=20;
 ChannelNamesStringGrid->ColWidths[1]=ChannelNamesStringGrid->Width-ChannelNamesStringGrid->ColWidths[0]-20;
 ChannelNamesStringGrid->Cells[0][0]="Channel #";
 ChannelNamesStringGrid->Cells[1][0]="Channel Name";
 for(int i=0;i<int(ChannelNames.size());i++)
 {
  ChannelNamesStringGrid->Cells[0][i+1]=IntToStr(i);
  ChannelNamesStringGrid->Cells[1][i+1]=ChannelNames[i].c_str();
 }
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::CommandTimerTimer(TObject *Sender)
{
 if(WaitForSingleObject(CommandQueueUnlockEvent,10) == WAIT_TIMEOUT)
  return;

try {
 ResetEvent(CommandQueueUnlockEvent);
 while(!CommandQueue.empty())
 {
  CurrentProcessedCommand=CommandQueue.back();
  CommandQueue.pop_back();
  SetEvent(CommandQueueUnlockEvent);

  bool is_processed=ProcessControlCommand(CurrentProcessedCommand, ResponseType, Response);

  if(!is_processed)
   is_processed=ProcessRPCCommand(CurrentProcessedCommand, ResponseType, Response);

  if(!is_processed)
   is_processed=ProcessPtzCommand(CurrentProcessedCommand, ResponseType, Response);

  if(!is_processed)
  {
  }

  if(CommandResponseEncoder)
  {
   CommandResponseEncoder(ResponseType, Response, EncodedResponse);
   SendCommandResponse(CurrentProcessedCommand.first, EncodedResponse);
  }
  else
  {
//   SendCommandErrorResponse(CurrentProcessedCommand,0);
  }
  ResetEvent(CommandQueueUnlockEvent);
 }
 SetEvent(CommandQueueUnlockEvent);
}
catch (...)
{
 SetEvent(CommandQueueUnlockEvent);
 throw;
}
// SetEvent(CommandQueueUnlockEvent);
}
//---------------------------------------------------------------------------


void __fastcall TUServerControlForm::TcpServerAccept(TObject *Sender, TCustomIpClient *ClientSocket)
{
/* vector<unsigned char> client_buffer;
 while(ClientSocket->Connected)
 {
  bool ReadReady = false;
  if(ClientSocket->Select(&ReadReady, NULL, NULL, 1000) )
  {
   client_buffer.resize(200000);
   int length=client_buffer.size();
   length=ClientSocket->ReceiveBuf(&client_buffer[0], length);
   if(length>0)
   {
	client_buffer.resize(length);
	PacketReader.ProcessDataPart(client_buffer);
	if(PacketReader.GetNumPackets()>0)
	{
	 Packet=PacketReader.GetLastPacket();
	 PacketReader.DelLastPacket();
	 if(Packet.GetNumParams()>0)
	 {
//	  PacketXml.resize(Packet.GetParamSize(0));
	  UServerCommand args;
	  args["Request"].resize(Packet.GetParamSize(0));
	  if(Packet.GetParamSize(0)>0)
	   memcpy(&args["Request"][0],&Packet(0)[0],Packet.GetParamSize(0));
	  ResetEvent(CommandQueueUnlockEvent);
	  CommandQueue.push_back(args);
	  SetEvent(CommandQueueUnlockEvent);
	 }
	}
   }
  }
 } */
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::TcpServerListening(TObject *Sender)
{
 //
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::TcpServerGetThread(TObject *Sender, TClientSocketThread *&ClientSocketThread)

{
 //
}
//---------------------------------------------------------------------------




void __fastcall TUServerControlForm::IdTCPServerDisconnect(TIdContext *AContext)
{
 std::string bind=AnsiString(AContext->Binding->PeerIP).c_str();
 bind+=":";
 bind+=RDK::sntoa(AContext->Binding->PeerPort);

 std::map<std::string, RDK::UTransferReader>::iterator I=PacketReaders.find(bind);
 if(I != PacketReaders.end())
 {
  PacketReaders.erase(I);
 }

 Engine_LogMessage(RDK_EX_INFO, (std::string("Client Disconnected: ")+bind).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::IdTCPServerExecute(TIdContext *AContext)
{
 vector<unsigned char> client_buffer;
 TIdBytes VBuffer;
 int length=AContext->Connection->IOHandler->InputBuffer->Size;
 if(length>0)
 {
  std::string bind=AnsiString(AContext->Binding->PeerIP).c_str();
  bind+=":";
  bind+=RDK::sntoa(AContext->Binding->PeerPort);

   AContext->Connection->IOHandler->ReadBytes(VBuffer, length);
   length=VBuffer.Length;
   client_buffer.resize(length);
   Engine_LogMessage(RDK_EX_DEBUG, (std::string("Data received from: ")+bind+std::string(" size (bytes)=")+sntoa(length)).c_str());

   if(length>0)
   {
	memcpy(&client_buffer[0],&VBuffer[0],length);
	client_buffer.resize(length);
	std::map<std::string, RDK::UTransferReader>::iterator I=PacketReaders.find(bind);
	if(I == PacketReaders.end())
	{
     SetEvent(CommandQueueUnlockEvent);
	 return;
	}
	I->second.ProcessDataPart(client_buffer);
	Engine_LogMessage(RDK_EX_DEBUG, (std::string("Number of decoded packets: ")+sntoa(I->second.GetNumPackets())).c_str());
	while(I->second.GetNumPackets()>0)
	{
	 UTransferPacket packet=I->second.GetFirstPacket();
	 I->second.DelFirstPacket();
	 if(packet.GetNumParams()>0)
	 {
//	  PacketXml.resize(Packet.GetParamSize(0));
	  UServerCommand args;
//	  args.resize(Packet.GetParamSize(0));
//	  if(Packet.GetParamSize(0)>0)
//	   memcpy(&args[0],&Packet(0)[0],Packet.GetParamSize(0));
	  UServerCommand cmd;
	  cmd.first=bind;
	  cmd.second=packet(0);
	  ResetEvent(CommandQueueUnlockEvent);
	  CommandQueue.push_back(cmd);
	  SetEvent(CommandQueueUnlockEvent);
	  std::string str;
	  ConvertVectorToString(cmd.second,str);
	  Engine_LogMessage(RDK_EX_DEBUG, (std::string("Command pushed to queue: \n")+str).c_str());
	 }
	}
   }
 }
 else
  Sleep(10);
//  Memo1.Lines.Add(LLine);
//  AContext.Connection.IOHandler.WriteLn('OK');
//  TIdNotify.NotifyMethod( StopStartServerdMessage );
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::IdTCPServerConnect(TIdContext *AContext)
{
 std::string bind=AnsiString(AContext->Binding->PeerIP).c_str();
 bind+=":";
 bind+=RDK::sntoa(AContext->Binding->PeerPort);
 PacketReaders[bind].ResetProcessing();
 PacketReaders[bind].ClearPacketList();
 Engine_LogMessage(RDK_EX_INFO, (std::string("Client connected: ")+bind).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::ServerRestartTimerTimer(TObject *Sender)
{
 if(IdTCPServer->Active)
  return;

 if(AutoStartFlag)
 {
  ServerStartButtonClick(Sender);
 }
}
//---------------------------------------------------------------------------


void __fastcall TUServerControlForm::FormClose(TObject *Sender, TCloseAction &Action)

{
 ServerStopButtonClick(Sender);
// Sleep(100);
}
//---------------------------------------------------------------------------




