/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UALIBRARY_CPP
#define UALIBRARY_CPP

#include "ULibrary.h"
#include "UNet.h"
#include "UMockUNet.h"
#include "UComponentFactory.h"

namespace RDK {

/// Список статически загруженных библиотек
//std::list<ULibrary*> ULibrary::LibraryList;


// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULibrary::ULibrary(const string &name, const string &version, int type, int revision)
: Name(name), Version(version), Revision(revision), Type(type), Storage(0)
{
}

ULibrary::ULibrary(const string &name, const string &version, const RDK::UVersion &core_version, int type, int revision)
: Name(name), Version(version), Revision(revision), Type(type), Storage(0)
{
 CoreVersion=new RDK::UVersion(core_version);
}

ULibrary::~ULibrary(void)
{
 if(CoreVersion)
 {
  delete CoreVersion;
  CoreVersion=0;
 }
}
// --------------------------

// --------------------------
// Методы управления статически загруженными библиотеками
// --------------------------
/*
/// Возвращает коллекцию статически загруженных библиотек
const std::list<ULibrary*>& ULibrary::GetLibraryList(void)
{
 return LibraryList;
}

/// Очищает коллекцию библиотек
void ULibrary::ClearLibraryList(void)
{
 LibraryList.clear();
}

/// Проверяет наличие библиотеки по ее имени
bool ULibrary::CheckLibrary(const std::string &name)
{
 std::list<ULibrary*>::const_iterator I=LibraryList.begin();
 std::list<ULibrary*>::const_iterator J=LibraryList.end();
 for(;I!=J;++I)
 {
  if((*I)->GetName() == name)
   return true;
 }
 return false;
}

/// Добавляет библиотеку в список, если библиотеки с таким именем еще нет в списке
bool ULibrary::AddUniqueLibrary(ULibrary* const lib)
{
 if(!lib)
  return false;

 if(CheckLibrary(lib->GetName()))
  return false;  /// Заглушка!! здесь может быть warning исключение

 LibraryList.push_back(lib);
 return true;
}

/// Удаляет библиотеку из списка.
/// Если список становится пуст, то уничтожает его
void ULibrary::RemoveLibrary(ULibrary* const lib)
{
 std::list<ULibrary*>::iterator I=LibraryList.begin();
 std::list<ULibrary*>::iterator J=LibraryList.end();
 for(;I!=J;++I)
 {
  if((*I) == lib)
  {
   LibraryList.erase(I);
   break;
  }
 }
} */
// --------------------------

// --------------------------
// Методы доступа к параметрам библиотеки
// --------------------------
// Возвращает имя библиотеки
const string& ULibrary::GetName(void) const
{
 return Name;
}

// Возвращает версию библиотеки
const string& ULibrary::GetVersion(void) const
{
 return Version;
}

/// Возвращает ревизию системы контроля версий
int ULibrary::GetRevision(void) const
{
 return Revision;
}

/// Возвращает версию ядра, использованного при сборке библиотеки
const UEPtr<RDK::UVersion> ULibrary::GetCoreVersion(void) const
{
 return CoreVersion;
}


/// Тип библиотеки
/// 0 - Внутренняя библиотека (собрана вместе с ядром)
/// 1 - Внешняя библиотека (загружена из внешней dll)
/// 2 - Библиотека, созданная во время выполнения
int ULibrary::GetType(void) const
{
 return Type;
}

/// Зависимости библиотеки от других библиотек
const std::vector<pair<string, string> > ULibrary::GetDependencies(void) const
{
 return Dependencies;
}
// --------------------------

// --------------------------
// Методы доступа к данным загрузки
// --------------------------
/// Возвращает true если коллекция предоставляет класс с таким именем
bool ULibrary::IsClassNamePresent(const std::string &class_name) const
{
 for(size_t i=0;i<ClassesList.size();i++)
  if(ClassesList[i] == class_name)
   return true;
 return false;
}

/// Имена классов библиотеки
const vector<string>& ULibrary::GetClassesList(void) const
{
 return ClassesList;
}

// Содержит имена всех успешно загруженных образцов
const vector<string>& ULibrary::GetComplete(void) const
{
 return Complete;
}

// Содержит имена всех не загруженных образцов
const vector<string>& ULibrary::GetIncomplete(void) const
{
 return Incomplete;
}

// Очищает оба списка Complete и Incomplete
void ULibrary::ClearIncompleteAndComplete(void)
{
    Incomplete.clear();
    Complete.clear();
}
// --------------------------

// --------------------------
// Методы доступа к данным библиотеки
// --------------------------
// Заполняет заданное хранилище набором образцов классов.
// Если класс с заданным именем уже существует, то он пропускается.
// Возвращает число реально загруженных классов.
int ULibrary::Upload(UStorage *storage)
{
 /*Complete.clear();
 Incomplete.clear();
 ULibrary::Upload(storage);
 return Complete.size();
 */
 int count=0;

 Storage=storage;

 if(!Storage)
  return 0;

 Incomplete.clear();
 CreateClassSamples(Storage);
 count=int(Complete.size());

 //Оставление ссылки на Storage для RunTime библиотек
// if(Type != 2)
//    Storage=0;
 return count;
}
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
/// Проверяет зависимости библиотеки от других библиотек
/// и возвращает список недостающих библиотек
/// Возвращает true если все необходимые библиотеки уже загружены
bool ULibrary::CheckDependencies(UStorage *storage, std::vector<pair<string, string> > &dependencies) const
{
 if(!storage)
  return false;

 if(Dependencies.empty())
  return true;

 dependencies.clear();
 int num_libraries=storage->GetNumCollections();
 for(size_t i=0;i<Dependencies.size();i++)
 {
  bool dep_found=false;
  for(int j=0;j<num_libraries;j++)
  {
   UEPtr<ULibrary> lib=storage->GetCollection(j);
   if(lib && lib->GetName() == Dependencies[i].first
	&& (Dependencies[i].second.empty() || Dependencies[i].second == lib->GetVersion()))
   {
	dep_found=true;
	break;
   }
  }
  if(!dep_found)
   dependencies.push_back(Dependencies[i]);
 }
 if(dependencies.empty())
  return true;

 return false;
}

// Добавляет в хранилище очередной класс
bool ULibrary::UploadClass(const UId &classid, UEPtr<UComponent> cont)
{
 return false;//ULibrary::UploadClass(classid, cont);
}

bool ULibrary::UploadClass(const string &name, UEPtr<UComponent> cont)
{
 if(!cont)
  return false;

 if(name.size() == 0)
 {
  delete cont;
  return false;
 }

 if(Storage->CheckClass(name))
  return true;

 std::vector<std::string>::iterator I;
 UEPtr<UVirtualMethodFactory> factory;
 try
 {
  cont->SetLogger(Storage->GetLogger());
  cont->SetStorage(Storage);
  cont->Build();
  factory = new UVirtualMethodFactory(cont);
 }
 catch(...)
 {
  if(find(Incomplete.begin(),Incomplete.end(),name) == Incomplete.end())
   Incomplete.push_back(name);
  I=find(ClassesList.begin(),ClassesList.end(),name);
  if(I != ClassesList.end())
   ClassesList.erase(I);
  I=find(Complete.begin(),Complete.end(),name);
  if(I != Complete.end())
   Complete.erase(I);
  delete factory;
  return false;
 }

 if(!Storage->AddClass(factory,name))
 {
  if(find(Incomplete.begin(),Incomplete.end(),name) == Incomplete.end())
   Incomplete.push_back(name);
  I=find(ClassesList.begin(),ClassesList.end(),name);
  if(I != ClassesList.end())
   ClassesList.erase(I);
  I=find(Complete.begin(),Complete.end(),name);
  if(I != Complete.end())
   Complete.erase(I);

  delete factory;
  return false;
 }

 if(find(ClassesList.begin(),ClassesList.end(),name) == ClassesList.end())
  ClassesList.push_back(name);
 if(find(Complete.begin(),Complete.end(),name) == Complete.end())
  Complete.push_back(name);
 I=find(Incomplete.begin(),Incomplete.end(),name);
 if(I != Incomplete.end())
  Incomplete.erase(I);

 return true;
}

bool ULibrary::UploadClass(const std::string &class_name, const std::string &component_name, UComponent* (*funcPointer)(void))
{
 if(!funcPointer)
  return false;

 if(class_name.size() == 0)
 {
  return false;
 }

 if(Storage->CheckClass(class_name))
  return true;

 std::vector<std::string>::iterator I;
 UEPtr<UComponentFactoryMethod> factory = new UComponentFactoryMethod(Storage,funcPointer,component_name);

 if(!Storage->AddClass(factory,class_name))
 {
  if(find(Incomplete.begin(),Incomplete.end(),class_name) == Incomplete.end())
   Incomplete.push_back(class_name);
  I=find(ClassesList.begin(),ClassesList.end(),class_name);
  if(I != ClassesList.end())
   ClassesList.erase(I);
  I=find(Complete.begin(),Complete.end(),class_name);
  if(I != Complete.end())
   Complete.erase(I);

  delete factory;
  return false;
 }

 if(find(ClassesList.begin(),ClassesList.end(),class_name) == ClassesList.end())
  ClassesList.push_back(class_name);
 if(find(Complete.begin(),Complete.end(),class_name) == Complete.end())
  Complete.push_back(class_name);
 I=find(Incomplete.begin(),Incomplete.end(),class_name);
 if(I != Incomplete.end())
  Incomplete.erase(I);

 return true;
}

/// Удаление заданного класса из списка успешно загруженных
/// Класс переносится в незагруженные (Incomplete)
void ULibrary::RemoveClassFromCompletedList(const string &name)
{
 std::vector<std::string>::iterator I;

 I=find(ClassesList.begin(),ClassesList.end(),name);
 if(I != ClassesList.end())
  ClassesList.erase(I);

 I=find(Complete.begin(),Complete.end(),name);
 if(I != Complete.end())
  Complete.erase(I);

 I=find(Incomplete.begin(),Incomplete.end(),name);
 if(I == Incomplete.end())
  Incomplete.push_back(name);
}

/// Заполняет библиотеку-заглушку всеми XML описаниями собственных компонентов
bool ULibrary::FillMockLibrary(UMockLibrary* lib)
{
    USerStorageXML ComponentStruct;
    ComponentStruct.Create("stub");

    // Проход по всем успешно созданным классам
    for(vector<string>::iterator it = Complete.begin(); it !=  Complete.end(); ++it)
    {
        UEPtr<UNet> cont=dynamic_pointer_cast<UNet>(Storage->TakeObject(*it));

        if(!cont)
            return false;

        ComponentStruct.Destroy();

        // Сохранение XML всего описания компонента
        if(!cont->SaveComponent(&ComponentStruct, true, ptAny|pgPublic))
            return false;

        lib->AddNewCompDescription(ComponentStruct);

        Storage->ReturnObject(cont);
    }

    Storage->FreeObjectsStorage();

    return true;
}
// --------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URuntimeLibrary::URuntimeLibrary(const string &name, const string &version, const string& path)
 : ULibrary(name,version,2), LibPath(path)
{

}

URuntimeLibrary::~URuntimeLibrary(void)
{

}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------

/// Возращает путь библиотеки
const std::string& URuntimeLibrary::GetLibPath() const
{
    return LibPath;
}

/// Загружает описание компонент из файлов в массив строк
bool URuntimeLibrary::LoadCompDescriptions()
{
    // Проход по всем существующим xml файлам в папке
    // с записью их данных в строки ClassesStructures
    std::vector<std::string> comp_descriptions;

    if(RDK::FindFilesList(LibPath,"*.xml",true,comp_descriptions))
        return false;

    ClassesStructures.resize(comp_descriptions.size());
    for(int i = 0 ; i < comp_descriptions.size(); i++)
    {
        // Парсинг текущего файла
        CurrentComponentStruct.LoadFromFile(LibPath+"/"+comp_descriptions[i],"");
        // Запись описания в строку
        CurrentComponentStruct.Save(ClassesStructures[i]);
    }

    return true;
}

/// Добавляет новый компонент (сохранение в файл)
bool URuntimeLibrary::AddNewClass(const std::string &new_class_name, const std::string &new_comp_name, UContainer *newclass)
{
    UEPtr<UContainer> p = newclass;
    UEPtr<UNet> cont = dynamic_pointer_cast<UNet>(p);

    if(!cont)
        return false;

    std::string buff;

    // XML парсер ведет себя проблемно.
    // И если делать Destroy() не инициализоварованого, то ломается. Поэтому сначала Create заглушки
    CurrentComponentStruct.Create("stub");

    CurrentComponentStruct.Destroy();

    // Сохранение XML и добавление нового поля RTname с именем
    if(!cont->SaveComponent(&CurrentComponentStruct, true, ptAny|pgPublic))
        return false;

    CurrentComponentStruct.SelectNode(cont->GetName());
    CurrentComponentStruct.RenameNode(new_comp_name);
    CurrentComponentStruct.SetNodeAttribute("RTname", new_class_name);
    CurrentComponentStruct.Save(buff);

    CurrentComponentStruct.SaveToFile(LibPath+"/"+new_class_name+".xml");

    UEPtr<UContainer> cont_1 = CreateClassSample(Storage, CurrentComponentStruct);

    if(!cont_1)
        return false;

    if(!UploadClass(new_class_name, cont_1))
    {
        if(Storage->GetLogger())
            Storage->GetLogger()->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "UploadClass failed while uploading \""+new_class_name + "\" class");
        return false;
    }

    ClassesStructures.push_back(buff);

    return true;
}

/// Удаляет класс из коллекции и Storage
bool URuntimeLibrary::DelClass(const std::string &class_name)
{
    if(class_name.empty())
        return false;
    // Удаление сущ. класса
    if(Storage->CheckClass(class_name))
    {
        // Если класс из другой библиотеки
        if(Storage->FindCollection(class_name).Get()!= static_cast<ULibrary*>(this))
        {
            if(Storage->GetLogger())
                Storage->GetLogger()->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "\""+class_name + "\" class exists in another library. Not in \"" + Name +"\"");
            return false;
        }

        // Может выбросить исключение
        Storage->DelClass(Storage->FindClassId(class_name));
    }

    // Поиск в массиве компонент
    for(vector<string>::iterator it = ClassesStructures.begin(); it != ClassesStructures.end(); ++it)
    {
        CurrentComponentStruct.Load(*it,"");
        std::string search_name = CurrentComponentStruct.GetNodeAttribute("RTname");
        if(class_name == search_name)
        {
            ClassesStructures.erase(it);
            std::string xml_path = LibPath +"/" + search_name + ".xml";
            RDK::RemoveFile(xml_path.c_str());
            break;
        }
    }
    return true;
}

/// Удаляет директорию библиотеки вместе со всеми файлами
bool URuntimeLibrary::DeleteOwnDirectory(void)
{
    return (!RDK::DeleteDirectory(LibPath.c_str()));
}
// --------------------------

// --------------------------
/// Создает компонент из описания xml
UEPtr<UContainer> URuntimeLibrary::CreateClassSample(UStorage *storage, USerStorageXML &xml)
{
 UEPtr<UNet> cont;

 if(!storage)
    return 0;

 std::string class_name=xml.GetNodeAttribute("Class");

 cont=dynamic_pointer_cast<UNet>(storage->TakeObject(class_name));

 if(!cont)
    return 0;

 if(!cont->LoadComponent(&xml,true))
 {
    if(Storage->GetLogger())
        Storage->GetLogger()->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Error while LoadComponent() from XML file for class \"" +class_name +"\"");

    storage->ReturnObject(cont);
    return 0;
 }

 return cont;
}

// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
void URuntimeLibrary::CreateClassSamples(UStorage *storage)
{
    int num_classes = ClassesStructures.size();

    for(int i=0;i<num_classes;i++)
    {
        try
        {
            CurrentComponentStruct.Load(ClassesStructures[i],"");

            UEPtr<UContainer> cont=CreateClassSample(storage, CurrentComponentStruct);

            if(!cont)
                return;

            std::string class_name=CurrentComponentStruct.GetNodeAttribute("RTname");

            UploadClass(class_name,cont);
        }
        catch(UException &ex)
        {
            if(Storage->GetLogger())
                Storage->GetLogger()->LogMessage(RDK_EX_DEBUG, __FUNCTION__, ex.what());
        }
    }
}
// --------------------------



// Библиотеки-заглушки

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UMockLibrary::UMockLibrary(const string &name, const string &version, const string& path)
 : ULibrary(name,version,3), LibPath(path)
{

}

UMockLibrary::~UMockLibrary(void)
{

}

// Добавляет описание компонента в ClassesStructures
bool UMockLibrary::AddNewCompDescription(USerStorageXML& descript)
{
    std::string added;
    descript.Save(added);
    ClassesStructures.push_back(added);
    return true;
}

bool UMockLibrary::SaveLibraryToFile()
{
    //Создание папки, если требуется
    if(RDK::CreateNewDirectory("../../../MockLibs/"))
        return false;

    std::string all_comps;

    // Сложение всех строчек
    for(std::vector<std::string>::iterator it = ClassesStructures.begin(); it != ClassesStructures.end(); ++it)
    {
        all_comps += (*it)+"\n";
    }

    CurrentComponentStruct.Create("a");
    CurrentComponentStruct.Destroy();

    CurrentComponentStruct.Create("MockLib");
    CurrentComponentStruct.SelectRoot();

    CurrentComponentStruct.SetNodeAttribute("Name", Name);
    CurrentComponentStruct.SetNodeAttribute("Version", Version);
    CurrentComponentStruct.SetNodeAttribute("Revision", std::to_string(Revision));
    CurrentComponentStruct.SetNodeAttribute("CoreVersion",GetGlobalVersion().ToStringFull());

    // XML со всеми компонентами
    USerStorageXML CompStruct;
    CompStruct.Load(all_comps,"");

    // Добавление компонентов в XML библиотеки
    CurrentComponentStruct.LoadToNode(CompStruct,true);

    std::string file_name = LibPath + "/" + Name + ".xml";
    CurrentComponentStruct.SaveToFile(file_name);

    return true;
}

bool UMockLibrary::LoadFromXML(USerStorageXML& xml)
{
    xml.SelectRoot();

    // Создание описаний компонентов поочередно
    for(int i = 0, size = xml.GetNumNodes() ; i < size; i++)
    {
        xml.SelectNode(i);
        std::string add;
        xml.SaveFromNode(add);
        ClassesStructures.push_back(add);

        xml.SelectUp();
    }

}


/// Создает компонент из описания xml
UEPtr<UContainer> UMockLibrary::CreateClassSample(USerStorageXML &xml, UStorage *storage)
{
    std::string class_name=xml.GetNodeAttribute("Class");

    UEPtr<UMockUNet> mock = new UMockUNet(&xml,storage);

    UEPtr<UNet> cont =dynamic_pointer_cast<UNet>(mock);

    if(!cont)
    {
        delete mock;
        return 0;
    }

    /*
    if(!cont->LoadComponent(&xml,true))
    {
       if(Storage->GetLogger())
           Storage->GetLogger()->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Error while LoadComponent() from XML file for class \"" +class_name +"\"");

       delete mock;

       return 0;
    }
    */
    return cont;
}

// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
void UMockLibrary::CreateClassSamples(UStorage *storage)
{
    int num_classes = ClassesStructures.size();

    for(int i=0;i<num_classes;i++)
    {
        try
        {
            CurrentComponentStruct.Load(ClassesStructures[i],"");

            UEPtr<UContainer> cont=CreateClassSample(CurrentComponentStruct,Storage);

            if(!cont)
                return;
            CurrentComponentStruct.Load(ClassesStructures[i],"");
            std::string class_name=CurrentComponentStruct.GetNodeAttribute("Class");
            if(class_name == "UOpenCvDetector")
            {
                int g= 9;
                g++;
            }
            UploadClass(class_name,cont);
        }
        catch(UException &ex)
        {
            if(Storage->GetLogger())
                Storage->GetLogger()->LogMessage(RDK_EX_DEBUG, __FUNCTION__, ex.what());
        }
    }
}

}

#endif


