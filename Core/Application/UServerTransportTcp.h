#ifndef UServerTransportTcpH
#define UServerTransportTcpH

#include "UServerTransport.h"

namespace RDK{
class UApplication;

class RDK_LIB_TYPE UServerTransportTcp: public UServerTransport
{
protected: // Параметры

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UServerTransportTcp(void);
virtual ~UServerTransportTcp(void);
// --------------------------

// --------------------------
// Методы транспортировки данных
// --------------------------

/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
//virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);
};
















}//namespace RDK
#endif
