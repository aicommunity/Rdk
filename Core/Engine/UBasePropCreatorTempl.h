#ifndef UBASEPROPCREATORTEMPL_H
#define UBASEPROPCREATORTEMPL_H

#include "UCreatorPropTemplate.h"
namespace RDK {

// �������� �� �������� ������� ��� ������������ ������� �������
class UBasePropCreatorTempl
{

public:
	UBasePropCreatorTempl();

	static const std::vector<std::string>& GetForbiddenInputs();

	static const std::vector<std::string>& GetForbiddenOutputs();

	// ������� �������� ����������� ������� � ����������� �� ���� (������) ��������
 //	template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
 //	static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);
};

// �������� ������� �������� ������� ���� ��������� ����� (����������� � Storage)
template <class CreatorT>
bool BaseCrPropMockTempl(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);


template <class CreatorT>
bool BaseCrPropMockTempl(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
    // ������ �� ����� �������: Paramenters -> State -> Input -> Output
    for(int i = serstorage->GetNumNodes() - 1; i >= 0; i--)
    {
        if(!serstorage->SelectNode(i))
            return false;
        std::string prop_type = serstorage->GetNodeName();

        // ������ �� ���������� ��������� ������ ���� (� �������� �������)
        for(int j = 0, props = serstorage->GetNumNodes(); j < props; j++)
        {
            try{
                if(!serstorage->SelectNode(j))
                    return false;
                std::string prop_name   = serstorage->GetNodeName();
                unsigned int p_type     = RDK::atoi(serstorage->GetNodeAttribute("PType"));
                unsigned int io_type    = RDK::atoi(serstorage->GetNodeAttribute("IoType"));
                if(prop_type == "Parameters")
                {
                    CreatorT::template CreateProperty<ULProperty,ptPubParameter>(serstorage,mock_unet,p_type);
                }
                else if(prop_type == "State")
                {
                    CreatorT::template CreateProperty<ULProperty,ptPubState>(serstorage,mock_unet,p_type);
                }
                else if(prop_type == "Input")
                {
                    // �������� �� ����������� �����
                    if(std::find(UBasePropCreatorTempl::GetForbiddenInputs().begin(), UBasePropCreatorTempl::GetForbiddenInputs().end(),serstorage->GetNodeName()) != UBasePropCreatorTempl::GetForbiddenInputs().end())
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    // UPropertyInputData
                    if((io_type & (ipSingle | ipData)) == (ipSingle | ipData))
                    {
                        CreatorT::template CreateProperty<UPropertyInputData,ptPubInput>(serstorage,mock_unet,p_type);
                        serstorage->SelectUp();
                        continue;
                    }
                    // UPropertyInputCData
                    if((io_type & (ipRange  | ipData)) == (ipRange  | ipData))
                    {
                        CreatorT::template CreateProperty<UPropertyInputCData,ptPubInput>(serstorage,mock_unet,p_type);
                        serstorage->SelectUp();
                        continue;
                    }
                    CreatorT::template CreateProperty<UPropertyInputData,ptPubInput>(serstorage,mock_unet,p_type);
                }
                else if(prop_type == "Output")
                {
                    // �������� �� ����������� �����
                    if(std::find(UBasePropCreatorTempl::GetForbiddenOutputs().begin(),UBasePropCreatorTempl::GetForbiddenOutputs().end(),serstorage->GetNodeName()) != UBasePropCreatorTempl::GetForbiddenOutputs().end())
                    {
                        serstorage->SelectUp();
                        continue;
                    }
                    CreatorT::template CreateProperty<UPropertyOutputData,ptPubOutput>(serstorage,mock_unet,p_type);
                }
            }
            catch(UComponent::EPropertyNameAlreadyExist& )
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
