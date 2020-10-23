#ifndef UMOCKUNET_CPP
#define UMOCKUNET_CPP

#include "UMockUNet.h"
#include "UStorage.h"

namespace RDK {

/* *************************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UMockUNet::UMockUNet(RDK::USerStorageXML *serstorage, UStorage* storage)
{
    std::string comp_name = serstorage->GetNodeName();
    serstorage->SelectNode(comp_name);
    this->SetName(comp_name);
    std::string class_name = serstorage->GetNodeAttribute("Class");

    SetStorage(storage);

    // Вызов всех добавленных функций создания свойств
    if(!GetStorage())
        return;
    std::vector<funcCrPropMock> funcs = GetStorage()->GetFunctionsCrPropMock();
    int size = funcs.size();
    size++;
    for (std::vector<funcCrPropMock>::iterator f = funcs.begin();
         f != funcs.end(); ++f)
    {
        (*f)(serstorage, this);
    }
    // TODO обработка на то, если какие-либо свойства не создались
    // нельзя делать UploadClass (или можно?)

    // Список свойств текущей заглушки
    std::map<NameT,UVariable> propers = this->GetPropertiesList();

    // Список свойств из xml-ки
    std::vector<std::pair<std::string,std::string> > PropsNames;

    // Проход по ТИПАМ свойств: Paramenters -> State -> Input -> Output
    for(int i =0, params = serstorage->GetNumNodes(); i <params; i++)
    {
        serstorage->SelectNode(i);

        std::string prop_type = serstorage->GetNodeName();
        for(int j = 0, props = serstorage->GetNumNodes(); j < props; j++)
        {
            serstorage->SelectNode(j);
            std::string prop_name = serstorage->GetNodeName();
            std::string type = serstorage->GetNodeAttribute("Type");

            PropsNames.push_back(make_pair(prop_name,type));

            serstorage->SelectUp();
        }
        serstorage->SelectUp();
    }

    std::vector<std::string> errors;
    errors.clear();
    // проверка на то все ли созданы
    for(std::vector<std::pair<std::string,std::string> >::iterator p = PropsNames.begin(); p != PropsNames.end(); ++p)
    {
        if(propers.find((*p).first) == propers.end())
        {
            std::string temp;

            if(std::find(ForbiddenInputs.begin(),ForbiddenInputs.end(),(*p).first) != ForbiddenInputs.end())
                continue;

            if(std::find(ForbiddenOutputs.begin(),ForbiddenOutputs.end(),(*p).first) != ForbiddenOutputs.end())
                continue;

            temp = (*p).first + " in " + class_name +"  Type: " + (*p).second;
            errors.push_back(temp);
        }
    }

    SetStorage(0);

    ClassDesriptionXML.Destroy();

    std::string temp;
    serstorage->Save(temp);

    ClassDesriptionXML.Load(temp,"");

    //this->SaveComponent(&ClassDesriptionXML, false, ptAny|pgPublic);
}

UMockUNet* UMockUNet::New(void)
{
    return new UMockUNet(&ClassDesriptionXML,GetStorage());
}

UMockUNet::~UMockUNet()
{
}


}
#endif

