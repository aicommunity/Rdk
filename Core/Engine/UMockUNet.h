#ifndef UMOCKUNET_H
#define UMOCKUNET_H

#include "UNet.h"

// RTV для UBVSObject
//#include "../../../Libraries/Rtv-BasicLib/Core/RTVSupport.h"

// RDK include
//#include "../../../Libraries/Rdk-PtzCameraControlLib/Core/UBPtzCameraControlPelco.h"

// Сериализация компонента UBVSObject
//#include "../../../Libraries/Rtv-BasicLib/Core/RTVXMLSerialize.h"

namespace RDK {

class RDK_LIB_TYPE UMockUNet: public UNet
{
protected: // Основные свойства

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------

UMockUNet(RDK::USerStorageXML *serstorage=0, UStorage* storage = 0);
virtual ~UMockUNet(void);

// Выделяет память для новой чистой копии объекта этого класса
virtual UMockUNet* New(void);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
bool LoadComponent(RDK::USerStorageXML *serstorage, bool links);


private:
// Описание класса (отсюда формируются свойства)
USerStorageXML ClassDesriptionXML;

};

}

#endif
