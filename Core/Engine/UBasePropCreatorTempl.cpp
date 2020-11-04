#include "UBasePropCreatorTempl.h"
#include "UComponent.h"


namespace RDK {


UBasePropCreatorTempl::UBasePropCreatorTempl()
{

}

const std::vector<std::string>& UBasePropCreatorTempl::GetForbiddenInputs()
{
    static std::vector<std::string> ForbiddenInputs;
    if(ForbiddenInputs.empty())
    {
     ForbiddenInputs.push_back("DataInput0");
     ForbiddenInputs.push_back("DataInput1");
     ForbiddenInputs.push_back("DataInput2");
     ForbiddenInputs.push_back("DataInput3");
     ForbiddenInputs.push_back("DataInput4");
     ForbiddenInputs.push_back("DataInput5");
    }

    return ForbiddenInputs;
}

const std::vector<std::string>& UBasePropCreatorTempl::GetForbiddenOutputs()
{
    static std::vector<std::string> ForbiddenOutputs;
    if(ForbiddenOutputs.empty())
    {
     ForbiddenOutputs.push_back("DataOutput0");
     ForbiddenOutputs.push_back("DataOutput1");
     ForbiddenOutputs.push_back("DataOutput2");
     ForbiddenOutputs.push_back("DataOutput3");
     ForbiddenOutputs.push_back("DataOutput4");
     ForbiddenOutputs.push_back("DataOutput5");
    }

    return ForbiddenOutputs;
}
	  /*
// Основная функция создания свойств всех возможных типов (добавляется в Storage)
bool UBasePropCreatorTempl::BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
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
                    if(std::find(GetForbiddenInputs().begin(),GetForbiddenInputs().end(),serstorage->GetNodeName()) != GetForbiddenInputs().end())
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreateProperty<UPropertyInputData,ptPubInput>(serstorage,mock_unet);
                }
                else if(prop_type == "Output")
                {
                    // проверки на запрещенные имена
                    if(std::find(GetForbiddenOutputs().begin(),GetForbiddenOutputs().end(),serstorage->GetNodeName()) != GetForbiddenOutputs().end())
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreateProperty<UPropertyOutputData,ptPubOutput>(serstorage,mock_unet);
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
}     */

}
