#ifndef UBASEPROPCREATORVECTOR_H
#define UBASEPROPCREATORVECTOR_H

#include "UBasePropCreatorTempl.h"

namespace RDK {

// Отвечает за создание функции для формирования базовых свойств
class UBasePropCreatorVector: public UBasePropCreatorTempl
{

public:
	// Основная функция создания свойств всех возможных типов (добавляется в Storage)
    static bool BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

	// Функция вызывает необходимые фукнции в зависимости от типа (строка) свойства
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);

	// Функция для создания свойства типа вектор
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);

	// Функция для создания свойства типа вектор с простейшими типами данным (simpleVector)
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertySimpleVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);

	// Функция для создания свойства типа вектор простейших векторов std::vector<simpleVector> >
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfSimVec(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);

	// Функция для создания свойства типа вектор пар
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfPair(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);
};


// Функция вызывает необходимые фукнции в зависимости от типа (строка) свойства
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorVector::CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
{
    std::string type = serstorage->GetNodeAttribute("Type");

    // Исключительный случай для свойства NetStructure
    if(type == "NetStructure")
    {
        CreatorProperty<PropType, TypeInt, vector<vector<vector<vector<vector<UId> > > > > >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }

    //Для вектора (тип данных вектора определяется далее внутри метода CreatePropertyByType) - аналогично для остальных контейнеров
    if(type == "std::vector")
    {
        CreatePropertyVector<PropType, TypeInt>(serstorage,mock_unet, ptype);
        return;
	}

    // Для векторов с простейщими типами (bool, int, double)
    if(type == "simplevector")
    {
        CreatePropertySimpleVector<PropType, TypeInt>(serstorage,mock_unet, ptype);
		return;
	}
}

// Функция для создания свойства типа вектор
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorVector::CreatePropertyVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
{
    std::string prop_name = serstorage->GetNodeName();

    int size=serstorage->GetNumNodes();
    // если вектор пуст берем тип - elemType
    // если есть элементы берем тип элементов
    std::string type;
    if(size == 0)
    {
        type = serstorage->GetNodeAttribute("elemType");
    }
    else
    {
        serstorage->SelectNode("elem",size-1);
        type = serstorage->GetNodeAttribute("Type");
        serstorage->SelectUp();
    }


    if(type == typeid(bool).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<bool> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(char).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<char> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(unsigned char).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<unsigned char> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(short).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<short> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(unsigned short).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<unsigned short> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(int).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<int> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(unsigned int).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<unsigned int> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(long).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<long> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(unsigned long).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<unsigned long> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(long long).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<long long> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(unsigned long long).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<unsigned long long> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(float).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<float> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(double).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<double> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(long double).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<long double> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == "UColorT")
    {
        CreatorProperty<PropType, TypeInt, std::vector<UColorT> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == "std::string")
    {
        CreatorProperty<PropType, TypeInt, std::vector<std::string> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == "UBitmapParam")
    {
        CreatorProperty<PropType, TypeInt, std::vector<UBitmapParam> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == "UBitmap")
    {
        CreatorProperty<PropType, TypeInt, std::vector<UBitmap> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == "UBRect")
    {
        CreatorProperty<PropType, TypeInt, std::vector<UBRect> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == "UBPoint")
    {
        CreatorProperty<PropType, TypeInt, std::vector<UBPoint> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    //Из RTV
//    if(type == "UBVSObject")
//    {
//        CreatorProperty<PropType, TypeInt, std::vector<RTV::UBVSObject3D> >::CreatePropertyByType(serstorage, mock_unet, ptype);
//        return;
//    }

	// вектор пар
	if(type == "std::pair")
	{
        CreatePropertyVectorOfPair<PropType, TypeInt>(serstorage, mock_unet, ptype);
		return;
	}

    // вектор из простейщих векторов
    if(type == "simplevector")
    {
        CreatePropertyVectorOfSimVec<PropType, TypeInt>(serstorage, mock_unet, ptype);
        return;
    }

    // вектор из указателей
    if(type == "pointer")
    {
        CreatorProperty<PropType, TypeInt, std::vector<void*> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }

}

// Функция для создания свойства типа вектор с простейшими типами данным (simpleVector)
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorVector::CreatePropertySimpleVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
{
    std::string prop_name = serstorage->GetNodeName();
    std::string type = serstorage->GetNodeAttribute("elemType");

    if(type == typeid(bool).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<bool> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(int).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<int> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(double).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<double> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
}

// Функция для создания свойства типа вектор простейших векторов std::vector<simpleVector> >
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorVector::CreatePropertyVectorOfSimVec(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
{
    std::string prop_name = serstorage->GetNodeName();
    int size=serstorage->GetNumNodes();

    std::string type;

    if(size==0)
    {
        type = serstorage->GetNodeAttribute("elemSVType");
    }
    else
    {
        if(!(serstorage->SelectNode("elem",size-1)))
            return;

        type = serstorage->GetNodeAttribute("elemType");
        serstorage->SelectUp();
    }

    if(type == typeid(bool).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<std::vector<char> > >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(int).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<std::vector<int> > >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
    if(type == typeid(double).name())
    {
        CreatorProperty<PropType, TypeInt, std::vector<std::vector<double> > >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
}

// Функция для создания свойства типа вектор пар
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorVector::CreatePropertyVectorOfPair(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
{
    std::string prop_name = serstorage->GetNodeName();
    int size=serstorage->GetNumNodes();
    if(!(serstorage->SelectNode("elem",size-1)))
        return;
    size=serstorage->GetNumNodes();
    if(size!=2)
        return;
    std::string first_type;
    std::string second_type;

    serstorage->SelectNode("first");
    first_type = serstorage->GetNodeAttribute("Type");
    serstorage->SelectUp();

    serstorage->SelectNode("second");
    second_type = serstorage->GetNodeAttribute("Type");
    serstorage->SelectUp();
    serstorage->SelectUp();

    if(first_type == "std::string")
    {
        if(second_type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, std::pair<std::string,bool> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
    }
}

}
#endif // UBASEPROPCREATOR_H
