#ifndef URpcDecoderCommonH
#define URpcDecoderCommonH


#include "URpcDecoder.h"

namespace RDK {

///
class RDK_LIB_TYPE URpcDecoderCommon: public URpcDecoder
{

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoderCommon(void);
virtual ~URpcDecoderCommon(void);
// --------------------------

// --------------------------
// Методы управления командами
// --------------------------
/// Проверяет, поддерживается ли команда диспетчером
/// ожидает, что команда уже декодирована иначе всегда возвращает false
virtual bool IsCmdSupported(const UEPtr<URpcCommand> &command) const;

/// Создает копию этого декодера
virtual URpcDecoderCommon* New(void);

/// Осуществляет декодирование и вызов команды по текущим данным
/// Возвращает false если команда не поддерживается
virtual bool AProcessCommand(const UEPtr<URpcCommand> &command);

virtual const char* RemoteCall(const char *request, int &return_value, int &channel_index);
virtual std::string ARemoteCall(const std::string &cmd, RDK::USerStorageXML &xml, const std::string &component_name, int engine_index, int &return_value);
// --------------------------
};

}

#endif
