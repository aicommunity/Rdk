#ifndef UBASEPROPCREATORTEMPL_H
#define UBASEPROPCREATORTEMPL_H

#include "UCreatorPropTemplate.h"
namespace RDK {

// Отвечает за создание функции для формирования базовых свойств
class UBasePropCreatorTempl
{

public:
	UBasePropCreatorTempl();

	static const std::vector<std::string>& GetForbiddenInputs();

	static const std::vector<std::string>& GetForbiddenOutputs();

	// Функция вызывает необходимые фукнции в зависимости от типа (строка) свойства
 //	template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
 //	static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);
};

// Основная функция создания свойств всех возможных типов (добавляется в Storage)
template <class CreatorT>
bool BaseCrPropMockTempl(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);


template <class CreatorT>
bool BaseCrPropMockTempl(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
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
                    CreatorT::template CreateProperty<ULProperty,ptPubParameter>(serstorage,mock_unet);
                }
                else if(prop_type == "State")
                {
                    CreatorT::template CreateProperty<ULProperty,ptPubState>(serstorage,mock_unet);
                }
                else if(prop_type == "Input")
                {
                    // проверки на запрещенные имена
					if(std::find(CreatorT::GetForbiddenInputs().begin(),CreatorT::GetForbiddenInputs().end(),serstorage->GetNodeName()) != CreatorT::GetForbiddenInputs().end())
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreatorT::template CreateProperty<UPropertyInputData,ptPubInput>(serstorage,mock_unet);
                }
                else if(prop_type == "Output")
                {
                    // проверки на запрещенные имена
					if(std::find(CreatorT::GetForbiddenOutputs().begin(),CreatorT::GetForbiddenOutputs().end(),serstorage->GetNodeName()) != CreatorT::GetForbiddenOutputs().end())
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreatorT::template CreateProperty<UPropertyOutputData,ptPubOutput>(serstorage,mock_unet);
                }
            }
            catch(UComponent::EPropertyNameAlreadyExist& e)
            {

            }
            catch(...)
            {

            }
            serstorage->SelectUp();
        }
        serstorage->SelectUp();
    }
    return true;
}



}
#endif // UBASEPROPCREATOR_H
