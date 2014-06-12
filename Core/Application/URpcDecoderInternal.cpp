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
// ������������ � �����������
// --------------------------
URpcDecoderInternal::URpcDecoderInternal(void)
{

}

URpcDecoderInternal::~URpcDecoderInternal(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� ���������
// --------------------------
/// ������� ����� ����� ��������
URpcDecoderInternal* New(void)
{
 return new URpcDecoderInternal;
}

/// ������������ ������������� � ����� ������� �� ������� ������
/// ���������� false ���� ������� �� ��������������
bool URpcDecoderInternal::ProcessCommand(UEPtr<URpcCommand> &command)
{
 if(!command)
 {
  // ������ - ������� �� ������
  return false;
 }

 UEPtr<URpcCommandInternal> cmd=dynamic_pointer_cast<URpcCommandInternal>(command);
 if(!cmd)
 {
  // ������ - ������� �� �������������� ���������
  return false;
 }

 const char* response=RemoteCall(cmd->Request.c_str(), cmd->ResponseStatus, cmd->ChannelIndex);

 if(cmd->ResponseStatus == 2001)
 {
  MEngine_FreeBufString(cmd->ChannelIndex, response);
  response=PtzRemoteCall(cmd->Request.c_str(), cmd->ResponseStatus, cmd->ChannelIndex);
 }

 if(response)
  cmd->Response=response;

 MEngine_FreeBufString(cmd->ChannelIndex, response);

 if(cmd->ResponseStatus == 2001)
  return false;

 return true;
}
// --------------------------

}
#endif

