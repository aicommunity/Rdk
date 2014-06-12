#ifndef URpcCommand_H
#define URpcCommand_H

#include "UProject.h"

namespace RDK {

class URpcCommand
{
public: // Данные
/// Идентификатор команды
unsigned CmdId;

/// Имя функции
std::string FunctionName;

/// Индекс канала
int ChannelIndex;

/// Имя компонента
std::string ComponentName;

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

/// Осуществляет декодирование основных данных и заполняет соответствующие поля
virtual bool DecodeBasicData(void)=0;
// --------------------------

};


}

#endif

