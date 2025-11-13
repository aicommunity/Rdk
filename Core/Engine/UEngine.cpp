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
#include "UComponentFactory.h"
#include "UMockUNet.h"
#include "UBasePropCreator.h"
#include "UBasePropCreatorStd.h"
#include "UBasePropCreatorVector.h"
#include "UBasePropCreatorMatrix.h"
#include <glog/logging.h>

// --------------------------------------
// ���������� �������������� �������
// --------------------------------------
// ���������� ��������� �� �������
extern RDK::UStorage* GetStorage(size_t i);

// ���������� ����� �� �������
extern RDK::UEnvironment*  GetEnvironment(size_t i);

// ���������� ������ �� �������
extern RDK::UEngine*  GetEngine(size_t i);

// ������� ����� ��������� � �������� � ����� �������
// ���������� ��������� �� ���������
extern RDK::UStorage*  AddNewStorage(void);

// ������� ����� ����� � �������� � ����� �������
// ���������� ��������� �� �����
extern RDK::UEnvironment*  AddNewEnvironment(void);

// ������� ����� ������ � �������� � ����� �������
// ���������� ��������� �� ������
extern RDK::UEngine*  AddNewEngine(void);
// --------------------------------------

namespace RDK{

// --------------------------
// ������������ � �����������
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
// ������ ���������� ����������� �������������
// --------------------------
/// ����� �������� ���������� ��������� ���������� ���
/// ������������ ��������
/// 0 - ���� ���������� ��� ���� �������, ������������ ����� ���
/// 1 - ���������� ���������� � �������������� ������� ������� �������
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

// ��� ����� �������������
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

// ��� ����� �������� ���������� �������
const string& UEngine::GetClassesDescriptionFileName(void) const
{
 return ClassesDescriptionFileName;
}

bool UEngine::SetClassesDescriptionFileName(const string& value)
{
 if(ClassesDescriptionFileName == value)
  return true;

 ClassesDescriptionFileName=value;
 return true;
}

// ��� ����� �������� ����� ���������� �������
const string& UEngine::GetCommonClassesDescriptionFileName(void) const
{
 return CommonClassesDescriptionFileName;
}

bool UEngine::SetCommonClassesDescriptionFileName(const string& value)
{
 if(CommonClassesDescriptionFileName == value)
  return true;

 CommonClassesDescriptionFileName=value;
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ���������� �����������
// --------------------------
/// ������� � ������ ��������� ������ ����� ������
/// � ���������� ������ �� ���
std::string& UEngine::CreateTempString(void) const
{
 switch (BufObjectsMode)
 {
 case 0:
 {
  if(TempStrings.empty())
  {
   std::shared_ptr<string> pstr=std::make_shared<string>();
   TempStrings.insert(std::pair<std::shared_ptr<string>,bool>(pstr,true));
  }
  return *TempStrings.begin()->first;
 }
 break;

 case 1:
 {
  std::map<std::shared_ptr<std::string>,bool>::iterator I,J;
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

  std::shared_ptr<string> pstr=std::make_shared<string>();
  TempStrings.insert(std::pair<std::shared_ptr<string>,bool>(pstr,true));
  return *pstr;
 }
 break;
 }
 return DummyTempString;
}

/// ���������� ��������� ������
/// �� ��������� �� �� ������
/// �� ������� ��������� ������!
std::string& UEngine::FindTempString(const char *str_data) const
{
 std::map<std::shared_ptr<std::string>,bool>::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  if(I->first->c_str() == str_data && I->second)
   return *I->first;

 }
 return DummyTempString;
}

/// ������� ��������� ������
/// �� ��������� �� �� ������
void UEngine::DestroyTempString(const char *str_data) const
{
 if(!str_data)
  return;

 if(BufObjectsMode == 0)
  return;

 std::map<std::shared_ptr<std::string>,bool>::iterator I,J;
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

/// ������� ��������� ������
/// �� ������ �� ���
void UEngine::DestroyTempString(const std::string &ref) const
{
 if(BufObjectsMode == 0)
  return;

 std::map<std::shared_ptr<std::string>,bool>::iterator I,J;
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

/// ������� ��� ��������� �����
void UEngine::ClearAllTempStrings(void) const
{
 std::map<std::shared_ptr<std::string>,bool>::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  // delete I->first; // shared_ptr will auto-delete
 }
 TempStrings.clear();
}

/// ���������� ����� ��������� �����
int UEngine::GetNumTempStrings(void) const
{
 if(BufObjectsMode == 0)
  return 0;

 return int(TempStrings.size());
}
// --------------------------

// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
/*// ���� ������ �������
int UEngine::IsRunned(void)
{
 return Runned;
} */

/// ������ �������� ������ � �������������� ����������
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

 // SetChannelIndex удален - используется glog
 return true;
}
// --------------------------


// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ��������� �� �����
UEnvironment* UEngine::GetEnvironment(void)
{
 return Environment;
}

// ���������� ��������� �� ������
UContainer* UEngine::GetModel(void)
{
 return Environment->GetModel().get();
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ��������� �� ������
// GetLogger удален - используется glog

// SetLogger удален - используется glog

// �������������� ������ ������
void UEngine::Init(void)
{
 // ��������
}

bool UEngine::Init(UStorage* storage, UEnvironment* env)
{
 if(!Default())
  return false;

 AccessCache.clear();

 Storage=storage;
 Environment=env;

 // ClearLog, RegisterEnvironment, SetChannelIndex удалены - используется glog
 Environment->SetChannelIndex(ChannelIndex);
 // Environment->SetLogger удален - используется glog

 if(!Storage)
  return false;
 // Storage->SetLogger удален - используется glog


 RDK_SYS_TRY
 {
  try
  {
   LibrariesList.clear();
   ClassesList.clear();
   if(LoadPredefinedLibraries())
	return false;
   if(LoadPredefinedCrPropFunctions())
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

 if(!CommonClassesDescriptionFileName.empty())
 {
  std::string common_classes_description_xml;
  if(LoadFile(CommonClassesDescriptionFileName,common_classes_description_xml))
  {
   if(Storage_LoadCommonClassesDescription(common_classes_description_xml.c_str()) != RDK_SUCCESS)
	LOG(ERROR) << "UEngine::" << __FUNCTION__ << " - Failed to apply common classes descriptions from file " << CommonClassesDescriptionFileName;
  }
  else
   LOG(ERROR) << "UEngine::" << __FUNCTION__ << " - Failed to load common classes description file " << CommonClassesDescriptionFileName;
 }

 if(!ClassesDescriptionFileName.empty())
 {
  std::string classes_description_xml;
  if(LoadFile(ClassesDescriptionFileName,classes_description_xml))
  {
   if(Storage_LoadClassesDescription(classes_description_xml.c_str()) != RDK_SUCCESS)
	LOG(ERROR) << "UEngine::" << __FUNCTION__ << " - Failed to apply classes descriptions from file " << ClassesDescriptionFileName;
  }
  else
   LOG(ERROR) << "UEngine::" << __FUNCTION__ << " - Failed to load classes description file " << ClassesDescriptionFileName;
 }

 return true;
}

// ���������������� ������ ������
// � ��������� ������� ���������
void UEngine::UnInit(void)
{
 AccessCache.clear();
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
int UEngine::Storage_GetNumClasses(void)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->GetNumClasses();
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

// ���������� id ������� � ���������. ������ ������ ���� ��������
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


// ���������� ����� ������� � ��������� � ���� ������, ����������� ��������
const char* UEngine::Storage_GetClassesNameList(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
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
   ProcessException(exception);
   DestroyTempString(TempString);
  }
  catch (std::exception &exception)
  {
   ProcessException(RDK::UExceptionWrapperStd(exception));
   DestroyTempString(TempString);
  }
 }
 RDK_SYS_CATCH
 {
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  DestroyTempString(TempString);
 }
 return 0;
}

 // ���������� ��� ������ �� ��� id.
const char * UEngine::Storage_GetClassName(int id) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
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
   ProcessException(exception);
   DestroyTempString(TempString);
  }
  catch (std::exception &exception)
  {
   ProcessException(RDK::UExceptionWrapperStd(exception));
   DestroyTempString(TempString);
  }
 }
 RDK_SYS_CATCH
 {
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  DestroyTempString(TempString);
 }
 return 0;
}

// ���������� Id ������ �� ��� �����
int UEngine::Storage_GetClassId(const char *name) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->FindClassId(name);
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
 return ForbiddenId;
}

// ������� ������� ������ ������� �� ���������
// ���� 'force' == true �� ������������� ������� �� ���������
// ��� ������� ����� ������
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

// �������� ��� ��������� ������� �� ���������
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

// ������� ��� ������� �� ���������
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

// ��������� ��������� ����� �������� � ���������
int UEngine::Storage_CalcNumObjects(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->CalcNumObjects();
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

int UEngine::Storage_CalcNumObjectsById(int classid) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->CalcNumObjects(classid);
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

int UEngine::Storage_CalcNumObjectsByName(const char* classname) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return int(Storage->CalcNumObjects(classname));
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

// ���������� �������� ������ �� ��� id � ������� xml
const char* UEngine::Storage_GetClassDescription(const char* classname)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
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
 return TempString.c_str();
}

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
int UEngine::Storage_SetClassDescription(const char* classname, const char* description)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   USerStorageXML xml;
   xml.Load(description, classname);
   Storage->LoadClassDescription(classname,xml); // TODO: ��� �������� �� ��������� ������
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

// ��������� �������� ���� ������� � xml
const char* UEngine::Storage_SaveClassesDescription(void)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
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
 return TempString.c_str();
;
}

// ��������� �������� ���� ������� �� xml
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

// ��������� ����� �������� ���� ������� � xml
const char* UEngine::Storage_SaveCommonClassesDescription(void)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
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
 return TempString.c_str();
}

// ��������� ����� �������� ���� ������� �� xml
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

// ��������� �������� ���� ������� � xml ������� ����� ��������
const char* UEngine::Storage_SaveAllClassesDescription(void)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
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
 return TempString.c_str();
}

// ��������� �������� ���� ������� �� xml ������� ����� ��������
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

// ���������� �������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char* UEngine::Storage_GetClassProperties(const char *stringid, unsigned int type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UVirtualMethodFactory> factory=dynamic_pointer_cast<RDK::UVirtualMethodFactory>(Storage->GetComponentFactory(stringid));


			if(!factory)
	return TempString.c_str();

			std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(factory->GetComponent());

   if(!cont)
    return TempString.c_str();

   XmlStorage.Create(cont->GetName());
   XmlStorage.AddNode(UVariable::GetPropertyTypeNameByType(type_mask));

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
 DestroyTempString(TempString);
 return 0;
}

// ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char* UEngine::Storage_GetClassStructure(const char *stringid, unsigned int type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 std::shared_ptr<UNet> cont;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UComponentAbstractFactory> factory=Storage->GetComponentFactory(stringid);
   cont=dynamic_pointer_cast<RDK::UNet>(factory->New());

   if(!cont)
	return TempString.c_str();

   XmlStorage.Create(cont->GetName());
   XmlStorage.SetNodeAttribute("Class",/*RDK::sntoa(cont->GetClass())*/Storage->FindClassName(cont->GetClass()));
   if(!cont->SaveComponentStructure(&XmlStorage, true, type_mask))
   {
	DestroyTempString(TempString);
    cont.reset(); // shared_ptr handles deletion
	return 0;
   }

   XmlStorage.Save(TempString);
   cont.reset(); // shared_ptr handles deletion
   return TempString.c_str();
  }
  catch (RDK::UException &exception)
  {
   cont.reset(); // shared_ptr handles deletion
   ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   cont.reset(); // shared_ptr handles deletion
   ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  cont.reset(); // shared_ptr handles deletion
  ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 DestroyTempString(TempString);
 return 0;
}
// ----------------------------

// ----------------------------
// ������ ���������� ����������� ���������
// ----------------------------
// ���������� ����� ���������
int UEngine::Storage_GetNumClassLibraries(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Storage->GetNumCollections();
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


// ���������� ������ ��������� � ���� ������, ����������� ��������
const char * UEngine::Storage_GetClassLibrariesList(void) const
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
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
 return TempString.c_str();
}

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// library_name - ��� ����������
const char * UEngine::Storage_GetLibraryClassNames(const char *library_name) const
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString.clear();
   std::shared_ptr<ULibrary> lib=Storage->GetCollection(library_name);
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
 return TempString.c_str();
}

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// index - ������ ����������
const char * UEngine::Storage_GetLibraryClassNamesByIndex(int index) const
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString.clear();
   std::shared_ptr<ULibrary> lib=Storage->GetCollection(index);
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
 return TempString.c_str();
}


// ���������� ��� ���������� �� �������
const char * UEngine::Storage_GetClassLibraryNameByIndex(int index)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Storage->GetCollectionName(index);
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
 return TempString.c_str();
}

// ���������� ������ ���������� �� �������
const char * UEngine::Storage_GetClassLibraryVersionByIndex(int index)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Storage->GetCollectionVersion(index);
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
 return TempString.c_str();
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
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

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
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

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
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

// ��������� ��������� �� ����� dll-�����
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

// ��������� runtime-���������
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

// ��������� runtime-���������
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

/// ������� ����� runtime-����������
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

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
int UEngine::Storage_CreateClass(const char* stringid, const char *class_name, const char *collection_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> sample=FindComponent(stringid);
   std::shared_ptr<URuntimeLibrary> library=dynamic_pointer_cast<URuntimeLibrary>(Storage->GetCollection(collection_name));

   if(!sample)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(!library)
	return RDK_E_STORAGE_COLLECTION_NOT_FOUND;

   if(!Storage->AddClassToCollection(class_name, "", false, sample.get(), collection_name))
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

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
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
// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
int UEngine::Env_GetPredefinedStructure(void) const
{
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

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool UEngine::Env_IsStoragePresent(void) const
{
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

// ���������� ��������� �������������
bool UEngine::Env_IsInit(void) const
{
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

// ������� ������� �������������� ���������
bool UEngine::Env_IsStructured(void) const
{
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

// ���������� ��������� ���������� ������������
bool UEngine::Env_GetEventsLogMode(void) const
{
 RDK_SYS_TRY
 {
  try
  {
   return false; // GetEventsLogMode удален - используется glog
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

// ��������/��������� ��������� ������������
int UEngine::Env_SetEventsLogMode(bool value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // SetEventsLogMode удален - используется glog
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

// ������������� �����
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

// ��������������� �����
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

// ��������� �������������� �������� ������ ���������
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

// ���������� ������� ������ ���������
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

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
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


// �������������� ������
int UEngine::Env_ModelInit(const char *stringid)
{
    int res=RDK_UNHANDLED_EXCEPTION;
    RDK_SYS_TRY
    {
     try
     {
      if(!stringid)
      {
       Environment->ModelInit();
      }
      else
      {
       std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);
       if(destcont)
        destcont->Init();
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

// ���������������� ������
int UEngine::Env_ModelUnInit(const char *stringid)
{
    int res=RDK_UNHANDLED_EXCEPTION;
    RDK_SYS_TRY
    {
     try
     {
      if(!stringid)
      {
       Environment->ModelUnInit();
      }
      else
      {
       std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);
       if(destcont)
        destcont->UnInit();
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


// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
int UEngine::Env_Calculate(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RDK::ULongId id;
   if(!stringid)
   {
	Environment->SetModelCalculationComponent(id);
   }
   else
   {
        std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);
	if(destcont)
	 destcont->GetLongId(Environment->GetModel(),id);
	else
	 return RDK_E_MODEL_COMPONENT_NOT_FOUND;
	Environment->SetModelCalculationComponent(id);
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

// ������ ���� ������ � �������� �������
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


/// ������ ������ �������� ������������� calc_intervsal ������ � ����������� ��������� ���������
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

// ����� ������ �����
// ���� stringid == 0 �� ���������� ��� ������ �������,
// ����� - ������ ��������� ��������� ������
int UEngine::Env_Reset(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RDK::ULongId id;
   if(!stringid)
   {
	Environment->SetModelCalculationComponent(id);
	if(!Environment->Reset())
	 throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
   }
   else
   {
        std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);
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

/// ����� ������ ���������� �� �������� �� ���������
/// ���� stringid == 0 �� ���������� ��� ������ �������,
/// ����� - ������ ��������� ��������� ������
/// ���� subcomps == true �� ����� ���������� ��������� ���� �������� ���������
int UEngine::Env_Default(const char* stringid, bool subcomps)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<RDK::UContainer> destcont;
   if(!stringid)
   {
	destcont=std::shared_ptr<UContainer>(GetModel());
   }
   else
   {
	destcont=FindComponent(stringid);
   }

   if(!destcont->DefaultAll(destcont.get(), subcomps))
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



// ���������� ���������� ������� ������ �� ��������� ��������
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

/// ������������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
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

/// ���������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
unsigned long long UEngine::Env_GetMinInterstepsInterval(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetMinInterstepsInterval();
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

// �����, ����������� �� ��������� RT-������
double UEngine::Env_GetRTLastDuration(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetRTLastDuration();
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
 return 0.0;
}

/// �����, ����������� � ������ �� ���� ����� RTCalculate;
double UEngine::Env_GetRTModelCalcTime(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetRTModelCalcTime();
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
 return 0.0;
}

/// ������������������ RT ������� (��������� RTModelCalcTime/RTLastDuration)
double UEngine::Env_CalcRTPerformance(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->CalcRTPerformance();
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
 return 0.0;
}

// !!! ��������� ������ ���������� ������� ����������� ������ �� ���
// ������, ������������ � ����������� �� ���������� id !!!
// ������������� ������� ��������� (��������� ������������ ����� - ������)
int UEngine::Env_SelectCurrentComponent(const char *stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   Environment->ResetCurrentComponent();

   // ���� ������ ������ - �����, �� ��������� ��� ��������� id
   if(stringid[0]>=0x30 && stringid[0]<=0x39)
   {
	ULongId longid;
	longid.DecodeFromString(stringid);
	Environment->SelectCurrentComponent(longid);
   }
   else // ...����� ���������� ��� ��������� ���
   {
	Environment->SelectCurrentComponent(stringid);
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

// ���������� ������� ��������� � ��������� �� ��������� (������)
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

// ������ ������� ��������� �� ��� �������� (������ �� ������� �����)
// ���� ��� �� ������� ������, �� �� ������ ������
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

// ������ ������� ��������� �� ��� �������� �� ������������ ������ �����������
// (����� �� N ������� ���� ������������ �������� ����������)
int UEngine::Env_DownCurrentComponent(const char *stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // ���� ������ ������ - �����, �� ��������� ��� ��������� id
   if(stringid[0]>=0x30 && stringid[0]<=0x39)
   {
	ULongId longid;
	longid.DecodeFromString(stringid);
	Environment->DownCurrentComponent(longid);
   }
   else // ...����� ���������� ��� ��������� ���
   {
	Environment->DownCurrentComponent(stringid);
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

// ���������� ������� ��� �������� ����������
const char* UEngine::Env_GetCurrentComponentName(void) const
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   auto model = Environment->GetModel();
   if(model) {
    Environment->GetCurrentComponent()->GetLongName(model, TempString);
   }
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
 return TempString.c_str();
}

// ���������� ������� ��������� id �������� ����������
const char* UEngine::Env_GetCurrentComponentId(void) const
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   ULongId longid;
   Environment->GetCurrentComponent()->GetLongId(Environment->GetModel(),longid);
   longid.EncodeToString(TempString);
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
 return TempString.c_str();
}

// ���������� ��� �������� �������� ��� �������� ������
const char* UEngine::Env_GetCurrentDataDir(void) const
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Environment->GetCurrentDataDir();
   return TempString.c_str();
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
 DestroyTempString(TempString);
 return 0;
}

// ������������� ��� �������� �������� ��� �������� ������
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

// ���������� ��� �������� �������� ������
const char* UEngine::Env_GetSystemDir(void) const
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=Environment->GetSystemDir().c_str();
   return TempString.c_str();
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
 DestroyTempString(TempString);
 return 0;
}

// ������������� ��� �������� �������� ������
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

/// ���������� ��������� ����� ����������� ������ �����
bool UEngine::Env_GetDebugMode(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return false; // GetDebugMode удален - используется glog
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

/// ������������� ��������� ����� ����������� ������ �����
int UEngine::Env_SetDebugMode(bool value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // SetDebugMode удален - используется glog
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

/// ���������� ����� ��������� ������� ��� �����������
unsigned int UEngine::Env_GetDebugSysEventsMask(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0; // GetDebugSysEventsMask удален - используется glog
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

/// ������������� ����� ��������� ������� ��� �����������
int UEngine::Env_SetDebugSysEventsMask(unsigned int value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // SetDebugSysEventsMask удален - используется glog
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


/// ���������� ���� ��������� ������ ���� � ��������
bool UEngine::Env_GetDebuggerMessageFlag(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return false; // GetDebuggerMessageFlag удален - используется glog
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

/// ������������� ���� ��������� ������ ���� � ��������
int UEngine::Env_SetDebuggerMessageFlag(bool value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // SetDebuggerMessageFlag удален - используется glog
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

// ������ ����� ������ �����
void UEngine::Env_SetNumInputImages(int number)
{
 //GetEnvironment()->SetNumInputImages(number);
}

// ������ ����� ������� �����
void UEngine::Env_SetNumOutputImages(int number)
{
 //GetEnvironment()->SetNumOutputImages(number);
}

// ���������� ����� ������ �����
int UEngine::Env_GetNumInputImages(void)
{
 return 0;//return GetEnvironment()->GetNumInputImages();
}

// ���������� ����� ������� �����
int UEngine::Env_GetNumOutputImages(void)
{
 return 0;//return GetEnvironment()->GetNumOutputImages();
}

// ������ ���������� �� ��������� (������� ����������)
void UEngine::Env_SetInputRes(int number, int width, int height)
{
 //GetEnvironment()->SetInputImageRes(number,width,height);
}

// ������ ���� ��������� �������� ����������� ������ �������������� ���
void UEngine::Env_SetReflectionXFlag(bool value)
{
 //GetEnvironment()->SetReflectionXFlag(value);
}

// ���������� ���������� �� ��������� (������� ����������)
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

// ���������� ������� �������� ����������
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


/// ���������� ��������� � ���� � ������ ��������� ������
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


// ������ ���������� �������
// ----------------------------
// ������� ������
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

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
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

// ������� ������
int UEngine::Model_Clear(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   std::shared_ptr<UContainer> model=Environment->GetModel();

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

// ���������, ���������� �� ������
bool UEngine::Model_Check(void)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return (Environment->GetModel())?true:false;
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

// ���������, ���������� �� � ������ ��������� � ������ stringid)
bool UEngine::Model_CheckComponent(const char* stringid) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> destcont=FindComponent(stringid);
   if(destcont)
	return true;
  }
  catch (UContainer::EComponentNameNotExist &)
  {
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
 return false;
}

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'classname'
// ���� stringid - ������ ������, �� ��������� � ���� ������
// ���������� ��� ���������� � ������ ������
const char* UEngine::Model_AddComponent(const char* stringid, const char *classname)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> destcont=FindComponent(stringid);
   std::shared_ptr<UContainer> cont=Storage->TakeObject(classname);

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
	if(!cont->IsInit())
	 cont->Init(); // TODO: �������� ��� ���� ��������� ��������� ������������?
	else
     cont->Reset();
	TempString=cont->GetName();
   }
   else
	TempString.clear();
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
 return TempString.c_str();
}

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'name'
// ���� stringid - ������ ������, �� ������� �� ����� ������
int UEngine::Model_DelComponent(const char* stringid, const char *name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
        std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);

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

/// ���������� ���������� � ������ ���������
/// ���� comp �� ����������� ����� ����������, ��� target ����� �������� ��
/// ����� ���������� storage, ��� target �� ����� ������� � ���� ���������
/// �� ���������� false � �� ������ ������
int UEngine::Model_MoveComponent(const char* component, const char* target)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
  std::shared_ptr<RDK::UContainer> comp=FindComponent(component);
  std::shared_ptr<RDK::UContainer> target_comp=FindComponent(target);

  if(!comp)
   return RDK_E_MODEL_COMPONENT_NOT_FOUND;

  if(!target_comp)
   return RDK_E_MODEL_TARGET_COMPONENT_NOT_FOUND;

  if(!comp->GetOwner())
   return RDK_E_MODEL_COMPONENT_OWNER_NOT_FOUND;

  if(comp == target_comp)
   return RDK_E_MODEL_COMPONENTS_DONT_HAVE_TO_MATCH;

  if(!comp->GetOwner()->MoveComponent(comp, target_comp))
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


/// ��������� ��������� �� ����� ���������� � ����������� �������
/// ���� new_name - ������ ������, �� ��� ����������� �������������
int UEngine::Model_CloneComponent(const char* component_name, const char* new_name)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> component=dynamic_pointer_cast<RDK::UNet>(FindComponent(component_name));

   if(!component)
    return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   std::shared_ptr<UNet> owner=std::dynamic_pointer_cast<RDK::UNet>(component->GetOwner());
   if(!owner)
    return RDK_E_MODEL_COMPONENT_OWNER_NOT_FOUND;

   std::shared_ptr<UNet> new_component=std::dynamic_pointer_cast<RDK::UNet>(Storage->TakeObject(component->GetClass(),component));
   if(!new_component)
    return RDK_E_STORAGE_TAKE_OBJECT_FAIL;

   if(!new_name || strlen(new_name) == 0)
    new_component->Name = component->GetName();
   else
    new_component->Name = new_name;

   if(!owner->AddComponent(new_component))
   {
    Storage->ReturnObject(new_component);
    return RDK_E_MODEL_ADD_COMPONENT_FAIL;
   }

 //  component->Copy(new_component);
   RDK::MVector<double,3> coord=new_component->GetCoord();
   coord(0)+=1;
   coord(1)+=1;
   new_component->Coord = coord;

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

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
int UEngine::Model_GetNumComponents(const char* stringid)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
        std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);

   if(!destcont)
	return 0;

   return destcont->GetNumComponents();
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

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
int UEngine::Model_GetComponentsList(const char* stringid, int *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
        std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);

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
}

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ','
const char* UEngine::Model_GetComponentsNameList(const char* stringid)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString.clear();
        std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);

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

 DestroyTempString(TempString);
 return 0;
}

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ',' � ������� ��� ������ 'class_name'
// ���� find_all == true �� ����� ������� � �� ���� ��������������
const char* UEngine::Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString.clear();

   if(!class_name || !strlen(class_name))
	return TempString.c_str();

        std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);

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
 DestroyTempString(TempString);
 return 0;
}

// ���������� ��������� � ������� �������� index ��� ������ 'name' ����� ���
// ���� �� ������ �� �������� ����� ���������
// ����������� ��� ��������� ������� ������� ���������
// ���� �������� 'step' ������� �� ������� �������, �� ��������� ���������������
// �� ��� �������
int UEngine::Model_ChangeComponentPosition(const char* stringid, int step)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
        std::shared_ptr<RDK::UContainer> destcont=FindComponent(stringid);

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


// ���������� xml-������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
const char* UEngine::Model_GetConnectorsList(const char* stringid,
                          int sublevel, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   std::shared_ptr<UContainer> owner_level=FindComponent(owner_level_stringid);

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
 return TempString.c_str();
}

// ���������� xml-������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
const char* UEngine::Model_GetItemsList(const char* stringid,
                            int sublevel, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   std::shared_ptr<UContainer> owner_level=FindComponent(owner_level_stringid);

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
 return TempString.c_str();
}

// ���������� xml-������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� �������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
const char* UEngine::Model_GetNetsList(const char* stringid,
                            int sublevel, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   std::shared_ptr<UContainer> owner_level=FindComponent(owner_level_stringid);

   TempString="";
   if(!cont)
	return TempString.c_str();

   ULongIdVector buffer;
   cont->GetNetsList(buffer,sublevel,owner_level);

   XmlStorage.Create("Nets");
   XmlStorage<<buffer;

   XmlStorage.Save(TempString);
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
 return TempString.c_str();
}


// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
const char* UEngine::Model_GetComponentName(const char* stringid)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString="";
   std::shared_ptr<UContainer> destcont=FindComponent(stringid);

   if(!destcont)
	return TempString.c_str();

   TempString=destcont->GetName();
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
 return TempString.c_str();
}


// ���������� ������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
const char* UEngine::Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> destcont=FindComponent(stringid);
   std::shared_ptr<UContainer> owner_level=FindComponent(owner_level_stringid);

   if(!destcont)
	return TempString.c_str();

   return destcont->GetLongName(owner_level,TempString).c_str();
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
 return TempString.c_str();
}

// ���������� ������� id ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
const char* UEngine::Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> destcont=FindComponent(stringid);
   std::shared_ptr<UContainer> owner_level=FindComponent(owner_level_stringid);

   if(!destcont)
	return TempString.c_str();

   ULongId id;

   destcont->GetLongId(owner_level, id);
   id.EncodeToString(TempString);
   return TempString.c_str();
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
 return TempString.c_str();
}

// ���������� ��� ������ ���������� � ��������� �� �������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������ ������
const char* UEngine::Model_GetComponentClassName(const char* stringid)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString="";
   std::shared_ptr<UContainer> destcont=FindComponent(stringid);

   if(!destcont)
	return TempString.c_str();

   TempString=Storage->FindClassName(destcont->GetClass());
   return TempString.c_str();
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
 return TempString.c_str();
}

// ���������� ������ ������� ���������� ����������� ��������
const char* UEngine::Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> cont=FindComponent(stringid);

   if(!cont)
	return TempString.c_str();

   RDK::UContainer::VariableMapT props=cont->GetPropertiesList();

   RDK::UContainer::VariableMapCIteratorT I,J;

   I=props.begin();
   J=props.end();
   while(I != J)
   {
	if(I->second.CheckMask(type_mask))
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
 DestroyTempString(TempString);
 return 0;
}

const char* UEngine::Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> cont=FindComponent(stringid);

   if(!cont)
	return TempString.c_str();

   RDK::UContainer::VariableMapT props=cont->GetPropertiesList();

   RDK::UContainer::VariableMapCIteratorT I,J;

   I=props.begin();
   J=props.end();
   while(I != J)
   {
	if(I->second.CheckMask(type_mask))
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
 DestroyTempString(TempString);
 return 0;
}


// ���������� �������� ���������� �� ��������������
const char* UEngine::Model_GetComponentProperties(const char *stringid, unsigned int type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return TempString.c_str();

   XmlStorage.Create(cont->GetLongName(Environment->GetCurrentComponent(),CompName));
   XmlStorage.AddNode(UVariable::GetPropertyTypeNameByType(type_mask));

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
 DestroyTempString(TempString);
 return 0;
}

// ���������� ���������� �������� ���������� �� ��������������
const char* UEngine::Model_GetComponentSelectedProperties(const char *stringid)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   DestroyTempString(TempString);
   return 0;
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
 DestroyTempString(TempString);
 return 0;
}

// ���������� �������� ���������� �� �������������� � ����������
const char* UEngine::Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   if(!cont)
	return TempString.c_str();
   std::string comp_name;
   USerStorageXML xml;
   xml.Create(cont->GetLongName(Environment->GetCurrentComponent(),comp_name));
   xml.AddNode(UVariable::GetPropertyTypeNameByType(type_mask));

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
 DestroyTempString(TempString);
 return 0;
}

// ���������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
const char * UEngine::Model_GetComponentPropertyValue(const char *stringid, const char *paramname)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString="";
   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   if(!cont)
	return TempString.c_str();

   cont->GetPropertyValue(paramname,TempString);
   return TempString.c_str();
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
 DestroyTempString(TempString);
 return 0;
}

// ������������� �������� ���������� �� ��������������
int UEngine::Model_SetComponentProperties(const char *stringid, const char* buffer)
{
 RDK::USerStorageXML XmlStorage;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   XmlStorage.Load(buffer, cont->GetLongName(Environment->GetCurrentComponent(),CompName));
   for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
   {
	if(XmlStorage.SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
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

// ������������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
int UEngine::Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> cont=FindComponent(stringid);
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


// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
int UEngine::Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));
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

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
int UEngine::Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));
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

// ���������� ��������� void* �� ������ �������� ����������
const void* UEngine::Model_GetComponentPropertyData(const char *stringid, const char *property_name)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   std::shared_ptr<UIProperty> iproperty=cont->FindProperty(property_name);
   return iproperty->GetMemoryArea();
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

// �������� ������ 'data' � �������� �������� ����������
int UEngine::Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   std::shared_ptr<UIProperty> iproperty=cont->FindProperty(property_name);
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

   std::shared_ptr<UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   bool temp_res=model->CreateLink(stringid1,item_property_name,stringid2,connector_property_name, connector_c_index);
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

   std::shared_ptr<UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

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

// ��������� ��� �����
int UEngine::Model_BreakAllLinks(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

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
// ��������� ����� �� ����� connector_index ���������� 'connectorid'
int UEngine::Model_BreakConnectorLink(const char* connectorname, int connector_index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

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
}*/

// ��������� ��� ������� � �������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentLinks(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> model=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

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

// ��������� ��� ������� ����� ���������� ����������
int UEngine::Model_BreakAllComponentInputLinks(const char* stringid)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> model=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   model->DisconnectAllItems();
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

// ��������� ��� �������� ����� ���������� ����������
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

// ���������, ���������� �� ������� �����
bool UEngine::Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 return false; // TODO: ����� �������
 /*
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UItem> cont1;
   std::shared_ptr<UConnector> cont2;
   try
   {
    cont1=dynamic_pointer_cast<RDK::UItem>(FindComponent(stringid1));
	cont2=dynamic_pointer_cast<RDK::UConnector>(FindComponent(stringid2));
   }
   catch (UException &)// ��������!! ����� ������ ����������
   {
	return false;
   }
   if(!cont1 || !cont2)
	return false;

   return cont1->CheckLink(cont2,output_number,input_number);
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
 return false;*/
}

bool UEngine::Model_CheckLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UItem> cont1;
   std::shared_ptr<UConnector> cont2;
   try
   {
	cont1=dynamic_pointer_cast<RDK::UItem>(FindComponent(stringid1));
	cont2=dynamic_pointer_cast<RDK::UConnector>(FindComponent(stringid2));
   }
   catch (UException &)// ��������!! ����� ������ ����������
   {
	return false;
   }
   if(!cont1 || !cont2)
	return false;

   return cont1->CheckLink(cont2,item_property_name,connector_property_name, -1); // TODO: -1 �������� �� ��������
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

/// ����������� ��� ����� ������������ � ������ ���������� 1 �� ����� ���������� 2
int UEngine::Model_SwitchOutputLinks(const char* item_name1, const char* item_property_name1, const char* item_name2, const char* item_property_name2)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!item_name1)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(!item_name2)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   std::shared_ptr<UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

   if(!model)
	return RDK_E_MODEL_NOT_FOUND;

   bool temp_res=model->SwitchOutputLinks(item_name1,item_property_name1,item_name2,item_property_name2);
   if(!temp_res)
    return RDK_E_MODEL_SWITCH_LINK_FAIL;
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



// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char* UEngine::Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   std::shared_ptr<UNet> owner;
   if(owner_level_stringid)
	owner=dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

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
 return TempString.c_str();
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� � ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����������� ������� �������� ����������
int UEngine::Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   std::shared_ptr<UNet> owner;
   if(owner_level_stringid)
	owner=dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

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

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * UEngine::Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   std::shared_ptr<UNet> owner;
   if(owner_level_stringid)
	dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

   TempString="";
   if(!cont)
	return TempString.c_str();

   XmlStorage.Create("Links");

   if(!cont->GetComponentInputLinks(&XmlStorage,owner.get(),sublevel))
   {
	DestroyTempString(TempString);
	return 0;
   }

   TempString="";
   XmlStorage.Save(TempString);
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
 return TempString.c_str();
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * UEngine::Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   std::shared_ptr<UNet> owner;
   if(owner_level_stringid)
	dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

   TempString="";
   if(!cont)
	return TempString.c_str();

   XmlStorage.Create("Links");

   if(!cont->GetComponentOutputLinks(&XmlStorage,owner.get(), sublevel))
   {
	DestroyTempString(TempString);
	return 0;
   }

   TempString="";
   XmlStorage.Save(TempString);
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
 return TempString.c_str();
}

// ��������� ��� ������� ����� c ����������� cont � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char* UEngine::Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
   std::shared_ptr<UNet> owner;
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
 return TempString.c_str();
}

// ���������� ����� ������ � ����������
int UEngine::Model_GetComponentNumInputs(const char *stringid)
{
 return 0; // TODO: ����� �������
 /*
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

   if(!cont)
	return 0;

   return cont->GetNumInputs();
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
 return 0;*/
}

// ���������� ������ ����� ���������� � ����� ���������
int UEngine::Model_GetComponentInputDataSize(const char *stringid, int index)
{
     return 0; // TODO: ����� �������
     /*
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

   if(!cont)
	return 0;

   return cont->GetInputDataSize(index)[1]; // TODO: ����� ���������� ������ ������������
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
 return 0;*/
}

// ���������� ������ �������� ����� � ������
int UEngine::Model_GetComponentInputElementSize(const char *stringid, int index)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0;
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

// ���������� ������ ����� ���������� � ������ ���������
int UEngine::Model_GetComponentInputByteSize(const char *stringid, int index)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0;
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

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
unsigned char* UEngine::Model_GetComponentInputData(const char *stringid, int index)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0;
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

// ���������� ����� ������� � ����������
int UEngine::Model_GetComponentNumOutputs(const char *stringid)
{
 return 0; // TODO: ����� �������
 /*
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

   if(!cont)
	return 0;

   return cont->GetNumOutputs();
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
 return 0;*/
}

// ���������� ������ ������ ���������� � ����� ���������
int UEngine::Model_GetComponentOutputDataSize(const char *stringid, int index)
{
 return 0; // TODO: ����� �������
 /*
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

   if(!cont)
	return 0;

   return cont->GetOutputDataSize(index)[1]; // TODO: ��� ���� ���������� ����������� ������� ��������
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
 return 0;*/
}

// ���������� ������ ������ ���������� � ������ ���������
int UEngine::Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0;
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

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
unsigned char* UEngine::Model_GetComponentOutputData(const char *stringid, int index)
{
 return 0; // TODO: ����� �������
 /*
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   return cont->GetOutputData(index).UChar;
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
 return 0;*/
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
const char *  UEngine::Model_SaveComponent(const char *stringid, unsigned int params_type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

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
 return TempString.c_str();
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
int UEngine::Model_SaveComponentToFile(const char *stringid, const char* file_name, unsigned int params_type_mask)
{
 return 0;
}


// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
int UEngine::Model_LoadComponent(const char *stringid, const char* buffer)
{
 RDK::USerStorageXML XmlStorage;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   AccessCache.clear();
   if(!XmlStorage.Load(buffer,"Save"))
   {
	LOG(ERROR) << "UEngine::" << __FUNCTION__ << " - Failed to parse model file. Possible xml syntax error.";
	return RDK_E_MODEL_LOAD_COMPONENT_FAIL;
   }
   std::string xml_model_name=XmlStorage.GetNodeAttribute("ModelName");
   XmlStorage.SelectNode(0);

   Environment->Reset();

   if(!Environment->GetModel())
   {
	std::string name=XmlStorage.GetNodeAttribute("Class");
	UId id=Storage->FindClassId(name);
	std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(Environment->GetModel());
	if(!cont || cont->GetClass() != id)
	{
	 Model_Destroy();
	 Model_Create(name.c_str());
	 cont=dynamic_pointer_cast<RDK::UNet>(Environment->GetModel());
	}

	if(!cont->LoadComponent(&XmlStorage,true))
	 return RDK_E_MODEL_LOAD_COMPONENT_FAIL;
   }
   else
   {
	if((strlen(stringid) == 0) && (xml_model_name != Environment->GetModel()->GetName()))
	 RDK_RAW_THROW(EErrorEngineModelNameDontMatch(xml_model_name, Environment->GetModel()->GetName()));

	std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

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

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
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

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
const char * UEngine::Model_SaveComponentProperties(const char *stringid, unsigned int type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

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
 return TempString.c_str();
}

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
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

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
int UEngine::Model_LoadComponentProperties(const char *stringid, const char* buffer)
{
 RDK::USerStorageXML XmlStorage;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(!XmlStorage.Load(buffer,"SaveProperties"))
   {
	LOG(ERROR) << "UEngine::" << __FUNCTION__ << " - Failed to parse properties (parametes/state) file. Possible xml syntax error.";
	return RDK_E_MODEL_LOAD_COMPONENT_PROPERTIES_FAIL;
   }

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

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
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

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
const char* UEngine::Model_SaveComponentDrawInfo(const char *stringid)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

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
 return TempString.c_str();
}

// ��������� ����� ����� ������ �� ���������
unsigned int UEngine::Model_GetDefaultTimeStep(void) const
{
 return DefaultTimeStep;
}

int UEngine::Model_SetDefaultTimeStep(unsigned int value)
{
 DefaultTimeStep=value;
 return RDK_SUCCESS;
}

// ��������� ����� ����� ����������
unsigned int UEngine::Model_GetTimeStep(const char *stringid) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetTimeStep();
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

int UEngine::Model_SetTimeStep(const char *stringid, unsigned int value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   cont->TimeStep = value;
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

// ������������� ��� ����� ���������� � ���� ��� �������� ���������
int UEngine::Model_SetGlobalTimeStep(const char *stringid, unsigned int value)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

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

// ���������� ������� ����� ������
unsigned long long UEngine::Model_GetTime(void)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetTime();
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

double UEngine::Model_GetDoubleTime(void)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetDoubleTime();
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

// ������������� ������� ����� ������
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

// ���������� �������� �����
unsigned long long UEngine::Model_GetRealTime(void)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetRealTime();
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

double UEngine::Model_GetDoubleRealTime(void)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetDoubleRealTime();
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

// ������������� �������� �����
int UEngine::Model_SetRealTime(unsigned long long value)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!Environment->GetTime().SetRealTime(value))
	return RDK_E_ENV_SET_TIME_FAIL;
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

// ����������� �������� ����� �� �������� ��������
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

// ���������� ���������� ��� � �������� �������
unsigned long long UEngine::Model_GetRealTimeStep(void)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetRealTimeStep();
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

double UEngine::Model_GetDoubleRealTimeStep(void)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetDoubleRealTimeStep();
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
 return 0.0;
}

double UEngine::Model_GetDoubleSourceTime(void) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return Environment->GetTime().GetSourceCurrentGlobalTime();
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
 return 0.0;
}

// ������������� ����� ������� ���������� ������ � ����
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

// ���������� ����� ������� ���������� ��� ������� ������� �������� ��������� (��)
unsigned long long UEngine::Model_GetStepDuration(const char *stringid) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetStepDuration();
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

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
unsigned long long UEngine::Model_GetFullStepDuration(const char *stringid) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetFullStepDuration();
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


// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double UEngine::Model_GetInstantPerformance(const char *stringid) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetInstantPerformance();
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
 return 0.0;
}

// �����, ��������� ����� ����� ���������� ���������� �����
unsigned long long UEngine::Model_GetInterstepsInterval(const char *stringid) const
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   return cont->GetInterstepsInterval();
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
// --------------------------

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
// ������������ �������� ����� ����������� ��� RDK::MDMatrix*
// ���� ����� �� �������� ������ ������ ����, �� ���������� 0
const /* RDK::MDMatrix* */void* UEngine::Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   // ���� ��������� �� �������� ������
   std::shared_ptr<UIPropertyOutput> output_property=dynamic_pointer_cast<UIPropertyOutput>(cont->FindProperty(property_name));
   if(!output_property)
	return 0;

   if(dynamic_pointer_cast<UVBaseDataProperty<MDMatrix<double>>>(output_property))
    return &dynamic_pointer_cast<UVBaseDataProperty<MDMatrix<double>>>(output_property)->GetData();

   if(dynamic_pointer_cast<UVBaseDataProperty<MDVector<double>>>(output_property))
       return &dynamic_pointer_cast<UVBaseDataProperty<MDVector<double>>>(output_property)->GetData();

   return 0;
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

const /* RDK::MDMatrix* */void* UEngine::Model_GetComponentOutputAsMatrix(const char *stringid, int index)
{
 return 0; // TODO: ����� �������
 /*
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   // ���� ��������� �� �������� ������
   UIProperty* output_property=0;
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
 return 0;*/
}

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const RDK::UBitmap* UEngine::Model_GetComponentOutput(const char *stringid, const char *property_name)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!property_name)
	return 0;

   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   std::shared_ptr<UIProperty> iproperty=cont->FindProperty(property_name);
   std::shared_ptr<UVBaseDataProperty<UBitmap> > property=dynamic_pointer_cast<UVBaseDataProperty<UBitmap> >(iproperty);
   if(!property)
	return 0;
   return (const RDK::UBitmap*)iproperty->GetMemoryArea();
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

const RDK::UBitmap* UEngine::Model_GetComponentOutput(const char *stringid, int index)
{
    return 0; // TODO: ����� �������
/*
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   if(index<0 || index >= cont->GetNumOutputs())
	return 0;

   UIProperty* iproperty=0;
   cont->FindOutputProperty(index, iproperty);
   std::shared_ptr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
   if(!property)
	return 0;
   return (const RDK::UBitmap*)property->GetMemoryArea();
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
 return 0;*/
}

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const RDK::UBitmap* UEngine::Model_GetComponentBitmapOutput(const char *stringid, const char *property_name)
{
 return Model_GetComponentOutput(stringid, property_name);
}

const RDK::UBitmap* UEngine::Model_GetComponentBitmapOutput(const char *stringid, int index)
{
 return 0; // TODO: ����� �������
 /*
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   if(index<0 || index >= cont->GetNumOutputs())
	return 0;

   UIProperty* iproperty=0;
   cont->FindOutputProperty(index, iproperty);
   std::shared_ptr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
   if(!property)
	return 0;
   return (const RDK::UBitmap*)property->GetMemoryArea();
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
 return 0;*/
}

/// �������� ������ � ���������� ����������� ������ � �������� 'index' ���������� 'id'
/// � �������� bmp_param
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
}

/// �������� ����������� ������ � �������� 'index' ���������� 'id'
/// ����� ������������, ��� bmp ��� ����� ���������� ������ ��� ���������� ���������� �������
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



// ���������� ��������� �� ���� � �������� 'index' ���������� 'id'
const RDK::UBitmap* UEngine::Model_GetComponentBitmapInput(const char *stringid, const char *property_name)
{
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!property_name)
	return 0;

   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   std::shared_ptr<UIProperty> iproperty=cont->FindProperty(property_name);
   std::shared_ptr<UVBaseDataProperty<UBitmap> > property=dynamic_pointer_cast<UVBaseDataProperty<UBitmap> >(iproperty);
   if(!property)
	return 0;
   return (const RDK::UBitmap*)property->GetMemoryArea();
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

const RDK::UBitmap* UEngine::Model_GetComponentBitmapInput(const char *stringid, int index)
{
    return 0; // TODO: ����� �������
/*
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return 0;

   if(index<0 || index >= cont->GetNumInputs())
	return 0;

   UIProperty* iproperty=0;
   cont->FindInputProperty(index, iproperty);
   std::shared_ptr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
   if(!property)
	return 0;
   return (const RDK::UBitmap*)property->GetMemoryArea();
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
 return 0;*/
}

// �������� ����������� ������ � �������� 'index' ���������� 'id'
int UEngine::Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const RDK::UBitmap* bmp, bool reflect)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!bmp || !property_name)
	return RDK_E_NULL_POINTER_IN_ARGUMENT;
   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   std::shared_ptr<UIProperty> iproperty=cont->FindProperty(property_name);
   std::shared_ptr<UVBaseDataProperty<UBitmap> > property=dynamic_pointer_cast<UVBaseDataProperty<UBitmap> >(iproperty);
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
}

int UEngine::Model_SetComponentBitmapOutput(const char *stringid, int index, const RDK::UBitmap* bmp, bool reflect)
{
 return 0; // TODO: ����� �������
 /*
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(index<0 || index >= cont->GetNumOutputs())
	return RDK_E_MODEL_PROPERTY_INDEX_OUT_OF_RANGE;

   UIProperty* iproperty=0;
   cont->FindOutputProperty(index, iproperty);
   std::shared_ptr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
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
 return res;*/
}

// �������� ����������� ����� � �������� 'index' ���������� 'id'
int UEngine::Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const RDK::UBitmap* const bmp, bool reflect)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!bmp || !property_name)
	return RDK_E_NULL_POINTER_IN_ARGUMENT;
   std::shared_ptr<UContainer> cont=FindComponent(stringid);
   std::shared_ptr<UIProperty> iproperty=cont->FindProperty(property_name);
   std::shared_ptr<UVBaseDataProperty<UBitmap> > property=dynamic_pointer_cast<UVBaseDataProperty<UBitmap> >(iproperty);
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
}

int UEngine::Model_SetComponentBitmapInput(const char *stringid, int index, const RDK::UBitmap* const bmp, bool reflect)
{
    return 0; // TODO: ����� �������
/*
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   std::shared_ptr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return RDK_E_MODEL_COMPONENT_NOT_FOUND;

   if(index<0 || index >= cont->GetNumInputs())
	return RDK_E_MODEL_PROPERTY_INDEX_OUT_OF_RANGE;


   UIProperty* iproperty=0;
   cont->FindInputProperty(index, iproperty);
   std::shared_ptr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
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
 return res;*/
}

// --------------------------
// ������ ���������� ������������
// --------------------------
// ���������� ������ ����� ����
const char* UEngine::GetLog(int &error_level) const
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=""; // GetLog удален - используется glog
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
 return TempString.c_str();
}

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* UEngine::GetUnreadLog(int &error_level, int &number, time_t &time)
{
 std::string& TempString=CreateTempString();
 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   TempString=""; // GetUnreadLog удален - используется glog
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
 return TempString.c_str();
}

/// ���������� � ��� ����� ���������
int UEngine::Engine_LogMessage(int log_level, const char *message, int error_event_number)
{
 // Logger удален - используется glog

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // LogMessage заменен на glog
   switch(log_level) {
     case RDK_EX_FATAL: LOG(FATAL) << message; break;
     case RDK_EX_ERROR: LOG(ERROR) << message; break;
     case RDK_EX_WARNING: LOG(WARNING) << message; break;
     case RDK_EX_INFO: LOG(INFO) << message; break;
     case RDK_EX_DEBUG: VLOG(1) << message; break;
     default: LOG(INFO) << message; break;
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

int UEngine::Engine_LogMessage(int msg_level, const char *method_name, const char *message, int error_event_number)
{
 // Logger удален - используется glog

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // LogMessage заменен на glog
   switch(msg_level) {
     case RDK_EX_FATAL: LOG(FATAL) << method_name << " - " << message; break;
     case RDK_EX_ERROR: LOG(ERROR) << method_name << " - " << message; break;
     case RDK_EX_WARNING: LOG(WARNING) << method_name << " - " << message; break;
     case RDK_EX_INFO: LOG(INFO) << method_name << " - " << message; break;
     case RDK_EX_DEBUG: VLOG(1) << method_name << " - " << message; break;
     default: LOG(INFO) << method_name << " - " << message; break;
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

int UEngine::Engine_LogMessageEx(int msg_level, const char *object_name, const char *message, int error_event_number)
{
 // Logger удален - используется glog

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // LogMessageEx заменен на glog
   switch(msg_level) {
     case RDK_EX_FATAL: LOG(FATAL) << object_name << " - " << message; break;
     case RDK_EX_ERROR: LOG(ERROR) << object_name << " - " << message; break;
     case RDK_EX_WARNING: LOG(WARNING) << object_name << " - " << message; break;
     case RDK_EX_INFO: LOG(INFO) << object_name << " - " << message; break;
     case RDK_EX_DEBUG: VLOG(1) << object_name << " - " << message; break;
     default: LOG(INFO) << object_name << " - " << message; break;
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

int UEngine::Engine_LogMessageEx(int msg_level, const char *object_name, const char *method_name, const char *message, int error_event_number)
{
 // Logger удален - используется glog

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // LogMessageEx заменен на glog
   switch(msg_level) {
     case RDK_EX_FATAL: LOG(FATAL) << object_name << "::" << method_name << " - " << message; break;
     case RDK_EX_ERROR: LOG(ERROR) << object_name << "::" << method_name << " - " << message; break;
     case RDK_EX_WARNING: LOG(WARNING) << object_name << "::" << method_name << " - " << message; break;
     case RDK_EX_INFO: LOG(INFO) << object_name << "::" << method_name << " - " << message; break;
     case RDK_EX_DEBUG: VLOG(1) << object_name << "::" << method_name << " - " << message; break;
     default: LOG(INFO) << object_name << "::" << method_name << " - " << message; break;
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

// ���������� ��������-������������ ����������
// GetExceptionHandler удален - используется glog

// SetExceptionHandler удален - используется glog

// ������������ ����� �������� ����������
// ���� 0, �� �������������
/*int UEngine::GetMaxExceptionsLogSize(void) const
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // GetMaxExceptionsLogSize удален - используется glog
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

int UEngine::SetMaxExceptionsLogSize(int value)
{
 // Logger удален - используется glog

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // SetMaxExceptionsLogSize удален - используется glog
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


/// ���������� ����� ������������� ����� ����
int UEngine::GetNumUnreadLogLines(void) const
{
 // Logger удален - используется glog

 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0; // GetNumUnreadLogLines удален - используется glog
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

/// ���������� ����� ����� ����
int UEngine::GetNumLogLines(void) const
{
 // Logger удален - используется glog

 //int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   return 0; // GetNumLogLines удален - используется glog
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

/// ������� ��� ����������� ���������
int UEngine::ClearReadLog(void)
{
 // Logger удален - используется glog

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // ClearReadLog удален - используется glog
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
// ������ ����������� ���������� �������
// --------------------------
// ������������ ��������� ����������
/// ���������� RDK_UNHANDLED_EXCEPTION ���� �� ������� �������� ������ ����������
/// ����� ���������� RDK_EXCEPTION_CATCHED
int UEngine::ProcessException(UException &exception) const
{
 LOG(ERROR) << "Unhandled exception: " << exception.what();
 return RDK_UNHANDLED_EXCEPTION;

 return RDK_EXCEPTION_CATCHED;
 //return ProcessException((const UException &)(exception));
}

int UEngine::ProcessException(const UException &exception) const
{
 LOG(ERROR) << "Unhandled exception: " << exception.what();
 return RDK_UNHANDLED_EXCEPTION;

 return RDK_EXCEPTION_CATCHED;
}

// ������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
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
		// external_classes is list<UContainer*> - raw pointers
		// These objects should be managed via shared_ptr elsewhere, but here they're passed as raw pointers
		// ARCHITECTURAL ISSUE: This should be changed to list<std::shared_ptr<UContainer>>
		// For now, use safe_shared_cast with non-owning deleter as temporary solution
		// TODO: Refactor to use shared_ptr throughout the codebase
		std::shared_ptr<UComponent> cont = safe_shared_cast<UComponent>(*I);
		// cont->SetLogger удален - используется glog
		cont->SetStorage(Storage);
		cont->Build();
		std::shared_ptr<UVirtualMethodFactory> factory = std::make_shared<UVirtualMethodFactory>(std::dynamic_pointer_cast<UContainer>(cont));
		Storage->AddClass(factory);
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
	 // external_libs is list<ULibrary*> - raw pointers
	 // ARCHITECTURAL ISSUE: This should be changed to list<std::shared_ptr<ULibrary>>
	 // For now, create shared_ptr with non-owning deleter as temporary solution
	 // TODO: Refactor to use shared_ptr throughout the codebase
	 std::shared_ptr<ULibrary> lib(*I, [](ULibrary*){}); // Non-owning deleter - library is managed elsewhere
	 Storage->AddCollection(lib);
	 ++I;
	}
   }

   LOG(INFO) << "Build storage has been started...";
   Storage->InitRTlibs();

   Storage->BuildStorage();
   Storage->LoadClassesDescription();
   /*
   Storage->CreateMockLibs();
   Storage->SaveMockLibs();
   */

   LOG(INFO) << "Build storage has been finished";
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

// ��������� ����� ����������������� ���������
int UEngine::LoadPredefinedLibraries(void)
{
 RdkLoadPredefinedLibraries(LibrariesList);

 return 0;
}

//��������� ������� ������������ ������� ��� ��������� ����������� �� ��������� � ���������
int UEngine::LoadPredefinedCrPropFunctions(void)
{
 if(!Storage)
     return 0;

 // ���������� �������� ������ ����������-�������
 Storage->AddCrPropMockFunc(UBasePropCreator::BaseCrPropMock);
 Storage->AddCrPropMockFunc(UBasePropCreatorStd::BaseCrPropMock);
 Storage->AddCrPropMockFunc(UBasePropCreatorVector::BaseCrPropMock);
 Storage->AddCrPropMockFunc(UBasePropCreatorMatrix::BaseCrPropMock);

 RdkLoadPredefinedCrPropFunctions(Storage);
 return 0;
}

// ��������� ����� �������
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

// ��������� ����� ��������� � ��������� ��������� �� ��� � ������ external_libs
// ���������� 0 � ������ ������
// ������ ���� ��������� ������� �� ini-����� 'inifile', ������ 'section'
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
// ������ ����������� ���������� ��������
// --------------------------
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ������������ ����� ���������� �� �������� ���������� id
// ���� ��������� id �� ������, �� ���������� ��������� �� ������
std::shared_ptr<UContainer> UEngine::FindComponent(const char *stringid) const
{
 std::shared_ptr<UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

 if(!model)
  return 0;

 RDK::ULongId longid;

 std::shared_ptr<UContainer> cont;
 if(!stringid || *stringid == '\0')
  cont=model;
 else
 {
  // ���� ������ ������ - �����, �� ��������� ��� ��������� id
  if(stringid[0]>=0x30 && stringid[0]<=0x39)
  {
   longid.DecodeFromString(stringid);
   if(!longid.GetSize() || longid[0] == ForbiddenId)
    cont=model;
   else
    cont=model->GetComponentL(longid);
  }
  else // ...����� ���������� ��� ���
  {
   cont=model->GetComponentL(stringid);
  }

 }

// if(stringid)
//  AccessCache[stringid]=cont;
 return cont;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool UEngine::ADefault(void)
{
 // ��� ����� �������������
 OptionsFileName="options.ini";

 // ��� ������ ������ ��������� ���������
 ComponentLibrariesSectionName="ComponentLibraries";

 ComponentClassesSectionName="ComponentClasses";

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UEngine::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool UEngine::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool UEngine::ACalculate(void)
{
 return true;
}
// --------------------------

}
#endif
