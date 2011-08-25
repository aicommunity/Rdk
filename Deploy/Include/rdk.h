#ifndef RDK_H
#define RDK_H

#include "../../ThirdParty/ThirdParty.h"
#include "../../Core/Math/UMath.h"
#include "../../Core/Utilities/Utilities.h"
#include "../../Core/Serialize/Serialize.h"
#include "../../Core/Graphics/Graphics.h"
#include "../../Core/Engine/Engine.h"

namespace RDK {

// ������� ������ ��������� � ���������� ��������� �� ����
UAContainerStorage* NewUStorage(void);

// ������� ����� � ���������� ��������� �� ���
UAContainerEnvironment* NewUAContainerEnvironment(void);

// ������� ������ � ���������� ��������� �� ����
UEngine* NewUEngine(void);

// �������������� ������ ��������� � ���������� ��������� �� ����
UAContainerStorage* CreateStorage(UAContainerStorage *storage);

// �������������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
UAContainerEnvironment* CreateEnvironment(UAContainerEnvironment *environment, UAContainerStorage *storage=0, bool isinit=true, list<UAContainer*>* external_classes=0, list<UALibrary*>* external_libs=0);

}
#endif





