#ifndef UCONTAINER_DESCRIPTION_H
#define UCONTAINER_DESCRIPTION_H

#include <map>
#include <vector>
#include "UComponentDescription.h"

namespace RDK {

// ����� �������� �������� ����������
class RDK_LIB_TYPE UPropertyDescription
{
public: // ������
// ������� �������� (���������) ��������
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

/// ��� ��������
unsigned int PropertyType;


public: // ������
UPropertyDescription(void);

};

class RDK_LIB_TYPE UContainerDescription: public UComponentDescription
{
protected: // ������
// �������� �������
std::map<std::string, UPropertyDescription> Properties;

std::map<std::string, std::string> Favorites;

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
const UPropertyDescription& GetPropertyDescription(const std::string &name);
bool SetPropertyDescription(const std::string &name, const UPropertyDescription& value, bool force=false);

// ������� �������� �� ����� ��������, ���� �� ���� � ����� ���������
bool RemoveCommonDuplicatesDescription(const std::unordered_map<std::string, UPropertyDescription> &common_descriptions, const std::string &name);

// ������� ��� �������� �� ����� ��������, ���� ��� ���� � ����� ���������
bool RemoveCommonDuplicatesDescriptions(const std::unordered_map<std::string, UPropertyDescription> &common_descriptions);

// �������� �������� ������� �������� (����� � ���������)
virtual void CreateProperties();

const std::map<std::string, UPropertyDescription>& GetProperties();

// Favorites
const std::map<std::string, std::string>& GetFavorites();

void AddNewFavorite(const std::string& name, const std::string& path);

void DeleteFavorite(const std::string& name);
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

