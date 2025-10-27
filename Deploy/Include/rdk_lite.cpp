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
std::unique_ptr<UStorage> NewUAStorage(void)
{
 return std::make_unique<UStorage>();
}

// ������� ����� � ���������� ��������� �� ���
std::unique_ptr<UEnvironment> NewUAEnvironment(void)
{
 return std::make_unique<UEnvironment>();
}


// ������� ������ ��������� � ���������� ��������� �� ����
std::unique_ptr<UStorage> CreateAStorage(std::unique_ptr<UStorage> storage)
{
 return storage;
}

// ������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
std::unique_ptr<UEnvironment> CreateAEnvironment(std::unique_ptr<UEnvironment> environment, std::unique_ptr<UStorage> storage, bool isinit, ULibrary** external_libs, int numlibs)
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





