#ifndef UIVisualController_H
#define UIVisualController_H
  
#include "../../Core/Graphics/UBitmap.h"
#include "../../Core/Graphics/UAGraphics.h"
#include "../../Core/Graphics/UGraphicsIO.h"
#include "../../Core/Graphics/UGraphics.h"
#include "../../Core/Graphics/UFont.h"
  
namespace RDK {

// ���������� ��������� �������
extern RDK::UBitmapFontCollection GlobalFonts;

// ������� ��������� ���������� �������
void ClearClobalFonts(void);

// ��������� ����� ���������� �����
bool AddGlobalFont(const std::string &font_file_name);

// ����� ��������-���������� �����������
class UIVisualController
{
public:
// �����, ���������� ����� ������� ������
virtual void BeforeReset(void)=0;

// �����, ���������� ����� ������ ������
virtual void AfterReset(void)=0;

// �����, ���������� ����� ����� �������
virtual void BeforeCalculate(void)=0;

// �����, ���������� ����� ���� �������
virtual void AfterCalculate(void)=0;

// ���������� ����������
virtual void UpdateInterface(bool force_update=true)=0;

// ������� ���������� � �������� ���������
virtual void ClearInterface(void)=0;

// ���������� ���������� ��� ����������
virtual std::string GetName(void)=0;

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
};

// ����� ���������-���������� �����������
class UIVisualControllerStorage
{
public:
// ������ ������������, ������� ������ ���� ������� ����� �������
static std::vector<RDK::UIVisualController*> InterfaceUpdaters;

public:
// ��������� ���������� � ������
static void AddInterface(RDK::UIVisualController *value);

// ������� ���������� �� ������
static void DelInterface(RDK::UIVisualController *value);

// �����, ���������� ����� ������� ������
static void BeforeReset(void);

// �����, ���������� ����� ������ ������
static void AfterReset(void);

// �����, ���������� ����� ����� �������
static void BeforeCalculate(void);

// �����, ���������� ����� ���� �������
static void AfterCalculate(void);

// ���������� ����������
static void UpdateInterface(void);

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
};

}

#endif

