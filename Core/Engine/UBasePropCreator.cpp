#include "UBasePropCreator.h"
#include "UComponent.h"
#include "../Utilities/USupport.h"

std::vector<std::string> ForbiddenInputs;
std::vector<std::string> ForbiddenOutputs;

using namespace RDK;

UBasePropCreator::UBasePropCreator()
{
 if(ForbiddenInputs.empty())
 {
  ForbiddenInputs.push_back("DataInput0");
  ForbiddenInputs.push_back("DataInput1");
  ForbiddenInputs.push_back("DataInput2");
  ForbiddenInputs.push_back("DataInput3");
  ForbiddenInputs.push_back("DataInput4");
  ForbiddenInputs.push_back("DataInput5");
 }

 if(ForbiddenOutputs.empty())
 {
  ForbiddenOutputs.push_back("DataOutput0");
  ForbiddenOutputs.push_back("DataOutput1");
  ForbiddenOutputs.push_back("DataOutput2");
  ForbiddenOutputs.push_back("DataOutput3");
  ForbiddenOutputs.push_back("DataOutput4");
  ForbiddenOutputs.push_back("DataOutput5");
 }
}

bool UBasePropCreator::BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
    // Проход по ТИПАМ свойств: Paramenters -> State -> Input -> Output
    for(int i =0, params = serstorage->GetNumNodes(); i <params; i++)
    {
        if(!serstorage->SelectNode(i))
            return false;
        std::string prop_type = serstorage->GetNodeName();

        // Проход по конкретным свойствам внутри типа
        for(int j =0, props = serstorage->GetNumNodes(); j < props; j++)
        {
            try{
                if(!serstorage->SelectNode(j))
                    return false;
                std::string prop_name = serstorage->GetNodeName();

                if(prop_type == "Parameters")
                {
                    CreateProperty<ULProperty,ptPubParameter>(serstorage,mock_unet);
                }
                else if(prop_type == "State")
                {
                    CreateProperty<ULProperty,ptPubState>(serstorage,mock_unet);
                }
                else if(prop_type == "Input")
                {
                    // проверки на запрещенные имена
                    if(std::find(ForbiddenInputs.begin(),ForbiddenInputs.end(),serstorage->GetNodeName()) != ForbiddenInputs.end())
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreateProperty<UPropertyInputData,ptPubInput>(serstorage,mock_unet);
                }
                else if(prop_type == "Output")
                {
                    // проверки на запрещенные имена
                    if(std::find(ForbiddenOutputs.begin(),ForbiddenOutputs.end(),serstorage->GetNodeName()) != ForbiddenOutputs.end())
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreateProperty<UPropertyOutputData,ptPubOutput>(serstorage,mock_unet);
                }
            }
            catch(UComponent::EPropertyNameAlreadyExist& e)
            {
                std::string got = e.what();
                int k =0;
            }
            catch(...)
            {

            }/*
            catch(ParseException& e)
            {
                static std::string er_sum;
                std::string got = e.what();
                if (er_sum.find(got) == std::string::npos)
                {
                    er_sum+=got;
                }
            }
            */
            serstorage->SelectUp();
        }
        serstorage->SelectUp();
    }
    return true;
}

template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreator::CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
    std::string type = serstorage->GetNodeAttribute("Type");

    // Проверка на встроенный ли тип данных у свойства
    // Если да - создание и выход
    if(CreateSimpleTypeProperty<PropType,TypeInt>(serstorage,mock_unet))
        return;

    // Строка
    if(type == "std::string")
    {
        CreatorProperty<PropType, TypeInt, std::string>::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    //Для вектора (тип данных вектора определяется далее внутри метода CreatePropertyByType) - аналогично для остальных контейнеров
    if(type == "std::vector")
    {
        CreatorProperty<PropType, TypeInt, std::vector<UnKnow> >::CreatePropertyByType(serstorage,mock_unet);
        return;
    }
    // Для векторов с простейщими типами (bool, int, double)
    if(type == "simplevector")
    {
        CreatorProperty<PropType, TypeInt, simpleVector >::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    // Для list
    if(type == "std::list")
    {
        CreatorProperty<PropType, TypeInt, std::list<UnKnow> >::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    // Для pair
    if(type == "std::pair")
    {
        CreatorProperty<PropType, TypeInt, std::pair<UnKnow,UnKnow> >::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    // Для map
    if(type == "std::map")
    {
        CreatorProperty<PropType, TypeInt, std::map<UnKnow,UnKnow> >::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    //UBMColorModel
    if(type == "UBMColorModel")
    {
        CreatorProperty<PropType, TypeInt, UBMColorModel>::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    // UColorT
    if(type == "UColorT")
    {
        CreatorProperty<PropType, TypeInt, UColorT>::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    // UBitmap
    if(type == "UBitmap")
    {
        CreatorProperty<PropType, TypeInt, UBitmap>::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    // UBRect
    if(type == "UBRect")
    {
        CreatorProperty<PropType, TypeInt, UBRect>::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    // UBPoint
    if(type == "UBPoint")
    {
        CreatorProperty<PropType, TypeInt, UBPoint>::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    /*
    // pointer // не знаю как обрабатывать
    if(type == "pointer")
    {
       // CreatorProperty<PropType, TypeInt, ...>::CreatePropertyByType(serstorage,this);
        return;
    }
    */

    // MDMatrix
    if(type.find("MDMatrix",0) == 0)
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<UnKnow> >::CreatePropertyByType(serstorage,mock_unet);
        return;
    }

    // MDVector
    if(type.find("MDVector",0) == 0)
    {
        CreatorProperty<PropType, TypeInt, MDVector<UnKnow> >::CreatePropertyByType(serstorage,mock_unet);
        return;
    }
#ifndef __BORLANDC__   // TODO: Разобраться как обеспечить сборку в билдере этой части кода
	// MVector
	if(type.find("MVector",0) == 0)
	{
		CreatorProperty<PropType, TypeInt, MVector<UnKnow,1> >::CreatePropertyByType(serstorage,mock_unet);
		return;
	}

	// MMatrix
	if(type.find("MMatrix",0) == 0)
	{
		CreatorProperty<PropType, TypeInt, MMatrix<UnKnow,1,1> >::CreatePropertyByType(serstorage,mock_unet);
		return;
	}
#endif
}

template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
bool UBasePropCreator::CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet * mock_unet)
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
    //throw ParseException("can't handle simple type: " + type);
    return false;

}

// Основная функция (для любых определенных типов Т)
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
bool CreatorProperty<PropType, TypeInt, T>::CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
{
    std::string prop_name = serstorage->GetNodeName();
    PropType<T, UMockUNet, TypeInt>* p = new PropType<T, UMockUNet, TypeInt>(prop_name, mock_unet);
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
        if(type == "std::string")
        {
            CreatorProperty<PropType, TypeInt, std::vector<std::string> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == "UBitmapParam")
        {
            CreatorProperty<PropType, TypeInt, std::vector<UBitmapParam> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == "UBitmap")
        {
            CreatorProperty<PropType, TypeInt, std::vector<UBitmap> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
//        //Из RTV
//        if(type == "UBVSObject")
//        {
//            CreatorProperty<PropType, TypeInt, std::vector<RTV::UBVSObject3D> >::CreatePropertyByType(serstorage, mock_unet);
//            return true;
//        }
        // вектор пар
        if(type == "std::pair")
        {
            CreatorProperty<PropType, TypeInt, std::vector<std::pair<UnKnow,UnKnow> > >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        // вектор из простейщих векторов
        if(type == "simplevector")
        {
            CreatorProperty<PropType, TypeInt, std::vector<simpleVector> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
		}

        //вектор из MVector<T>
        if(type.find("MVector",0) == 0)
        {
            CreatorProperty<PropType, TypeInt, std::vector<MVector<UnKnow, 1> > >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        //вектор из MDMatrix<T>
        if(type.find("MDMatrix",0) == 0)
        {
            CreatorProperty<PropType, TypeInt, std::vector<MDMatrix<UnKnow> > >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
    }
};

// Частичная специализация для вектора с простейщими типами данным
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, simpleVector >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string prop_name = serstorage->GetNodeName();
        std::string type = serstorage->GetNodeAttribute("elemType");

        if(type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(double).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
    }
};

// Частичная специализация для вектора простейщих векторов
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, std::vector<simpleVector> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string prop_name = serstorage->GetNodeName();
        int size=serstorage->GetNumNodes();
        if(!(serstorage->SelectNode("elem",size-1)))
            return false;

        std::string type = serstorage->GetNodeAttribute("elemType");
        serstorage->SelectUp();

        if(type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<std::vector<char> > >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<std::vector<int> > >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(double).name())
        {
            CreatorProperty<PropType, TypeInt, std::vector<std::vector<double> > >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }

    }
};

// Частичная специализация для вектора пар
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, std::vector<std::pair<UnKnow,UnKnow> > >
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
            if(second_type == typeid(bool).name())
            {
                CreatorProperty<PropType, TypeInt, std::pair<std::string,bool> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }


    }
};

// Частичная специализация для вектора из MVector<T>
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, std::vector<MVector<UnKnow, 1> > >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string prop_name = serstorage->GetNodeName();

        if(!(serstorage->SelectNode("elem")))
            return false;

        std::string type = serstorage->GetNodeAttribute("Type");
        int size= RDK::atoi(serstorage->GetNodeAttribute("Size"));

        // Удаление слова MVector
        size_t pos = type.find("MVector");
        if (pos != std::string::npos)
        {
            type.erase(pos, std::string("MVector").length());
        }
        // Удаление символов '<' и '>'
        type.erase(std::remove(type.begin(), type.end(), '>'), type.end());
        type.erase(std::remove(type.begin(), type.end(), '<'), type.end());

        if(type == typeid(double).name())
        {
            if(size == 2)
            {
                CreatorProperty<PropType, TypeInt, std::vector<MVector<double,2> > >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(size == 3)
            {
                CreatorProperty<PropType, TypeInt, std::vector<MVector<double,3> > >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(size == 4)
            {
                CreatorProperty<PropType, TypeInt, std::vector<MVector<double,4> > >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(size == 6)
            {
                CreatorProperty<PropType, TypeInt, std::vector<MVector<double,6> > >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
         }

    }
};

// Частичная специализация для вектора из MDMatrix<T>
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, std::vector<MDMatrix<UnKnow> > >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
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

        // Удаление слова MDMatrix
        size_t pos = type.find("MDMatrix");
        if (pos != std::string::npos)
        {
            type.erase(pos, std::string("MDMatrix").length());
        }
        // Удаление символов '<' и '>'
        type.erase(std::remove(type.begin(), type.end(), '>'), type.end());
        type.erase(std::remove(type.begin(), type.end(), '<'), type.end());

        if(type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<bool> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(char).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned char).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(short).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned short).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned int).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long long).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long long).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(float).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<float> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(double).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long double).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<long double> >::CreatePropertyByType(serstorage, mock_unet);
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
            type = serstorage->GetNodeAttribute("elemType");
            serstorage->SelectUp();
        }


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

// Частичная специализация для MDVector
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, MDVector<UnKnow> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string type = serstorage->GetNodeAttribute("Type");
        std::string prop_name = serstorage->GetNodeName();

        // Удаление слова MDVector
        size_t pos = type.find("MDVector");
        if (pos != std::string::npos)
        {
            type.erase(pos, std::string("MDVector").length());
        }
        // Удаление символов '<' и '>'
        type.erase(std::remove(type.begin(), type.end(), '>'), type.end());
        type.erase(std::remove(type.begin(), type.end(), '<'), type.end());

        std::string matrix_type = type;

        if(type == typeid(bool).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<bool> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(char).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned char).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned char> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(short).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned short).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned short> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(int).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned int).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned int> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long long).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(unsigned long long).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<unsigned long long> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(float).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<float> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(double).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
        if(type == typeid(long double).name())
        {
            CreatorProperty<PropType, TypeInt, MDVector<long double> >::CreatePropertyByType(serstorage, mock_unet);
            return true;
        }
    }
};

// Частичная специализация для MVector  size - 2,3,4   тип - double
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, MVector<UnKnow,1> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string type = serstorage->GetNodeAttribute("Type");
        std::string prop_name = serstorage->GetNodeName();

        int size= RDK::atoi(serstorage->GetNodeAttribute("Size"));

        // Удаление слова MVector
        size_t pos = type.find("MVector");
        if (pos != std::string::npos)
        {
            type.erase(pos, std::string("MVector").length());
        }
        // Удаление символов '<' и '>'
        type.erase(std::remove(type.begin(), type.end(), '>'), type.end());
        type.erase(std::remove(type.begin(), type.end(), '<'), type.end());

        if(type == typeid(double).name())
        {
            if(size == 2)
            {
                CreatorProperty<PropType, TypeInt, MVector<double,2> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(size == 3)
            {
                CreatorProperty<PropType, TypeInt, MVector<double,3> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(size == 4)
            {
                CreatorProperty<PropType, TypeInt, MVector<double,4> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if(size == 6)
            {
                CreatorProperty<PropType, TypeInt, MVector<double,6> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
         }

    }
};

// Частичная специализация для MMatrix [rows,cols] - [3,3][4,4] типы - double, int, bool
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
class CreatorProperty<PropType, TypeInt, MMatrix<UnKnow,1,1> >
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet)
    {
        std::string type = serstorage->GetNodeAttribute("Type");
        std::string prop_name = serstorage->GetNodeName();

        int rows= RDK::atoi(serstorage->GetNodeAttribute("Rows"));
        int cols= RDK::atoi(serstorage->GetNodeAttribute("Cols"));

        // Удаление слова MMatrix
        size_t pos = type.find("MMatrix");
        if (pos != std::string::npos)
        {
            type.erase(pos, std::string("MMatrix").length());
        }
        // Удаление символов '<' и '>' для выделения символа типа
        type.erase(std::remove(type.begin(), type.end(), '>'), type.end());
        type.erase(std::remove(type.begin(), type.end(), '<'), type.end());

        if(type == typeid(bool).name())
        {
            if( (rows == 3) && (cols == 3))
            {
                CreatorProperty<PropType, TypeInt, MMatrix<bool,3,3> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if( (rows == 4) && (cols == 4))
            {
                CreatorProperty<PropType, TypeInt, MMatrix<bool,4,4> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }
        if(type == typeid(int).name())
        {
            if( (rows == 3) && (cols == 3))
            {
                CreatorProperty<PropType, TypeInt, MMatrix<int,3,3> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if( (rows == 4) && (cols == 4))
            {
                CreatorProperty<PropType, TypeInt, MMatrix<int,4,4> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }
        if(type == typeid(double).name())
        {
            if( (rows == 3) && (cols == 3))
            {
                CreatorProperty<PropType, TypeInt, MMatrix<double,3,3> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
            if( (rows == 4) && (cols == 4))
            {
				CreatorProperty<PropType, TypeInt, MMatrix<double,4,4> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
        }

    }
};

// Частичная специализация для std::pair
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

        if(first_type == typeid(double).name())
        {
            if(second_type == typeid(int).name())
            {
                CreatorProperty<PropType, TypeInt, std::pair<double,int> >::CreatePropertyByType(serstorage, mock_unet);
                return true;
            }
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
//            if(second_type == "UBPtzPelcoCmd")
//            {
//                CreatorProperty<PropType, TypeInt, std::map<std::string, UBPtzPelcoCmd> >::CreatePropertyByType(serstorage, mock_unet);
//                return true;
//            }
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
