#ifndef URpcDecoderInternal_CPP
#define URpcDecoderInternal_CPP

#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../Deploy/PtzLib/ptzlib_initdll.h"
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
  return false;
 }

 const char* response=RemoteCall(cmd->Request.c_str(), cmd->ResponseStatus, cmd->ChannelIndex);

 if(cmd->ResponseStatus == 2001)
 {
  cmd->ResponseStatus=0;
  MEngine_FreeBufString(cmd->ChannelIndex, response);
  response=PtzRemoteCall(cmd->Request.c_str(), cmd->ResponseStatus, cmd->ChannelIndex);
 }

 if(response)
  cmd->Response=response;
 else
  cmd->Response=RDK::sntoa(cmd->ResponseStatus);

 MEngine_FreeBufString(cmd->ChannelIndex, response);

 if(cmd->ResponseStatus == 2001)
  return false;

 cmd->IsProcessed=true;
 return true;
}
// --------------------------

}
#endif

