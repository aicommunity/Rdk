#ifndef URpcCommandInternal_CPP
#define URpcCommandInternal_CPP

#include "URpcCommandInternal.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcCommandInternal::URpcCommandInternal(void)
{

}

URpcCommandInternal::~URpcCommandInternal(void)
{

}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// --------------------------

// --------------------------
// Методы декодирования
// --------------------------
/// Задает строку запроса
void URpcCommandInternal::SetRequest(const std::string &request)
{
 Request=request;
 PrepareProcess();
}

/// Строка с xml-данными ответа
const std::string& URpcCommandInternal::GetResponse(void) const
{
 return Response;
}

/// Статус выполнения команды
int URpcCommandInternal::GetResponseStatus(void) const
{
 return ResponseStatus;
}

/// Инициализирует процесс обработки новой команды
void URpcCommandInternal::PrepareProcess(void)
{
 URpcCommand::PrepareProcess();
 ResponseStatus == 2001;
 Response.clear();
}


/// Осуществляет декодирование основных данных и заполняет соответствующие поля
bool URpcCommandInternal::DecodeBasicData(void)
{
 const char* ReturnString=0;

 RDK::USerStorageXML xml;

 xml.Load(Request,"RpcRequest");

 if(!ExtractCmd(xml,FunctionName) || FunctionName.empty())
 {
  return false;
 }

 if(!ExtractCC(xml,ChannelIndex,ComponentName))
 {
  ExtractChannel(xml,ChannelIndex);
  ExtractComponent(xml,ComponentName);
 }

 xml.ReadData("Id",CmdId);

 return true;
}
// --------------------------


}
#endif

