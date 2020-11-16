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
        // Если дошли до секции компонентов и связей
        if(serstorage->GetNodeName() == "Components" || serstorage->GetNodeName() == "Links")
        {
            serstorage->SelectUp();
            break;
        }
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

    std::string temp;
    serstorage->Save(temp);
    ClassDesriptionXML.Load(temp,"");

    // Загрузка компонентов и связей
    DelAllComponents();

    if(!serstorage->SelectNode("Components"))
    {
        LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Components section not found"));
        return;
    }
    storage = GetStorage();
    for(int i=0;i<serstorage->GetNumNodes();i++)
    {
        serstorage->SelectNode(i);
        std::string nodename=serstorage->GetNodeName();
        std::string name=serstorage->GetNodeAttribute("Class");
        try
        {
            int id=Storage->FindClassId(name);
            UEPtr<UNet> newcont=dynamic_pointer_cast<UNet>(storage->TakeObject(id));
            if(!newcont)
                continue;
            if(FindStaticComponent(name,nodename) == 0) // Это НЕ уже существующий статический компонент
            {
                if(AddComponent(static_pointer_cast<UContainer>(newcont)) == ForbiddenId)
                {
                    storage->ReturnObject(newcont);
                    continue;
                }
            }

            if(!newcont->LoadComponent(serstorage,false))
            {
                std::string tempname;
                LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("LoadComponent failed: ")+newcont->GetFullName(tempname));
            }
        }
        catch(UException &exception)
        {
            if(Logger)
                Logger->ProcessException(exception);
        }
        serstorage->SelectUp();
    }
    serstorage->SelectUp();

    // связи
    serstorage->SelectNode("Links");
    if(!SetComponentInternalLinks(serstorage,0))
    {
        LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Links creating failed in class ") + class_name);
        return;
    }
    serstorage->SelectUp();

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

