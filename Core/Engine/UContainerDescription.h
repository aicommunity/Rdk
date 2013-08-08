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
//static std::map<std::string, UPropertyDescription> CommonProperties;

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
/*static const UPropertyDescription& GetCommonProperty(const std::string &name);
static bool SetCommonProperty(const std::string &name, const UPropertyDescription& value);

// ��������� ������� ������ �������� � �������� ������
static bool CheckCommonProperty(const std::string &name);
*/
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// �������� ��������
const UPropertyDescription& GetDescription(const std::string &name);
bool SetDescription(const std::string &name, const UPropertyDescription& value, bool force=false);

// ������� �������� �� ����� ��������, ���� �� ���� � ����� ���������
bool RemoveCommonDuplicatesDescription(const std::map<std::string, UPropertyDescription> &common_descriptions, const std::string &name);

// ������� ��� �������� �� ����� ��������, ���� ��� ���� � ����� ���������
bool RemoveCommonDuplicatesDescriptions(const std::map<std::string, UPropertyDescription> &common_descriptions);
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
/*
// ��������� ������ ������ � XML
static bool SaveCommon(USerStorageXML &xml);

// ��������� ������ ������ �� XML
static bool LoadCommon(USerStorageXML &xml);
*/
// --------------------------
};


}

#endif

