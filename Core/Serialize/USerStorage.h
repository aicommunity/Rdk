/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef USerStorageH
#define USerStorageH

namespace RDK {

class USerStorage
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
// ������� ������ �������
USerStorage(void);
virtual ~USerStorage(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���������� �������� � ��������� ������������
/*template<typename T>
bool Save(const T &value);

// �������� �������� �� ��������� ������������
template<typename T>
bool Load(T &value);        */
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
// --------------------------
};

}
#endif
