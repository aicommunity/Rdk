
//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include "rdk.h"
#include "rdkdll_loader.h"
#include "rdk_genginedll.h"
#pragma hdrstop
//---------------------------------------------------------------------------
//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you
//---------------------------------------------------------------------------
//extern RDK::UBEngine* PEngine;

//extern RDK::UAEnvironment* PEnvironment;

//extern RDK::UAStorage* PStorage;

extern int Engine_Create(RDK::UEngine *engine, const char *inifilename, void *pCreateNewStorage, void *pCreateNewEnvironment);


RDK::UBAStorage* CreateNewStorage(void)
{
 return new RDK::UBAStorage;
}

RDK::UBAEnvironment* CreateNewEnvironment(void)
{
 return new RDK::UBAEnvironment;
}

RDK::UBEngine* CreateNewEngine(void)
{
 return new RDK::UBEngine;
}

int LoadEngine(void)
{
 if(RDK::LoadDll("rdk.dll"))
  return -1;

 if(!RDK::DLLDllInit(CreateNewStorage, CreateNewEnvironment, CreateNewEngine))
  return -2;


 return 0;
}

#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
 if(LoadEngine())
  return -2;

 RDK::UBEngine* pengine=dynamic_cast<RDK::UBEngine*>(RDK::DLLAddNewEngine());

 if(!pengine)
  return -3;

 if(Engine_Create(pengine, "options.ini",CreateNewStorage,CreateNewEnvironment))
  return -4;

 return 1;
}
//---------------------------------------------------------------------------
