#ifndef UCOMPONENT_DESCRIPTION_H
#define UCOMPONENT_DESCRIPTION_H

#include <string>
//#include <map>
#include "UXMLEnvSerialize.h"
#include <memory>

namespace RDK {

class UStorage;

class RDK_LIB_TYPE UComponentDescription
{
protected: // ������
// ��� ������
std::string ClassName;

// ������� �������� (���������) ������
std::string Header;

// ��������
std::string Description;

// ���������
UStorage* Storage;

public: // ������ �������� ����������
 /* ������� ����� �������� ���������� */
 class IException
 {
 public: // ������ ����������

 public: // ������
 // --------------------------
 // ������������ � �����������
 // --------------------------
 IException(void);
// IException(const UContainer *cont);
 IException(const IException &copy);
 virtual ~IException(void);
 // --------------------------
 };
/* **************************** */


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UComponentDescription(void);
UComponentDescription(const UComponentDescription &copy);
//UComponentDescription(std::string classname, UComponent *prototype);
virtual ~UComponentDescription(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ��� ������
const std::string& GetClassNameValue(void) const;
void SetClassNameValue(const std::string& value);

// ��������� ������
const std::string& GetHeader(void) const;
void SetHeader(const std::string& value);

// ��������
const std::string& GetDescription(void) const;
void SetDescription(const std::string& value);
// --------------------------

// ��������� ��������� �� ���������
void SetStorage(UStorage* storage);

// �������� �������� ������� �������� (����� � ���������)
virtual void CreateProperties();

// --------------------------
// ������ ����� ������ �������
// --------------------------
// ��������� ������ ������ � XML
virtual bool Save(USerStorageXML &xml);

// ��������� ������ ������ �� XML
virtual bool Load(USerStorageXML &xml);
// --------------------------

};


}

#endif

