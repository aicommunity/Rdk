#ifndef URpcDecoderInternal_H
#define URpcDecoderInternal_H

#include "URpcDecoder.h"

namespace RDK {

/// 
class URpcDecoderInternal: public URpcDecoder
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
/// Создает копию этого декодера
virtual URpcDecoderInternal* New(void);

/// Осуществляет декодирование и вызов команды по текущим данным
/// Возвращает false если команда не поддерживается
virtual bool ProcessCommand(UEPtr<URpcCommand> &command);
// --------------------------
};

}

#endif

