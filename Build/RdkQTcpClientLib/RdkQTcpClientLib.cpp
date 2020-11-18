#include "RdkQTcpClient.h"
#include "RdkQTcpClientLib.h"


std::vector<RdkQTcpClient*> ClientsArray;

int Rtcl_SumTest(int a, int b)
{
    int s = a+b;
    qDebug()<<"Rtcl_SumTest sum: "<<s;
    return s;
}

// Метод обработки простой команды
int ProcessSimpleCommand(const std::string &command_name, int server_index, int channel_index, int timeout, RDK::USerStorageXML &request, RDK::USerStorageXML &response)
{
 try
 {
 if((int)ClientsArray.size()-1 < server_index)
  return RDK_RPC_SERVER_NOT_EXIST;

 if(!ClientsArray[server_index])
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!ClientsArray[server_index]->SocketIsConnected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 RDK::USerStorageXML xml;
 int cmdId=ClientsArray[server_index]->PrepareCommandXml(xml, command_name.c_str(), channel_index);

 request.SelectRoot();
 xml.SelectRoot();
 xml.LoadFieldsToNode(request, false);

 //ResetEvent(ClientsArray[server_index]->Thread->PacketReceivedEvent);
 ClientsArray[server_index]->Thread->PacketReceivedEvent->reset();
 int sendres=ClientsArray[server_index]->SendControlCommand(xml);
 if(sendres != 0)
  return sendres;

 if(timeout > 0)
 {
//  RDK::USerStorageXML response;
  int res=ClientsArray[server_index]->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
//   MainFormArray[server_index]->ServerAnswerDebug=answ.c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  if(res == 0)
  {
   ClientsArray[server_index]->AddIdCmdsRemovablePacket(cmdId);
   return RDK_RPC_RESPONSE_NOT_RECIEVED;
  }
  else
  {
   return res;
  }
 }
 else
 {
   ClientsArray[server_index]->AddIdCmdsRemovablePacket(cmdId);
 }

 }
 catch(...)//(EIdSocketError &exception)
 {
  return RDK_RPC_CONNECTION_ERROR;
 }
 return 0;
}

// Метод обработки команды ядра
int ProcessEngineCommand(const std::string &command_name,
int server_index, int channel_index, const char* component_name, int timeout,
RDK::USerStorageXML &request, RDK::USerStorageXML &response)
{
 try
 {
 if((int)ClientsArray.size()-1 < server_index)
  return RDK_RPC_SERVER_NOT_EXIST;

 if(!ClientsArray[server_index])
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!ClientsArray[server_index]->SocketIsConnected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 RDK::USerStorageXML PtzControlXml;
 int cmdId=ClientsArray[server_index]->PreparePtzControlXml(PtzControlXml, command_name, component_name, channel_index);
// std::string request_data;
// request.Save(request_data);
 request.SelectRoot();
 PtzControlXml.SelectRoot();
 PtzControlXml.LoadFieldsToNode(request, false);
 //ResetEvent(ClientsArray[server_index]->Thread->PacketReceivedEvent);
 ClientsArray[server_index]->Thread->PacketReceivedEvent->reset();
 int sendres=ClientsArray[server_index]->SendControlCommand(PtzControlXml);
 if(sendres != 0)
  return sendres;

 if(timeout > 0)
 {
  int res=ClientsArray[server_index]->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
//   MainFormArray[server_index]->ServerAnswerDebug=answ.c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  if(res == 0)
  {
   ClientsArray[server_index]->AddIdCmdsRemovablePacket(cmdId);
   return RDK_RPC_RESPONSE_NOT_RECIEVED;
  }
  else
  {
   return res;
  }
 }
 else
 {
  ClientsArray[server_index]->AddIdCmdsRemovablePacket(cmdId);
 }

 }
 catch(...)//EIdSocketError &exception)
 {
  return RDK_RPC_CONNECTION_ERROR;
 }

 return 0;
}


//int Ptz_GetMoveParamValuePrototype(const std::string &command_name,
//	int server_index, int channel_index, const char* camera_name,
//	const char *param_name, double &results, int timeout)
//{
// RDK::USerStorageXML request,response;

// request.Create("Request");
// request.WriteString("ParamName", param_name);
// results=0.0;
// int res=ProcessEngineCommand(command_name, server_index, channel_index, camera_name, timeout, request, response);
// if(res)
//  return res;

// results=StrToFloat(response.ReadString("Data", "").c_str());
// return 0;
//}

//int Ptz_SetMoveParamValuePrototype(const std::string &command_name,
//	int server_index, int channel_index, const char* camera_name,
//	const char *param_name, double param_value, bool &results, int timeout)
//{
// RDK::USerStorageXML request,response;

// request.Create("Request");
// request.WriteString("ParamName", param_name);
// request.WriteFloat("ParamValue", param_value);
// results=false;
// int res=ProcessEngineCommand(command_name, server_index, channel_index, camera_name, timeout, request, response);
// if(res)
//  return res;

// results=StrToBool(response.ReadString("Data", "").c_str());
// return 0;
//}

/// Выделение имени камеры и индекса канала из составного имени компоненты
bool Rpc_CameraNameSeparator(const std::string &source, std::string &camera_name, int &channel_index)
{
 std::string temp;
// std::string sourceStr=source;
 std::string::size_type i=source.find_first_of("@");
 if(i == std::string::npos)
  return false;

 channel_index=atoi(source.substr(0,i).c_str());
 camera_name=source.substr(i+1);
 return true;
}

/// Проверка состояния RdkQTcpClient
bool Rpc_IsClientConnected(int server_index)
{
 if((int)ClientsArray.size()-1 < server_index)
  return false;

 if(ClientsArray[server_index])
 {
  //if(WaitForSingleObject(ClientsArray[server_index]->Thread->CalcStarted, 0) == WAIT_TIMEOUT)
  if(ClientsArray[server_index]->Thread->CalcStarted->wait(0)==false)
   return false;

  try
  {
   return ClientsArray[server_index]->SocketIsConnected();
  }
  catch(QException &exception)
  {
   return false;
  }
 }

 return false;
}

//----------------------------------------------------
const char* Rpc_GetServerAnswerDebug(int server_index)
{
 if((int)ClientsArray.size()-1 < server_index)
  return 0;

 if(ClientsArray[server_index])
  return 0;//MainFormArray[server_index]->ServerAnswerDebug;

 return 0;
}

bool Rpc_SetServerAnswerDebug(int server_index, const char* value)
{
// if(MainFormArray[server_index]->ServerAnswerDebug == value)
//  return true;

// MainFormArray[server_index]->ServerAnswerDebug=value;
 return true;
}

/// Инициализация
int Rpc_Init(int num_server)
{
 if(!ClientsArray.size())
 {
  for(int i=0; i<num_server; i++)
   Rpc_AddServer();
 }
 return 0;
}

int Rpc_UnInit(void)
{
 if(ClientsArray.size())
 {
  for(int i=0; i<ClientsArray.size(); i++)
  {
   delete ClientsArray[i];
  }

  ClientsArray.clear();
 }
 return 0;
}

bool Rpc_IsInit()
{
  return (ClientsArray.size()>0);
}


/// Коммуникация с сервером
int Rpc_Ping(int server_index, int timeout)
{
 try
 {
 if((int)ClientsArray.size()-1 < server_index)
  return RDK_RPC_SERVER_NOT_EXIST;

 if(!ClientsArray[server_index])
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!ClientsArray[server_index]->SocketIsConnected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 RDK::USerStorageXML xml;
 int cmdId=ClientsArray[server_index]->PrepareCommandXml(xml, "RpcPing", -1);
 //ResetEvent(ClientsArray[server_index]->Thread->PacketReceivedEvent);
 ClientsArray[server_index]->Thread->PacketReceivedEvent->reset();
 int send_res=ClientsArray[server_index]->SendControlCommand(xml);

 RDK::USerStorageXML response;
 int res=ClientsArray[server_index]->WaitServerResponse(cmdId, response, timeout);
 if(res == 1)
 {
  std::string answ="Server is active";
//  MainFormArray[server_index]->ServerAnswerDebug=answ.c_str();
  return 0;
 }
 else
 {
  std::string answ="Server is not active";
//  MainFormArray[server_index]->ServerAnswerDebug=answ.c_str();
  return RDK_RPC_SERVER_NOT_ACTIVE;
 }
 }
 catch(QException &exception)
 {
  return RDK_RPC_CONNECTION_ERROR;
 }

 return 0;
}


int Rpc_Connect(int server_index, const char* serverAddress, int serverPort)
{
 if((int)ClientsArray.size()-1 < server_index)
  return RDK_RPC_SERVER_NOT_EXIST;

 if(!ClientsArray[server_index])
 {
//  MainFormArray[server_index]=new TDllMainForm(0);
  return RDK_RPC_LIBRARY_NOT_INIT;
 }

 std::string s = ClientsArray[server_index]->GetHost();
 const char* old_host=s.c_str();
 int old_port=ClientsArray[server_index]->GetPort();

 if(!Rpc_IsClientConnected(server_index) || (*old_host!=*serverAddress)||(old_port!=serverPort))
 {
  Rpc_Disconnect(server_index);
  try
  {
   ClientsArray[server_index]->ClearIdCmdsRemovablePackets();
   //ClientsArray[server_index]->IdTCPClient->Host=serverAddress;
   //ClientsArray[server_index]->IdTCPClient->Port=serverPort;
   //ClientsArray[server_index]->IdTCPClient->Connect();
   ClientsArray[server_index]->Connect(serverAddress, serverPort);
  }
  //catch(EIdAlreadyConnected &ex)
  //{c
  //}
  catch(QException &ex)
  {
   return RDK_RPC_CONNECTION_ERROR;
  }

  //SetEvent(MainFormArray[server_index]->Thread->CalcStarted);
  ClientsArray[server_index]->Thread->CalcStarted->set();
  return 0;
 }
 return RDK_RPC_CONNECTION_ERROR;
}

int Rpc_Disconnect(int server_index)
{
 if((int)ClientsArray.size()-1 < server_index)
  return RDK_RPC_SERVER_NOT_EXIST;

 try
 {

 if(ClientsArray[server_index])
 {
  //ResetEvent(ClientsArray[server_index]->Thread->CalcStarted);
  ClientsArray[server_index]->Thread->CalcStarted->reset();
  //if(ClientsArray[server_index]->IdTCPClient->Connected())
  // ClientsArray[server_index]->IdTCPClient->IOHandler->InputBuffer->Clear();
  //ClientsArray[server_index]->IdTCPClient->Disconnect(false);
  ClientsArray[server_index]->Disconnect();
  //if(ClientsArray[server_index]->IdTCPClient->IOHandler != 0)
  // ClientsArray[server_index]->IdTCPClient->IOHandler->InputBuffer->Clear();
  //ResetEvent(ClientsArray[server_index]->Thread->CalcStarted);
  ClientsArray[server_index]->Thread->CalcStarted->reset();

  //if(WaitForSingleObject(ClientsArray[server_index]->Thread->PacketReaderUnlockEvent, INFINITE) != WAIT_TIMEOUT)
  if(ClientsArray[server_index]->Thread->PacketReaderUnlockEvent->exclusive_lock())
  {
//   ResetEvent(MainFormArray[server_index]->Thread->PacketReaderUnlockEvent);
   ClientsArray[server_index]->PacketReader.ResetProcessing();
   ClientsArray[server_index]->PacketReader.ClearPacketList();
   //ReleaseMutex(ClientsArray[server_index]->Thread->PacketReaderUnlockEvent);
   ClientsArray[server_index]->Thread->PacketReaderUnlockEvent->exclusive_unlock();
  }
  else
  {
//   MainFormArray[server_index]->ClearIdCmdsRemovablePackets();
//   delete MainFormArray[server_index];
//   MainFormArray[server_index]=0;
   return 100010;
  }
//  MainFormArray[server_index]->ClearIdCmdsRemovablePackets();
//  delete MainFormArray[server_index];
//  MainFormArray[server_index]=0;
 }
 }
 catch(QException &exception)
 {
  //ClientsArray[server_index]->IdTCPClient->Disconnect(false);
  ClientsArray[server_index]->Disconnect();
  //if(ClientsArray[server_index]->IdTCPClient->IOHandler != 0)
  // ClientsArray[server_index]->IdTCPClient->IOHandler->InputBuffer->Clear();
  return RDK_RPC_CONNECTION_ERROR;
 }
 return 0;
}

int Rpc_AddServer(void)
{
 //TDllMainForm *MainForm=new TDllMainForm(0);
 //MainFormArray.push_back(MainForm);

 RdkQTcpClient* client = new RdkQTcpClient();
 ClientsArray.push_back(client);

 return 0;
}

int Rpc_LastServerId()
{
 return (int)ClientsArray.size()-1;
}

int Rpc_DelServer(int server_index)
{
 if((int)ClientsArray.size()-1 < server_index)
  return RDK_RPC_SERVER_NOT_EXIST;

 if(!ClientsArray[server_index])
  return RDK_RPC_LIBRARY_NOT_INIT;

 try
 {
 if(ClientsArray[server_index]->SocketIsConnected())
  Rpc_Disconnect(server_index);
 }
 catch(QException &exception)
 {
  return RDK_RPC_CONNECTION_ERROR;
 }
 delete ClientsArray[server_index];
 ClientsArray.erase(ClientsArray.begin()+server_index);

 return 0;
}

int Rpc_StartChannel(int server_index, int channel_index, int timeout)
{
 RDK::USerStorageXML request, response;
 return ProcessSimpleCommand("StartChannel", server_index, channel_index, timeout, request, response);
}

int Rpc_ResetChannel(int server_index, int channel_index, int timeout)
{
 RDK::USerStorageXML request, response;
 return ProcessSimpleCommand("ResetChannel", server_index, channel_index, timeout, request, response);
}

int Rpc_StopChannel(int server_index, int channel_index, int timeout)
{
 RDK::USerStorageXML request, response;
 return ProcessSimpleCommand("StopChannel", server_index, channel_index, timeout, request, response);
}

int Rpc_GetNumChannels(int server_index, int &results, int timeout)
{
 RDK::USerStorageXML request,response;
 results=0;
 int res=ProcessSimpleCommand("GetNumChannels", server_index, -1, timeout, request, response);
 if(res)
  return res;

 try
 {
  results=QString(response.ReadString("Data", "").c_str()).toInt();
 }
 catch(QException &exception)
 {
  results=0;
  return RDK_RPC_UNSUCCESSFULL_DECODING;
 }
 return 0;
}

int Rpc_SetNumChannels(int server_index, int num_channels, int timeout)
{
 RDK::USerStorageXML request,response;

 request.Create("Request");
 request.WriteInteger("NumChannels", num_channels);
 return ProcessSimpleCommand("SetNumChannels", server_index, -1, timeout, request, response);
}

int Rpc_GetChannelName(int server_index, int channel_index, const char* &result, int timeout)
{
 RDK::USerStorageXML request,response;
 int res=ProcessSimpleCommand("GetChannelName", server_index, channel_index, timeout, request, response);
 if(res)
  return res;

 static std::string res_string;
 res_string=response.ReadString("Data", "").c_str();
 result=res_string.c_str();
 return 0;
}

int Rpc_GetLastError(int server_index, const char* &result, int timeout)
{
 RDK::USerStorageXML request,response;
 int res=ProcessSimpleCommand("GetLastError", server_index, -1, timeout, request, response);
 if(res)
  return res;

 static std::string res_string;
 res_string=response.ReadString("Data", "").c_str();
 result=res_string.c_str();
 return 0;
}

int  Rpc_GetDeploymentState(int server_index, int &dp_state, int& dp_progress, int& dp_cap, int timeout)
{
    RDK::USerStorageXML request,response;
    request.Create("Request");
    request.WriteInteger("Test", -1);
    int res=ProcessSimpleCommand("GetDeploymentState", server_index, -1, timeout, request, response);
    if(res)
     return res;

    static std::string res_string;
    res_string=response.ReadString("Data", "").c_str();

    //int state=-1;
    std::stringstream ss(res_string.c_str());
    ss>>dp_state>>dp_progress>>dp_cap;

    //dp_state=state;
    return res;
}

int  Rpc_GetLastError(int server_index, int channel_indes, int timeout, const char* &err_str)
{
    RDK::USerStorageXML request,response;
    request.Create("Request");
    int res=ProcessSimpleCommand("GetLastError", server_index, -1, timeout, request, response);
    /*if(res)
     return res;*/

    static std::string res_string;
    res_string=response.ReadString("Data", "").c_str();

    err_str = res_string.c_str();

    return res;
}

int Rpc_SetChannelName(int server_index, int channel_index, const char* channel_name, int timeout)
{
 RDK::USerStorageXML request,response;

 request.Create("Request");
 request.WriteString("Name", channel_name);
 return ProcessSimpleCommand("SetChannelName", server_index, channel_index, timeout, request, response);
}

/// Сохраняет состояние открытого проекта на сервере
int Rpc_SaveProject(int server_index, int timeout)
{
 RDK::USerStorageXML request,response;
 return ProcessSimpleCommand("SaveProject", server_index, -1, timeout, request, response);
}


int Rpc_LoadProject(int server_index, const char* project_path, int timeout)
{
    RDK::USerStorageXML request,response;

    request.Create("Request");
    request.WriteString("FileName", project_path);
    return ProcessSimpleCommand("LoadProject", server_index, -1, timeout, request, response);
}


int Rpc_DeployProject(int server_index, int task_index, int &resp, int timeout)
{
    RDK::USerStorageXML request,response;
    request.Create("Request");
    request.WriteInteger("TaskId", task_index);
    int res=ProcessSimpleCommand("DeployProject", server_index, 0, timeout, request, response);

    static std::string res_string;
    res_string=response.ReadString("Data", "").c_str();
    resp = atoi(res_string.c_str());


    //result=res_string.c_str();

    if(res)
     return res;
}

int Rpc_PrepareProject(int server_index, const char* &verbose_response, int timeout)
 {
    RDK::USerStorageXML request,response;
    return ProcessSimpleCommand("PrepareProject", server_index, -1, timeout, request, response);
}

int Rpc_GetPreparationResult(int server_index, const char* &verbose_response, int timeout)
 {
    RDK::USerStorageXML request,response;
    int res = ProcessSimpleCommand("GetPreparationResult", server_index, -1, timeout, request, response);

    std::string res_string;

    res_string=response.ReadString("Data", "").c_str();

    QString qs = res_string.c_str();
    QStringList spl = qs.split("|");
    int rs = spl[0].trimmed().toInt();
    verbose_response = spl[1].toUtf8().constData();

    if(res)
     return res;
}

 int Rpc_OpenPreparedProject(int server_index, const char* &verbose_response, int timeout)
 {
    RDK::USerStorageXML request,response;
    return ProcessSimpleCommand("OpenPreparedProject", server_index, -1, timeout, request, response);
}


