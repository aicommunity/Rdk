#ifndef RDK_LITE_H
#define RDK_LITE_H

#include "../../ThirdParty/ThirdParty.h"
#include "../../Core/Math/UMath_lite.h"
#include "../../Core/Utilities/Utilities_lite.h"
#include "../../Core/Graphics/Graphics_lite.h"
#include "../../Core/Engine/Engine_lite.h"
#include "../../Core/Serialize/Serialize_lite.h"

namespace RDK {

// ������� ������ ��������� � ���������� ��������� �� ����
UAStorage* NewUAStorage(void);

// ������� ����� � ���������� ��������� �� ���
UAEnvironment* NewUAEnvironment(void);

// �������������� ������ ��������� � ���������� ��������� �� ����
UAStorage* CreateAStorage(UAStorage *storage);

// �������������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
UAEnvironment* CreateAEnvironment(UAEnvironment *environment, UAStorage *storage=0, bool isinit=true, UALibrary** external_libs=0, int numlibs=0);

}
#endif





