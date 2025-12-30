#ifndef UBASEPROPCREATORTEMPL_H
#define UBASEPROPCREATORTEMPL_H

#include "UCreatorPropTemplate.h"
namespace RDK {

// Base class for property creator template for component property creation
class UBasePropCreatorTempl
{

public:
	UBasePropCreatorTempl();

	static const std::vector<std::string>& GetForbiddenInputs();

	static const std::vector<std::string>& GetForbiddenOutputs();

	// Template method for creating component properties based on type (legacy)
 //	template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
 //	static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);
};

// Template function for creating all properties for component (connected to Storage)
template <class CreatorT>
bool BaseCrPropMockTempl(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);


template <class CreatorT>
bool BaseCrPropMockTempl(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
 // Process in reverse order: Parameters -> State -> Input -> Output
 for(int i = serstorage->GetNumNodes() - 1; i >= 0; i--)
 {
  if(!serstorage->SelectNode(i))
   return false;
  std::string prop_type = serstorage->GetNodeName();

  // Process all properties of this type (in reverse order)
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
     CreatorT::template CreateProperty<UProperty,ptPubParameter>(serstorage,mock_unet,p_type);
    }
    else if(prop_type == "State")
    {
     CreatorT::template CreateProperty<UProperty,ptPubState>(serstorage,mock_unet,p_type);
    }
    else if(prop_type == "Input")
    {
     // Check for forbidden names
     if(std::find(UBasePropCreatorTempl::GetForbiddenInputs().begin(), UBasePropCreatorTempl::GetForbiddenInputs().end(),serstorage->GetNodeName()) != UBasePropCreatorTempl::GetForbiddenInputs().end())
     {
      serstorage->SelectUp();
      continue;
     }
     // Unified Input property creation
     // All property types now use UProperty directly
     // Check only ipData (ipComp removed as legacy)
     if((io_type & ipDataRange) == ipDataRange)
     {
      // Vector property
      CreatorT::template CreateProperty<UProperty,ptPubInput>(serstorage,mock_unet,p_type);
      serstorage->SelectUp();
      continue;
     }
     // Regular property
     CreatorT::template CreateProperty<UProperty,ptPubInput>(serstorage,mock_unet,p_type);
    }
    else if(prop_type == "Output")
    {
     // Check for forbidden names
     if(std::find(UBasePropCreatorTempl::GetForbiddenOutputs().begin(),UBasePropCreatorTempl::GetForbiddenOutputs().end(),serstorage->GetNodeName()) != UBasePropCreatorTempl::GetForbiddenOutputs().end())
     {
      serstorage->SelectUp();
      continue;
     }
     // Unified Output property creation
     // All property types now use UProperty directly
     CreatorT::template CreateProperty<UProperty,ptPubOutput>(serstorage,mock_unet,p_type);
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
