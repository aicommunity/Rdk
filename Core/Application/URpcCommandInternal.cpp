#ifndef URpcCommandInternal_CPP
#define URpcCommandInternal_CPP

#include "URpcCommandInternal.h"
#include "../../Deploy/Include/rdk_rpc.h"
#include "../../Core/Serialize/UXMLStdSerialize.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcCommandInternal::URpcCommandInternal(void)
 : ResponseStatus(0)
{

}

URpcCommandInternal::URpcCommandInternal(const std::string &request)
 : ResponseStatus(0), Request(request)
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
 IsPrepared=false;
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
void URpcCommandInternal::APrepareProcess(void)
{
 ResponseStatus = 2001;
 Response.clear();
}


/// Осуществляет декодирование основных данных и заполняет соответствующие поля
bool URpcCommandInternal::ADecodeBasicData(void)
{
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

 xml.ReadInteger("Id",CmdId);

 return true;
}
// --------------------------


}
#endif

