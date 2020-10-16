#ifndef UMOCKUNET_CPP
#define UMOCKUNET_CPP

#include "UMockUNet.h"


namespace RDK {

std::string UMockUNet::ForbiddenInputs  = std::string("DataInput0") + "DataInput1" + "DataInput2" + "DataInput3" + "DataInput4";
std::string UMockUNet::ForbiddenOutputs = std::string("DataOutput0") + "DataOutput1" + "DataOutput2" + "DataOutput3" + "DataOutput4";

/* *************************************************************************** */
// --------------------------
// ������������ � �����������
// --------------------------
UMockUNet::UMockUNet(RDK::USerStorageXML *serstorage)
{
    USerStorageXML temp;
    serstorage = &temp;
    // ������� �������� �����
    serstorage->LoadFromFile("/home/user/Rtv-VideoAnalytics/Bin/RTlibs/sdf/qwsa.xml","");

    std::string comp_name = serstorage->GetNodeName();
    serstorage->SelectNode(comp_name);
    std::string class_name = serstorage->GetNodeAttribute("Class");

    // ������ �� ����� �������: Paramenters -> State -> Input -> Output
    for(int i =0, params = serstorage->GetNumNodes(); i <params; i++)
    {
        if(!serstorage->SelectNode(i))
            return;
        std::string prop_type = serstorage->GetNodeName();

        // ������ �� ���������� ��������� ������ ����
        for(int j =0, props = serstorage->GetNumNodes(); j < props; j++)
        {
            try{
                if(!serstorage->SelectNode(j))
                    return;
                std::string prop_name = serstorage->GetNodeName();

                if(prop_type == "Parameters")
                {
                    CreateProperty<ULProperty,ptPubParameter>(serstorage);
                }
                else if(prop_type == "State")
                {
                    CreateProperty<ULProperty,ptPubState>(serstorage);
                }
                else if(prop_type == "Input")
                {
                    // �������� �� ����������� �����
                    if(ForbiddenInputs.find(serstorage->GetNodeName()) != std::string::npos)
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreateProperty<UPropertyInputData,ptPubInput>(serstorage);
                }
                else if(prop_type == "Output")
                {
                    // �������� �� ����������� �����
                    if(ForbiddenOutputs.find(serstorage->GetNodeName()) != std::string::npos)
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreateProperty<UPropertyOutputData,ptPubOutput>(serstorage);
                }
            }
            catch(...) //EPropertyNameAlreadyExist& e
            {
                //��
            }
            serstorage->SelectUp();
        }
        serstorage->SelectUp();
    }

    ClassDesriptionXML.Destroy();

    this->SaveComponent(&ClassDesriptionXML, false, ptAny|pgPublic);
}

UMockUNet* UMockUNet::New(void)
{
    return new UMockUNet(&ClassDesriptionXML);
}

UMockUNet::~UMockUNet()
{
}

template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UMockUNet::CreateProperty(RDK::USerStorageXML* serstorage)
{
    std::string type = serstorage->GetNodeAttribute("Type");

    // �������� �� ���������� �� ��� ������ � ��������
    // ���� �� - �������� � �����
    if(CreateSimpleTypeProperty<PropType,TypeInt>(serstorage,this))
        return;

    // ������
    if(type == "std::string")
    {
        CreatorProperty<PropType, TypeInt, std::string>::CreatePropertyByType(serstorage,this);
        return;
    }

    //��� ������� (��� ������ ������� ������������ ����� ������ ������ CreatePropertyByType) - ���������� ��� ��������� �����������
    if(type == "std::vector" || type == "simplevector")
    {
        CreatorProperty<PropType, TypeInt, std::vector<UnKnow> >::CreatePropertyByType(serstorage,this);
        return;
    }

    // ��� list
    if(type == "std::list")
    {
        CreatorProperty<PropType, TypeInt, std::list<UnKnow> >::CreatePropertyByType(serstorage,this);
        return;
    }

    // ��� pair
    if(type == "std::pair")
    {
        CreatorProperty<PropType, TypeInt, std::pair<UnKnow,UnKnow> >::CreatePropertyByType(serstorage,this);
        return;
    }

    // ��� map
    if(type == "std::map")
    {
        CreatorProperty<PropType, TypeInt, std::map<UnKnow,UnKnow> >::CreatePropertyByType(serstorage,this);
        return;
    }

    // MDMatrix
    if(type.find("MDMatrix",0) == 0)
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<UnKnow> >::CreatePropertyByType(serstorage,this);
        return;
    }

    // UBitmap
    if(type == "UBitmap")
    {
        CreatorProperty<PropType, TypeInt, UBitmap>::CreatePropertyByType(serstorage,this);
        return;
    }


}

template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
bool UMockUNet::CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, UMockUNet * mock_unet)
{
    std::string type = serstorage->GetNodeAttribute("Type");
    std::string prop_name = serstorage->GetNodeName();

    // ���������� ����
    if(type == typeid(bool).name())
    {
        CreatorProperty<PropType, TypeInt, bool>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(char).name())
    {
        CreatorProperty<PropType, TypeInt, char>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(unsigned char).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned char>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(short).name())
    {
        CreatorProperty<PropType, TypeInt, short>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(unsigned short).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned short>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(int).name())
    {
        CreatorProperty<PropType, TypeInt, int>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(unsigned int).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned int>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(long).name())
    {
        CreatorProperty<PropType, TypeInt, long>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(unsigned long).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned long>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(long long).name())
    {
        CreatorProperty<PropType, TypeInt, long long>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(unsigned long long).name())
    {
        CreatorProperty<PropType, TypeInt, unsigned long long>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(float).name())
    {
        CreatorProperty<PropType, TypeInt, float>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(double).name())
    {
        CreatorProperty<PropType, TypeInt, double>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    if(type == typeid(long double).name())
    {
        CreatorProperty<PropType, TypeInt, long double>::CreatePropertyByType(serstorage, mock_unet);
        return true;
    }
    return false;

}

// �������� ������� (��� ����� ������������ ����� �)
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
bool CreatorProperty<PropType, TypeInt, T>::CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
{
    std::string prop_name = serstorage->GetNodeName();
    PropType<T, UMockUNet, TypeInt>* p = new PropType<T, UMockUNet, TypeInt>(prop_name, mock_unet);
    p->Load(serstorage);
    return true;
}

// ��������� ������������� ��� �������
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, std::vector<UnKnow> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string prop_name = serstorage->GetNodeName();
        int size=serstorage->GetNumNodes();
        if(!(serstorage->SelectNode("elem",size-1)))
            return false;

        std::string type = serstorage->GetNodeAttribute("Type");
        serstorage->SelectUp();

        if(type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<bool> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(char).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned char).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<unsigned char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(short).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned short).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<unsigned short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned int).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<unsigned int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<unsigned long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long long).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long long).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<unsigned long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(float).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<float> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(double).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long double).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<long double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == "UColorT")
        {
            CreatorProperty<PropType, TypeInt, std::vector<UColorT> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }

    }
};

// ��������� ������������� ��� list
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, std::list<UnKnow> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string prop_name = serstorage->GetNodeName();
        int size=serstorage->GetNumNodes();
        if(!(serstorage->SelectNode("elem",size-1)))
            return false;

        std::string type = serstorage->GetNodeAttribute("Type");
        serstorage->SelectUp();

        if(type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<bool> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(char).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned char).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<unsigned char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(short).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned short).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<unsigned short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned int).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<unsigned int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<unsigned long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long long).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long long).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<unsigned long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(float).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<float> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(double).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long double).name())
        {
            CreatorProperty<PropType, TypeInt, std::list<long double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
    }
};

// ��������� ������������� ��� MDMatrix
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, MDMatrix<UnKnow> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string type = serstorage->GetNodeAttribute("Type");
        std::string prop_name = serstorage->GetNodeName();

        // �������� ����� MDMatrix
        size_t pos = type.find("MDMatrix");
        if (pos != std::string::npos)
        {
            type.erase(pos, std::string("MDMatrix").length());
        }
        // �������� �������� '<' � '>'
        type.erase(std::remove(type.begin(), type.end(), '>'), type.end());
        type.erase(std::remove(type.begin(), type.end(), '<'), type.end());

        std::string matrix_type = type;

        if(type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<bool> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(char).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned char).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<unsigned char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(short).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned short).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<unsigned short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned int).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<unsigned int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<unsigned long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long long).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long long).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<unsigned long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(float).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<float> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(double).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long double).name())
        {
            CreatorProperty<PropType, TypeInt, MDMatrix<long double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
    }
};

// ��������� ������������� ��� std::pair
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, std::pair<UnKnow, UnKnow> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string prop_name = serstorage->GetNodeName();

        int size=serstorage->GetNumNodes();
        if(size!=2)
            return false;

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
            if(second_type == "std::string")
            {
                CreatorProperty<PropType, TypeInt, std::pair<std::string,std::string> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(second_type == typeid(int).name())
            {
                CreatorProperty<PropType, TypeInt, std::pair<std::string,int> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(second_type == typeid(bool).name())
            {
                CreatorProperty<PropType, TypeInt, std::pair<std::string,bool> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }

        if(second_type == "std::string")
        {
            if(first_type == typeid(int).name())
            {
                CreatorProperty<PropType, TypeInt, std::pair<int,std::string> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }

        if(first_type == typeid(int).name())
        {
            if(second_type == "UColorT")
            {
                CreatorProperty<PropType, TypeInt, std::pair<int,UColorT> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }

    }
};

// ��������� ������������� ��� std::map
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, std::map<UnKnow, UnKnow> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string prop_name = serstorage->GetNodeName();
        int size=serstorage->GetNumNodes();
        if(!(serstorage->SelectNode("elem",size-1)))
            return false;
        size=serstorage->GetNumNodes();
        if(size!=2)
            return false;
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
            if(second_type == "std::string")
            {
                CreatorProperty<PropType, TypeInt, std::map<std::string,std::string> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(second_type == typeid(int).name())
            {
                CreatorProperty<PropType, TypeInt, std::map<std::string,int> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(second_type == typeid(bool).name())
            {
                CreatorProperty<PropType, TypeInt, std::map<std::string,bool> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }

        if(second_type == "std::string")
        {
            if(first_type == typeid(int).name())
            {
                CreatorProperty<PropType, TypeInt, std::map<int,std::string> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }

        if(first_type == typeid(int).name())
        {
            if(second_type == "UColorT")
            {
                CreatorProperty<PropType, TypeInt, std::map<int,UColorT> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }

    }
};


}
#endif

