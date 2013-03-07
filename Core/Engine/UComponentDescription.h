#ifndef UCOMPONENT_DESCRIPTION_H
#define UCOMPONENT_DESCRIPTION_H

#include <string>
//#include <map>
#include "../Serialize/USerStorageXML.h"

namespace RDK {

//class UComponent;

class UComponentDescription
{
protected: // ������
// ��� ������
std::string ClassName;

// ��������� ������
std::string Header;

// ��������
std::string Description;

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

// --------------------------
// ������ ����� ������ �������
// --------------------------
// ��������� ������ ������ � XML
virtual bool Save(Serialize::USerStorageXML &xml);

// ��������� ������ ������ �� XML
virtual bool Load(Serialize::USerStorageXML &xml);
// --------------------------

};


}

#endif

