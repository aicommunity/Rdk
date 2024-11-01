#ifndef URpcCommand_CPP
#define URpcCommand_CPP

#include "URpcCommand.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
URpcCommand::URpcCommand(void)
 : CmdId(0), ChannelIndex(-1), IsPrepared(false), IsDecoded(false), IsProcessed(false)
{

}

URpcCommand::~URpcCommand(void)
{

}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ������������� ���������� ������
const std::string& URpcCommand::GetRecepientId(void) const
{
 return RecepientId;
}

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
 if(IsPrepared)
  return;
 FunctionName.clear();
 ComponentName.clear();
 ChannelIndex=-1;
 CmdId=0;
 IsProcessed=false;
 IsDecoded=false;
 APrepareProcess();
 IsPrepared=true;
}

/// ������������ ������������� �������� ������ � ��������� ��������������� ����
bool URpcCommand::DecodeBasicData(void)
{
 if(IsDecoded)
  return true;

 PrepareProcess();

 if(!ADecodeBasicData())
  return false;

 IsDecoded=true;
 return true;
}
// --------------------------




}
#endif

