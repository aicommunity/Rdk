/* ***********************************************************
@Copyright @COPYRIGHT_NAME@, @COPYRIGHT_YEAR@.
E-mail:        @COPYRIGHT_EMAIL@
Url:           @COPYRIGHT_URL@

This file is part of the project: @COPYRIGHT_PROJECT_NAME@

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef @FILE_NAME@_CPP
#define @FILE_NAME@_CPP

#include "@FILE_NAME@.h"

namespace @NAMESPACE_NAME@ {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
@CLASS_NAME@::@CLASS_NAME@(void)
{
}

@CLASS_NAME@::~@CLASS_NAME@(void)
{
}
// --------------------------    


// ---------------------
// ������ ���������� �����������
// ---------------------
// ---------------------

// ---------------------
// ������ ���������� ����������� ���������
// ---------------------
// ---------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
@CLASS_NAME@* @CLASS_NAME@::New(void)
{
 return new @CLASS_NAME@;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������ 
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool @CLASS_NAME@::ADefault(void)
{           
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool @CLASS_NAME@::ABuild(void)
{
 return true;
}

// ����� �������� ����� ��� ������ ��������
bool @CLASS_NAME@::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool @CLASS_NAME@::ACalculate(void)
{
 return true;
}
// --------------------------

}
#endif



