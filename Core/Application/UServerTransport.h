#ifndef UServerTransportH
#define UServerTransportH

#include "../../Deploy/Include/rdk.h"
#include "UIVisualController.h"
#include "URpcCommandInternal.h"

namespace RDK {
class UApplication;
class RDK_LIB_TYPE UServerTransport: public UAppController
{
friend class UApplication;
protected: // ���������

protected: // ������
/// ��������� �� ��������� ����������
UEPtr<UApplication> Application;

/// ����� ������� �������, �� ����� ���������� ���������� ������
std::map<std::string, RDK::UTransferReader> PacketReaders;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UServerTransport(void);
virtual ~UServerTransport(void);
// --------------------------

// --------------------------
// ������ ��������������� ������
// --------------------------

//������ ����� � ���� ��������� ���������� �������
virtual void SetServerBinding(std::string &interface_address, int port);

//��������� ������ ���������� ���������� ��������
virtual std::string GetServerBindingInterfaceAddress(void);

//��������� ������ ���������� ���������� ��������
virtual int GetServerBindingPort(void) const;

/// ������������ �������� ������, �������������� ������ � ���������
/// 01.04.2020 ���� � ���� ������, �������� ����� ����� � �������� *Tcp, *Http, *Udp
void ProcessIncomingData(std::string &bind, std::vector<URpcCommandInternal> &command_list);

/// ������ �������� ����� �� ���������� ���������, �������� ��������
/// ������ ������������ ������� ��� ����������� �� ���� ����������
virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);

/// ��������� ����� �� ������� ���������������� ����������
virtual void SendResponseBuffer(std::vector<unsigned char> buffer, std::string &responce_addr);

///������������ ������� ��� ������ ������
///���������� - ������� ������ ��� ��������� ������� �� ����������� �������
virtual void ConnectClient(std::string& bind);
///����������� ������� ��� ������ ������
///���������� - ������� ������ ��� ��������� ������� �� ����������� �������
virtual void Dis�onnectClient(std::string& bind);
///������������ ��������� �������, ��������� ��� ���������
virtual void ServerStop();
/// �� �������� � ServerStop, ������������ ��������� �������
/// (�������������� � �������� �������)
virtual void ServerStart();

/// �������� ������ � ������
void ConvertStringToVector(const std::string &source, UParamT &dest);
void ConvertVectorToString(const UParamT &source, std::string &dest);

// --------------------------
// ����� ������ ���������� ������������
// --------------------------
// ��������� ��������� ���������� � xml
//virtual void SaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
//virtual void LoadParameters(RDK::USerStorageXML &xml);
// --------------------------

};



}//namespace RDK
#endif
