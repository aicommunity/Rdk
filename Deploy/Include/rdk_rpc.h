#ifndef rdk_rpcH
#define rdk_rpcH

#include "rdk_initdll.h"
//#include "../PtzLib/ptzlib_init.h"
#include "myrdk.h"

namespace RDK{

//extern std::vector<std::string> RpcReturnString;

RDK_LIB_TYPE const char* RDK_CALL RemoteCallInternal(const char *request, int &return_value, int &channel_index);

//RDK_LIB_TYPE const char* RDK_CALL PtzRemoteCall(const char *request, int &return_value, int &channel_index);


/// ���������� ����� �� ����� ���������� � ������� ������
/// ���� ��������� �������� "CC"
/// ���������� ��������� ������������ �����
/// ����� ����: ������_������@��� ����������
RDK_LIB_TYPE bool RDK_CALL ExtractCC(USerStorageXML &xml, int &channel_index, std::string &component_name);

/// ���������� ��� ����������
/// ���� ��������� �������� "Component"
RDK_LIB_TYPE bool RDK_CALL ExtractComponent(USerStorageXML &xml, std::string &component_name);

/// ���������� ������� ������
/// ���� ��������� �������� "Channel"
RDK_LIB_TYPE bool RDK_CALL ExtractChannel(USerStorageXML &xml, int &channel_index);

/// ���������� ��� �������
/// ���� ��������� �������� "Cmd"
RDK_LIB_TYPE bool RDK_CALL ExtractCmd(USerStorageXML &xml, std::string &cmd_name);

}

#endif
