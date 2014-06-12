#ifndef URpcCommand_CPP
#define URpcCommand_CPP

#include "URpcCommand.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
URpcCommand::URpcCommand(void)
 : ChannelIndex(-1)
{

}

URpcCommand::~URpcCommand(void)
{

}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ������������� �������
unsigned URpcCommand::GetCmdId(void) const
{
 return CmdId;
}

/// ��� �������
const std::string& URpcCommand::GetFunctionName(void) const
{
 return FunctionName;
}

/// ������ ������
int URpcCommand::GetChannelIndex(void) const
{
 return ChannelIndex;
}

/// ��� ����������
const std::string& URpcCommand::GetComponentName(void) const
{
 return ComponentName;
}
// --------------------------


// --------------------------
// ������ �������������
// --------------------------
/// �������������� ������� ��������� ����� �������
void URpcCommand::PrepareProcess(void)
{
 FunctionName.clear();
 ComponentName.clear();
 ChannelIndex=-1;
 CmdId=0;
 IsProcessed=false;
}
// --------------------------




}
#endif

