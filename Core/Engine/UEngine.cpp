/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UEngine_CPP
#define UEngine_CPP

#include "UEngine.h"
//#include "UEnvException.h"
#include "UXMLEnvSerialize.h"
//#include "Libraries/IO/UFileIO.h"
#include "../Application/UIVisualController.h"
#include "../../Deploy/Include/rdk_exceptions.h"
#include "UEnvException.h"
#include "../../Deploy/Include/rdk_error_codes.h"

// --------------------------------------
// Объявления дополнительных функций
// --------------------------------------
// Возвращает хранилище по индексу
extern RDK::UStorage* GetStorage(size_t i);

// Возвращает среду по индексу
extern RDK::UEnvironment*  GetEnvironment(size_t i);

// Возвращает движок по индексу
extern RDK::UEngine*  GetEngine(size_t i);

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
extern RDK::UStorage*  AddNewStorage(void);

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
extern RDK::UEnvironment*  AddNewEnvironment(void);

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
extern RDK::UEngine*  AddNewEngine(void);
// --------------------------------------

namespace RDK{

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngine::UEngine(void)
 : Storage(0), Environment(0)
{
// Runned=-1;
 ChannelIndex=0;
 BufObjectsMode=1;
// CurrentExceptionsLogSize=0;
// ExceptionHandler=0;

// LastReadExceptionLogIndex=-1;
// MaxExceptionsLogSize=1000;
}

UEngine::~UEngine(void)
{
 ClearAllTempStrings();
// Stop();
}
// --------------------------

// --------------------------
// Методы управления параметрами инициализации
// --------------------------
/// Режим создания внутренних временных переменных для
/// возвращаемых значений
/// 0 - одна переменная для всех методов, возвращающих такой тип
/// 1 - уникальные переменные с необходимостью вызвова функции очистки
int UEngine::GetBufObjectsMode(void) const
{
 return BufObjectsMode;
}

bool UEngine::SetBufObjectsMode(int mode)
{
 if(BufObjectsMode == mode)
  return true;
 BufObjectsMode=mode;
 TempStrings.clear();
 return true;
}

// Имя файла инициализации
const string& UEngine::GetOptionsFileName(void) const
{
 return OptionsFileName;
}

bool UEngine::SetOptionsFileName(const string& value)
{
 if(OptionsFileName == value)
  return true;

 OptionsFileName=value;
 return true;
}
// --------------------------

// --------------------------
// Методы управления временными переменными
// --------------------------
/// Создает в списке временных строку новую строку
/// и возвращает ссылку на нее
std::string& UEngine::CreateTempString(void) const
{
 switch (BufObjectsMode)
 {
 case 0:
 {
  if(TempStrings.empty())
  {
   UEPtr<string> pstr=new std::string;
   TempStrings.insert(std::pair<UEPtr<string>,bool>(pstr,true));
  }
  return *TempStrings.begin()->first;
 }
 break;

 case 1:
 {
  std::map<UEPtr<std::string>,bool>::iterator I,J;
  I=TempStrings.begin();
  J=TempStrings.end();
  for(;I!=J;++I)
  {
   if(!I->second)
   {
    I->first->clear();
	return *I->first;
   }
  }

  UEPtr<string> pstr=new std::string;
  TempStrings.insert(std::pair<UEPtr<string>,bool>(pstr,true));
  return *pstr;
 }
 break;
 }
 return DummyTempString;
}

/// Возвращает временную строку
/// по указателю на ее данные
/// не находит свободные строки!
std::string& UEngine::FindTempString(const char *str_data) const
{
 std::map<UEPtr<std::string>,bool>::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  if(I->first->c_str() == str_data && I->second)
   return *I->first;

 }
 return DummyTempString;
}

/// Удаляет временную строку
/// по указателю на ее данные
void UEngine::DestroyTempString(const char *str_data) const
{
 if(!str_data)
  return;

 if(BufObjectsMode == 0)
  return;

 std::map<UEPtr<std::string>,bool>::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  if(I->first && I->first->c_str() == str_data)
  {
   I->second=false;
   break;
  }
 }
}

/// Удаляет временную строку
/// по ссылке на нее
void UEngine::DestroyTempString(const std::string &ref) const
{
 if(BufObjectsMode == 0)
  return;

 std::map<UEPtr<std::string>,bool>::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  if(*I->first == ref)
  {
   I->second=false;
   break;
  }
 }
}

/// Удаляет все временные строк
void UEngine::ClearAllTempStrings(void) const
{
 std::map<UEPtr<std::string>,bool>::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  delete I->first;
 }
 TempStrings.clear();
}

/// Возвращает число временных строк
int UEngine::GetNumTempStrings(void) const
{
 if(BufObjectsMode == 0)
  return 0;

 return int(TempStrings.size());
}
// --------------------------

// --------------------------
// Методы доступа к переменным состояния
// --------------------------
/*// Флаг работы системы
int UEngine::IsRunned(void)
{
 return Runned;
} */

/// Индекс текущего канала в многоканальной библиотеке
int UEngine::GetChannelIndex(void) const
{
 return ChannelIndex;
}

bool UEngine::SetChannelIndex(int value)
{
 if(ChannelIndex == value)
  return true;

 ChannelIndex=value;
 if(Environment)
  Environment->SetChannelIndex(ChannelIndex);

 if(Logger)
  Logger->SetChannelIndex(ChannelIndex);
 return true;
}
// --------------------------


// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает указатель на среду
UEnvironment* UEngine::GetEnvironment(void)
{
 return Environment;
}

// Возвращает указатель на модель
UContainer* UEngine::GetModel(void)
{
 return Environment->GetModel();
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Указатель на логгер
UEPtr<ULoggerEnv> const UEngine::GetLogger(void) const
{
 return Logger;
}

bool UEngine::SetLogger(UEPtr<ULoggerEnv> logger)
{
 if(Logger == logger)
  return true;

 if(Logger)
 {
  Logger->UnRegisterEnvironment();
 }

 Logger=logger;
 if(!Logger)
  return true;

 Logger->SetChannelIndex(ChannelIndex);
 if(Environment)
 {
  Logger->ClearLog();
  Logger->RegisterEnvironment(Environment);
 }
 return true;
}

// Инициализирует данные движка
void UEngine::Init(void)
{
 // Заглушка
}

bool UEngine::Init(UEPtr<UStorage> storage, UEPtr<UEnvironment> env)
{
 if(!Default())
  return false;

 AccessCache.clear();

 Storage=storage;
 Environment=env;

 if(Logger)
 {
  Logger->ClearLog();
  Logger->RegisterEnvironment(Environment);
  Logger->SetChannelIndex(ChannelIndex);
 }
 Environment->SetChannelIndex(ChannelIndex);
 Environment->SetLogger(Logger);

 if(!Storage)
  return false;
 Storage->SetLogger(Logger);


 RDK_SYS_TRY
 {
  try
  {
   LibrariesList.clear();
   ClassesList.clear();
   if(LoadPredefinedLibraries())
	return false;
   if(LoadClasses())
	return false;
   if(LoadLibraries())
	return false;
  }
  catch (RDK::UException &exception)
  {
   ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 CreateEnvironment(true,&ClassesList, &LibrariesList);

 if(!Storage || !Environment || Environment->GetStorage() != Storage)
 {
  return false;
 }

 return true;
}

// Деинициализирует данные движка
// и сохраняет текущие настройки
void UEngine::UnInit(void)
{
 AccessCache.clear();
}
// --------------------------

// --------------------------
// Методы управления средой
// --------------------------
// Методы управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
int UEngine::Storage_GetNumClasses(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->GetNumClasses();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает id классов в хранилище. Память должна быть выделена
int UEngine::Storage_GetClassesList(int *buffer) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::vector<UId> temp;
   Storage->GetClassIdList(temp);
   if(temp.size())
	memcpy(buffer,&temp[0],temp.size()*sizeof(UId));
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}


// Возвращает имена классов в хранилище в виде строки, разделенной запятыми
const char* UEngine::Storage_GetClassesNameList(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 std::string& TempString=CreateTempString();
 RDK_SYS_TRY
 {
  try
  {
   std::vector<std::string> temp;
   Storage->GetClassNameList(temp);
   for(size_t i=0;i<temp.size();i++)
   {
	TempString+=temp[i];
	if(i<temp.size()-1)
	 TempString+=",";
   }

   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
   DestroyTempString(TempString);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
   DestroyTempString(TempString);
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  DestroyTempString(TempString);
 }
 return 0;
}

 // Возвращает имя класса по его id.
const char * UEngine::Storage_GetClassName(int id) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 std::string& TempString=CreateTempString();
 RDK_SYS_TRY
 {
  try
  {
   TempString=Storage->FindClassName(id);
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
   DestroyTempString(TempString);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
   DestroyTempString(TempString);
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  DestroyTempString(TempString);
 }
 return 0;
}

// Возвращает Id класса по его имени
int UEngine::Storage_GetClassId(const char *name) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->FindClassId(name);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return ForbiddenId;
}

// Удаляет образец класса объекта из хранилища
// Если 'force' == true то принудительно удаляет из хранилища
// все объекты этого класса
int UEngine::Storage_DelClass(int classid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   Storage->DelClass(classid);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Удалаяет все свободные объекты из хранилища
int UEngine::Storage_FreeObjectsStorage(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   Storage->FreeObjectsStorage();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Удаляет все объекты из хранилища
int UEngine::Storage_ClearObjectsStorage(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   Storage->ClearObjectsStorage();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Вычисляет суммарное число объектов в хранилище
int UEngine::Storage_CalcNumObjects(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->CalcNumObjects();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

int UEngine::Storage_CalcNumObjectsById(int classid) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->CalcNumObjects(classid);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

int UEngine::Storage_CalcNumObjectsByName(const char* classname) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->CalcNumObjects(classname);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает описание класса по его id в формате xml
const char* UEngine::Storage_GetClassDescription(const char* classname)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   USerStorageXML xml;
   xml.Create(classname);
   Storage->SaveClassDescription(classname,xml);
   xml.SelectUp();
   xml.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Устанавливает описание класса по его id, считывая его из формата xml
int UEngine::Storage_SetClassDescription(const char* classname, const char* description)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   USerStorageXML xml;
   xml.Load(description, classname);
   Storage->LoadClassDescription(classname,xml); // TODO: Нет проверки на ошибочное чтение
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Сохраняет описание всех классов в xml
const char* UEngine::Storage_SaveClassesDescription(void)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   USerStorageXML xml;
   xml.Create("Root");
   xml.AddNode("ClassesDescription");
   Storage->SaveClassesDescription(xml);
   xml.SelectRoot();
   xml.Save(TempString);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
;
}

// Загружает описание всех классов из xml
int UEngine::Storage_LoadClassesDescription(const char* xmltext)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   USerStorageXML xml;
   xml.Load(xmltext, "Root");
   if(!xml.SelectNode("ClassesDescription"))
   {
	res=RDK_E_XML_TARGET_NODE_NOT_FOUND;
    return res;
   }
   Storage->LoadClassesDescription(xml);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Сохраняет общее описание всех классов в xml
const char* UEngine::Storage_SaveCommonClassesDescription(void)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   USerStorageXML xml;
   xml.Create("Root");
   xml.AddNode("CommonClassesDescription");
   Storage->SaveCommonClassesDescription(xml);
   xml.SelectRoot();
   xml.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Загружает общее описание всех классов из xml
int UEngine::Storage_LoadCommonClassesDescription(const char* xmltext)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   USerStorageXML xml;
   xml.Load(xmltext, "Root");
   if(!xml.SelectNode("CommonClassesDescription"))
   {
	res=RDK_E_XML_TARGET_NODE_NOT_FOUND;
    return res;
   }
   if(!Storage->LoadCommonClassesDescription(xml))
	res=RDK_E_STORAGE_LOAD_COMMON_CLASSES_DESCR_FAIL;
   else
    res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Сохраняет описание всех классов в xml включая общее описание
const char* UEngine::Storage_SaveAllClassesDescription(void)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   USerStorageXML xml;
   xml.Create("Root");
   xml.AddNode("CommonClassesDescription");
   Storage->SaveCommonClassesDescription(xml);
   xml.SelectUp();
   xml.AddNode("ClassesDescription");
   Storage->SaveClassesDescription(xml);
   xml.SelectRoot();
   xml.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Загружает описание всех классов из xml включая общее описание
int UEngine::Storage_LoadAllClassesDescription(const char* xmltext)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   USerStorageXML xml;
   xml.Load(xmltext, "Root");
   if(!xml.SelectNode("CommonClassesDescription"))
   {
	res=RDK_E_XML_TARGET_NODE_NOT_FOUND;
    return res;
   }
   if(!Storage->LoadCommonClassesDescription(xml))
   {
	res=RDK_E_STORAGE_LOAD_COMMON_CLASSES_DESCR_FAIL;
	return res;
   }
   xml.SelectUp();
   if(!xml.SelectNode("ClassesDescription"))
   {
	res=RDK_E_XML_TARGET_NODE_NOT_FOUND;
    return res;
   }
   Storage->LoadClassesDescription(xml);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
// ----------------------------

// ----------------------------
// Методы управления коллекциями компонент
// ----------------------------
// Возвращает число библиотек
int UEngine::Storage_GetNumClassLibraries(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->GetNumCollections();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}


// Возвращает список библиотек в виде строки, разделенной запятыми
const char * UEngine::Storage_GetClassLibrariesList(void) const
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString.clear();
   for(int i=0;i<Storage->GetNumCollections();i++)
   {
	TempString+=Storage->GetCollectionName(i);
	if(i<Storage->GetNumCollections()-1)
	 TempString+=",";
   }
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// library_name - имя библиотеки
const char * UEngine::Storage_GetLibraryClassNames(const char *library_name) const
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString.clear();
   UEPtr<ULibrary> lib=Storage->GetCollection(library_name);
   if(lib)
   {
	const vector<string> &classes=lib->GetComplete();
	for(int i=0;i<int(classes.size());i++)
	{
	 TempString+=classes[i];
	 if(i<int(classes.size())-1)
	  TempString+=",";
	}
   }
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// index - индекс библиотеки
const char * UEngine::Storage_GetLibraryClassNamesByIndex(int index) const
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString.clear();
   UEPtr<ULibrary> lib=Storage->GetCollection(index);
   if(lib)
   {
	const vector<string> &classes=lib->GetComplete();
	for(int i=0;i<int(classes.size());i++)
	{
	 TempString+=classes[i];
	 if(i<int(classes.size())-1)
	  TempString+=",";
	}
   }
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}


// Возвращает имя библиотеки по индексу
const char * UEngine::Storage_GetClassLibraryNameByIndex(int index)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Storage->GetCollectionName(index);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возвращает версию библиотеки по индексу
const char * UEngine::Storage_GetClassLibraryVersionByIndex(int index)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Storage->GetCollectionVersion(index);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
int UEngine::Storage_DelClassLibraryByIndex(int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(Storage->DelCollection(index))
   {
	res=RDK_E_STORAGE_DEL_COLLECTION_FAIL;
	return res;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
int UEngine::Storage_DelClassLibraryByName(const char *name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(!Storage->DelCollection(name))
   {
	res=RDK_E_STORAGE_DEL_COLLECTION_FAIL;
	return res;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
int UEngine::Storage_DelAllClassLibraries(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(Storage->DelAllCollections())
   {
	res=RDK_E_STORAGE_DEL_COLLECTION_FAIL;
	return res;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Загружает коллекцию по имени dll-файла
int UEngine::Storage_LoadBinaryCollectionFromFile(const char *filename)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
    res=RDK_NOT_IMPLEMENTED;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Загружает runtime-коллекцию
int UEngine::Storage_LoadRuntimeCollectionFromFile(const char *filename)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
    res=RDK_NOT_IMPLEMENTED;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int UEngine::Storage_LoadRuntimeCollectionFromString(const char *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   res=RDK_NOT_IMPLEMENTED;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Сохраняет runtime-коллекцию
int UEngine::Storage_SaveRuntimeCollectionToFile(const char *filename)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   res=RDK_NOT_IMPLEMENTED;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int UEngine::Storage_SaveRuntimeCollectionToString(const char *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   res=RDK_NOT_IMPLEMENTED;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

/// Создает новую runtime-библиотеку
int UEngine::Storage_CreateRuntimeCollection(const char *collection_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(Storage->CreateRuntimeCollection(collection_name))
	res=RDK_SUCCESS;
   else
    res=RDK_E_STORAGE_RT_COLLECTION_CREATION_FAIL;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
int UEngine::Storage_CreateClass(const char* stringid, const char *class_name, const char *collection_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<UNet> sample=FindComponent(stringid);
   UEPtr<URuntimeLibrary> library=dynamic_pointer_cast<URuntimeLibrary>(Storage->GetCollection(collection_name));

   if(!sample)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(!library)
	return RDK_E_STORAGE_COLLECTION_NOT_FOUND;

   if(!Storage->AddClassToCollection(class_name, sample,library))
	return RDK_E_STORAGE_ADD_COLLECTION_FAIL;

   AccessCache.clear();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
int UEngine::Storage_BuildStorage(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(!Storage->BuildStorage())
   {
	res=RDK_E_STORAGE_BUILD_FAIL;
    return res;
   }

   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
// ----------------------------


// ----------------------------
// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
int UEngine::Env_GetPredefinedStructure(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetPredefinedStructure();
  }
  catch (RDK::UException &exception)
  {
   ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

int UEngine::Env_SetPredefinedStructure(int value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(!Environment->SetPredefinedStructure(value))
   {
	res=RDK_E_ENGINE_SET_PREDEFINED_STRUCTURE_FAIL;
    return res;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool UEngine::Env_IsStoragePresent(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->IsStoragePresent();
  }
  catch (RDK::UException &exception)
  {
   ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

// Возвращает состояние инициализации
bool UEngine::Env_IsInit(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->IsInit();
  }
  catch (RDK::UException &exception)
  {
   ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

// Признак наличия сформированной структуры
bool UEngine::Env_IsStructured(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->IsStructured();
  }
  catch (RDK::UException &exception)
  {
   ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

// Возвращает состояние внутренего логгирования
bool UEngine::Env_GetEventsLogMode(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Logger->GetEventsLogMode();
  }
  catch (RDK::UException &exception)
  {
   ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

// Включает/выключает внутренне логгирование
int UEngine::Env_SetEventsLogMode(bool value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Logger->SetEventsLogMode(value))
	res=RDK_E_LOGGER_SET_EVENTS_LOG_MODE_FAIL;
   else
    res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Инициализация среды
int UEngine::Env_Init(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   Environment->Init();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Деинициализация среды
int UEngine::Env_UnInit(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   Environment->UnInit();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Формирует предварительно заданную модель обработки
int UEngine::Env_CreateStructure(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(!Environment->CreateStructure())
   {
	res=RDK_E_ENGINE_CREATE_STRUCTURE_FAIL;
    return res;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Уничтожает текущую модель обработки
int UEngine::Env_DestroyStructure(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(Environment->DestroyStructure())
   {
	res=RDK_E_ENGINE_DESTROY_STRUCTURE_FAIL;
    return res;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
int UEngine::Env_Destroy(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   Environment->DestroyModel();
   Storage->ClearObjectsStorage();
   Storage->ClearClassesStorage();
   Storage->DelAllCollections();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
int UEngine::Env_Calculate(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!stringid)
   {
	Environment->SetModelCalculationComponent("");
   }
   else
   {
	RDK::UNet* destcont=FindComponent(stringid);
	std::string name;
	if(destcont)
	 destcont->GetLongName(Environment->GetModel(),name);
	else
	 return RDK_E_MODEL_COMPONENT_NOT_FOUND;
	Environment->SetModelCalculationComponent(name);
   }

   if(!Environment->Calculate())
	throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Расчет всей модели в реальном времени
int UEngine::Env_RTCalculate(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->RTCalculate();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}


/// Расчет модели порциями длительностью calc_intervsal секунд с максимально возможной скоростью
int UEngine::Env_FastCalculate(double calc_interval)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->FastCalculate(calc_interval);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Метод сброса счета
// Если stringid == 0 то сбрасывает всю модель целиком,
// иначе - только указанный компонент модели
int UEngine::Env_Reset(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::string name;
   if(!stringid)
   {
	Environment->SetModelCalculationComponent("");
	if(!Environment->Reset())
	 throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
   }
   else
   {
	RDK::UContainer* destcont=FindComponent(stringid);
	if(destcont)
	{
	 if(!destcont->Reset())
	  throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
	}
	else
	 return RDK_E_MODEL_COMPONENT_NOT_FOUND;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

/// Метод сброса параметров на значения по умолчанию
/// Если stringid == 0 то сбрасывает всю модель целиком,
/// иначе - только указанный компонент модели
/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
int UEngine::Env_Default(const char* stringid, bool subcomps)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RDK::UContainer* destcont;
   if(!stringid)
   {
	destcont=GetModel();
   }
   else
   {
	destcont=FindComponent(stringid);
   }

   if(!destcont->DefaultAll(destcont,subcomps))
	throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}



// Производит увеличение времени модели на требуемую величину
int UEngine::Env_IncreaseModelTimeByStep(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->IncreaseModelTimeByStep();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

/// Устанавливает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
int UEngine::Env_SetMinInterstepsInterval(unsigned long long value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Environment->SetMinInterstepsInterval(value))
    throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

/// Возвращает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
unsigned long long UEngine::Env_GetMinInterstepsInterval(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetMinInterstepsInterval();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Время, потраченное на последний RT-расчет
double UEngine::Env_GetRTLastDuration(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetRTLastDuration();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0.0;
}

/// Время, расчитанное в модели за один вызов RTCalculate;
double UEngine::Env_GetRTModelCalcTime(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetRTModelCalcTime();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0.0;
}

/// Производительность RT расчета (отношение RTModelCalcTime/RTLastDuration)
double UEngine::Env_CalcRTPerformance(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->CalcRTPerformance();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0.0;
}

// !!! Следующие методы управления текущим компонентом влияют на все
// методы, обращающиеся к компонентам по строковому id !!!
// Устанавливает текущий компонент (адресация относительно корня - модели)
int UEngine::Env_SelectCurrentComponent(const char *stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->ResetCurrentComponent();
   Environment->SelectCurrentComponent(stringid);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Сбрасывает текущий компонент в состояние по умолчению (модель)
int UEngine::Env_ResetCurrentComponent(const char *stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->ResetCurrentComponent();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Меняет текущий компонент на его родителя (подъем на уровень вверх)
// Если уже на верхнем уровне, то не делает ничего
int UEngine::Env_UpCurrentComponent(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->UpCurrentComponent();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Меняет текущий компонент на его дочерний на произвольном уровне вложенности
// (спуск на N уровней вниз относительно текущего компонента)
int UEngine::Env_DownCurrentComponent(const char *stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->DownCurrentComponent(stringid);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Возвращает длинное имя текущего компонента
const char* UEngine::Env_GetCurrentComponentName(void) const
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->GetCurrentComponent()->GetLongName(Environment->GetModel(),TempString);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}
	/*
// Возвращает длинный строковой id текущего компонента
const char* UEngine::Env_GetCurrentComponentId(void) const
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::string name;
   Environment->GetCurrentComponent()->GetLongName(Environment->GetModel(),name);
   longid.EncodeToString(TempString);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}
              */
// Возвращает имя текущего каталога для хранения данных
const char* UEngine::Env_GetCurrentDataDir(void) const
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Environment->GetCurrentDataDir();
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}

// Устанавливает имя текущего каталога для хранения данных
int UEngine::Env_SetCurrentDataDir(const char *dir)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->SetCurrentDataDir(dir);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Возвращает имя каталога бинарных файлов
const char* UEngine::Env_GetSystemDir(void) const
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Environment->GetSystemDir().c_str();
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}

// Устанавливает имя каталога бинарных файлов
int UEngine::Env_SetSystemDir(const char *dir)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->SetSystemDir(dir);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

/// Возвращает состояние флага отладочного режима среды
bool UEngine::Env_GetDebugMode(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Logger->GetDebugMode();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

/// Устанавливает состояние флага отладочного режима среды
int UEngine::Env_SetDebugMode(bool value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Logger->SetDebugMode(value))
   {
    return RDK_E_LOGGER_SET_FLAG_FAIL;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

/// Возвращает маску системных событий для логирования
unsigned int UEngine::Env_GetDebugSysEventsMask(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Logger->GetDebugSysEventsMask();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

/// Устанавливает маску системных событий для логирования
int UEngine::Env_SetDebugSysEventsMask(unsigned int value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Logger->SetDebugSysEventsMask(value))
   {
    return RDK_E_LOGGER_SET_FLAG_FAIL;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}


/// Возвращает флаг включения вывода лога в отладчик
bool UEngine::Env_GetDebuggerMessageFlag(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Logger->GetDebuggerMessageFlag();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

/// Устанавливает флаг включения вывода лога в отладчик
int UEngine::Env_SetDebuggerMessageFlag(bool value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Logger->SetDebuggerMessageFlag(value))
   {
    return RDK_E_LOGGER_SET_FLAG_FAIL;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Задает число входов среды
void UEngine::Env_SetNumInputImages(int number)
{
 //GetEnvironment()->SetNumInputImages(number);
}

// Задает число выходов среды
void UEngine::Env_SetNumOutputImages(int number)
{
 //GetEnvironment()->SetNumOutputImages(number);
}

// Возвращает число входов среды
int UEngine::Env_GetNumInputImages(void)
{
 return 0;//return GetEnvironment()->GetNumInputImages();
}

// Возвращает число выходов среды
int UEngine::Env_GetNumOutputImages(void)
{
 return 0;//return GetEnvironment()->GetNumOutputImages();
}

// Задает разрешение по умолчанию (рабочее разрешение)
void UEngine::Env_SetInputRes(int number, int width, int height)
{
 //GetEnvironment()->SetInputImageRes(number,width,height);
}

// Задает флаг отражения входного изображения вокруг горизонтальной оси
void UEngine::Env_SetReflectionXFlag(bool value)
{
 //GetEnvironment()->SetReflectionXFlag(value);
}

// Возвращает разрешение по умолчанию (рабочее разрешение)
int UEngine::Env_GetInputImageWidth(int number)
{
 return 0;//return GetEnvironment()->GetInputImageWidth(number);
}

int UEngine::Env_GetInputImageHeight(int number)
{
 return 0;//return GetEnvironment()->GetInputImageHeight(number);
}

int UEngine::Env_GetInputImageColorModel(int number)
{
 return 0;//return GetEnvironment()->GetInputImageColorModel(number);
}

// Возвращает текущее выходное разрешение
int UEngine::Env_GetOutputImageWidth(int number)
{
 return 0;// return GetEnvironment()->GetOutputImageWidth(number);
}

int UEngine::Env_GetOutputImageHeight(int number)
{
 return 0;// return GetEnvironment()->GetOutputImageHeight(number);
}

int UEngine::Env_GetOutputImageColorModel(int number)
{
 return 0;// return GetEnvironment()->GetOutputImageColorModel(number);
}

void UEngine::Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
/* TempBmp.AttachBuffer(width,height,image,RDK::UBMColorModel(cmodel));
 BResizeEdges.SetNewWidth(GetEnvironment()->GetInputImageWidth(number));
 BResizeEdges.SetNewHeight(GetEnvironment()->GetInputImageHeight(number));
 BResizeEdges(TempBmp,TempBmp2);
 GetEnvironment()->SetInputImage(number,TempBmp2);
 TempBmp.DetachBuffer();*/
}

unsigned char* UEngine::Env_GetInputImage(int index)
{
 return 0;// return GetEnvironment()->GetInputImage(index).GetData();
}

unsigned char* UEngine::Env_GetOutputImage(int index)
{
 return 0;// return GetEnvironment()->GetOutputImage(index).GetData();
}

unsigned char* UEngine::Env_GetOutputImageY8(int index)
{
 return 0;// return GetEnvironment()->GetOutputImage(index).GetData();
}


/// Инициирует извещение о сбое в работе источника данных
int UEngine::Env_CallSourceController(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Environment)
	return RDK_E_ENV_NOT_FOUND;
   if(!Environment->CallSourceController())
	return RDK_E_ENV_SOURCE_CONTROLLER_CALL_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}


// Методы управления моделью
// ----------------------------
// Удаляет модель
int UEngine::Model_Destroy(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(!Environment->DestroyModel())
    throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
int UEngine::Model_Create(const char *classname)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(!Environment->CreateModel(classname))
    throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Очищает модель
int UEngine::Model_Clear(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   UEPtr<RDK::UContainer> model=dynamic_pointer_cast<RDK::UContainer>(Environment->GetModel());

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   model->DelAllComponents();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Проверяет, существует ли модель
bool UEngine::Model_Check(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return (Environment->GetModel())?true:false;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

// Проверяет, существует ли в модели компонент с именем stringid)
bool UEngine::Model_CheckComponent(const char* stringid) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> destcont=FindComponent(stringid);
   if(destcont)
	return true;
  }
  catch (UContainer::EComponentNameNotExist &exception)
  {
   return false;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'classname'
// если stringid - пустая строка, то добавляет в саму модель
// Возвращает имя компонента в случае успеха
const char* UEngine::Model_AddComponent(const char* stringid, const char *classname)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> destcont=FindComponent(stringid);
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(Storage->TakeObject(classname));

   if(!cont)
   {
	DestroyTempString(TempString);
	return 0;
   }

   if(!destcont)
   {
	if(cont)
     Storage->ReturnObject(cont);
	DestroyTempString(TempString);
	return 0;
   }

   if(destcont->AddComponent(cont))
   {
	TempString=cont->GetName();
   }
   else
	TempString.clear();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'name'
// если stringid - пустая строка, то удаляет из самой модели
int UEngine::Model_DelComponent(const char* stringid, const char *name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RDK::UContainer* destcont=FindComponent(stringid);

   if(!destcont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   destcont->DelComponent(name);
   AccessCache.clear();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}


/// Перемещает компоненту в другой компонент
/// Если comp не принадлежит этому компоненту, или target имеет отличный от
/// этого компонента storage, или target не может принять в себя компонент
/// то возвращает false и не делает ничего
int UEngine::Model_MoveComponent(const char* component, const char* target)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
  RDK::UContainer* comp=FindComponent(component);
  RDK::UContainer* target_comp=FindComponent(target);

  if(!comp)
   return RDK_E_MODEL_COMPONENT_NOT_FOUND;

  if(!target_comp)
   return RDK_E_MODEL_TARGET_COMPONENT_NOT_FOUND;

  if(!comp->GetOwner())
   return RDK_E_MODEL_COMPONENT_OWNER_NOT_FOUND;

  if(comp == target_comp)
   return RDK_E_MODEL_COMPONENTS_DONT_HAVE_TO_MATCH;

  if(!comp->GetOwner()->MoveComponent(comp,target_comp))
   return RDK_E_MODEL_MOVE_COMPONENTS_FAIL;

   AccessCache.clear();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}


// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
int UEngine::Model_GetNumComponents(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RDK::UContainer* destcont=FindComponent(stringid);

   if(!destcont)
	return 0;

   return destcont->GetNumComponents();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
/*
int UEngine::Model_GetComponentsList(const char* stringid, int *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RDK::UContainer* destcont=FindComponent(stringid);

   if(!destcont)
	return -4;

   std::vector<int> tempbuffer;

   destcont->GetComponentsList(tempbuffer);
   if(tempbuffer.size())
	memcpy(buffer,&tempbuffer[0],tempbuffer.size()*sizeof(int));
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}      */

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ','
const char* UEngine::Model_GetComponentsNameList(const char* stringid)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString.clear();
   RDK::UContainer* destcont=FindComponent(stringid);

   if(!destcont)
	return TempString.c_str();

   std::vector<std::string> tempbuffer;

   destcont->GetComponentsList(tempbuffer);
   for(int i=0;i<int(tempbuffer.size());i++)
   {
	TempString+=tempbuffer[i];
	if(i<int(tempbuffer.size())-1)
	 TempString+=",";
   }
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 DestroyTempString(TempString);
 return 0;
}

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ',' и имеющих имя класса 'class_name'
// Если find_all == true то поиск ведется и во всех сабкомпонентах
const char* UEngine::Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString.clear();

   if(!class_name || !strlen(class_name))
	return TempString.c_str();

   RDK::UContainer* destcont=FindComponent(stringid);

   if(!destcont)
	return TempString.c_str();

   int class_id=Storage->FindClassId(class_name);

   if(class_id == ForbiddenId)
	return TempString.c_str();

   std::vector<std::string> tempbuffer;
   destcont->GetComponentsNameByClassName(class_name, tempbuffer, find_all);
   size_t numComp=tempbuffer.size();

   for(size_t i=0; i<numComp; i++)
   {
	TempString+=tempbuffer[i];
	if( int(i) < int(tempbuffer.size())-1 )
	 TempString+=",";
   }

   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}

// Перемещает компонент с текущим индексом index или именем 'name' вверх или
// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
// на эту границу
int UEngine::Model_ChangeComponentPosition(const char* stringid, int step)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RDK::UContainer* destcont=FindComponent(stringid);

   if(destcont == Environment->GetModel())
	return RDK_SUCCESS;

   if(!destcont)
    return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(!destcont->GetOwner())
    return RDK_E_MODEL_COMPONENT_OWNER_NOT_FOUND;

   if(destcont->GetOwner()->ChangeComponentPosition(destcont->GetName(),step))
	res=RDK_SUCCESS;
   else
	res=RDK_E_MODEL_CHANGE_COMPONENT_POSITION_FAIL;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}


// Возвращает xml-список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
/*
const char* UEngine::Model_GetConnectorsList(const char* stringid,
                          int sublevel, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   UEPtr<RDK::UNet> owner_level=FindComponent(owner_level_stringid);

   TempString="";
   if(!cont)
	return TempString.c_str();


   ULongIdVector buffer;
   cont->GetConnectorsList(buffer,sublevel,owner_level);

   XmlStorage.Create("Connectors");
   XmlStorage<<buffer;
   XmlStorage.SelectRoot();

   XmlStorage.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возвращает xml-список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
const char* UEngine::Model_GetItemsList(const char* stringid,
                            int sublevel, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   UEPtr<RDK::UNet> owner_level=FindComponent(owner_level_stringid);

   TempString="";
   if(!cont)
	return TempString.c_str();


   ULongIdVector buffer;
   cont->GetItemsList(buffer,sublevel,owner_level);

   XmlStorage.Create("Items");
   XmlStorage<<buffer;

   XmlStorage.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}            */

// Возвращает xml-список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
const char* UEngine::Model_GetComponentsListEx(const char* stringid,
                            int sublevel, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   UEPtr<RDK::UNet> owner_level=FindComponent(owner_level_stringid);

   TempString="";
   if(!cont)
	return TempString.c_str();

   std::list<std::string> buffer;
   cont->GetComponentsListEx(buffer,sublevel,owner_level);

   XmlStorage.Create("Nets");
   XmlStorage<<buffer;

   XmlStorage.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}


// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
const char* UEngine::Model_GetComponentName(const char* stringid)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString="";
   UEPtr<RDK::UNet> destcont=FindComponent(stringid);

   if(!destcont)
	return TempString.c_str();

   TempString=destcont->GetName();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}


// Возвращает длинное имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
const char* UEngine::Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> destcont=FindComponent(stringid);
   UEPtr<RDK::UNet> owner_level=FindComponent(owner_level_stringid);

   if(!destcont)
	return TempString.c_str();

   return destcont->GetLongName(owner_level,TempString).c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возвращает длинный id компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
/*const char* UEngine::Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> destcont=FindComponent(stringid);
   UEPtr<RDK::UNet> owner_level=FindComponent(owner_level_stringid);

   if(!destcont)
	return TempString.c_str();

   ULongId id;

   destcont->GetLongId(owner_level,id);
   id.EncodeToString(TempString);
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}    */

// Возвращает имя класса компонента в хранилище по длинному 'stringid'
// если stringid - пустая строка, то возвращает имя класса модели
const char* UEngine::Model_GetComponentClassName(const char* stringid)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString="";
   UEPtr<RDK::UNet> destcont=FindComponent(stringid);

   if(!destcont)
	return TempString.c_str();

   TempString=Storage->FindClassName(destcont->GetClass());
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возвращает список свойств компонента разделенный запятыми
const char* UEngine::Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);

   if(!cont)
	return TempString.c_str();

   RDK::UContainer::VariableMapT props=cont->GetPropertiesList();

   RDK::UContainer::VariableMapCIteratorT I,J;

   I=props.begin();
   J=props.end();
   while(I != J)
   {
	if(I->second->CheckMask(type_mask))
	{
	 if(TempString.size()>0)
	  TempString+=",";
	 TempString+=I->first;
	}
	++I;
   }

   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}

const char* UEngine::Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);

   if(!cont)
	return TempString.c_str();

   RDK::UContainer::VariableMapT props=cont->GetPropertiesList();

   RDK::UContainer::VariableMapCIteratorT I,J;

   I=props.begin();
   J=props.end();
   while(I != J)
   {
	if(I->second->CheckMask(type_mask))
	{
	 if(TempString.size()>0)
	  TempString+=",";
	 TempString+=I->first;
	 TempString+=":";
	 TempString+=sntoa(0);
	}
	++I;
   }

   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}


// Возвращает свойства компонента по идентификатору
const char* UEngine::Model_GetComponentProperties(const char *stringid, unsigned int type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return TempString.c_str();

   XmlStorage.Create(cont->GetLongName(Environment->GetCurrentComponent(),CompName));
   XmlStorage.AddNode(UComponent::GetPropertyTypeNameByType(type_mask));

   if(!cont->GetComponentProperties(&XmlStorage, type_mask))
   {
	DestroyTempString(TempString);
	return 0;
   }

   XmlStorage.SelectUp();
   XmlStorage.Save(TempString);
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}

// Возвращает выборочные свойства компонента по идентификатору
const char* UEngine::Model_GetComponentSelectedProperties(const char *stringid)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   DestroyTempString(TempString);
   return 0;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}

// Возвращает свойства компонента по идентификатору с описаниями
const char* UEngine::Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   if(!cont)
	return TempString.c_str();
   std::string comp_name;
   USerStorageXML xml;
   xml.Create(cont->GetLongName(Environment->GetCurrentComponent(),comp_name));
   xml.AddNode(UComponent::GetPropertyTypeNameByType(type_mask));

   if(!cont->GetComponentPropertiesEx(&xml, type_mask))
   {
	DestroyTempString(TempString);
	return 0;
   }

   xml.SelectUp();
   xml.Save(TempString);
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}

// Возвращает значение свойства компонента по идентификатору компонента и имени свойства
const char * UEngine::Model_GetComponentPropertyValue(const char *stringid, const char *paramname)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString="";
   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   if(!cont)
	return TempString.c_str();

   cont->GetPropertyValue(paramname,TempString);
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}

// устанавливает свойства компонента по идентификатору
int UEngine::Model_SetComponentProperties(const char *stringid, const char* buffer)
{
 RDK::USerStorageXML XmlStorage;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   XmlStorage.Load(buffer, cont->GetLongName(Environment->GetCurrentComponent(),CompName));
   for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
   {
	if(XmlStorage.SelectNode(UComponent::GetPropertyTypeNameByType(mask)))
	{
	 int prop_res=cont->SetComponentProperties(&XmlStorage);
	 if(prop_res != RDK_SUCCESS)
	  res=RDK_E_MODEL_NOT_ALL_PROPERTIES_SET;

	 XmlStorage.SelectUp();
	}
   }
   if(res == RDK_UNHANDLED_EXCEPTION)
    res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Устанавливает значение свойства компонента по идентификатору компонента и имени свойства
int UEngine::Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   cont->SetPropertyValue(paramname,buffer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}


// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
int UEngine::Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));
   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   UId classid=Storage->FindClassId(class_stringid);
   if(classid == ForbiddenId)
	return RDK_E_STORAGE_CLASS_NOT_FOUND;

   cont->SetGlobalComponentPropertyValue(classid, paramname, buffer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
int UEngine::Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));
   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   UId classid=Storage->FindClassId(class_stringid);
   UId owner_classid=Storage->FindClassId(class_owner_stringid);
   if(classid == ForbiddenId || owner_classid == ForbiddenId)
	return RDK_E_STORAGE_CLASS_NOT_FOUND;

   cont->SetGlobalOwnerComponentPropertyValue(classid, owner_classid, paramname, buffer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Возвращает указатель void* на данные свойства компонента
const void* UEngine::Model_GetComponentPropertyData(const char *stringid, const char *property_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
   return iproperty->GetMemoryArea();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return 0;
}

// Копирует данные 'data' в заданное свойство компонента
int UEngine::Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
   if(!iproperty->ReadFromMemory(data))
    return RDK_E_MODEL_READ_PROPERTY_MEMORY_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Связывает выбранные контейнеры друг с другом
/*
int UEngine::Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!stringid1)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(!stringid2)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   RDK::ULongId longid1, longid2;
   if(stringid1[0]>=0x30 && stringid1[0]<=0x39)
	longid1.DecodeFromString(stringid1);
   else
   {
	UEPtr<UContainer> cont=FindComponent(stringid1);
	longid1=cont->GetLongId(model);
   }

   if(stringid2[0]>=0x30 && stringid2[0]<=0x39)
	longid2.DecodeFromString(stringid2);
   else
   {
	UEPtr<UContainer> cont=FindComponent(stringid2);
	longid2=cont->GetLongId(model);
   }
   bool temp_res=model->CreateLink(longid1,output_number,longid2,input_number);
   if(!temp_res)
	return RDK_E_MODEL_CREATE_LINK_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
  */
int UEngine::Model_CreateLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name, int connector_c_index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!stringid1)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(!stringid2)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   bool res=model->CreateLink(stringid1,item_property_name,stringid2,connector_property_name, connector_c_index);
   if(!res)
    return RDK_E_MODEL_CREATE_LINK_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
			   /*

// Связывает все компоненты выбранного компонента по возрастанию id в формате: 0 выход к 0 входу
int UEngine::Model_ChainLinking(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

   cont->BreakLinks(cont);

   if(cont->GetNumComponents() == 0)
	return RDK_SUCCESS;

   ULongId id1,id2;

   UEPtr<UNet> item=dynamic_pointer_cast<UNet>(cont->GetComponentByIndex(0));
   item->GetLongId(cont,id1);

   // Подключаем выходы модели
   int minsize=cont->GetNumOutputs();
   if(minsize>item->GetNumInputs())
	minsize=item->GetNumInputs();
   for(int j=0;j<minsize;j++)
	cont->CreateLink(UIdVector(ForbiddenId),j,id1,j);

   for(int i=1;i<cont->GetNumComponents();i++)
   {
	UEPtr<UNet> connector=dynamic_pointer_cast<UNet>(cont->GetComponentByIndex(i));
	connector->GetLongId(cont,id2);
	minsize=item->GetNumOutputs();
	if(minsize>connector->GetNumInputs())
	 minsize=connector->GetNumInputs();
	for(int j=0;j<minsize;j++)
	 cont->CreateLink(id1,j,id2,j);
	id1=id2;
	item=connector;
   }

   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Связывает все компоненты выбранного компонента параллельно, подключая их к необходимому числу выходов модели
// Используется для тестирования производительности
int UEngine::Model_ParallelLinking(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

   cont->BreakLinks(cont);

   if(cont->GetNumComponents() == 0)
	return RDK_SUCCESS;

   ULongId id1,id2;

   UEPtr<UNet> item=cont;

   for(int i=0;i<cont->GetNumComponents();i++)
   {
	UEPtr<UNet> connector=dynamic_pointer_cast<UNet>(cont->GetComponentByIndex(i));
	connector->GetLongId(cont,id2);
	int minsize=item->GetNumOutputs();
	if(minsize>connector->GetNumInputs())
	 minsize=connector->GetNumInputs();
	for(int j=0;j<minsize;j++)
	 cont->CreateLink(id1,j,id2,j);
   }

   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
              */
// Разрывает выбранную связь
/*
int UEngine::Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!stringid1)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(!stringid2)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   RDK::ULongId longid1, longid2;
   if(stringid1[0]>=0x30 && stringid1[0]<=0x39)
	longid1.DecodeFromString(stringid1);
   else
   {
	UEPtr<UContainer> cont=FindComponent(stringid1);
	longid1=cont->GetLongId(model);
   }

   if(stringid2[0]>=0x30 && stringid2[0]<=0x39)
	longid2.DecodeFromString(stringid2);
   else
   {
	UEPtr<UContainer> cont=FindComponent(stringid2);
	longid2=cont->GetLongId(model);
   }

   bool temp_res=model->BreakLink(longid1,output_number,longid2,input_number);
   if(!temp_res)
	return RDK_E_MODEL_BREAK_LINK_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
} */

int UEngine::Model_BreakLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!stringid1)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(!stringid2)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   bool temp_res=model->BreakLink(stringid1,item_property_name,stringid2,connector_property_name);
   if(!temp_res)
    return RDK_E_MODEL_BREAK_LINK_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Разрывает все связи
int UEngine::Model_BreakAllLinks(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   model->BreakLinks();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
	 /*
// Разрывает связь ко входу connector_index коннектора 'connectorid'
int UEngine::Model_BreakConnectorLink(const char* connectorname, int connector_index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   model->BreakConnectorLink(connectorname, connector_index);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}        */

// Разрывает все входные и выходные связи выбранного контейнера
int UEngine::Model_BreakAllComponentLinks(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   model->BreakLinks();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Разрывает все входные связи выбранного контейнера
int UEngine::Model_BreakAllComponentInputLinks(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   if(model->BreakInputLinks())
	res=RDK_SUCCESS;
   else
    res=RDK_E_MODEL_BREAK_LINK_FAIL;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Разрывает все выходные связи выбранного контейнера
int UEngine::Model_BreakAllComponentOutputLinks(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return RDK_NOT_IMPLEMENTED;
//   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Проверяет, существует ли заданна связь
/*
bool UEngine::Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UADItem> cont1;
   UEPtr<RDK::UConnector> cont2;
   try
   {
	cont1=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid1));
	cont2=dynamic_pointer_cast<RDK::UConnector>(FindComponent(stringid2));
   }
   catch (UException &exception)// Заглушка!! здесь другое исключение
   {
	return false;
   }
   if(!cont1 || !cont2)
	return false;

   return cont1->CheckLink(cont2,output_number,input_number);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}
		   */
bool UEngine::Model_CheckLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<UNet> model=FindComponent("");
   if(!model)
	return false;

   return model->IsLinkExists(stringid1,item_property_name,stringid2,connector_property_name);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char* UEngine::Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   UEPtr<RDK::UNet> owner;
   if(owner_level_stringid)
	dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

   TempString="";
   if(!cont)
	return TempString.c_str();

   XmlStorage.Create("Links");

   if(cont->GetComponentInternalLinks(&XmlStorage,owner))
	return TempString.c_str();

   XmlStorage.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются с уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то применяется уровень текущего компонента
int UEngine::Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   UEPtr<RDK::UNet> owner;
   if(owner_level_stringid)
	dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   XmlStorage.Load(buffer,"Links");

   if(!cont->SetComponentInternalLinks(&XmlStorage,owner))
	return RDK_E_MODEL_CREARE_INTERNAL_LINKS_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * UEngine::Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   UEPtr<RDK::UNet> owner;
   if(owner_level_stringid)
	dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

   TempString="";
   if(!cont)
	return TempString.c_str();

   XmlStorage.Create("Links");

   if(!cont->GetComponentInputLinks(&XmlStorage,owner,sublevel))
   {
	DestroyTempString(TempString);
	return 0;
   }

   TempString="";
   XmlStorage.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * UEngine::Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   UEPtr<RDK::UNet> owner;
   if(owner_level_stringid)
	dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

   TempString="";
   if(!cont)
	return TempString.c_str();

   XmlStorage.Create("Links");

   if(!cont->GetComponentOutputLinks(&XmlStorage,owner, sublevel))
   {
	DestroyTempString(TempString);
	return 0;
   }

   TempString="";
   XmlStorage.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char* UEngine::Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   UEPtr<RDK::UNet> owner;
   if(owner_level_stringid)
	owner=dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

   TempString="";
   if(!cont)
	return TempString.c_str();

   XmlStorage.Create("Links");

   if(cont->GetComponentPersonalLinks(&XmlStorage,owner))
	return TempString.c_str();

   TempString="";
   XmlStorage.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возвращает число входов у компонента
int UEngine::Model_GetComponentNumInputs(const char *stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);

   if(!cont)
	return 0;

   return cont->GetNumInputs();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает размер входа компонента в числе элементов
/*int UEngine::Model_GetComponentInputDataSize(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UItem> cont=dynamic_pointer_cast<RDK::UItem>(FindComponent(stringid));

   if(!cont)
	return 0;

   return cont->GetInputDataSize(index)[1]; // TODO: Нужно возвращать массив размерностей
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает размер элемента входа в байтах
int UEngine::Model_GetComponentInputElementSize(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает размер входа компонента в байтах элементов
int UEngine::Model_GetComponentInputByteSize(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
unsigned char* UEngine::Model_GetComponentInputData(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}               */

// Возвращает число выходов у компонента
int UEngine::Model_GetComponentNumOutputs(const char *stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=FindComponent(stringid);

   if(!cont)
	return 0;

   return cont->GetNumOutputs();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}
/*
// Возвращает размер выхода компонента в числе элементов
int UEngine::Model_GetComponentOutputDataSize(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UItem> cont=dynamic_pointer_cast<RDK::UItem>(FindComponent(stringid));

   if(!cont)
	return 0;

   return cont->GetOutputDataSize(index)[1]; // TODO: тут надо передавать полноценную матрицу размеров
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает размер выхода компонента в байтах элементов
int UEngine::Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
unsigned char* UEngine::Model_GetComponentOutputData(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   return cont->GetOutputData(index).UChar;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}                  */

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
const char *  UEngine::Model_SaveComponent(const char *stringid, unsigned int params_type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   XmlStorage.DelNode();
   XmlStorage.Create("Save");
   XmlStorage.SetNodeAttribute("ModelName",Environment->GetModel()->GetName());

   if(!cont->SaveComponent(&XmlStorage, true, params_type_mask))
   {
	DestroyTempString(TempString);
	return 0;
   }

  XmlStorage.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
int UEngine::Model_SaveComponentToFile(const char *stringid, const char* file_name, unsigned int params_type_mask)
{
 return 0;
}


// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponent(const char *stringid, const char* buffer)
{
 RDK::USerStorageXML XmlStorage;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   XmlStorage.Load(buffer,"Save");
   std::string xml_model_name=XmlStorage.GetNodeAttribute("ModelName");
   XmlStorage.SelectNode(0);

   Environment->Reset();

   if(!Environment->GetModel())
   {
	std::string name=XmlStorage.GetNodeAttribute("Class");
	UId id=Storage->FindClassId(name);
	UEPtr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(Environment->GetModel()).Get();
	if(!cont || cont->GetClass() != id)
	{
	 Model_Destroy();
	 Model_Create(name.c_str());
	 cont=dynamic_pointer_cast<RDK::UNet>(Environment->GetModel()).Get();
	}

	if(!cont->LoadComponent(&XmlStorage,true))
	 return RDK_E_MODEL_LOAD_COMPONENT_FAIL;
   }
   else
   {
	if((strlen(stringid) == 0) && (xml_model_name != Environment->GetModel()->GetName()))
	 RDK_RAW_THROW(EErrorEngineModelNameDontMatch(xml_model_name, Environment->GetModel()->GetName()));

	UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

	if(!cont)
	 return RDK_E_MODEL_COMPONENT_NOT_FOUND;

	if(!cont->LoadComponent(&XmlStorage,true))
	 return RDK_E_MODEL_LOAD_COMPONENT_FAIL;
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponentFromFile(const char *stringid, const char* file_name)
{
 int res=RDK_SUCCESS;
 RDK_SYS_TRY
 {
  try
  {
   fstream file(file_name, ios::in);
   string result;

   if(!file || !file.is_open())
	return false;

   while(!file.eof() && !file.fail())
   {
	std::string buffer;
	if(!std::getline(file,buffer))
	 break;
	result+=buffer;
   }

   file.close();
   return Model_LoadComponent(stringid, result.c_str());
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Сохраняет все свойства компонента и его дочерних компонент в xml
const char * UEngine::Model_SaveComponentProperties(const char *stringid, unsigned int type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
   {
	DestroyTempString(TempString);
	return 0;
   }

   XmlStorage.DelNode();
   XmlStorage.Create("SaveProperties");
   XmlStorage.SetNodeAttribute("ModelName",Environment->GetModel()->GetName());

   if(!cont->SaveComponentProperties(&XmlStorage, type_mask))
   {
	DestroyTempString(TempString);
	return 0;
   }

   TempString="";
   XmlStorage.Save(TempString);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Сохраняет все свойства компонента и его дочерних компонент в xml
int UEngine::Model_SaveComponentPropertiesToFile(const char *stringid, const char* file_name, unsigned int type_mask)
{  /*
 const char* save_data=Model_SaveComponentProperties(stringid,type_mask);

 try
 {
  if(!save_data)
   return 871121;

  UFileIO file;
  file.Default();
  file.SetDirection(1);
  file.SetFileName(file_name);
  file.SetBinFlag(0);
  file.SetDataString(save_data);
  file.Calculate();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }*/
 return 0;
}

// Загружает все свойства компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentProperties(const char *stringid, const char* buffer)
{
 RDK::USerStorageXML XmlStorage;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   XmlStorage.Load(buffer,"SaveProperties");
   if(XmlStorage.GetNodeAttribute("ModelName") != Environment->GetModel()->GetName())
	RDK_RAW_THROW(EErrorEngineModelNameDontMatch(XmlStorage.GetNodeAttribute("ModelName"), Environment->GetModel()->GetName()));

   XmlStorage.SelectNode(0);

   if(!cont->LoadComponentProperties(&XmlStorage))
    return RDK_E_MODEL_LOAD_COMPONENT_PROPERTIES_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponentPropertiesFromFile(const char *stringid, const char* file_name)
{/*
 try
 {
  UFileIO file;
  file.Default();
  file.SetDirection(0);
  file.SetFileName(file_name);
  file.SetBinFlag(0);
  file.Calculate();
  if(file.GetDataString().empty())
   return 871122;
  return Model_LoadComponentProperties(stringid, file.GetDataString().c_str());
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }*/
 return 0;
}

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
const char* UEngine::Model_SaveComponentDrawInfo(const char *stringid)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
   {
	DestroyTempString(TempString);
	return 0;
   }
   USerStorageXML xml;
   xml.DelNode();
   xml.Create("Save");
   xml.SetNodeAttribute("ModelName",Environment->GetModel()->GetName());

   if(!cont->SaveComponentDrawInfo(&xml))
   {
	DestroyTempString(TempString);
	return 0;
   }

   xml.Save(TempString);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Управляет шагом счета модели по умолчанию
unsigned int UEngine::Model_GetDefaultTimeStep(void) const
{
 return DefaultTimeStep;
}

int UEngine::Model_SetDefaultTimeStep(unsigned int value)
{
 DefaultTimeStep=value;
 return RDK_SUCCESS;
}

// Управляет шагом счета компонента
unsigned int UEngine::Model_GetTimeStep(const char *stringid) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetTimeStep();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

int UEngine::Model_SetTimeStep(const char *stringid, unsigned int value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   cont->SetTimeStep(value);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Устанавливает шаг счета компонента и всех его дочерних компонент
int UEngine::Model_SetGlobalTimeStep(const char *stringid, unsigned int value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   cont->SetGlobalTimeStep(value);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Возвращает текущее время модели
unsigned long long UEngine::Model_GetTime(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetTime();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

double UEngine::Model_GetDoubleTime(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetDoubleTime();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Устанавливает текущее время модели
int UEngine::Model_SetTime(unsigned long long value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Environment->GetTime().SetTime(value))
	res=RDK_E_ENV_SET_TIME_FAIL;
   else
	res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Возвращает реальное время
unsigned long long UEngine::Model_GetRealTime(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetRealTime();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

double UEngine::Model_GetDoubleRealTime(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetDoubleRealTime();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Устанавливает реальное время
int UEngine::Model_SetRealTime(unsigned long long value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Environment->GetTime().SetRealTime(value))
	return RDK_E_ENV_SET_TIME_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Увеличивает реальное время на заданную величину
int UEngine::Model_IncreaseRealTime(unsigned long long value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Environment->GetTime().IncreaseRealTime(value))
	return RDK_E_ENV_SET_TIME_FAIL;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Возвращает мгновенный шаг в реальном времени
unsigned long long UEngine::Model_GetRealTimeStep(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetRealTimeStep();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

double UEngine::Model_GetDoubleRealTimeStep(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetDoubleRealTimeStep();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0.0;
}

double UEngine::Model_GetDoubleSourceTime(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetSourceCurrentGlobalTime();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0.0;
}

// Устанавливает время внешних источников данных в днях
int UEngine::Model_SetDoubleSourceTime(double value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().SetSourceCurrentGlobalTime(value);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Возвращает время расчета компонента без времени расчета дочерних компонент (мс)
unsigned long long UEngine::Model_GetStepDuration(const char *stringid) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetStepDuration();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

// Возвращает время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
unsigned long long UEngine::Model_GetFullStepDuration(const char *stringid) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetFullStepDuration();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}


// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double UEngine::Model_GetInstantPerformance(const char *stringid) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetInstantPerformance();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0.0;
}

// Время, прошедшее между двумя последними итерациями счета
unsigned long long UEngine::Model_GetInterstepsInterval(const char *stringid) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetInterstepsInterval();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}
// --------------------------

// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::MDMatrix*
// если выход не содержит данных такого типа, то возвращает 0
const /* RDK::MDMatrix* */void* UEngine::Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   // Ищем указатель на выходные данные
   UIPropertyOutput* output_property=cont->FindOutputProperty(property_name);
   if(!output_property)
	return 0;

   if(output_property->GetLanguageType() == typeid(MDMatrix<double>) ||
	 output_property->GetLanguageType() == typeid(MDVector<double>))
   {
    return output_property->GetMemoryArea();
   }

   return 0;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}
	  /*
const void* UEngine::Model_GetComponentOutputAsMatrix(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   // Ищем указатель на выходные данные
   UIPropertyOutput* output_property=0;
   cont->FindOutputProperty(index, output_property);
   if(!output_property)
	return 0;

   if(output_property->GetLanguageType() == typeid(MDMatrix<double>) ||
	 output_property->GetLanguageType() == typeid(MDVector<double>))
   {
	return output_property->GetPointer(0);
   }

  return 0;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}
                   */
// Возвращает указатель на выход с индексом 'index' компонента 'id'
const RDK::UBitmap* UEngine::Model_GetComponentOutput(const char *stringid, const char *property_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!property_name)
	return 0;

   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
   if(!iproperty)
	return 0;
   if(iproperty->GetLanguageType() != typeid(UBitmap))
	return 0;

   return (const RDK::UBitmap*)iproperty->GetMemoryArea();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return 0;
}
						   /*
const RDK::UBitmap* UEngine::Model_GetComponentOutput(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   if(index<0 || index >= cont->GetNumOutputs())
	return 0;

   UIProperty* iproperty=0;
   cont->FindOutputProperty(index, iproperty);
   UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
   if(!property)
	return 0;
   return (const RDK::UBitmap*)property->GetMemoryArea();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}                    */

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const RDK::UBitmap* UEngine::Model_GetComponentBitmapOutput(const char *stringid, const char *property_name)
{
 return Model_GetComponentOutput(stringid, property_name);
}
			   /*
const RDK::UBitmap* UEngine::Model_GetComponentBitmapOutput(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   if(index<0 || index >= cont->GetNumOutputs())
	return 0;

   UIProperty* iproperty=0;
   cont->FindOutputProperty(index, iproperty);
   UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
   if(!property)
	return 0;
   return (const RDK::UBitmap*)property->GetMemoryArea();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}
                        */
/// Копирует данные о разрешении изображения выхода с индексом 'index' компонента 'id'
/// в стрктуру bmp_param
int UEngine::Model_CopyComponentBitmapOutputHeader(const char *stringid, const char *property_name, RDK::UBitmapParam* bmp_param)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!bmp_param)
	return RDK_E_NULL_POINTER_IN_ARGUMENT;

   const RDK::UBitmap *temp_bmp=Model_GetComponentBitmapOutput(stringid, property_name);
   if(!temp_bmp)
	return RDK_E_MODEL_BITMAP_OUTPUT_NOT_FOUND;

   bmp_param->Width=temp_bmp->GetWidth();
   bmp_param->Height=temp_bmp->GetHeight();
   bmp_param->ColorModel=temp_bmp->GetColorModel();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
/*
int UEngine::Model_CopyComponentBitmapOutputHeaderByIndex(const char *stringid, int index, RDK::UBitmapParam* bmp_param)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!bmp_param)
	return RDK_E_NULL_POINTER_IN_ARGUMENT;

   const RDK::UBitmap *temp_bmp=Model_GetComponentBitmapOutput(stringid, index);
   if(!temp_bmp)
	return RDK_E_MODEL_BITMAP_OUTPUT_NOT_FOUND;

   bmp_param->Width=temp_bmp->GetWidth();
   bmp_param->Height=temp_bmp->GetHeight();
   bmp_param->ColorModel=temp_bmp->GetColorModel();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}                     */

/// Копирует изображение выхода с индексом 'index' компонента 'id'
/// метод предполагает, что bmp уже имеет выделенную память под изобржение требуемого размера
int UEngine::Model_CopyComponentBitmapOutput(const char *stringid, const char *property_name, RDK::UBitmap* bmp)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!bmp)
	return RDK_E_NULL_POINTER_IN_ARGUMENT;

   const RDK::UBitmap *temp_bmp=Model_GetComponentBitmapOutput(stringid, property_name);
   if(!temp_bmp)
	return RDK_E_MODEL_BITMAP_OUTPUT_NOT_FOUND;

   if(temp_bmp->GetWidth() != bmp->GetWidth() || temp_bmp->GetHeight() != bmp->GetHeight() ||
	temp_bmp->GetColorModel() != bmp->GetColorModel())
	return RDK_E_MODEL_IMAGES_MUST_HAVE_SAME_SIZE;

   memcpy(bmp->GetData(),temp_bmp->GetData(),bmp->GetByteLength());
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
/*
int UEngine::Model_CopyComponentBitmapOutput(const char *stringid, int index, RDK::UBitmap* bmp)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!bmp)
	return RDK_E_NULL_POINTER_IN_ARGUMENT;

   const RDK::UBitmap *temp_bmp=Model_GetComponentBitmapOutput(stringid, index);
   if(!temp_bmp)
	return RDK_E_MODEL_BITMAP_OUTPUT_NOT_FOUND;

   if(temp_bmp->GetWidth() != bmp->GetWidth() || temp_bmp->GetHeight() != bmp->GetHeight() ||
	temp_bmp->GetColorModel() != bmp->GetColorModel())
	return RDK_E_MODEL_IMAGES_MUST_HAVE_SAME_SIZE;

   memcpy(bmp->GetData(),temp_bmp->GetData(),bmp->GetByteLength());
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
 */


// Возвращает указатель на вход с индексом 'index' компонента 'id'
const RDK::UBitmap* UEngine::Model_GetComponentBitmapInput(const char *stringid, const char *property_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!property_name)
	return 0;

   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
   if(!iproperty)
	return 0;
   if(iproperty->GetLanguageType() != typeid(UBitmap))
	return 0;

   return (const RDK::UBitmap*)iproperty->GetMemoryArea();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}
/*
const RDK::UBitmap* UEngine::Model_GetComponentBitmapInput(const char *stringid, int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   if(index<0 || index >= cont->GetNumInputs())
	return 0;

   UIProperty* iproperty=0;
   cont->FindInputProperty(index, iproperty);
   UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
   if(!property)
	return 0;
   return (const RDK::UBitmap*)property->GetMemoryArea();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}
*/
// Замещает изображение выхода с индексом 'index' компонента 'id'
int UEngine::Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const RDK::UBitmap* bmp, bool reflect)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!bmp || !property_name)
	return RDK_E_NULL_POINTER_IN_ARGUMENT;
   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
   if(!iproperty)
	return RDK_E_MODEL_PROPERTY_NOT_FOUND;
   if(iproperty->GetLanguageType() != typeid(UBitmap))
	return RDK_E_MODEL_PROPERTY_HAVE_INCOMPATIBLE_TYPE;

   if(reflect)
   {
	const_cast<UBitmap*>(bmp)->ReflectionX(&TempBmp);
	iproperty->ReadFromMemory(&TempBmp);
   }
   else
	iproperty->ReadFromMemory(bmp);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
  /*
int UEngine::Model_SetComponentBitmapOutput(const char *stringid, int index, const RDK::UBitmap* bmp, bool reflect)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(index<0 || index >= cont->GetNumOutputs())
	return RDK_E_MODEL_PROPERTY_INDEX_OUT_OF_RANGE;

   UIProperty* iproperty=0;
   cont->FindOutputProperty(index, iproperty);
   UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
   if(!property)
	return RDK_E_MODEL_PROPERTY_NOT_FOUND;

   if(reflect)
   {
	const_cast<UBitmap*>(bmp)->ReflectionX(&TempBmp);
	property->SetData(TempBmp);
   }
   else
	property->SetData(*bmp);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}   */

// Замещает изображение входа с индексом 'index' компонента 'id'
int UEngine::Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const RDK::UBitmap* const bmp, bool reflect)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!bmp || !property_name)
	return RDK_E_NULL_POINTER_IN_ARGUMENT;
   UEPtr<RDK::UNet> cont=FindComponent(stringid);
   UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
   if(!iproperty)
	return RDK_E_MODEL_PROPERTY_NOT_FOUND;
   if(iproperty->GetLanguageType() != typeid(UBitmap))
	return RDK_E_MODEL_PROPERTY_HAVE_INCOMPATIBLE_TYPE;

   if(reflect)
   {
	const_cast<UBitmap*>(bmp)->ReflectionX(&TempBmp);
	iproperty->ReadFromMemory(&TempBmp);
   }
   else
	iproperty->ReadFromMemory(bmp);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
	  /*
int UEngine::Model_SetComponentBitmapInput(const char *stringid, int index, const RDK::UBitmap* const bmp, bool reflect)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(index<0 || index >= cont->GetNumInputs())
	return RDK_E_MODEL_PROPERTY_INDEX_OUT_OF_RANGE;


   UIProperty* iproperty=0;
   cont->FindInputProperty(index, iproperty);
   UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
   if(!property)
	return RDK_E_MODEL_PROPERTY_NOT_FOUND;

   if(reflect)
   {
	const_cast<UBitmap*>(bmp)->ReflectionX(&TempBmp);
	property->SetData(TempBmp);
   }
   else
	property->SetData(*bmp);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}     */

// --------------------------
// Методы управления исключениями
// --------------------------
// Возвращает массив строк лога
const char* UEngine::GetLog(int &error_level) const
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Logger->GetLog(error_level);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* UEngine::GetUnreadLog(int &error_level, int &number, time_t &time)
{
 std::string& TempString=CreateTempString();
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Logger->GetUnreadLog(error_level, number, time);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return TempString.c_str();
}

/// Записывает в лог новое сообщение
int UEngine::Engine_LogMessage(int log_level, const char *message, int error_event_number)
{
 if(!Logger)
  return RDK_E_LOGGER_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Logger->LogMessage(log_level,message,error_event_number);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int UEngine::Engine_LogMessage(int msg_level, const char *method_name, const char *message, int error_event_number)
{
 if(!Logger)
  return RDK_E_LOGGER_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Logger->LogMessage(msg_level,method_name,message,error_event_number);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int UEngine::Engine_LogMessageEx(int msg_level, const char *object_name, const char *message, int error_event_number)
{
 if(!Logger)
  return RDK_E_LOGGER_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Logger->LogMessageEx(msg_level,object_name,message,error_event_number);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int UEngine::Engine_LogMessageEx(int msg_level, const char *object_name, const char *method_name, const char *message, int error_event_number)
{
 if(!Logger)
  return RDK_E_LOGGER_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Logger->LogMessageEx(msg_level,object_name,method_name,message,error_event_number);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Управление функцией-обработчиком исключений
ULoggerEnv::PExceptionHandler UEngine::GetExceptionHandler(void) const
{
 if(!Logger)
  return 0;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Logger->GetExceptionHandler();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

int UEngine::SetExceptionHandler(ULoggerEnv::PExceptionHandler value)
{
 if(!Logger)
  return RDK_E_LOGGER_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Logger->SetExceptionHandler(value))
    res=RDK_E_LOGGER_SET_EXCEPTION_HANDLER_FAIL;
   else
	res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

// Максимальное число хранимых исключений
// Если 0, то неограниченно
/*int UEngine::GetMaxExceptionsLogSize(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Logger)
    return 0;
   return Logger->GetMaxExceptionsLogSize();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

int UEngine::SetMaxExceptionsLogSize(int value)
{
 if(!Logger)
  return RDK_E_LOGGER_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Logger->SetMaxExceptionsLogSize(value);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}     */


/// Возвращает число непрочитанных строк лога
int UEngine::GetNumUnreadLogLines(void) const
{
 if(!Logger)
  return 0;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Logger->GetNumUnreadLogLines();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

/// Возвращает число строк лога
int UEngine::GetNumLogLines(void) const
{
 if(!Logger)
  return 0;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Logger->GetNumLogLines();
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return 0;
}

/// Очищает лог прочитанных сообщений
int UEngine::ClearReadLog(void)
{
 if(!Logger)
  return RDK_E_LOGGER_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Logger->ClearReadLog();
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}
// --------------------------



// --------------------------
// Методы внутреннего управления движком
// --------------------------
// Обрабатывает возникшее исключение
/// Возвращает RDK_UNHANDLED_EXCEPTION если не удалось записать данные исключения
/// иначе возвращает RDK_EXCEPTION_CATCHED
int UEngine::ProcessException(UException &exception) const
{
 if(Logger)
  Logger->ProcessException(exception);
 else
  return RDK_UNHANDLED_EXCEPTION;

 return RDK_EXCEPTION_CATCHED;
 //return ProcessException((const UException &)(exception));
}

int UEngine::ProcessException(const UException &exception) const
{
 if(Logger)
  Logger->ProcessException(const_cast<UException &>(exception));
 else
  return RDK_UNHANDLED_EXCEPTION;

 return RDK_EXCEPTION_CATCHED;
}

// Создает среду и возвращает указатель на нее.
// Если задано хранилище 'storage', то связывает его со средой.
// Если флаг 'isinit' == true, то инициализирует хранилище стандартными библиотеками
// Если указатель на массив external_libs != 0, дополнительно инициализирует хранилище этими бибилиотеками
void UEngine::CreateEnvironment(bool isinit, list<UContainer*>* external_classes, list<ULibrary*>* external_libs)
{
 if(!Storage)
  return;

 if(!Environment)
  return;

 RDK_SYS_TRY
 {
  try
  {
   Environment->Default();

   if(!Environment->SetStorage(Storage) || !isinit)
	return;

   if(external_classes != 0)
   {
	list<UContainer*>::iterator I,J;
	I=external_classes->begin();
	J=external_classes->end();
	while(I != J)
	{
	 Storage->AddClass(*I);
	 ++I;
	}
   }

   if(external_libs != 0)
   {
	list<ULibrary*>::iterator I,J;
	I=external_libs->begin();
	J=external_libs->end();
	while(I != J)
	{
	 Storage->AddCollection(*I);
	 ++I;
	}
   }

   Logger->LogMessage(RDK_EX_DEBUG, "Build storage has been started...");
   Storage->BuildStorage();
   Logger->LogMessage(RDK_EX_DEBUG, "Build storage has been finished");
  }
  catch (RDK::UException &exception)
  {
   ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
}

// Загружает набор предустановленных библиотек
int UEngine::LoadPredefinedLibraries(void)
{
 RdkLoadPredefinedLibraries(LibrariesList);

 return 0;
}

// Загружает набор классов
int UEngine::LoadClasses(void)
{
 vector<string> variables;
 int res=0;
 UContainer *dllclass=0;

 Options.GetVariableList(ComponentClassesSectionName, variables);

 for(size_t i=0;i<variables.size();i++)
 {
  //dllclass=LoadUClass(Options(ComponentClassesSectionName,variables[i],""),variables[i]);
  if(!dllclass)
   res|=1;
  ClassesList.push_back(dllclass);
 }
 return res;
}

// Загружает набор библиотек и добавляет указатели на нее в массив external_libs
// возвращает 0 в случае успеха
// Список имен библиотек берется из ini-файла 'inifile', секция 'section'
int UEngine::LoadLibraries(void)
{
 vector<string> variables;
 int res=0;
 ULibrary *library=0;

 Options.GetVariableList(ComponentLibrariesSectionName, variables);

 for(size_t i=0;i<variables.size();i++)
 {
  //library=LoadULibrary(Options(ComponentLibrariesSectionName,variables[i],""),variables[i]);
  if(!library)
   res|=1;
  LibrariesList.push_back(library);
 }
 return res;
}
// --------------------------

// --------------------------
// Методы внутреннего управления консолью
// --------------------------
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Осуществляет поиск компонента по длинному строковому id
// Если строковое id не задано, то возвращает указатель на модель
UEPtr<UNet> UEngine::FindComponent(const char *stringid) const
{
 UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

 if(!model)
  return 0;

 UEPtr<RDK::UNet> cont;
 if(!stringid)
  cont=model;
 else
 {
   if(strlen(stringid) == 0)
	cont=model;
   else
	cont=model->GetComponentL<RDK::UNet>(stringid);
 }

// if(stringid)
//  AccessCache[stringid]=cont;
 return cont;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool UEngine::ADefault(void)
{
 // Имя файла инициализации
 OptionsFileName="options.ini";

 // Имя файла описаний параметров классов
 ClassesDescriptionFileName="ClassesDescription.xml";

 // Имя секции выбора основной библиотеки
// MainLibrarySectionName="MainDLL";

 // Имя переменной основной библиотеки
// MainLibraryName="MainLibraryName";
// Options(MainLibrarySectionName,MainLibraryName,string("nmsdk.dll"));

 // Имя секции выбора библиотек компонент
 ComponentLibrariesSectionName="ComponentLibraries";

 ComponentClassesSectionName="ComponentClasses";

 // Индекс используемого хранилища в библиотеке
 // Если < 0, то новое хранилище будет создано
// StorageIndex=-1;
// Options("General","StorageIndex",sntoa(StorageIndex));

 // Индекс используемой среды в библиотеке
 // Если < 0, то новая среда будет создана
// EnvironmentIndex=-1;
// Options("General","EnvironmentIndex",sntoa(EnvironmentIndex));

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UEngine::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool UEngine::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool UEngine::ACalculate(void)
{
 return true;
}
// --------------------------

}
#endif
