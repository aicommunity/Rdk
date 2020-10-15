#ifndef UMOCKUNET_CPP
#define UMOCKUNET_CPP

#include "UMockUNet.h"


namespace RDK {

std::string UMockUNet::ForbiddenInputs  = std::string("DataInput0") + "DataInput1" + "DataInput2" + "DataInput3" + "DataInput4";
std::string UMockUNet::ForbiddenOutputs = std::string("DataOutput0") + "DataOutput1" + "DataOutput2" + "DataOutput3" + "DataOutput4";

/* *************************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UMockUNet::UMockUNet(RDK::USerStorageXML *serstorage)
{
    USerStorageXML temp;

    serstorage = &temp;
    // Парсинг текущего файла
    serstorage->LoadFromFile("/home/user/Rtv-VideoAnalytics/Bin/RTlibs/sdf/qwsa.xml","");

    std::string comp_name = serstorage->GetNodeName();
    serstorage->SelectNode(comp_name);
    std::string class_name = serstorage->GetNodeAttribute("Class");

    // Проход по ТИПАМ свойств: Paramenters -> State -> Input -> Output
    for(int i =0, params = serstorage->GetNumNodes(); i <params; i++)
    {
        if(!serstorage->SelectNode(i))
            return;
        std::string prop_type = serstorage->GetNodeName();

        // Проход по конкретным свойствам внутри типа
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
                    // проверки на запрещенные имена
                    if(ForbiddenInputs.find(serstorage->GetNodeName()) != std::string::npos)
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreateProperty<UPropertyInputData,ptPubInput>(serstorage);
                }
                else if(prop_type == "Output")
                {
                    // проверки на запрещенные имена
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
                //ок
            }
            serstorage->SelectUp();
        }
        serstorage->SelectUp();
    }

    ClassDesriptionXML.Destroy();
    this->SaveComponent(&ClassDesriptionXML,false,ptAny|pgPublic);
}

UMockUNet* UMockUNet::New(void)
{
    return new UMockUNet(&ClassDesriptionXML);
}

template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UMockUNet::CreateProperty(RDK::USerStorageXML* serstorage)
{
    std::string type = serstorage->GetNodeAttribute("Type");

    // Проверка на встроенный ли тип данных у свойства
    // Если да - создание и выход
    if(CreateSimpleTypeProperty<PropType,TypeInt>(serstorage,this))
        return;

    // Строка
    if(type == "std::string")
    {
        CreatorProperty<PropType, TypeInt, std::string>::CreatePropertyByType(serstorage,this);
        return;
    }

    //Для вектора (тип данных вектора не важен)
    if(type == "std::vector" || type == "simplevector")
    {
        CreatorProperty<PropType, TypeInt, std::vector<int>>::CreatePropertyByType(serstorage,this);
        return;
    }

    // Для list
    if(type == "std::list")
    {
        CreatorProperty<PropType, TypeInt, std::list<int>>::CreatePropertyByType(serstorage,this);
        return;
    }

    // Аналогично с pair

    // Для map
    if(type == "std::map")
    {
        CreatorProperty<PropType, TypeInt, std::map<int,int>>::CreatePropertyByType(serstorage,this);
        return;
    }

    // MDMatrix
    if(type.find("MDMatrix",0) == 0)
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<int>>::CreatePropertyByType(serstorage,this);
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
    // Встроенные типы
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

// Основная функция (для четко определенных типов Т)
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
bool CreatorProperty<PropType, TypeInt,T>::CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
{
    std::string prop_name = serstorage->GetNodeName();
    PropType<T, UMockUNet, TypeInt>* p = new PropType<T, UMockUNet, TypeInt>(prop_name,mock_unet);
    p->Load(serstorage);
    return true;
}

// Частичная специализация для вектора
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
            // TODO остальные
            CreatorProperty<PropType, TypeInt, std::vector<char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned char).name())
        {
            PropType<std::vector<unsigned char>, UMockUNet, TypeInt> *p = new PropType<std::vector<unsigned char>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(short).name())
        {
            PropType<std::vector<short>, UMockUNet, TypeInt> *p = new PropType<std::vector<short>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned short).name())
        {
            PropType<std::vector<unsigned short>, UMockUNet, TypeInt> *p = new PropType<std::vector<unsigned short>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(int).name())
        {
            PropType<std::vector<int>, UMockUNet, TypeInt> *p = new PropType<std::vector<int>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned int).name())
        {
            PropType<std::vector<unsigned int>, UMockUNet, TypeInt> *p = new PropType<std::vector<unsigned int>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(long).name())
        {
            PropType<std::vector<long>, UMockUNet, TypeInt> *p = new PropType<std::vector<long>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned long).name())
        {
            PropType<std::vector<unsigned long>, UMockUNet, TypeInt> *p = new PropType<std::vector<unsigned long>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(long long).name())
        {
            PropType<std::vector<long long>, UMockUNet, TypeInt> *p = new PropType<std::vector<long long>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned long long).name())
        {
            PropType<std::vector<unsigned long long>, UMockUNet, TypeInt> *p = new PropType<std::vector<unsigned long long>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(float).name())
        {
            PropType<std::vector<float>, UMockUNet, TypeInt> *p = new PropType<std::vector<float>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(double).name())
        {
            PropType<std::vector<double>, UMockUNet, TypeInt> *p = new PropType<std::vector<double>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(long double).name())
        {
            PropType<std::vector<long double>, UMockUNet, TypeInt> *p = new PropType<std::vector<long double>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == "UColorT")
        {
            PropType<std::vector<UColorT>, UMockUNet, TypeInt> *p = new PropType<std::vector<UColorT>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }

    }
};

// Частичная специализация для list
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
            PropType<std::list<bool>, UMockUNet, TypeInt> *p = new PropType<std::list<bool>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(char).name())
        {
            PropType<std::list<char>, UMockUNet, TypeInt> *p = new PropType<std::list<char>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned char).name())
        {
            PropType<std::list<unsigned char>, UMockUNet, TypeInt> *p = new PropType<std::list<unsigned char>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(short).name())
        {
            PropType<std::list<short>, UMockUNet, TypeInt> *p = new PropType<std::list<short>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned short).name())
        {
            PropType<std::list<unsigned short>, UMockUNet, TypeInt> *p = new PropType<std::list<unsigned short>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(int).name())
        {
            PropType<std::list<int>, UMockUNet, TypeInt> *p = new PropType<std::list<int>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned int).name())
        {
            PropType<std::list<unsigned int>, UMockUNet, TypeInt> *p = new PropType<std::list<unsigned int>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(long).name())
        {
            PropType<std::list<long>, UMockUNet, TypeInt> *p = new PropType<std::list<long>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned long).name())
        {
            PropType<std::list<unsigned long>, UMockUNet, TypeInt> *p = new PropType<std::list<unsigned long>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(long long).name())
        {
            PropType<std::list<long long>, UMockUNet, TypeInt> *p = new PropType<std::list<long long>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned long long).name())
        {
            PropType<std::list<unsigned long long>, UMockUNet, TypeInt> *p = new PropType<std::list<unsigned long long>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(float).name())
        {
            PropType<std::list<float>, UMockUNet, TypeInt> *p = new PropType<std::list<float>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(double).name())
        {
            PropType<std::list<double>, UMockUNet, TypeInt> *p = new PropType<std::list<double>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(long double).name())
        {
            PropType<std::list<long double>, UMockUNet, TypeInt> *p = new PropType<std::list<long double>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
    }
};

// Частичная специализация для MDMatrix
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, MDMatrix<UnKnow> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string type = serstorage->GetNodeAttribute("Type");
        std::string prop_name = serstorage->GetNodeName();

        // Удаление слова MDMatrix
        size_t pos = type.find("MDMatrix");
        if (pos != std::string::npos)
        {
            type.erase(pos, std::string("MDMatrix").length());
        }
        // Удаление символов '<' и '>'
        type.erase(std::remove(type.begin(), type.end(), '>'), type.end());
        type.erase(std::remove(type.begin(), type.end(), '<'), type.end());

        std::string matrix_type = type;

        if(type == typeid(bool).name())
        {
            PropType<MDMatrix<bool>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<bool>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(char).name())
        {
            PropType<MDMatrix<char>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<char>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned char).name())
        {
            PropType<MDMatrix<unsigned char>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<unsigned char>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(short).name())
        {
            PropType<MDMatrix<short>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<short>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned short).name())
        {
            PropType<MDMatrix<unsigned short>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<unsigned short>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(int).name())
        {
            PropType<MDMatrix<int>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<int>, UMockUNet, TypeInt>(prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned int).name())
        {
            PropType<MDMatrix<unsigned int>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<unsigned int>, UMockUNet, TypeInt> (prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(long).name())
        {
            PropType<MDMatrix<long>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<long>, UMockUNet, TypeInt> (prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned long).name())
        {
            PropType<MDMatrix<unsigned long>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<unsigned long>, UMockUNet, TypeInt> (prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(long long).name())
        {
            PropType<MDMatrix<long long>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<long long>, UMockUNet, TypeInt> (prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(unsigned long long).name())
        {
            PropType<MDMatrix<unsigned long long>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<unsigned long long>, UMockUNet, TypeInt> (prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(float).name())
        {
            PropType<MDMatrix<float>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<float>, UMockUNet, TypeInt> (prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(double).name())
        {
            PropType<MDMatrix<double>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<double>, UMockUNet, TypeInt> (prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
        if(type == typeid(long double).name())
        {
            PropType<MDMatrix<long double>, UMockUNet, TypeInt> *p = new PropType<MDMatrix<long double>, UMockUNet, TypeInt> (prop_name,mock_unet);
            p->Load(serstorage);
            return true;
        }
    }
};

// Частичная специализация для std::map
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
                PropType<std::map<std::string,std::string>, UMockUNet, TypeInt> *p = new PropType<std::map<std::string,std::string>, UMockUNet, TypeInt>(prop_name,mock_unet);
                p->Load(serstorage);
                return true;
            }
            if(second_type == typeid(int).name())
            {
                PropType<std::map<std::string,int>, UMockUNet, TypeInt> *p = new PropType<std::map<std::string,int>, UMockUNet, TypeInt>(prop_name,mock_unet);
                p->Load(serstorage);
                return true;
            }
            if(second_type == typeid(bool).name())
            {
                PropType<std::map<std::string,int>, UMockUNet, TypeInt> *p = new PropType<std::map<std::string,int>, UMockUNet, TypeInt>(prop_name,mock_unet);
                p->Load(serstorage);
                return true;
            }
        }

        if(second_type == "std::string")
        {
            if(first_type == typeid(int).name())
            {
                PropType<std::map<int,std::string>, UMockUNet, TypeInt> *p = new PropType<std::map<int,std::string>, UMockUNet, TypeInt>(prop_name,mock_unet);
                p->Load(serstorage);
                return true;
            }
        }

        if(first_type == typeid(int).name())
        {
            if(second_type == "UColorT")
            {
                PropType<std::map<int,UColorT>, UMockUNet, TypeInt> *p = new PropType<std::map<int,UColorT>, UMockUNet, TypeInt>(prop_name,mock_unet);
                p->Load(serstorage);
                return true;
            }
        }

    }
};


UMockUNet::~UMockUNet()
{
}

}
#endif

