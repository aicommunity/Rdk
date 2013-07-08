/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UMODULE_H
#define UMODULE_H

namespace RDK {

class UModule
{
protected: // ����� �������� ���������
/// ���� ���������� ������� � �����
/// ���� false, �� ����� ��������� ����� ����� ������ ����� build
bool Ready;

/// ���� ������������� �������
/// ���� false - ������ ����� ������������������
bool InitFlag;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UModule(void);
virtual ~UModule(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
/// ���� ���������� ������� � �����
bool IsReady(void) const;
/// ���� ���������� ������� � ��������� �������������
bool IsInit(void) const;
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
/// �������������� �������� �� ��������� � ����� �������� �����
virtual bool BeforeDefault(void);
virtual bool AfterDefault(void);
virtual bool Default(void);

/// ������������ ������ ���������� ��������� �������
/// ����� ��������� ����������
/// ������������� �������� ����� Reset() � ���������� Ready � true
/// � ������ �������� ������
virtual bool BeforeBuild(void);
virtual bool AfterBuild(void);
virtual bool Build(void);

/// ����� �������� ����� ��� ������ ��������
virtual bool BeforeReset(void);
virtual bool AfterReset(void);
virtual bool Reset(void);

/// ��������� ������ ����� �������
virtual bool BeforeCalculate(void);
virtual bool AfterCalculate(void);
virtual bool Calculate(void);

/// ��������� ������������� ����� �������
virtual void BeforeInit(void);
virtual void AfterInit(void);
virtual void Init(void);

/// ��������� ��������������� ����� �������
virtual void BeforeUnInit(void);
virtual void AfterUnInit(void);
virtual void UnInit(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// ����������� � �������� �������
// --------------------------
protected:
/// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ABeforeDefault(void);
virtual bool AAfterDefault(void);
virtual bool ADefault(void);

/// ������������ ������ ���������� ��������� �������
/// ����� ��������� ����������
/// ������������� �������� ����� Reset() � ���������� Ready � true
/// � ������ �������� ������
virtual bool ABeforeBuild(void);
virtual bool AAfterBuild(void);
virtual bool ABuild(void);

/// ����� �������� �����.
virtual bool ABeforeReset(void);
virtual bool AAfterReset(void);
virtual bool AReset(void);

/// ��������� ������ ����� �������
virtual bool ABeforeCalculate(void);
virtual bool AAfterCalculate(void);
virtual bool ACalculate(void);

/// ��������� ������������� ����� �������
virtual void ABeforeInit(void);
virtual void AAfterInit(void);
virtual void AInit(void);

/// ��������� ��������������� ����� �������
virtual void ABeforeUnInit(void);
virtual void AAfterUnInit(void);
virtual void AUnInit(void);
// --------------------------
};

}

#endif

