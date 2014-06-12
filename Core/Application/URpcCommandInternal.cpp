#ifndef URpcCommandInternal_CPP
#define URpcCommandInternal_CPP

#include "URpcCommandInternal.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
URpcCommandInternal::URpcCommandInternal(void)
{

}

URpcCommandInternal::~URpcCommandInternal(void)
{

}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// --------------------------

// --------------------------
// ������ �������������
// --------------------------
/// ������ ������ �������
void URpcCommandInternal::SetRequest(const std::string &request)
{
 Request=request;
 PrepareProcess();
}

/// ������ � xml-������� ������
const std::string& URpcCommandInternal::GetResponse(void) const
{
 return Response;
}

/// ������ ���������� �������
int URpcCommandInternal::GetResponseStatus(void) const
{
 return ResponseStatus;
}

/// �������������� ������� ��������� ����� �������
void URpcCommandInternal::PrepareProcess(void)
{
 URpcCommand::PrepareProcess();
 ResponseStatus == 2001;
 Response.clear();
}


/// ������������ ������������� �������� ������ � ��������� ��������������� ����
bool URpcCommandInternal::DecodeBasicData(void)
{
 const char* ReturnString=0;

 RDK::USerStorageXML xml;

 xml.Load(Request,"RpcRequest");

 if(!ExtractCmd(xml,FunctionName) || FunctionName.empty())
 {
  return false;
 }

 if(!ExtractCC(xml,ChannelIndex,ComponentName))
 {
  ExtractChannel(xml,ChannelIndex);
  ExtractComponent(xml,ComponentName);
 }

 xml.ReadData("Id",CmdId);

 return true;
}
// --------------------------


}
#endif

