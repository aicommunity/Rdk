#ifndef URpcDecoderCommon_CPP
#define URpcDecoderCommon_CPP

#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../Deploy/Include/rdk_rpc.h"
#include "../../Deploy/Include/myrdk.h"
#include "URpcDecoderCommon.h"
#include "URpcCommandInternal.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoderCommon::URpcDecoderCommon(void)
{

}

URpcDecoderCommon::~URpcDecoderCommon(void)
{

}
// --------------------------

// --------------------------
// Методы управления командами
// --------------------------
/// Создает копию этого декодера
URpcDecoderCommon* URpcDecoderCommon::New(void)
{
 return new URpcDecoderCommon;
}

/// Проверяет, поддерживается ли команда диспетчером
/// ожидает, что команда уже декодирована иначе всегда возвращает false
bool URpcDecoderCommon::IsCmdSupported(const UEPtr<URpcCommand> &command) const
{
 if(!command || !command->IsDecoded)
  return false;

 std::string cmd=command->FunctionName;
 if(cmd == "RpcPing")
 {
  return true;
 }
 else
 if(cmd == "SetNumChannels")
 {
  return true;
 }
 else
 if(cmd == "GetNumChannels")
 {
  return true;
 }
 else
 if(cmd == "GetServerName")
 {
  return true;
 }
 else
 if(cmd == "GetServerId")
 {
  return true;
 }
 else
 if(cmd == "GetChannelName")
 {
  return true;
 }
 else
 if(cmd == "SetChannelName")
 {
  return true;
 }
 else
 if(cmd == "GetChannelVideoSource")
 {
  return true;
 }
 else
 if(cmd == "SetChannelVideoSource")
 {
  return true;
 }
 else
 if(cmd == "CheckChannelVideoSourceConnection")
 {
  return true;
 }
 else
 if(cmd == "GetChannelBroacaster")
 {
  return true;
 }
 else
 if(cmd == "SetChannelBroadcaster")
 {
  return true;
 }
 else
 if(cmd == "ResetChannel")
 {
  return true;
 }
 else
 if(cmd == "StartChannel")
 {
  return true;
 }
 else
 if(cmd == "StopChannel")
 {
  return true;
 }
 else
 if(cmd == "RegisterMetadataReceiver")
 {
  return true;
 }
 else
 if(cmd == "UnRegisterMetadataReceiver")
 {
  return true;
 }
 else
 if(cmd == "LoadProject")
 {
  return true;
 }
 else
 if(cmd == "SaveProject")
 {
  return true;
 }
 else
 if(cmd == "ReadImageFromVideoSource")
 {
  return true;
 }
 else
 if(cmd == "StartVideoSource")
 {
  return true;
 }
 else
 if(cmd == "StopVideoSource")
  return true;

 return false;
}

/// Осуществляет декодирование и вызов команды по текущим данным
/// Возвращает false если команда не поддерживается
bool URpcDecoderCommon::AProcessCommand(const UEPtr<URpcCommand> &command)
{
 if(!command)
 {
  // Ошибка - команда не задана
  return false;
 }

 UEPtr<URpcCommandInternal> cmd=dynamic_pointer_cast<URpcCommandInternal>(command);
 if(!cmd)
 {
  // Ошибка - команда не поддерживается декодером
  MEngine_LogMessage(command->ChannelIndex, RDK_EX_WARNING, (std::string("RPC Common Decoder : Command internal structure not supported. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command=")+command->FunctionName).c_str());
  return false;
 }

 const char* response=RemoteCall(cmd->Request.c_str(), cmd->ResponseStatus, cmd->ChannelIndex);

 if(response)
  cmd->Response=response;
 else
  cmd->Response=RDK::sntoa(cmd->ResponseStatus);

 MEngine_FreeBufString(cmd->ChannelIndex, response);

 if(cmd->ResponseStatus == 2001)
 {
  MEngine_LogMessage(command->ChannelIndex, RDK_EX_WARNING, (std::string("RPC Common Decoder: Unknown command. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command=")+command->FunctionName).c_str());
  return false;
 }

 cmd->IsProcessed=true;
 return true;
}

const char* RDK_CALL URpcDecoderCommon::RemoteCall(const char *request, int &return_value, int &channel_index)
{
 return_value=INT_MAX;

 RDK::USerStorageXML xml,xml_data;

 xml.Load(request,"RpcRequest");

 int engine_index=-1;
 std::string cmd;
 std::string component_name;
 channel_index=0;


 if(!ExtractCmd(xml,cmd) || cmd.empty())
 {
  return_value=2000;
  return 0;
 }

 if(!ExtractCC(xml,engine_index,component_name))
 {
  ExtractChannel(xml,engine_index);
  ExtractComponent(xml,component_name);
 }
  /*
 if((engine_index < 0 || engine_index >= GetNumEngines()) &&
	(cmd != "GetNumChannels" && cmd !="SetNumChannels"))
 {
  return_value=2001;
  return 0;
 }  */

 channel_index=engine_index;
 std::string response,temp;
 std::string name;

 RDK::USerStorageXML result;
 result.Create("RpcResponse");
 result.WriteString("Id", xml.ReadString("Id",""));
// if(cmd == "EngineInit")
// {
// }
// else
// {
 response=ARemoteCall(cmd,xml,component_name, engine_index, return_value);
 if(return_value == 2001)
 {
  if(cmd == "RpcPing")
  {
   return_value=0;
  }
  else
  if(cmd == "SetNumChannels")
  {
   int num_engines=xml.ReadInteger("NumChannels",GetNumEngines());
   if(GetApplication()->SetNumEngines(num_engines))
	return_value=0;//UServerControlForm->SetNumChannels(num_engines);
   else
	return_value=112112;//UServerControlForm->SetNumChannels(num_engines);
  }
  else
  if(cmd == "GetNumChannels")
  {
   response=RDK::sntoa(GetApplication()->GetNumEngines());
   return_value=0;
  }
  else
  if(cmd == "GetServerName")
  {
   response=GetApplication()->GetServerControl()->GetServerName();
   return_value=0;
  }
  else
  if(cmd == "GetServerId")
  {
   response=GetApplication()->GetServerControl()->GetServerId();
   return_value=0;
  }
  else
  if(cmd == "GetChannelName")
  {
   response=GetApplication()->GetProjectConfig().ChannelsConfig[engine_index].ChannelName;
   return_value=0;
  }
  else
  if(cmd == "SetChannelName")
  {
   std::string ch_name=xml.ReadString("Name","");
   if(!ch_name.empty())
   {
	TProjectConfig config=GetApplication()->GetProjectConfig();
	config.ChannelsConfig[engine_index].ChannelName=ch_name;
	GetApplication()->SetProjectConfig(config);
   }
   return_value=0;
  }
  else
  if(cmd == "ResetChannel")
  {
   GetApplication()->ResetEngine(engine_index);
   return_value=0;
  }
  else
  if(cmd == "StartChannel")
  {
   GetApplication()->StartEngine(engine_index);
   return_value=0;
  }
  else
  if(cmd == "StopChannel")
  {
   GetApplication()->PauseEngine(engine_index);
   return_value=0;
  }
  else
  if(cmd == "RegisterMetadataReceiver")
  {
   string address=xml.ReadString("Address","");
   int port=xml.ReadInteger("Port",8888);
   return_value=GetApplication()->GetServerControl()->RegisterMetadataReceiver(address, port);
  }
  else
  if(cmd == "UnRegisterMetadataReceiver")
  {
   string address=xml.ReadString("Address","");
   int port=xml.ReadInteger("Port",1000);
   return_value=GetApplication()->GetServerControl()->UnRegisterMetadataReceiver(address, port);
  }
  else
  if(cmd == "LoadProject")
  {
   std::string file_name=xml.ReadString("FileName","");
   if(!file_name.empty())
   {
	return_value=1;//UServerControlForm->LoadProject(engine_index,file_name);
   }
  }
  else
  if(cmd == "SaveProject")
  {
   return_value=1;//UServerControlForm->SaveProject();
  }
 }
// }

 result.SelectRoot();
 if(!response.empty())
 {
  result.WriteString("Data",response);
//  MEngine_FreeBufString(engine_index,response);
 }
 else
  result.WriteString("Data","");
 result.WriteInteger("Res",return_value);
 MLockEngine(0);
 std::string &buffer=GetEngine(0)->CreateTempString();
 result.Save(buffer);
 MUnLockEngine(0);

 return buffer.c_str();
}

std::string RDK_CALL URpcDecoderCommon::ARemoteCall(const std::string &cmd, RDK::USerStorageXML &xml, const std::string &component_name, int engine_index, int &return_value)
{
 return_value=2001;
 return "";
}
// --------------------------

}
#endif

