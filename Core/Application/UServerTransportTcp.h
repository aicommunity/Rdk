#ifndef UServerTransportTcpH
#define UServerTransportTcpH

#include "UServerTransport.h"

namespace RDK{
class UApplication;

class RDK_LIB_TYPE UServerTransportTcp: public UServerTransport
{
protected: // ���������

protected: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UServerTransportTcp(void);
virtual ~UServerTransportTcp(void);
// --------------------------

// --------------------------
// ������ ��������������� ������
// --------------------------

/// ������ �������� ����� �� ���������� ���������, �������� ��������
/// ������ ������������ ������� ��� ����������� �� ���� ����������
//virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);
};
















}//namespace RDK
#endif
