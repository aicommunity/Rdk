#ifndef URpcCommandInternal_H
#define URpcCommandInternal_H

#include "URpcCommand.h"

namespace RDK {

class RDK_LIB_TYPE URpcCommandInternal: public URpcCommand
{
public: // Входные данные
/// Строка с xml-данными запроса
std::string Request;

public: // Выходные данные
/// Строка с xml-данными ответа
std::string Response;

/// Статус выполнения команды
int ResponseStatus;


public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcCommandInternal(void);
URpcCommandInternal(const std::string &request);
virtual ~URpcCommandInternal(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// --------------------------

// --------------------------
// Методы декодирования
// --------------------------
/// Задает строку запроса
virtual void SetRequest(const std::string &request);

/// Строка с xml-данными ответа
const std::string& GetResponse(void) const;

/// Статус выполнения команды
int GetResponseStatus(void) const;

/// Инициализирует процесс обработки новой команды
void APrepareProcess(void);

/// Осуществляет декодирование основных данных и заполняет соответствующие поля
virtual bool ADecodeBasicData(void);
// --------------------------
};


}

#endif

