/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef USER_STORAGE_CPP
#define USER_STORAGE_CPP

#include <string.h>
#include "USerStorage.h"
//#include "USerStorageBinary.h"
//#include "Serialize.h"

namespace RDK {
namespace Serialize {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
// ������� ������ �������
USerStorage::USerStorage(void)
{
}

USerStorage::~USerStorage(void)
{
}
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
bool USerStorage::Save(const T &value)
{
 USerStorageBinary const * binary=dynamic_cast<USerStorageBinary const *>(this);
 if(binary)
 {
  *binary<<value;
  return true;
 }


 return false;
}

// �������� �������� �� ��������� ������������
template<typename T>
bool USerStorage::Load(T &value)
{
}     */
// --------------------------


// --------------------------
// ������� ������ ���������� �������
// --------------------------
// --------------------------
}
}
#endif

