#ifndef URpcCommandInternal_CPP
#define URpcCommandInternal_CPP

#include "URpcCommandInternal.h"
#include "../../Deploy/Include/rdk_rpc.h"
#include "../../Core/Serialize/UXMLStdSerialize.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
URpcCommandInternal::URpcCommandInternal(void)
{

}

URpcCommandInternal::URpcCommandInternal(const std::string &request)
 : Request(request)
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
 IsPrepared=false;
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
void URpcCommandInternal::APrepareProcess(void)
{
 ResponseStatus = 2001;
 Response.clear();
}


/// ������������ ������������� �������� ������ � ��������� ��������������� ����
bool URpcCommandInternal::ADecodeBasicData(void)
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

 xml.ReadInteger("Id",CmdId);

 return true;
}
// --------------------------


}
#endif

