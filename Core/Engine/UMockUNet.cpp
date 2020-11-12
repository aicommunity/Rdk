#ifndef UMOCKUNET_CPP
#define UMOCKUNET_CPP

#include "UMockUNet.h"
#include "UStorage.h"
#include "UBasePropCreatorTempl.h"


namespace RDK {

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

    // Установка хранилище и логера
    SetStorage(storage);
    SetLogger(storage->GetLogger());

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
        // Свойство не найдено - то есть не было создано
        if(CreatedProps.find((*p).first) == CreatedProps.end())
        {
            if(std::find(UBasePropCreatorTempl::GetForbiddenInputs().begin(),  UBasePropCreatorTempl::GetForbiddenInputs().end(),  (*p).first) != UBasePropCreatorTempl::GetForbiddenInputs().end())
                continue;

            if(std::find(UBasePropCreatorTempl::GetForbiddenOutputs().begin(), UBasePropCreatorTempl::GetForbiddenOutputs().end(), (*p).first) != UBasePropCreatorTempl::GetForbiddenOutputs().end())
                continue;

            LogMessageEx(RDK_EX_WARNING, __FUNCTION__,
                         "In class "+ class_name + ": failed to create property: " + (*p).first + " with type: " + (*p).second);

        }
    }

    // Сохранение собственного описания в XML
    ClassDesriptionXML.Destroy();

    // TODO сохранять желатемое описание в собственное (чтобы при дальнейших созданиях компонента был вывод ошибок в лог)
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

