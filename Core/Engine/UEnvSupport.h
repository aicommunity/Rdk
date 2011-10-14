/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UEnvSupportH
#define UEnvSupportH

#include <string>
#include "UAEnvSupport.h"


#include "../Serialize/Serialize.h"

namespace RDK {

typedef std::string NameT;
typedef Serialize::USerStorage UVariableData;
extern NameT ForbiddenName;

class UAContainer;

// ����� �������� ��������� ����������
class UIPointer
{
protected: // ������

public:
virtual UAContainer* const Get(void) const=0;

virtual void Set(UAContainer* source)=0;

virtual void Del(UAContainer* source)=0;

// ���������, ���������� �� ����� ��������� � ���� ������
// ���������� 0 ���� ��, � <0 ���� ���
virtual int Find(const UAContainer * cont) const=0;

// -----------------
// ���������
// -----------------
UIPointer& operator = (UAContainer *source)
{
 Set(source);
 return *this;
};
// -----------------
};

// ����� ������������ �������
class UIProperty
{
public:
 // ����� ���������� ��������� ��� ��������
 virtual const std::string& GetName(void) const=0;

 // ����� ���������� ��������� ��� ������-��������� ��������
 virtual std::string GetOwnerName(void) const=0;

 // ����� ���������� �������� �������� � �����
 virtual bool Save(Serialize::USerStorage *storage)=0;

 // ����� ������ �������� �������� �� ������
 virtual bool Load(Serialize::USerStorage *storage)=0;
};



// ��������� ������� ���������
struct UVariable
{
 // Id ���������
 UId Id;

 // ��������� �� ��������
 UIProperty* Property;

 // ���� ���������� �������� ������ �� ������� ��������� Property
 bool DelEnable;

// --------------------------
// ������������ � �����������
// --------------------------
UVariable(void);
UVariable(UId id, UIProperty *prop);
UVariable(const UVariable &copy);
virtual ~UVariable(void);
// --------------------------
};

// ��������� ������� ���������
struct UPVariable
{
 // Id ���������
 UId Id;

 // ��������� �� ��������
 UIPointer* Pointer;

// --------------------------
// ������������ � �����������
// --------------------------
UPVariable(void);
UPVariable(UId id, UIPointer *prop);
virtual ~UPVariable(void);
// --------------------------
};

// ��������� ����� ������ ���������������
ULongId& operator << (ULongId& id, const std::string &str);
std::string& operator >> (const ULongId& id, std::string &str);

}
#endif
