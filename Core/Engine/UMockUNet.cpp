#ifndef UMOCKUNET_CPP
#define UMOCKUNET_CPP

#include "UMockUNet.h"
#include "UStorage.h"

namespace RDK {
//TODO другое сохранение simplevector-a
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
    // нельзя делать UploadClass

    // Список свойств текущей заглушки
    std::map<NameT,UVariable> propers = this->GetPropertiesList();

    std::vector<std::string> PropsNames;

    // Проход по ТИПАМ свойств: Paramenters -> State -> Input -> Output
    for(int i =0, params = serstorage->GetNumNodes(); i <params; i++)
    {
        serstorage->SelectNode(i);
        std::string prop_type = serstorage->GetNodeName();
        for(int j = 0, props = serstorage->GetNumNodes(); j < props; j++)
        {
            serstorage->SelectNode(j);
            std::string prop_name = serstorage->GetNodeName();

            PropsNames.push_back(prop_name);
            serstorage->SelectUp();
         }
        serstorage->SelectUp();
    }
    static std::vector<std::string> errors;
    errors.clear();
    // проверка на то все ли созданы
    for(std::vector<std::string>::iterator p = PropsNames.begin(); p != PropsNames.end(); ++p)
    {
        if(propers.find(*p) == propers.end())
        {
            std::string temp;
            temp = *p + "Wasn't created in " + class_name;
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

