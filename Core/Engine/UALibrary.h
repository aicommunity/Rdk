/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UALibraryH
#define UALibraryH

//#include "UALibrary.h"
#include "UAContainerStorage.h"

namespace RDK {

class UALibrary//: public UALibrary
{
protected: // ���������
// ��� ����������
string Name;

// ������ ����������
string Version;

protected: // ������ ��������
// �������� ����� ���� ������� ����������� ��������
vector<string> Complete;

// �������� ����� ���� �� ����������� ��������
vector<string> Incomplete;

private: // ���������� ������
// ��������� ��� ��������
UAContainerStorage *Storage;
// ������ �������� �������, ����������� �����������
//map<string, UAComponent*> ClassSamples;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UALibrary(const string &name, const string &version);
virtual ~UALibrary(void);
// --------------------------

// --------------------------
// ������ ������� � ���������� ����������
// --------------------------
// ���������� ��� ����������
const string& GetName(void) const;

// ���������� ������ ����������
const string& GetVersion(void) const;
// --------------------------

// --------------------------
// ������ ������� � ������ ��������
// --------------------------
// �������� ����� ���� ������� ����������� ��������
const vector<string>& GetComplete(void) const;

// �������� ����� ���� �� ����������� ��������
const vector<string>& GetIncomplete(void) const;
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ��������� �������� ��������� ������� �������� �������.
// ���� ����� � �������� ������ ��� ����������, �� �� ������������.
// ���������� ����� ������� ����������� �������.
virtual int Upload(UAStorage *storage);
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
protected:
// ��������� � ��������� ��������� �����
virtual bool UploadClass(const UId &classid, UAComponent *cont);
virtual bool UploadClass(const string &name, UAComponent *cont);

// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
virtual void CreateClassSamples(UAStorage *storage)=0;
// --------------------------
};

typedef UALibrary* PUALibrary;

}

#endif
