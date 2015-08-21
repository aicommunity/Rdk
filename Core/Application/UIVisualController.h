#ifndef UIVisualController_H
#define UIVisualController_H

#include "../../Core/Graphics/UBitmap.h"
#include "../../Core/Graphics/UAGraphics.h"
#include "../../Core/Graphics/UGraphicsIO.h"
#include "../../Core/Graphics/UGraphics.h"
#include "../../Core/Graphics/UFont.h"
#include "../../Core/Serialize/USerStorageXML.h"
#include "../../Core/Engine/UELockVar.h"

namespace RDK {

// ���������� ��������� �������
extern RDK_LIB_TYPE RDK::UBitmapFontCollection GlobalFonts;
//extern RDK_LIB_TYPE RDK::UELockVar<RDK::UBitmapFontCollection> GlobalFonts;

// ������� ��������� ���������� �������
RDK_LIB_TYPE void ClearClobalFonts(void);

// ��������� ����� ���������� �����
RDK_LIB_TYPE bool AddGlobalFont(const std::string &font_file_name);

// ����� ��������-���������� �����������
class RDK_LIB_TYPE UIVisualController
{
public:
// �����, ���������� ����� �������� �������
virtual void AfterLoadProject(void)=0;

// �����, ���������� ����� ������� ������
virtual void BeforeReset(void)=0;

// �����, ���������� ����� ������ ������
virtual void AfterReset(void)=0;

// �����, ���������� ����� ����� �������
virtual void BeforeCalculate(void)=0;

// �����, ���������� ����� ���� �������
virtual void AfterCalculate(void)=0;

// ���������� ����������
virtual void UpdateInterface(bool force_update=false)=0;

// ������� ���������� � �������� ���������
virtual void ClearInterface(void)=0;

// ���������� ���������� ��� ����������
virtual std::string GetName(void)=0;

// ���������� ��� ������ ����������
virtual std::string GetClassName(void)=0;

// ���������� �������� ���������� ����������
virtual long GetUpdateInterval(void)=0;

// ���������� ���� ���������� ���������� ���������� ���� ���� �� �� �����
virtual bool GetAlwaysUpdateFlag(void)=0;

// ��������� ��������� ���������� � xml
virtual void SaveParameters(RDK::USerStorageXML &xml)=0;

// ��������� ��������� ���������� �� xml
virtual void LoadParameters(RDK::USerStorageXML &xml)=0;

// ��������� ������ ���������� �����������
/// ���������� ���� ��������� ����������� � ���� �������� �����
virtual void ResetCalculationStepUpdatedFlag(void)=0;

/// ���������� ���� ��������� ����������� � ���� �������� �����
virtual void SetCalculationStepUpdatedFlag(void)=0;

/// ���������� ��������� ����� ��������� ����������� � ���� �������� �����
virtual bool GetCalculationStepUpdatedFlag(void)=0;

/// ���������� ����� ���������� ���������� (��)
virtual unsigned long long GetUpdateTime(void)=0;
};

// ����� ���������-���������� �����������
class RDK_LIB_TYPE UIVisualControllerStorage
{
public:
// ������ ������������, ������� ������ ���� ������� ����� �������
static std::vector<RDK::UIVisualController*> InterfaceUpdaters;

/// ����� ����� ���������� ����������
static unsigned long long UpdateTime;

public:
// ��������� ���������� � ������
static void AddInterface(RDK::UIVisualController *value);

// ������� ���������� �� ������
static void DelInterface(RDK::UIVisualController *value);

// �����, ���������� ����� �������� �������
static void AfterLoadProject(void);

// �����, ���������� ����� ������� ������
static void BeforeReset(void);

// �����, ���������� ����� ������ ������
static void AfterReset(void);

// �����, ���������� ����� ����� �������
static void BeforeCalculate(void);

// �����, ���������� ����� ���� �������
static void AfterCalculate(void);

// ���������� ����������
static void UpdateInterface(bool force_update=false);

// ������� ���������� � �������� ���������
static void ClearInterface(void);

// ��������� ��������� ���������� � xml
static void SaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
static void LoadParameters(RDK::USerStorageXML &xml);

// ��������� ������ ���������� �����������
/// ���������� ���� ��������� ����������� � ���� �������� �����
static void ResetCalculationStepUpdatedFlag(void);

/// ���������� ���� ��������� ����������� � ���� �������� �����
static void SetCalculationStepUpdatedFlag(void);

/// ���������� ����� ���������� ���������� (��)
static unsigned long long GetUpdateTime(void);

};

}

#endif

