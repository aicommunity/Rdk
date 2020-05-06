#ifndef UServerTransportHttpH
#define UServerTransportHttpH

#include "UServerTransport.h"

namespace RDK{
class UApplication;

class RDK_LIB_TYPE UServerTransportHttp: public UServerTransport
{
protected: // Параметры

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UServerTransportHttp(void);
virtual ~UServerTransportHttp(void);
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
