/* ***********************************************************
@Copyright @COPYRIGHT_NAME@, @COPYRIGHT_YEAR@.
E-mail:        @COPYRIGHT_EMAIL@
Url:           @COPYRIGHT_URL@

This file is part of the project: @COPYRIGHT_PROJECT_NAME@

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef @FILE_NAME@_H
#define @FILE_NAME@_H

#include "@INCLUDE_FILE_NAME@"

namespace @NAMESPACE_NAME@ {

class @CLASS_NAME@: public @INHERITANCE_NAME@
{
protected: // ��������           

protected: // ���������� ���������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
@CLASS_NAME@(void);
virtual ~@CLASS_NAME@(void);
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
virtual @CLASS_NAME@* New(void);
// --------------------------        

// --------------------------
// ������� ������ ���������� ������ 
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};


}

#endif



