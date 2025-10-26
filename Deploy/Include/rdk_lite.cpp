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
std::shared_ptr<UStorage> NewUAStorage(void)
{
 return std::make_shared<UStorage>();
}

// ������� ����� � ���������� ��������� �� ���
std::shared_ptr<UEnvironment> NewUAEnvironment(void)
{
 return std::make_shared<UEnvironment>();
}


// ������� ������ ��������� � ���������� ��������� �� ����
std::shared_ptr<UStorage> CreateAStorage(std::shared_ptr<UStorage> storage)
{
 return storage;
}

// ������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
std::shared_ptr<UEnvironment> CreateAEnvironment(std::shared_ptr<UEnvironment> environment, std::shared_ptr<UStorage> storage, bool isinit, ULibrary** external_libs, int numlibs)
{
 if(!storage)
  return environment;

 if(!environment->SetStorage(storage.get()) || !isinit || !external_libs || !numlibs)
  return environment;

 for(int i=0; i<numlibs; i++) {
  // environment->AddClassLibrary(external_libs[i]); // Method not available in lite version
 }

 // environment->BuildStorage(); // Method not available in lite version

 return environment;
}

}
#endif





