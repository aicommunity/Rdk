#ifndef UCONTAINER_DESCRIPTION_H
#define UCONTAINER_DESCRIPTION_H

#include <map>
#include "UComponentDescription.h"

namespace RDK {

// ����� �������� ��������� ����������
class UParameterDescription
{
public: // ������
// ��������� ���������
std::string Header;

// �������� ���������
std::string Description;


public: // ������

};

class UContainerDescription: public UComponentDescription
{
protected: // ������
// �������� ����� ����������
static std::map<std::string, UParameterDescription> CommonParameters;

// �������� ����������
std::map<std::string, UParameterDescription> Parameters;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UContainerDescription(void);
UContainerDescription(const UContainerDescription &copy);
virtual ~UContainerDescription(void);
// --------------------------

// --------------------------
// ������ ���������� ������ �����������
// --------------------------
// �������� ������ ���������
static const UParameterDescription& GetCommonParameter(const std::string &name);
static bool SetCommonParameter(const std::string &name, const UParameterDescription& value);

// ��������� ������� ������ ��������� � �������� ������
static bool CheckCommonParameter(const std::string &name);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// �������� ���������
const UParameterDescription& GetParameter(const std::string &name);
bool SetParameter(const std::string &name, const UParameterDescription& value, bool force=false);

// ������� �������� �� ����� ��������, ���� �� ���� � ����� ���������
bool RemoveCommonDuplicatesParameter(const std::string &name);

// ������� ��� ��������� �� ����� ��������, ���� ��� ���� � ����� ���������
bool RemoveCommonDuplicatesParameters(void);
// --------------------------

// --------------------------
// ������ ����� ������ ������
// --------------------------
// ��������� ������ ������ � XML
virtual bool Save(Serialize::USerStorageXML &xml);

// ��������� ������ ������ �� XML
virtual bool Load(Serialize::USerStorageXML &xml);
// --------------------------

// --------------------------
// ������ ����� ������ ����� ������
// --------------------------
// ��������� ������ ������ � XML
static bool SaveCommon(Serialize::USerStorageXML &xml);

// ��������� ������ ������ �� XML
static bool LoadCommon(Serialize::USerStorageXML &xml);
// --------------------------
};


}

#endif

