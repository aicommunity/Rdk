#ifndef URpcCommand_H
#define URpcCommand_H

#include "UProject.h"

namespace RDK {

class RDK_LIB_TYPE URpcCommand
{
public: // Данные
/// Идентификатор получателя ответа
std::string RecepientId;

/// Идентификатор команды
unsigned CmdId;

/// Имя функции
std::string FunctionName;

/// Индекс канала
int ChannelIndex;

/// Имя компонента
std::string ComponentName;

/// Флаг, сообщающий, что команда была подготовлена к обработке
bool IsPrepared;

/// Флаг, сообщающий, что команда была декодирована
bool IsDecoded;

/// Флаг, сообщающий, что команда была обработана
bool IsProcessed;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcCommand(void);
virtual ~URpcCommand(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Идентификатор получателя ответа
const std::string& GetRecepientId(void) const;

/// Идентификатор команды
unsigned GetCmdId(void) const;

/// Имя функции
const std::string& GetFunctionName(void) const;

/// Индекс канала
int GetChannelIndex(void) const;

/// Имя компонента
const std::string& GetComponentName(void) const;
// --------------------------

// --------------------------
// Методы декодирования
// --------------------------
/// Инициализирует процесс обработки новой команды
virtual void PrepareProcess(void);
virtual void APrepareProcess(void)=0;

/// Осуществляет декодирование основных данных и заполняет соответствующие поля
virtual bool DecodeBasicData(void);
virtual bool ADecodeBasicData(void)=0;
// --------------------------

};


}

#endif

