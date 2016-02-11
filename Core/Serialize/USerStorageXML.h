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
protected: // ������
// ������ ������������
XMLNode RootNode;

// ������� ����
XMLNode CurrentNode;

protected: // ��������� ����������
#ifdef RDK_UNICODE_RUN
std::locale Locale;

mutable std::string SBuffer;
mutable std::wstring WBuffer,WBuffer2;
#endif


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

// ���������� xml
bool Destroy(void);

// ��������� xml �� ������
bool Load(const std::string &str, const std::string &root);
bool LoadToNode(const std::string &str, const std::string &root, bool node_clear);
bool LoadToNode(USerStorageXML &node, bool node_clear);
bool LoadFieldsToNode(USerStorageXML &node, bool node_clear);

// ��������� xml � ������
bool Save(std::string &str) const;
bool SaveFromNode(std::string &str);

// �������� ���� � �����
// ����� �� ������ ������, ���� FileName == ""
bool LoadFromFile(const std::string &file_name, const std::string &root);

// �������� ���� �� �����
// ����� �� ������ ������, ���� FileName == ""
bool SaveToFile(const std::string &file_name);
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

// ���� ���� � ����� ������ ���������� � ������� ���� �� ��������������� �� ����.
// ����� ������� ����� ����
// ������ ��������������� �� 0 ����!
bool SelectNodeForce(const std::string &name);

// ���������� SelectNodeForce, �� ��������������� ������ �� ��������� ����,
// � ������������ ��������� ���������� ����, � ������������ � ���� '/'
bool SelectNodeRoot(const std::string &name);

// ���������� ��� ����
const std::string GetNodeName(void) const;

// ������� ����� ���� � �������� ������ � ��������������� �� ����
bool AddNode(const std::string &name);

// ��������������� ������� ���� � ���������� ��������� �� ����
void RenameNode(const std::string &newname);

// ������� ������� ���� � ������������� ��������� ������� ����
void DelNode(void);

// ������� ���������� ����
void DelNodeContent(void);

// ������� ���������� ���� �������� ��� ����
void DelNodeInternalContent(void);
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
// �������������� ������ ���������� ������� �������� �������� ��� ini-������
// --------------------------
template<typename T>
bool ReadData(const std::string &name, T &data);

template<typename T>
bool WriteData(const std::string &name, const T &data);

template<typename T>
bool WriteDataOnce(const std::string &name, const T &data);

// ��������� ������ ��� ��������������� ���, ���� ������ �� ������� ��� �� ��������� �
// ��������� ��� - ��� ���������������� ��������� �� ���������
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

// ���������� ������ ��� ��������������� ���
void WriteString(const std::string &name, const std::string &value);
void WriteInteger(const std::string &name, int value);
void WriteFloat(const std::string &name, double value);
void WriteBool(const std::string &name, bool value);
// --------------------------

// --------------------------
// ������� ������ ���������� �������
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
