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
#include <locale>
#include "USerStorage.h"

#ifndef _UNICODE
//#define RDK_UNICODE_RUN
//#define _UNICODE
#endif

#define _CRT_SECURE_NO_WARNINGS
#include "../../ThirdParty/xmlParser/xmlParser.h"
#undef _CRT_SECURE_NO_WARNINGS

#ifdef RDK_UNICODE_RUN
//#undef _UNICODE
//#undef RDK_UNICODE_RUN
#endif

namespace RDK {

class RDK_LIB_TYPE USerStorageXML: public USerStorage
{
protected: // Данные
// Корень сериализации
XMLNode RootNode;

// Текущий узел
XMLNode CurrentNode;

protected: // Временные переменные
#ifdef RDK_UNICODE_RUN
std::locale Locale;

mutable std::string SBuffer;
mutable std::wstring WBuffer,WBuffer2;
#endif


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

// Уничтожает xml
bool Destroy(void);

// Загружает xml из строки
bool Load(const std::string &str, const std::string &root);
bool LoadToNode(const std::string &str, const std::string &root, bool node_clear);
bool LoadToNode(USerStorageXML &node, bool node_clear);
bool LoadFieldsToNode(USerStorageXML &node, bool node_clear);

// Сохраняет xml в строку
bool Save(std::string &str) const;
bool SaveFromNode(std::string &str);

// Прочесть файл с диска
// Метод не делает ничего, если FileName == ""
bool LoadFromFile(const std::string &file_name, const std::string &root);

// Обновить файл на диске
// Метод не делает ничего, если FileName == ""
bool SaveToFile(const std::string &file_name);
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

// Если узел с таким именем существует в текущем узле то позиционируемся на него.
// иначе создаем такой узел
// Всегда позиционируется на 0 узел!
bool SelectNodeForce(const std::string &name);

// Аналогично SelectNodeForce, но позиционируется всегда от корневого узла,
// и поддерживает составное именование узла, с разделителем в виде '/'
bool SelectNodeRoot(const std::string &name);

// Возвращает имя узла
const std::string GetNodeName(void) const;

// Создает новый узел с заданным именем и позиционируется на него
bool AddNode(const std::string &name);

// Переименовывает текущий узел и продолжает указывать на него
void RenameNode(const std::string &newname);

// Удаляет текущий узел и устанавливает указатель уровнем выше
void DelNode(void);

// Удаляет содержимое узла
void DelNodeContent(void);

// Удаляет содержимое узла сохраняя сам узел
void DelNodeInternalContent(void);
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
// Дополнительные методы управления данными текущего элемента как ini-файлом
// --------------------------
template<typename T>
bool ReadData(const std::string &name, T &data);

template<typename T>
bool WriteData(const std::string &name, const T &data);

template<typename T>
bool WriteDataOnce(const std::string &name, const T &data);

// Считывает данные как соответствующий тип, если данное не найдено или не приводимо в
// ожидаемый тип - оно инициализируется значением по умолчанию
const std::string ReadString(const std::string &name, const std::string &default_value);
const std::string ReadString(const std::string &name, int node_index, const std::string &default_value);
const std::string ReadString(int node_index, const std::string &default_value);
int ReadInteger(const std::string &name, int default_value);
int ReadInteger(const std::string &name, int node_index, int default_value);
int ReadInteger(int node_index, int default_value);
double ReadFloat(const std::string &name, double default_value);
double ReadFloat(const std::string &name, int node_index, double default_value);
double ReadFloat(int node_index, double default_value);
bool ReadBool(const std::string &name, bool default_value);
bool ReadBool(const std::string &name, int node_index, bool default_value);
bool ReadBool(int node_index, bool default_value);

// Записывает данные как соответствующий тип
void WriteString(const std::string &name, const std::string &value);
void WriteInteger(const std::string &name, int value);
void WriteFloat(const std::string &name, double value);
void WriteBool(const std::string &name, bool value);
// --------------------------

// --------------------------
// Скрытые методы управления данными
// --------------------------
protected:
// --------------------------
};
	 /*
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, T &data)
{
 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, T &data)
{
 return storage;
}
         */

template<typename T>
bool USerStorageXML::ReadData(const std::string &name, T &data)
{
 if(!SelectNode(name))
  return false;
 operator >> (*this,data);
 //(*this)>>data;
 SelectUp();
 return true;
}

template<typename T>
bool USerStorageXML::WriteData(const std::string &name, const T &data)
{
 if(!AddNode(name))
  return false;

 operator << (*this,data);
 //(*this)<<data;
 SelectUp();
 return true;
}

template<typename T>
bool USerStorageXML::WriteDataOnce(const std::string &name, const T &data)
{
 if(!SelectNode(name))
  if(!AddNode(name))
   return false;

 operator << (*this,data);
 //(*this)<<data;
 SelectUp();
 return true;
}

}
#endif
