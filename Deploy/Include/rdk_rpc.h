#ifndef rdk_rpcH
#define rdk_rpcH

#include "rdk_init.h"

namespace RDK{

extern std::vector<std::string> RpcReturnString;

const char* RemoteCallInternal(const char *request, int &return_value);

}

#endif