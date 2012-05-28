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
protected: // ������
// ������ ������������
XMLNode RootNode;

// ������� ����
XMLNode CurrentNode;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
// ������� ������ �������
USerStorageXML(void);
virtual ~USerStorageXML(void);
// --------------------------

// --------------------------
// ������ �������� xml
// --------------------------
// ������� ����� xml � �������� ���� ��������� ������ ������
bool Create(const std::string &rootname);

// ��������� xml �� ������
bool Load(const std::string &str, const std::string &root);
bool LoadToNode(const std::string &str, const std::string &root);

// ��������� xml � ������
bool Save(std::string &str);
bool SaveFromNode(std::string &str);
// --------------------------

// --------------------------
// ������ ���������� �������� �������� ����
// --------------------------
// ��������������� �� �������� ����
void SelectRoot(void);

// ��������������� �� ������������ ����
void SelectUp(void);

// ���������� ����� ����� � �������� ������
int GetNumNodes(const std::string &name);

// ���������� ����� �����
int GetNumNodes(void);

// ���� ���� � ����� ������ � �������� ���������� � ������� ���� �� ��������������� �� ����.
// index �������� �� ����� �������������� ��������
// ���� � ����� ������ � �������� �� ����������, �� ���������� ���� � �����
// ������ � �������� ������� �� 1 - �� ������� ����
bool SelectNode(const std::string &name, int index=0);

bool SelectNode(int index);

// ���������� ��� ����
const std::string GetNodeName(void) const;

// ������� ����� ���� � �������� ������ � ��������������� �� ����
bool AddNode(const std::string &name);

// ��������������� ������� ���� � ���������� ��������� �� ����
void RenameNode(const std::string &newname);

// ������� ������� ���� � ������������� ��������� ������� ����
void DelNode(void);
// --------------------------

// --------------------------
// ������ ���������� ������� �������� ��������
// --------------------------
// ������������� �������� �������� ����
bool SetNodeAttribute(const std::string &name, const std::string &value);

// ���������� �������� �������� ����
const std::string GetNodeAttribute(const std::string &name) const;

// ������� �������� �������
void DelNodeAttribute(const std::string &name);

// ������������� ����� ����
bool SetNodeText(const std::string &text);

// ���������� �������� ����
const std::string GetNodeText(void) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
// --------------------------
};

}
}
#endif
