#ifndef UCONTAINER_DESCRIPTION_H
#define UCONTAINER_DESCRIPTION_H

#include <map>
#include "UComponentDescription.h"

namespace RDK {

// ����� �������� �������� ����������
class UPropertyDescription
{
public: // ������
// ��������� ��������
std::string Header;

// �������� ��������
std::string Description;


public: // ������

};

class UContainerDescription: public UComponentDescription
{
protected: // ������
// �������� ����� �������
static std::map<std::string, UPropertyDescription> CommonProperties;

// �������� �������
std::map<std::string, UPropertyDescription> Properties;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UContainerDescription(void);
UContainerDescription(const UContainerDescription &copy);
virtual ~UContainerDescription(void);
// --------------------------

// --------------------------
// ������ ���������� ������ ����������
// --------------------------
// �������� ������ ��������
static const UPropertyDescription& GetCommonProperty(const std::string &name);
static bool SetCommonProperty(const std::string &name, const UPropertyDescription& value);

// ��������� ������� ������ �������� � �������� ������
static bool CheckCommonProperty(const std::string &name);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// �������� ��������
const UPropertyDescription& GetProperty(const std::string &name);
bool SetProperty(const std::string &name, const UPropertyDescription& value, bool force=false);

// ������� �������� �� ����� ��������, ���� �� ���� � ����� ���������
bool RemoveCommonDuplicatesProperty(const std::string &name);

// ������� ��� �������� �� ����� ��������, ���� ��� ���� � ����� ���������
bool RemoveCommonDuplicatesProperties(void);
// --------------------------

// --------------------------
// ������ ����� ������ ������
// --------------------------
// ��������� ������ ������ � XML
virtual bool Save(USerStorageXML &xml);

// ��������� ������ ������ �� XML
virtual bool Load(USerStorageXML &xml);
// --------------------------

// --------------------------
// ������ ����� ������ ����� ������
// --------------------------
// ��������� ������ ������ � XML
static bool SaveCommon(USerStorageXML &xml);

// ��������� ������ ������ �� XML
static bool LoadCommon(USerStorageXML &xml);
// --------------------------
};


}

#endif

