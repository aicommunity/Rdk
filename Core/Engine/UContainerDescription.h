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

// ��� ������ ��������
std::string Type;

// ��� ������ ��������� ������ ��������
// 0 - ������������ ������
// 1 - Checkbox
// 2 - ��������
// 3 - ������ ���������
// 4 - �������� � �������� �����
int DataSelectionType;

/// ������ ��������
/// � ������ ���������, ������ ������� �� ���� ��������
/// ������ � ����� ���������
std::vector<std::string> ValueList;

/// ��� �� ���������
std::string Step;


public: // ������
UPropertyDescription(void);

};

class UContainerDescription: public UComponentDescription
{
protected: // ������
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
};


}

#endif

