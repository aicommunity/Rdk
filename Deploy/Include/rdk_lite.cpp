#ifndef RDK_LITE_CPP
#define RDK_LITE_CPP

#include "rdk_lite.h"

#include "../../ThirdParty/ThirdParty.cpp"
#include "../../Core/Math/UMath_lite.cpp"
#include "../../Core/Utilities/Utilities_lite.cpp"
#include "../../Core/Graphics/Graphics_lite.cpp"
#include "../../Core/Engine/Engine_lite.cpp"
#include "../../Core/Serialize/Serialize_lite.cpp"

namespace RDK {

// ������� ������ ��������� � ���������� ��������� �� ����
UStorage* NewUAStorage(void)
{
 return new UStorage;
}

// ������� ����� � ���������� ��������� �� ���
UEnvironment* NewUAEnvironment(void)
{
 return new UEnvironment;
}


// ������� ������ ��������� � ���������� ��������� �� ����
UStorage* CreateAStorage(UStorage *storage)
{
 return storage;
}

// ������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
UEnvironment* CreateAEnvironment(UEnvironment *environment, UStorage *storage, bool isinit, ULibrary** external_libs, int numlibs)
{
 UEnvironment *env=environment;

 if(!storage)
  return env;

 if(!env->SetStorage(storage) || !isinit || !external_libs || !numlibs)
  return env;

 for(int i=0;i<numlibs;i++)
 {
  env->AddClassLibrary(external_libs[i]);
 }

 env->BuildStorage();

 return env;
}

}
#endif





