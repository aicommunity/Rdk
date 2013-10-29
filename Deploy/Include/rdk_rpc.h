#ifndef rdk_rpcH
#define rdk_rpcH

#include "rdk_init.h"
#include "../PtzLib/ptzlib_init.h"
#include "myrdk.h"

namespace RDK{

extern std::vector<std::string> RpcReturnString;

const char* RemoteCallInternal(const char *request, int &return_value);

const char* PtzRemoteCall(const char *request, int &return_value);


/// Возвращает набор из имени компонента и индекса канала
/// Если определен параметр "CC"
/// Содержимое параметра представляет собой
/// текст вида: индекс_канала@Имя компонента
bool ExtractCC(USerStorageXML &xml, int &channel_index, std::string &component_name);

/// Возвращает имя компонента
/// Если определен параметр "Component"
bool ExtractComponent(USerStorageXML &xml, std::string &component_name);

/// Возвращает индекса канала
/// Если определен параметр "Channel"
bool ExtractChannel(USerStorageXML &xml, int &channel_index);

/// Возвращает имя команды
/// Если определен параметр "Cmd"
bool ExtractCmd(USerStorageXML &xml, std::string &cmd_name);

}

#endif
