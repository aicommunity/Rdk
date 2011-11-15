#ifndef UEXCEPTION_H
#define UEXCEPTION_H

namespace RDK {

/* Базовый класс исключений */
class UException
{

};

/* Фатальные ошибки (обращение по 0 указателям и т.п.) */
class UFatalException: public UException
{

};

/* Ошибки, корректируемые пользователем */
class UErrException: public UException
{

};

/* Предупреждения (например об неэффективном использовании ресурсов) */
class UWarningException: public UException
{

};

/* Информационные сообщения, выдача которых инициируется пользователем */
class UInfoException: public UException
{

};

}

#endif

