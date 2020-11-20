#ifndef UBASEPROPCREATOR_H
#define UBASEPROPCREATOR_H

#include "UBasePropCreatorTempl.h"

namespace RDK {

// �������� �� �������� ������� ��� ������������ ������� �������
class UBasePropCreator: public UBasePropCreatorTempl
{

public:
    // �������� ������� �������� ������� ���� ��������� ����� (����������� � Storage)
    static bool BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� �������� ����������� ������� � ����������� �� ���� (������) ��������
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);

    // ��������� ��� �� ���������� ���� ������ � �������, ���� ��� ���
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static bool CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);

	// ������� ��� �������� �������� ���� list
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyList(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);

    // ������� ��� �������� �������� ���� std::pair
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyPair(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);

    // ������� ��� �������� �������� ���� std::map
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMap(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype);

};


// ������� �������� ����������� ������� � ����������� �� ���� (������) ��������
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreator::CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
{
    std::string type = serstorage->GetNodeAttribute("Type");

    // �������� �� ���������� �� ��� ������ � ��������
    // ���� �� - �������� � �����
    if(CreateSimpleTypeProperty<PropType, TypeInt>(serstorage,mock_unet, ptype))
        return;

    // ������
    if(type == "std::string")
    {
        CreatorProperty<PropType, TypeInt, std::string>::CreatePropertyByType(serstorage,mock_unet, ptype);
        return;
	}

    // ��� list
    if(type == "std::list")
    {
        CreatePropertyList<PropType, TypeInt>(serstorage,mock_unet, ptype);
        return;
    }

    // ��� pair
    if(type == "std::pair")
    {
        CreatePropertyPair<PropType, TypeInt>(serstorage,mock_unet, ptype);
        return;
    }

    // ��� map
    if(type == "std::map")
    {
        CreatePropertyMap<PropType, TypeInt>(serstorage,mock_unet, ptype);
        return;
    }

    //UBMColorModel
    if(type == "UBMColorModel")
    {
        CreatorProperty<PropType, TypeInt, UBMColorModel>::CreatePropertyByType(serstorage,mock_unet, ptype);
        return;
    }

    // UColorT
    if(type == "UColorT")
    {
        CreatorProperty<PropType, TypeInt, UColorT>::CreatePropertyByType(serstorage,mock_unet, ptype);
        return;
    }

    // UBitmap
    if(type == "UBitmap")
    {
        CreatorProperty<PropType, TypeInt, UBitmap>::CreatePropertyByType(serstorage,mock_unet, ptype);
        return;
    }

    // UBRect
    if(type == "UBRect")
    {
        CreatorProperty<PropType, TypeInt, UBRect>::CreatePropertyByType(serstorage,mock_unet, ptype);
        return;
    }

    // UBPoint
    if(type == "UBPoint")
    {
        CreatorProperty<PropType, TypeInt, UBPoint>::CreatePropertyByType(serstorage,mock_unet, ptype);
        return;
    }


    // pointer
    if(type == "pointer")
    {
        CreatorProperty<PropType, TypeInt, void*>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }


}

// ��������� ��� �� ���������� ���� ������ � �������, ���� ��� ���
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
bool UBasePropCreator::CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet * mock_unet, unsigned int ptype)
{
    std::string type = serstorage->GetNodeAttribute("Type");
    std::string prop_name = serstorage->GetNodeName();

    // ���������� ����
    if(type == typeid(bool).name())
    {
        CreatorProperty<PropType, TypeInt, bool>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    if(type == typeid(unsigned char).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned char>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    if(type == typeid(unsigned int).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned int>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    if(type == typeid(int).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned int>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    if(type == typeid(long).name())
    {
        CreatorProperty<PropType, TypeInt, long>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    if(type == typeid(unsigned long).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned long>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    if(type == typeid(long long).name())
    {
        CreatorProperty<PropType, TypeInt, long long>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    if(type == typeid(unsigned long long).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned long long>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    if(type == typeid(float).name())
    {
        CreatorProperty<PropType, TypeInt, float>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    if(type == typeid(double).name())
    {
        CreatorProperty<PropType, TypeInt, double>::CreatePropertyByType(serstorage, mock_unet, ptype);
        return true;
    }
    //throw ParseException("can't handle simple type: " + type);
    return false;

}

 // ������� ��� �������� �������� ���� list
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreator::CreatePropertyList(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
{
    std::string prop_name = serstorage->GetNodeName();
    int size=serstorage->GetNumNodes();
    // ���� ������ ���� ����� ��� - elemType
    // ���� ���� �������� ����� ��� ���������
    std::string type;
    if(size == 0)
    {
        type = serstorage->GetNodeAttribute("elemType");
    }
    else
    {
        serstorage->SelectNode("elem",size-1);
        type = serstorage->GetNodeAttribute("elemType");
        serstorage->SelectUp();
    }

    if(type == typeid(int).name())
    {
        CreatorProperty<PropType, TypeInt, std::list<int> >::CreatePropertyByType(serstorage, mock_unet, ptype);
        return;
    }
}

// ������� ��� �������� �������� ���� std::pair
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreator::CreatePropertyPair(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
{
    std::string prop_name = serstorage->GetNodeName();

    int size=serstorage->GetNumNodes();
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


    if(first_type == "std::string")
    {
        if(second_type == "std::string")
        {
            CreatorProperty<PropType, TypeInt, std::pair<std::string,std::string> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
        if(second_type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, std::pair<std::string,int> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
        if(second_type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, std::pair<std::string,bool> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
    }

    if(second_type == "std::string")
    {
        if(first_type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, std::pair<int,std::string> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
    }

    if(first_type == typeid(int).name())
    {
        if(second_type == "UColorT")
        {
            CreatorProperty<PropType, TypeInt, std::pair<int,UColorT> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
    }

    if(first_type == typeid(double).name())
    {
        if(second_type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, std::pair<double,int> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
    }
}

// ������� ��� �������� �������� ���� std::map
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreator::CreatePropertyMap(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
{
    std::string prop_name = serstorage->GetNodeName();

    std::string first_type;
    std::string second_type;

    int size=serstorage->GetNumNodes();

    //���� ���� ��������
    if(serstorage->SelectNode("elem",size-1))
    {
        size=serstorage->GetNumNodes();
        if(size!=2)
        {
            serstorage->SelectUp();
            return;
        }

        serstorage->SelectNode("first");
        first_type = serstorage->GetNodeAttribute("Type");
        serstorage->SelectUp();

        serstorage->SelectNode("second");
        second_type = serstorage->GetNodeAttribute("Type");
        serstorage->SelectUp();
        serstorage->SelectUp();
    }
    else
    {
        first_type = serstorage->GetNodeAttribute("firstType");
        second_type = serstorage->GetNodeAttribute("secondType");
    }


    if(first_type == "std::string")
    {
        if(second_type == "std::string")
        {
            CreatorProperty<PropType, TypeInt, std::map<std::string,std::string> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
        if(second_type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, std::map<std::string,int> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
        if(second_type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, std::map<std::string,bool> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }

        if(second_type.find("MDVector",0) == 0)
        {
            // �������� ����� MDVector
            size_t pos = second_type.find("MDVector");
            if (pos != std::string::npos)
            {
                second_type.erase(pos, std::string("MDVector").length());
            }
            // �������� �������� '<' � '>'
            second_type.erase(std::remove(second_type.begin(), second_type.end(), '>'), second_type.end());
            second_type.erase(std::remove(second_type.begin(), second_type.end(), '<'), second_type.end());

            std::string matrix_type = second_type;

            if(matrix_type == typeid(int).name())
            {
                CreatorProperty<PropType, TypeInt, std::map<std::string, MDVector<int> > >::CreatePropertyByType(serstorage, mock_unet, ptype);
                return;
            }
        }
    }

    if(first_type == typeid(int).name())
    {
        if(second_type == "UColorT")
        {
            CreatorProperty<PropType, TypeInt, std::map<int,UColorT> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
        if(second_type == "std::string")
        {
            CreatorProperty<PropType, TypeInt, std::map<int,std::string> >::CreatePropertyByType(serstorage, mock_unet, ptype);
            return;
        }
	}
}


}
#endif // UBASEPROPCREATOR_H
