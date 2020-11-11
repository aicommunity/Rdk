#ifndef UBASEPROPCREATORMATRIX_H
#define UBASEPROPCREATORMATRIX_H

#include "UBasePropCreatorTempl.h"

namespace RDK {

// Отвечает за создание функции для формирования базовых свойств
class UBasePropCreatorMatrix: public UBasePropCreatorTempl
{

public:
	// Основная функция создания свойств всех возможных типов (добавляется в Storage)
	static bool BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

	// Функция вызывает необходимые фукнции в зависимости от типа (строка) свойства
	template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
	static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа вектор из MVector<T>
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfMVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа вектор из MDMatrix<T>
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfMDMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа MDMatrix
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMDMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа MDVector
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMDVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа MVector  size - 2,3,4   тип - double
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа MMatrix [rows,cols] - [3,3][4,4] типы - double, int, bool
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

};


// Функция вызывает необходимые фукнции в зависимости от типа (строка) свойства
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorMatrix::CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
    std::string type = serstorage->GetNodeAttribute("Type");

	// MDMatrix
    if(type.find("MDMatrix",0) == 0)
    {
        CreatePropertyMDMatrix<PropType, TypeInt>(serstorage,mock_unet);
        return;
    }

    // MDVector
    if(type.find("MDVector",0) == 0)
    {
        CreatePropertyMDVector<PropType, TypeInt>(serstorage,mock_unet);
		return;
	}

	// MVector
	if(type.find("MVector",0) == 0)
	{
		CreatePropertyMVector<PropType, TypeInt>(serstorage,mock_unet);
		return;
	}

	// MMatrix
	if(type.find("MMatrix",0) == 0)
	{
		CreatePropertyMMatrix<PropType, TypeInt>(serstorage,mock_unet);
		return;
	}

    if(type == "std::vector")
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

	 //вектор из MVector<T>
	 if(type.find("MVector",0) == 0)
	 {
		CreatePropertyVectorOfMVector<PropType, TypeInt>(serstorage, mock_unet);
		return;
	 }
	 //вектор из MDMatrix<T>
	 if(type.find("MDMatrix",0) == 0)
	 {
		CreatePropertyVectorOfMDMatrix<PropType, TypeInt>(serstorage, mock_unet);
		return;
	 }
	}
}


// Функция для создания свойства типа вектор из MVector<T>
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorMatrix::CreatePropertyVectorOfMVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
    std::string prop_name = serstorage->GetNodeName();

    if(!(serstorage->SelectNode("elem")))
		return;

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
            return;
        }
        if(size == 3)
        {
            CreatorProperty<PropType, TypeInt, std::vector<MVector<double,3> > >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
        if(size == 4)
        {
            CreatorProperty<PropType, TypeInt, std::vector<MVector<double,4> > >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
        if(size == 6)
        {
            CreatorProperty<PropType, TypeInt, std::vector<MVector<double,6> > >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
    }
}

// Функция для создания свойства типа вектор из MDMatrix<T>
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorMatrix::CreatePropertyVectorOfMDMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
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
        return;
    }
    if(type == typeid(char).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<char> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned char).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned char> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(short).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<short> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned short).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned short> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(int).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<int> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned int).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned int> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(long).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned long).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(long long).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<long long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned long long).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned long long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(float).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<float> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(double).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<double> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(long double).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<long double> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
}

// Функция для создания свойства типа MDMatrix
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorMatrix::CreatePropertyMDMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
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
		return;
	}
	if(type == typeid(char).name())
	{
		CreatorProperty<PropType, TypeInt, MDMatrix<char> >::CreatePropertyByType(serstorage, mock_unet);
        return;
	}
	if(type == typeid(unsigned char).name())
	{
		CreatorProperty<PropType, TypeInt, MDMatrix<unsigned char> >::CreatePropertyByType(serstorage, mock_unet);
		return;
	}
	if(type == typeid(short).name())
	{
        CreatorProperty<PropType, TypeInt, MDMatrix<short> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned short).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<unsigned short> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(int).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<int> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned int).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<unsigned int> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(long).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned long).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<unsigned long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(long long).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<long long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned long long).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<unsigned long long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(float).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<float> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(double).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<double> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(long double).name())
    {
        CreatorProperty<PropType, TypeInt, MDMatrix<long double> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
}

// Функция для создания свойства типа MDVector
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorMatrix::CreatePropertyMDVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
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
        return;
    }
    if(type == typeid(char).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<char> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned char).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned char> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(short).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<short> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned short).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned short> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(int).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<int> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned int).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned int> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(long).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned long).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(long long).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<long long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(unsigned long long).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<unsigned long long> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(float).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<float> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(double).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<double> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
    if(type == typeid(long double).name())
    {
        CreatorProperty<PropType, TypeInt, MDVector<long double> >::CreatePropertyByType(serstorage, mock_unet);
        return;
    }
}

// Функция для создания свойства типа MVector  size - 2,3,4   тип - double
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorMatrix::CreatePropertyMVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
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
            return;
        }
        if(size == 3)
        {
            CreatorProperty<PropType, TypeInt, MVector<double,3> >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
        if(size == 4)
        {
            CreatorProperty<PropType, TypeInt, MVector<double,4> >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
        if(size == 6)
        {
            CreatorProperty<PropType, TypeInt, MVector<double,6> >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
     }
}

// Функция для создания свойства типа MMatrix [rows,cols] - [3,3][4,4] типы - double, int, bool
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void UBasePropCreatorMatrix::CreatePropertyMMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
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
            return;
        }
        if( (rows == 4) && (cols == 4))
        {
            CreatorProperty<PropType, TypeInt, MMatrix<bool,4,4> >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
    }
    if(type == typeid(int).name())
    {
        if( (rows == 3) && (cols == 3))
        {
            CreatorProperty<PropType, TypeInt, MMatrix<int,3,3> >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
        if( (rows == 4) && (cols == 4))
        {
            CreatorProperty<PropType, TypeInt, MMatrix<int,4,4> >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
    }
    if(type == typeid(double).name())
    {
        if( (rows == 3) && (cols == 3))
        {
            CreatorProperty<PropType, TypeInt, MMatrix<double,3,3> >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
        if( (rows == 4) && (cols == 4))
        {
            CreatorProperty<PropType, TypeInt, MMatrix<double,4,4> >::CreatePropertyByType(serstorage, mock_unet);
            return;
        }
    }
}

}
#endif // UBASEPROPCREATOR_H
