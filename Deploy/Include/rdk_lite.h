#ifndef RDK_LITE_H
#define RDK_LITE_H

#include <memory>
#include "../../ThirdParty/ThirdParty.h"
#include "../../Core/Math/UMath_lite.h"
#include "../../Core/Utilities/Utilities_lite.h"
#include "../../Core/Graphics/Graphics_lite.h"
#include "../../Core/Engine/Engine_lite.h"
#include "../../Core/Serialize/Serialize_lite.h"

namespace RDK {

// ������� ������ ��������� � ���������� ��������� �� ����
std::shared_ptr<UStorage> NewUAStorage(void);

// ������� ����� � ���������� ��������� �� ���
std::shared_ptr<UEnvironment> NewUAEnvironment(void);

// �������������� ������ ��������� � ���������� ��������� �� ����
std::shared_ptr<UStorage> CreateAStorage(std::shared_ptr<UStorage> storage);

// �������������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
std::shared_ptr<UEnvironment> CreateAEnvironment(std::shared_ptr<UEnvironment> environment, std::shared_ptr<UStorage> storage=nullptr, bool isinit=true, ULibrary** external_libs=nullptr, int numlibs=0);

}
#endif





