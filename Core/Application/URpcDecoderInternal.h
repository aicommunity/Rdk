#ifndef URpcDecoderInternal_H
#define URpcDecoderInternal_H

#include "URpcDecoder.h"

namespace RDK {

/// 
class RDK_LIB_TYPE URpcDecoderInternal: public URpcDecoder
{

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoderInternal(void);
virtual ~URpcDecoderInternal(void);
// --------------------------

// --------------------------
// Методы управления командами
// --------------------------
/// Проверяет, поддерживается ли команда диспетчером
/// ожидает, что команда уже декодирована иначе всегда возвращает false
virtual bool IsCmdSupported(const UEPtr<URpcCommand> &command) const;

/// Создает копию этого декодера
virtual URpcDecoderInternal* New(void);

/// Осуществляет декодирование и вызов команды по текущим данным
/// Возвращает false если команда не поддерживается
virtual bool AProcessCommand(const UEPtr<URpcCommand> &command);
// --------------------------
};

}

#endif

