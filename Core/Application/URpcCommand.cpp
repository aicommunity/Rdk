#ifndef URpcCommand_CPP
#define URpcCommand_CPP

#include "URpcCommand.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcCommand::URpcCommand(void)
 : ChannelIndex(-1), IsPrepared(false), IsDecoded(false), IsProcessed(false)
{

}

URpcCommand::~URpcCommand(void)
{

}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Идентификатор команды
unsigned URpcCommand::GetCmdId(void) const
{
 return CmdId;
}

/// Имя функции
const std::string& URpcCommand::GetFunctionName(void) const
{
 return FunctionName;
}

/// Индекс канала
int URpcCommand::GetChannelIndex(void) const
{
 return ChannelIndex;
}

/// Имя компонента
const std::string& URpcCommand::GetComponentName(void) const
{
 return ComponentName;
}
// --------------------------


// --------------------------
// Методы декодирования
// --------------------------
/// Инициализирует процесс обработки новой команды
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

/// Осуществляет декодирование основных данных и заполняет соответствующие поля
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

