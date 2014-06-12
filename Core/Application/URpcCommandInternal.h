#ifndef URpcCommandInternal_H
#define URpcCommandInternal_H

#include "URpcCommand.h"

namespace RDK {

class URpcCommandInternal: public URpcCommand
{
public: // ������� ������
/// ������ � xml-������� �������
std::string Request;

public: // �������� ������
/// ������ � xml-������� ������
std::string Response;

/// ������ ���������� �������
int ResponseStatus;


public:
// --------------------------
// ������������ � �����������
// --------------------------
URpcCommandInternal(void);
virtual ~URpcCommandInternal(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// --------------------------

// --------------------------
// ������ �������������
// --------------------------
/// ������ ������ �������
virtual void SetRequest(const std::string &request);

/// ������ � xml-������� ������
const std::string& GetResponse(void) const;

/// ������ ���������� �������
int GetResponseStatus(void) const;

/// �������������� ������� ��������� ����� �������
void PrepareProcess(void);

/// ������������ ������������� �������� ������ � ��������� ��������������� ����
virtual bool DecodeBasicData(void);
// --------------------------
};


}

#endif

