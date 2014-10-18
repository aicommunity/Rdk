/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UControllerH
#define UControllerH

#include <vector>
#include "UModule.h"

namespace RDK {

class UContainer;

class RDK_LIB_TYPE UController: public RDK::UModule
{
protected: // ���������
// ���� ���������� ���������� ����������
bool Enabled;

protected: // ������
// ��������� �� ����������
std::vector<UContainer*> Components;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UController(void);
virtual ~UController(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���� ���������� ���������� ����������
bool IsEnabled(void) const;
bool IsEnabled(bool value);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���������� ����� ��������� � ������� ��������� ����������
size_t GetNumComponents(void) const;

// ���������� ��������� � �������� ��������� ���������� �� �������
UContainer* GetComponents(size_t index);

// ���������� ������ ���������� �� ���������
int FindComponent(UContainer* component) const;
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// ��������� ��������� � �����������
virtual bool Link(UContainer* component, bool forchilds=false);

// ���������� ��������� �� ����������
virtual bool UnLink(int index, bool forchilds=false);
virtual bool UnLink(UContainer* component, bool forchilds=false);
virtual bool UnLinkAll(bool forchilds=false);

// ��������� ���������
virtual bool Update(void);
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
protected:
// ��������� ��������� � �����������
virtual bool ALink(UContainer* component, bool forchilds=false)=0;

// ���������� ��������� �� ����������
virtual bool AUnLink(int index, bool forchilds=false)=0;

// ��������� ���������
virtual bool AUpdate(void)=0;
// --------------------------
};

}

#endif
