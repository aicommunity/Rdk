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
#include "TIdTcpResultBroadcasterFormUnit.h"
#include "rdk_cpp_initdll.h"
#include "TUBitmap.h"
#include "../../Core/Graphics/Libraries/Hardware/PtzRpc.cpp"
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

 Clients=new TThreadList;
}

__fastcall TUServerControlForm::~TUServerControlForm(void)
{
 if(Clients)
  delete Clients;
}


const char* TUServerControlForm::ControlRemoteCall(const char *request, int &return_value)
{
 return_value=2001;

 RDK::USerStorageXML xml,xml_data;

 xml.Load(request,"RpcRequest");

 int engine_index=xml.ReadInteger("Channel",0);
 std::string cmd=xml.ReadString("Cmd","");

 ControlResponseString.clear();
 if(cmd == "SetNumChannels")
 {
  int num_engines=xml.ReadInteger("NumChannels",GetNumEngines());
  return_value=SetNumChannels(num_engines);
 }
 else
 if(cmd == "GetNumChannels")
 {
  ControlResponseString=RDK::sntoa(GetNumChannels());
  return_value=0;
 }
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
 if(cmd == "GetChannelVideoSourceType")
 {
  ControlResponseString=RDK::sntoa(GetChannelVideoSource(engine_index));
  return_value=0;
 }
 else
 if(cmd == "SetChannelVideoSourceType")
 {
  int mode=xml.ReadInteger("Mode",5);
  return_value=SetChannelVideoSource(engine_index,mode);
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
 if(cmd == "LoadProject")
 {
  std::string file_name=xml.ReadString("FileName","");
  if(!file_name.empty())
  {
   return_value=LoadProject(engine_index,file_name);
  }
 }

 RDK::USerStorageXML result;

 result.Create("RpcResponse");
 result.WriteString("Id", xml.ReadString("Id",""));
 result.WriteString("Data",ControlResponseString);
 result.WriteInteger("Res",return_value);
 result.Save(ControlResponseString);

 return ControlResponseString.c_str();
}

const char* TUServerControlForm::PtzRemoteCall(const char *request, int &return_value)
{
 return_value=2001;

 RDK::USerStorageXML xml,xml_data;

 xml.Load(request,"RpcRequest");

 int engine_index=xml.ReadInteger("Channel",0);
 std::string cmd=xml.ReadString("Cmd","");
 std::string camera=xml.ReadString("Camera","");

 ControlResponseString.clear();
 if(cmd == "Ptz_GetCameraNames")
 {
  ControlResponseString=PtzControl.Ptz_GetCameraNames();
  return_value=0;
 }
 else
 if(cmd == "Ptz_GetCameraType")
 {
  ControlResponseString=PtzControl.Ptz_GetCameraType(camera.c_str());
  return_value=0;
 }
 if(cmd == "Ptz_GetCameraParameter")
 {
  std::string param_name=xml.ReadString("Parameter","");
  ControlResponseString=PtzControl.Ptz_GetCameraParameter(camera.c_str(),param_name.c_str());
  return_value=0;
 }
 else
 if(cmd == "Ptz_GetImplementedCommands")
 {
  ControlResponseString=PtzControl.Ptz_GetImplementedCommands(camera.c_str());
  return_value=0;
 }
 else
 if(cmd == "Ptz_CameraConnect")
 {
  return_value=PtzControl.Ptz_CameraConnect(camera.c_str());
 }
 else
 if(cmd == "Ptz_CameraDisconnect")
 {
  return_value=PtzControl.Ptz_CameraDisconnect(camera.c_str());
 }
 else
 if(cmd == "Ptz_Stop")
 {
  return_value=PtzControl.Ptz_Stop(camera.c_str());
 }
 else
 if(cmd == "Ptz_GotoHome")
 {
  return_value=PtzControl.Ptz_GotoHome(camera.c_str());
 }
 else
 if(cmd == "Ptz_PresetPoint")
 {
  int point_index=xml.ReadInteger("Point",0);
  return_value=PtzControl.Ptz_PresetPoint(camera.c_str(),point_index);
 }
 else
 if(cmd == "Ptz_GotoPoint")
 {
  int point_index=xml.ReadInteger("Point",0);
  return_value=PtzControl.Ptz_GotoPoint(camera.c_str(),point_index);
 }
 else
 if(cmd == "Ptz_Move")
 {
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  double zoom_speed=xml.ReadFloat("ZoomSpeed",0.0);
  return_value=PtzControl.Ptz_Move(camera.c_str(),pan_speed,tilt_speed,zoom_speed);
 }
 else
 if(cmd == "Ptz_MovePan")
 {
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  return_value=PtzControl.Ptz_MovePan(camera.c_str(),pan_speed);
 }
 else
 if(cmd == "Ptz_MoveTilt")
 {
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  return_value=PtzControl.Ptz_MoveTilt(camera.c_str(),tilt_speed);
 }
 else
 if(cmd == "Ptz_MoveZoom")
 {
  double zoom_speed=xml.ReadFloat("ZoomSpeed",0.0);
  return_value=PtzControl.Ptz_MoveZoom(camera.c_str(),zoom_speed);
 }
 else
 if(cmd == "Ptz_MoveNative")
 {
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  double zoom_speed=xml.ReadFloat("ZoomSpeed",0.0);
  return_value=PtzControl.Ptz_MoveNative(camera.c_str(),pan_speed,tilt_speed,zoom_speed);
 }
 else
 if(cmd == "Ptz_MovePanNative")
 {
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  return_value=PtzControl.Ptz_MovePanNative(camera.c_str(),pan_speed);
 }
 else
 if(cmd == "Ptz_MoveTiltNative")
 {
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  return_value=PtzControl.Ptz_MoveTiltNative(camera.c_str(),tilt_speed);
 }
 else
 if(cmd == "Ptz_MoveZoomNative")
 {
  double zoom_speed=xml.ReadFloat("ZoomSpeed",0.0);
  return_value=PtzControl.Ptz_MoveZoomNative(camera.c_str(),zoom_speed);
 }

 RDK::USerStorageXML result;

 result.Create("RpcResponse");
 result.WriteString("Id", xml.ReadString("Id",""));
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
}

// ��������� ��������� ���������� � xml
void TUServerControlForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("AverageIterations",AverageIterations);
// xml.WriteInteger("ServerControlPort", UHttpServerFrame->GetListenPort());
// xml.WriteInteger("ServerControlPort", StrToInt(TcpServer->LocalPort));
 xml.WriteInteger("ServerControlPort", StrToInt(IdTCPServer->Bindings->Items[0]->Port));
 xml.WriteInteger("NumberOfChannels",GetNumChannels());
 xml.WriteInteger("AutoStartFlag",AutoStartFlag);
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
 SetNumChannels(GetNumEngines());
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
 TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_id);
 if(!frame)
  return -1;

 return frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->GetMode();
}

/// ������ �������� ����� ��� ������
/// � ������������ � �������� VideoOutputFrame
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
// TcpServer->Active=false;
 IdTCPServer->Active=false;
// UHttpServerFrame->ServerListenOff();
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
 if(StrToInt(IdTCPServer->Bindings->Items[0]->Port) != new_port)
 {
  IdTCPServer->Active=false;
  IdTCPServer->Bindings->Items[0]->Port=new_port;
  if(AutoStartFlag)
   ServerStartButtonClick(Sender);
 }

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


