#ifndef UServerTransportHttpH
#define UServerTransportHttpH

#include "UServerTransport.h"

namespace RDK{
class UApplication;

class RDK_LIB_TYPE UServerTransportHttp: public UServerTransport
{
protected: // ���������

protected: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UServerTransportHttp(void);
virtual ~UServerTransportHttp(void);
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
