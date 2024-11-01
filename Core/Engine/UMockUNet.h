#ifndef UMOCKUNET_H
#define UMOCKUNET_H

#include "UNet.h"

// RTV ��� UBVSObject
//#include "../../../Libraries/Rtv-BasicLib/Core/RTVSupport.h"

// RDK include
//#include "../../../Libraries/Rdk-PtzCameraControlLib/Core/UBPtzCameraControlPelco.h"

// ������������ ���������� UBVSObject
//#include "../../../Libraries/Rtv-BasicLib/Core/RTVXMLSerialize.h"

namespace RDK {

class RDK_LIB_TYPE UMockUNet: public UNet
{
protected: // �������� ��������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------

UMockUNet(RDK::USerStorageXML *serstorage=0, UStorage* storage = 0);
virtual ~UMockUNet(void);

// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UMockUNet* New(void);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
bool LoadComponent(RDK::USerStorageXML *serstorage, bool links);


private:
// �������� ������ (������ ����������� ��������)
USerStorageXML ClassDesriptionXML;

};

}

#endif
