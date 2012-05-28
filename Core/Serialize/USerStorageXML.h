/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef USerStorage_XMLH
#define USerStorage_XMLH

#include <string>
#include "USerStorage.h"
#undef _XMLWINDOWS
#define XML_NO_WIDE_CHAR
#define _CRT_SECURE_NO_WARNINGS
#include "../../ThirdParty/xmlParser/xmlParser.h"
#undef _CRT_SECURE_NO_WARNINGS

namespace RDK {
namespace Serialize {

//class USerStorageXMLBinary;
//class USerStorageXMLXML;

class USerStorageXML: public USerStorage
{
protected: // Данные
// Корень сериализации
XMLNode RootNode;

// Текущий узел
XMLNode CurrentNode;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Создает пустую очередь
USerStorageXML(void);
virtual ~USerStorageXML(void);
// --------------------------

// --------------------------
// Методы загрузки xml
// --------------------------
// Создает новый xml в корневом узле уничтожая старые данные
bool Create(const std::string &rootname);

// Загружает xml из строки
bool Load(const std::string &str, const std::string &root);
bool LoadToNode(const std::string &str, const std::string &root);

// Сохраняет xml в строку
bool Save(std::string &str);
bool SaveFromNode(std::string &str);
// --------------------------

// --------------------------
// Методы управления позицией текущего узла
// --------------------------
// Позиционируется на корневой узел
void SelectRoot(void);

// Позиционируется на родительский узел
void SelectUp(void);

// Возвращает число узлов с заданным именем
int GetNumNodes(const std::string &name);

// Возвращает число узлов
int GetNumNodes(void);

// Если узел с таким именем и индексом существует в текущем узле то позиционируемся на него.
// index указвает на номер повторяющегося элемента
// Если с таким именем и индексом не существует, но существует узел с таким
// именем и индексом меньшим на 1 - то создаем узел
bool SelectNode(const std::string &name, int index=0);

bool SelectNode(int index);

// Возвращает имя узла
const std::string GetNodeName(void) const;

// Создает новый узел с заданным именем и позиционируется на него
bool AddNode(const std::string &name);

// Переименовывает текущий узел и продолжает указывать на него
void RenameNode(const std::string &newname);

// Удаляет текущий узел и устанавливает указатель уровнем выше
void DelNode(void);
// --------------------------

// --------------------------
// Методы управления данными текущего элемента
// --------------------------
// Устанавливает значение атрибута узла
bool SetNodeAttribute(const std::string &name, const std::string &value);

// Возвращает значение атрибута узла
const std::string GetNodeAttribute(const std::string &name) const;

// Удаляет заданный атрибут
void DelNodeAttribute(const std::string &name);

// Устанавливает текст узла
bool SetNodeText(const std::string &text);

// Возвращает значение узла
const std::string GetNodeText(void) const;
// --------------------------

// --------------------------
// Скрытые методы управления данными
// --------------------------
protected:
// --------------------------
};

}
}
#endif
