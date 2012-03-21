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
UAStorage* NewUAStorage(void)
{
 return new UAStorage;
}

// ������� ����� � ���������� ��������� �� ���
UAEnvironment* NewUAEnvironment(void)
{
 return new UAEnvironment;
}


// ������� ������ ��������� � ���������� ��������� �� ����
UAStorage* CreateAStorage(UAStorage *storage)
{
 return storage;
}

// ������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
UAEnvironment* CreateAEnvironment(UAEnvironment *environment, UAStorage *storage, bool isinit, UALibrary** external_libs, int numlibs)
{
 UAEnvironment *env=environment;

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





