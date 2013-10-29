#ifndef rdk_rpcH
#define rdk_rpcH

#include "rdk_init.h"
#include "../PtzLib/ptzlib_init.h"
#include "myrdk.h"

namespace RDK{

extern std::vector<std::string> RpcReturnString;

const char* RemoteCallInternal(const char *request, int &return_value);

const char* PtzRemoteCall(const char *request, int &return_value);


/// ���������� ����� �� ����� ���������� � ������� ������
/// ���� ��������� �������� "CC"
/// ���������� ��������� ������������ �����
/// ����� ����: ������_������@��� ����������
bool ExtractCC(USerStorageXML &xml, int &channel_index, std::string &component_name);

/// ���������� ��� ����������
/// ���� ��������� �������� "Component"
bool ExtractComponent(USerStorageXML &xml, std::string &component_name);

/// ���������� ������� ������
/// ���� ��������� �������� "Channel"
bool ExtractChannel(USerStorageXML &xml, int &channel_index);

/// ���������� ��� �������
/// ���� ��������� �������� "Cmd"
bool ExtractCmd(USerStorageXML &xml, std::string &cmd_name);

}

#endif
