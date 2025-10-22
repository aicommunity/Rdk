#include "UServerControl.h"

#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "UProject.h"
#include "UApplication.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UServerControl::UServerControl(void)
{
 AutoStartFlag=false;
 ServerName="Server";
 ServerId="Server1";
 AverageIterations=4;
 PerformancePushIndex=0;
 DebugFolder = "";
 InitFlag=false;

 Name="ServerControl";
}

UServerControl::~UServerControl(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
/// ���� ���������� ������� ������� ��� ������
bool UServerControl::GetAutoStartFlag(void) const
{
 return AutoStartFlag;
}

bool UServerControl::SetAutoStartFlag(bool value)
{
 if(AutoStartFlag == value)
  return true;
 AutoStartFlag=value;
 return true;
}

/// ��� �������
const std::string& UServerControl::GetServerName(void) const
{
 return ServerName;
}

bool UServerControl::SetServerName(const std::string& value)
{
 ServerName=value;
 return true;
}

/// ������������� �������
const std::string& UServerControl::GetServerId(void) const
{
 return ServerId;
}

bool UServerControl::SetServerId(const std::string& value)
{
 ServerId=value;
 return true;
}

/// ����� ����� ���������� ������ ������������������
int UServerControl::GetAverageIterations(void) const
{
 return AverageIterations;
}

bool UServerControl::SetAverageIterations(int value)
{
 AverageIterations=value;
 return true;
}

const std::string& UServerControl::GetMetaComponentName(void) const
{
 return MetaComponentName;
}

bool UServerControl::SetMetaComponentName(const std::string& value)
{
 MetaComponentName=value;
 return true;
}

const std::string& UServerControl::GetMetaComponentStateName(void) const
{
 return MetaComponentStateName;
}

bool UServerControl::SetMetaComponentStateName(const std::string& value)
{
 MetaComponentStateName=value;
 return true;
}

///����� ��� ���������� ���������� ������
const std::string& UServerControl::GetDebugFolder(void) const
{
 return DebugFolder;
}

bool UServerControl::SetDebugFolder(const std::string& value)
{
 if(DebugFolder == value)
  return true;
 DebugFolder=value;
 return true;
}

///������ ���� �� ���� ������
const std::string& UServerControl::GetDebugOutputPath(void) const
{
 return DebugOutputPath;
}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------

// --------------------------
// ������ �������������
// --------------------------
/// ������������� ������ � ���������� ������
std::shared_ptr<URpcDispatcher> UServerControl::GetRpcDispatcher(void)
{
 return RpcDispatcher;
}

/// ������������� ����� ��������� ������
/// ��������������� �� ������������ ������ ���������� ����� �� ���������� �������
bool UServerControl::SetRpcDispatcher(const std::shared_ptr<URpcDispatcher> &value)
{
 if(RpcDispatcher == value)
  return true;

 if(RpcDispatcher)
  RpcDispatcher->SetApplication(0);
 RpcDispatcher=value;
 RpcDispatcher->SetApplication(Application);
 return true;
}
/// ���������� ��������� �� ��������� ����������
std::shared_ptr<UApplication> UServerControl::GetApplication(void)
{
 return Application;
}

bool UServerControl::SetApplication(std::shared_ptr<UApplication> value)
{
 if(Application == value)
  return true;

 Application=value;
 return true;
}
// --------------------------
/// ���������� ��������� �� ��������� ����������
std::shared_ptr<UServerTransport> UServerControl::GetServerTransport(void)
{
 return ServerTransport;
}
bool UServerControl::SetServerTransport(std::shared_ptr<UServerTransport> value)
{
 if(ServerTransport == value)
  return true;

 ServerTransport=value;
 return true;
}

std::shared_ptr<UServerTransport> UServerControl::GetServerTransportHttp(void)
{
 return ServerTransportHttp;
}
bool UServerControl::SetServerTransportHttp(std::shared_ptr<UServerTransport> value)
{
 if(ServerTransportHttp == value)
  return true;

 ServerTransportHttp=value;
 return true;
}
// --------------------------



// --------------------------
// ������ ��� ������ ������������������ � ���������� ���������� ����������
// --------------------------
/// ���������� ��������� ������������������, ��
const std::vector<std::vector<RDK::ULongTime> >& UServerControl::GetModelPerformanceResults(void) const
{
 return ModelPerformanceResults;
}

const std::vector<std::vector<RDK::ULongTime> >& UServerControl::GetTransportPerformanceResults(void) const
{
 return TransportPerformanceResults;
}

/// ������� �������� ������������ ������� �� ������ ������ � ������
const std::vector<RDK::ULongTime>& UServerControl::GetModelAvg(void) const
{
 return ModelAvg;
}

const std::vector<RDK::ULongTime>& UServerControl::GetTransportAvg(void) const
{
 return TransportAvg;
}
// --------------------------


// --------------------------
// ������ ����������
// --------------------------
/// ������������� (����������� ������)
void UServerControl::Init(void)
{
 if(IsInit())
  return;

 InitFlag=true;
}

/// ��������������� (����������� ���������)
void UServerControl::UnInit(void)
{
 if(!IsInit())
  return;

 InitFlag=false;
}

/// �������� ��������� �������������
bool UServerControl::IsInit(void) const
{
 return InitFlag;
}
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
/// ������������ ��������� �������� ����������
int UServerControl::RegisterMetadataReceiver(const std::string &address, int port)
{
 return 1;
}

/// ������� ��������� �������� ����������
int UServerControl::UnRegisterMetadataReceiver(const std::string &address, int port)
{
 return 1;
}
// --------------------------

// --------------------------
// ����� ������ ���������� ������������
// --------------------------
// ��������� ��������� ���������� � xml
void UServerControl::SaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("AverageIterations",AverageIterations);
// xml.WriteInteger("ServerControlPort", GetServerBindingPort());
// xml.WriteString("ServerControlAddress", GetServerBindingInterfaceAddress());

// xml.WriteInteger("Num",GetNumEngines());
 xml.WriteInteger("AutoStartFlag",AutoStartFlag);
 xml.WriteString("ServerName",ServerName);
 xml.WriteString("ServerId",ServerId);

 xml.WriteString("MetadataComponentName",MetaComponentName);
 xml.WriteString("MetadataComponentStateName",MetaComponentStateName);

}

// ��������� ��������� ���������� �� xml
void UServerControl::LoadParameters(RDK::USerStorageXML &xml)
{
 SetAverageIterations(xml.ReadInteger("AverageIterations",AverageIterations));

 SetMetaComponentName(xml.ReadString("MetadataComponentName",""));
 SetMetaComponentStateName(xml.ReadString("MetadataComponentStateName",""));

// int source_num_channels=xml.ReadInteger("NumberOfChannels",Core_GetNumChannels());
// SetNumEngines(source_num_channels);
 TProjectConfig config=GetApplication()->GetProjectConfig();
 for(size_t i=0;i<GetApplication()->GetProjectConfig().ChannelsConfig.size();i++)
 {
  config.ChannelsConfig[i].ChannelName=xml.ReadString(std::string("ChannelName_")+RDK::sntoa(i),RDK::sntoa(i));
 }
 GetApplication()->SetProjectConfig(config);

 SetAutoStartFlag(xml.ReadInteger("AutoStartFlag",true));
 SetServerName(xml.ReadString("ServerName","Server"));
 SetServerId(xml.ReadString("ServerId","Server"));
}
// --------------------------

// --------------------------
// ��������������� ������
// --------------------------
/// ������������ ������������������ �������
/// � ��������� �� ���� ���� ������������������
void UServerControl::CalculatePerformance(void)
{
 if(PerformancePushIndex>=int(ModelPerformanceResults.size()))
  PerformancePushIndex=0;

 if(ModelPerformanceResults.size() == 0)
  AfterReset();
 int num_channels=Core_GetNumChannels();
 ModelPerformanceResults[PerformancePushIndex].assign(num_channels,0);
 TransportPerformanceResults[PerformancePushIndex].assign(num_channels,0);
 for(size_t i=0;i<ModelPerformanceResults[PerformancePushIndex].size();i++)
 {
  UELockPtr<UEngine> engine=GetEngineLock(int(i));
  if(!engine)
   continue;
  if(!engine->Model_Check())
   continue;
  RDK::ULongTime model_time=engine->Model_GetFullStepDuration("");
  RDK::ULongTime ext_gui=engine->Model_GetInterstepsInterval("");
  ModelPerformanceResults[PerformancePushIndex][i]=model_time;
  TransportPerformanceResults[PerformancePushIndex][i]=ext_gui;
 }
 ++PerformancePushIndex;
 if(PerformancePushIndex>=int(ModelPerformanceResults.size()))
  PerformancePushIndex=0;

 ModelAvg.assign(num_channels,0);
 TransportAvg.assign(num_channels,0);
 int sum_number=0;

 /// ���������� ��������� ������������������, ��
 const std::vector<std::vector<RDK::ULongTime> >& model_performance=ModelPerformanceResults;
 const std::vector<std::vector<RDK::ULongTime> >& transport_performance=TransportPerformanceResults;

 for(size_t i=0;i<model_performance.size();i++)
 {
  for(size_t j=0;j<model_performance[i].size();j++)
  {
   ModelAvg[j]+=model_performance[i][j];
   TransportAvg[j]+=transport_performance[i][j];
  }
  if(model_performance[i].size()>0)
   ++sum_number;
 }

 if(sum_number>0)
  for(size_t j=0;j<ModelAvg.size();j++)
  {
   ModelAvg[j]/=sum_number;
   TransportAvg[j]/=sum_number;
  }
  /*
 perf_data.resize(ModelAvg.size());
 aver_perf_data.resize(ModelAvg.size());
 for(size_t j=0;j<ModelAvg.size();j++)
 {
  perf_data[j].push_back(ModelAvg[j]);
 }

 ///���������� �������� ��� ����������
 const int average_size = 20;

 for(unsigned int k=0; k<ModelAvg.size(); k++)
 {
	 unsigned long long current_perf=0;
	 if(perf_data[k].size()<=average_size)
	 {
		unsigned long long sum=0;
		for(unsigned int i=0; i<perf_data[k].size(); i++)
		{
			sum+=perf_data[k][i];
		}
		current_perf = sum/perf_data[k].size();
	 }
	 else
	 {
		unsigned long long sum=0;
		for(unsigned int i=perf_data[k].size()-average_size; i<perf_data[k].size(); i++)
		{
			sum+=perf_data[k][i];
		}
		current_perf = sum/average_size;
	 }
	 aver_perf_data[k].push_back(current_perf);
 }

 std::ofstream f;
 //������������ ������ ��������
	if(DebugOutputPath.empty())
	{
	 time_t t = time(NULL);
	 std::string st = ctime(&t);
	 st[st.size()-1]='-';
	 for(int i=0; i<st.size(); i++)
	 {
		 if(st[i]==' ')
			st[i]='_';
		 if(st[i]==':')
			st[i]='.';
	 }
	 if(!DebugFolder.empty()>0)
	 {
		std::string s = DebugFolder+st;
		RDK::CreateNewDirectory(s.c_str());
		DebugOutputPath = DebugFolder+st+"/";//"D:\\ML2\\";
	 }
	}

 f.open((DebugOutputPath+"performance.m").c_str());//String("D:\\Output\\performance.m").c_str());
 for(unsigned int k=0; k<aver_perf_data.size(); k++)
 {
	 f<<"P_"<<k<<" = [";
	 for(unsigned int i=0; i<aver_perf_data[k].size(); i++)
	 {
		 f<<aver_perf_data[k][i]<<"; ";
	 }
	 f<<"];\n\n";
 }
 f<<"Idx = [";
 if(!aver_perf_data.empty())
 for(unsigned int k=0; k<aver_perf_data[0].size(); k++)
 {
	f<<k<<"; ";
 }
 f<<"];\n\n";
 f<<"plot(";
 for(unsigned int k=0; k<aver_perf_data.size(); k++)
 {
	 f<<"Idx, P_"<<k<<",'k-'";
	 if(k<aver_perf_data.size()-1)
		f<<",";
 }
 f<<")";
 f.close();  */
}

// �����, ���������� ����� ������ ������
void UServerControl::AfterReset(void)
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
void UServerControl::AfterCalculate(void)
{
 CalculatePerformance();
}
// --------------------------

// --------------------------
// ��������������� ������ (��������� �������������� � �����������)
// --------------------------
/// �������� ������ � ������
void UServerControl::ConvertStringToVector(const std::string &source, UParamT &dest)
{
 dest.resize(source.size());
 if(source.size()>0)
  memcpy(&dest[0],source.c_str(),source.size());
}

/// �������� ������ � ������
void UServerControl::ConvertVectorToString(const UParamT &source, std::string &dest)
{
 dest.resize(source.size());
 if(source.size()>0)
  memcpy(&dest[0],&source[0],source.size());
}

void UServerControl::SendCommandResponse(UServerTransport *transport, std::string &client_address, RDK::UParamT &dest, std::vector<RDK::UParamT> &binary_data)
{
 UTransferPacket packet;
 packet.SetNumParams(1+int(binary_data.size()));
 packet.SetParamSize(0,int(dest.size()));
 packet.SetParam(0,dest);
 for(size_t i=0;i<binary_data.size();i++)
 {
  packet.SetParamSize(int(i+1),int(binary_data[i].size()));
  packet.SetParam(int(i+1),binary_data[i]);
 }
 RDK::UParamT buffer;
 packet.Save(buffer);

 transport->SendResponseBuffer(buffer, client_address);

 std::string str;
 str.resize(packet.GetParamSize(0));
 memcpy(&str[0],&(packet.operator ()((0),0)), packet.GetParamSize(0));
 Log_LogMessage(RDK_EX_DEBUG,(string("Response Sent: ")+str).c_str());
}

void UServerControl::ProcessCommandQueue(UServerTransport *transport)
{
 if(!GetRpcDispatcher())
  return;

  // ��������� ������� ����������� ������ ����������
 std::shared_ptr<URpcCommand> pcmd;
 RDK::UParamT response;
 std::vector<RDK::UParamT> binary_response;


  while(pcmd=GetRpcDispatcher()->PopProcessedCommand())
  {
   std::shared_ptr<URpcCommandInternal> pcmd_int=std::dynamic_pointer_cast<RDK::URpcCommandInternal>(pcmd);
   ConvertStringToVector(pcmd_int->Response, response);
   SendCommandResponse(transport, pcmd_int->RecepientId, response, binary_response);
   // No need to delete - shared_ptr handles cleanup automatically
  }
}

void UServerControl::ProcessIncomingData(std::string &bind, UServerTransport *transport)
{
 std::vector<RDK::URpcCommandInternal> commands;
 transport->ProcessIncomingData(bind, commands);

 if(!commands.empty())
 {
  for(int i=0; i<int(commands.size()); i++)
  {
			/*
	  if(!RdkApplication.GetRpcDispatcher()->IsCmdSupported(&CurrentProcessedCommand))
	  {
	   CurrentProcessedMainThreadCommand=CurrentProcessedCommand;
	   ProcessControlCommand();
//	   if(ProcessControlCommand(CurrentProcessedCommand, ResponseType, Response, BinaryResponse))
//		SendCommandResponse(AContext, Response, BinaryResponse);
	  }
	  else         */
	  //{
	   std::shared_ptr<URpcCommand> pcmd= std::make_shared<RDK::URpcCommandInternal>(commands[i]);
	   ///����������������  01.04.2020, ��� ��� ������, ���� ��� ���� �����
	   ///std::pair<std::string,std::shared_ptr<URpcCommand> > cmd_pair;
	   ///cmd_pair.first=CurrentProcessedCommand.RecepientId;
	   ///cmd_pair.second=pcmd;

//	   if(CurrentProcessedCommand.FunctionName != "Ptz_SetCameraParameter")
	   GetRpcDispatcher()->PushCommand(pcmd);
//	   else
//	    delete pcmd;
       //Log_LogMessage(RDK_EX_DEBUG, (std::string("Command pushed to queue: \n")+commands[i].Request).c_str());
  }
 }
 //02.04.2020 - ��� �� �������
 commands.clear();
}

void UServerControl::ProcessIncomingDataString(std::string &command_data,UServerTransport *transport)
{
 RDK::URpcCommandInternal CurrentProcessedCommand;
 CurrentProcessedCommand.Request=command_data;
 CurrentProcessedCommand.IsDecoded=false;
 if(!CurrentProcessedCommand.DecodeBasicData())
 {
  // TODO: ����� � ��� ������ �������������
  Log_LogMessage(RDK_EX_DEBUG, std::string("Command decode error!").c_str());
 }
 else
 {
  std::shared_ptr<URpcCommand> pcmd= std::make_shared<RDK::URpcCommandInternal>(CurrentProcessedCommand);
  GetRpcDispatcher()->PushCommand(pcmd);
  Log_LogMessage(RDK_EX_DEBUG, (std::string("Command pushed to queue: \n")+CurrentProcessedCommand.Request).c_str());
 }
}



// --------------------------
/// ���������� ������ �������
/// ���������� ��������������� �������
/// ���������� �� UApplication
// --------------------------
bool UServerControl::SetNumChannels(int number)
{
 int num=Core_GetNumChannels();
 if(num<=0)
  return false;

 //31.03.2020 - �������� ���� �� ���������, ���������� ��� ������������
 //������� �� ��������� ������ �� �������
 for(int i=number;i<num;i++)
 {
  if(Core_GetNumChannels()<=i)
   break;

  if((!MCore_IsChannelInit(i) || !MModel_Check(i)) && i != 0)
  {
//   UGEngineControlForm->CloneProject(0, i); // TODO: ���������� �������� ������������
//   MEnv_Reset(i,0);
  }
 }

 if(GetRpcDispatcher())
  GetRpcDispatcher()->UpdateDecoders();
 ///////////////////////////////////////////////


 if(!ASetNumChannels(number))
  return false;

 //31.03.2020 - �������� ���� �� ���������
 TProjectConfig config=Application->GetProjectConfig();
 for(size_t i=0;i<config.ChannelsConfig.size();i++)
 {
  if(config.ChannelsConfig[i].ChannelName.empty())
   config.ChannelsConfig[i].ChannelName=RDK::sntoa(i);
 }
 Application->SetProjectConfig(config);
 ///////////////////////////////////////////////

 AfterReset();
 return true;
}

bool UServerControl::ASetNumChannels(int num)
{
 return true;
}

bool UServerControl::InsertChannel(int index)
{

 if(!AInsertChannel(index))
  return false;
 AfterReset();
 return true;
}

bool UServerControl::AInsertChannel(int index)
{
 return true;
}

bool UServerControl::DeleteChannel(int index)
{

 if(!ADeleteChannel(index))
  return false;
 AfterReset();
 return true;
}

bool UServerControl::ADeleteChannel(int index)
{
 return true;
}
// --------------------------


}
