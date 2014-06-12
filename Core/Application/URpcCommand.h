#ifndef URpcCommand_H
#define URpcCommand_H

#include "UProject.h"

namespace RDK {

class URpcCommand
{
public: // ������
/// ������������� �������
unsigned CmdId;

/// ��� �������
std::string FunctionName;

/// ������ ������
int ChannelIndex;

/// ��� ����������
std::string ComponentName;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
URpcCommand(void);
virtual ~URpcCommand(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ������������� �������
unsigned GetCmdId(void) const;

/// ��� �������
const std::string& GetFunctionName(void) const;

/// ������ ������
int GetChannelIndex(void) const;

/// ��� ����������
const std::string& GetComponentName(void) const;
// --------------------------

// --------------------------
// ������ �������������
// --------------------------
/// �������������� ������� ��������� ����� �������
virtual void PrepareProcess(void);

/// ������������ ������������� �������� ������ � ��������� ��������������� ����
virtual bool DecodeBasicData(void)=0;
// --------------------------

};


}

#endif

