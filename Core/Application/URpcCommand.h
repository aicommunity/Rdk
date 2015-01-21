#ifndef URpcCommand_H
#define URpcCommand_H

#include "UProject.h"

namespace RDK {

class RDK_LIB_TYPE URpcCommand
{
public: // ������
/// ������������� ���������� ������
std::string RecepientId;

/// ������������� �������
unsigned CmdId;

/// ��� �������
std::string FunctionName;

/// ������ ������
int ChannelIndex;

/// ��� ����������
std::string ComponentName;

/// ����, ����������, ��� ������� ���� ������������ � ���������
bool IsPrepared;

/// ����, ����������, ��� ������� ���� ������������
bool IsDecoded;

/// ����, ����������, ��� ������� ���� ����������
bool IsProcessed;

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
/// ������������� ���������� ������
const std::string& GetRecepientId(void) const;

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
virtual void APrepareProcess(void)=0;

/// ������������ ������������� �������� ������ � ��������� ��������������� ����
virtual bool DecodeBasicData(void);
virtual bool ADecodeBasicData(void)=0;
// --------------------------

};


}

#endif

