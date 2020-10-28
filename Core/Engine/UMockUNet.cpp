#ifndef UMOCKUNET_CPP
#define UMOCKUNET_CPP

#include "UMockUNet.h"
#include "UStorage.h"

namespace RDK {
// TODO вывод в Logger информации о несозданных свойствах
/* *************************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UMockUNet::UMockUNet(RDK::USerStorageXML *serstorage, UStorage* storage)
{
    std::string comp_name = serstorage->GetNodeName();
    serstorage->SelectNode(comp_name);

    // Имя компонента
    this->SetName(comp_name);
    std::string class_name = serstorage->GetNodeAttribute("Class");

    // Установка указателя на хранилище
    SetStorage(storage);

    // Вызов всех добавленных функций создания свойств
    std::list<funcCrPropMock> funcs = GetStorage()->GetFunctionsCrPropMock();
    for (std::list<funcCrPropMock>::iterator f = funcs.begin(); f != funcs.end(); ++f)
    {
        // Вызов функций создания свойств по очереди
        (*f)(serstorage, this);
    }

    // Список сфомированных свойств текущей заглушки
    std::map<NameT,UVariable> CreatedProps = this->GetPropertiesList();

    // Список свойств из xml-ки
    std::vector<std::pair<std::string,std::string> > PropsNames;
    // Проход по всем свойствам для формирования списка свойств
    for(int i =0, params = serstorage->GetNumNodes(); i <params; i++)
    {
        serstorage->SelectNode(i);
        for(int j = 0, props = serstorage->GetNumNodes(); j < props; j++)
        {
            serstorage->SelectNode(j);

            std::string prop_name = serstorage->GetNodeName();
            std::string type = serstorage->GetNodeAttribute("Type");

            PropsNames.push_back(make_pair(prop_name, type));

            serstorage->SelectUp();
        }
        serstorage->SelectUp();
    }

    // Сравнение списков свойств
    for(std::vector<std::pair<std::string,std::string> >::iterator p = PropsNames.begin(); p != PropsNames.end(); ++p)
    {
        if(CreatedProps.find((*p).first) == CreatedProps.end())
        {
            if(std::find(ForbiddenInputs.begin(),ForbiddenInputs.end(),(*p).first) != ForbiddenInputs.end())
                continue;

            if(std::find(ForbiddenOutputs.begin(),ForbiddenOutputs.end(),(*p).first) != ForbiddenOutputs.end())
                continue;
            if(GetStorage()->GetLogger())
            {
                GetStorage()->GetLogger()->LogMessage(RDK_EX_DEBUG, __FUNCTION__,
                                                      "In class "+ class_name + ": failed to create property: " + (*p).first + "with type" + (*p).second);
            }
        }
    }

    // Сохранение собственного описания в XML
    ClassDesriptionXML.Destroy();
    std::string temp;
    serstorage->Save(temp);
    ClassDesriptionXML.Load(temp,"");
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

