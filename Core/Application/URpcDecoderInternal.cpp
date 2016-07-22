#ifndef URpcDecoderInternal_CPP
#define URpcDecoderInternal_CPP

#include "../../Deploy/Include/rdk_cpp_initdll.h"
//#include "../../Deploy/PtzLib/ptzlib_initdll.h"
#include "../../Deploy/Include/rdk_rpc.h"
#include "../../Deploy/Include/myrdk.h"
#include "URpcDecoderInternal.h"
#include "URpcCommandInternal.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoderInternal::URpcDecoderInternal(void)
{

}

URpcDecoderInternal::~URpcDecoderInternal(void)
{

}
// --------------------------

// --------------------------
// Методы управления командами
// --------------------------
/// Создает копию этого декодера
URpcDecoderInternal* URpcDecoderInternal::New(void)
{
 return new URpcDecoderInternal;
}

/// Проверяет, поддерживается ли команда диспетчером
/// ожидает, что команда уже декодирована иначе всегда возвращает false
bool URpcDecoderInternal::IsCmdSupported(const UEPtr<URpcCommand> &command) const
{
 if(!command || !command->IsDecoded)
  return false;

 std::string cmd=command->FunctionName;
 if(cmd == "RpcPing")
 {
  return false;
 }
 else
 if(cmd == "SetNumChannels")
 {
  return false;
 }
 else
 if(cmd == "GetNumChannels")
 {
  return false;
 }
 else
 if(cmd == "GetServerName")
 {
  return false;
 }
 else
 if(cmd == "GetServerId")
 {
  return false;
 }
 else
 if(cmd == "GetChannelName")
 {
  return false;
 }
 else
 if(cmd == "SetChannelName")
 {
  return false;
 }
 else
 if(cmd == "GetChannelVideoSource")
 {
  return false;
 }
 else
 if(cmd == "SetChannelVideoSource")
 {
  return false;
 }
 else
 if(cmd == "CheckChannelVideoSourceConnection")
 {
  return false;
 }
 else
 if(cmd == "GetChannelBroacaster")
 {
  return false;
 }
 else
 if(cmd == "SetChannelBroadcaster")
 {
  return false;
 }
 else
 if(cmd == "ResetChannel")
 {
  return false;
 }
 else
 if(cmd == "StartChannel")
 {
  return false;
 }
 else
 if(cmd == "StopChannel")
 {
  return false;
 }
 else
 if(cmd == "RegisterMetadataReceiver")
 {
  return false;
 }
 else
 if(cmd == "UnRegisterMetadataReceiver")
 {
  return false;
 }
 else
 if(cmd == "LoadProject")
 {
  return false;
 }
 else
 if(cmd == "SaveProject")
 {
  return false;
 }
 else
 if(cmd == "ReadImageFromVideoSource")
 {
  return false;
 }
 else
 if(cmd == "StartVideoSource")
 {
  return false;
 }
 else
 if(cmd == "StopVideoSource")
  return false;

 return true;
}

/// Осуществляет декодирование и вызов команды по текущим данным
/// Возвращает false если команда не поддерживается
bool URpcDecoderInternal::AProcessCommand(const UEPtr<URpcCommand> &command)
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
  MLog_LogMessage(command->ChannelIndex, RDK_EX_WARNING, (std::string("RPC Internal Decoder : Command internal structure not supported. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command=")+command->FunctionName).c_str());
  return false;
 }

 const char* response=Core_RemoteCall(cmd->Request.c_str(), cmd->ResponseStatus, cmd->ChannelIndex);
/*
 if(cmd->ResponseStatus == 2001)
 {
  cmd->ResponseStatus=0;
  MEngine_FreeBufString(cmd->ChannelIndex, response);
  response=PtzRemoteCall(cmd->Request.c_str(), cmd->ResponseStatus, cmd->ChannelIndex);
 }*/

 if(response)
  cmd->Response=response;
 else
  cmd->Response=RDK::sntoa(cmd->ResponseStatus);

 MEngine_FreeBufString(cmd->ChannelIndex, response);

 if(cmd->ResponseStatus == 2001)
 {
  MLog_LogMessage(command->ChannelIndex, RDK_EX_WARNING, (std::string("RPC Internal Decoder: Unknown command. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command=")+command->FunctionName).c_str());
  return false;
 }

 cmd->IsProcessed=true;
 return true;
}
// --------------------------

}
#endif

