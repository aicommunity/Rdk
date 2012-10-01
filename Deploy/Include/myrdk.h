#ifndef MYRDK_H
#define MYRDK_H

#include "../../Core/System/rdk_system.h"
#include "../../ThirdParty/ThirdParty.h"
#include "../../Core/Math/UMath.h"
#include "../../Core/Utilities/Utilities.h"
#include "../../Core/Serialize/Serialize.h"

#include "../../Core/Engine/UAEnvSupport.h"
#include "../../Core/Engine/UXMLEnvSerialize.h"
#include "../../Core/Engine/UBinaryEnvSerialize.h"
#include "../../Core/Graphics/UBitmap.h"
#include "../../Core/Graphics/UAGraphics.h"
#include "../../Core/Graphics/UGraphicsIO.h"
#include "../../Core/Graphics/UGraphics.h"
#include "../../Core/Graphics/UFont.h"
#include "../../Core/Graphics/UDrawEngine.h"
#include "../../Core/Graphics/UGraphicsXMLSerialize.h"
#include "../../Core/Graphics/UGraphicsBinarySerialize.h"

namespace RDK {

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
virtual void UpdateInterface(void)=0;

// ���������� ���������� ��� ����������
virtual std::string GetName(void)=0;

// ��������� ��������� ���������� � xml
virtual void SaveParameters(RDK::Serialize::USerStorageXML &xml)=0;

// ��������� ��������� ���������� �� xml
virtual void LoadParameters(RDK::Serialize::USerStorageXML &xml)=0;
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

// ��������� ��������� ���������� � xml
static void SaveParameters(RDK::Serialize::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
static void LoadParameters(RDK::Serialize::USerStorageXML &xml);
};

}
#endif

