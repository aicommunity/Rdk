#include <vcl.h>
#include <windows.h>

#pragma hdrstop
#pragma argsused

#include <tchar.h>

#include <stdio.h>
#include <conio.h>
#include "../../Deploy/Include/rdk_cpp_initdll.h"

std::fstream outfile;
UGenericMutex* mutex;

using namespace RDK;

class Locker
{
public:
Write(const std::string &name, const std::string &str)
{
  outfile<<name<<str<<std::endl;
};
};

Locker LockerModel;

UELockPtr<Locker> GetLockerModel(void)
{
 UEPtr<Locker> ptr=&LockerModel;

 return UELockPtr<Locker>(mutex,ptr);
}


class ThreadMutex: public TThread
{

 std::string Name;
public:
ThreadMutex(const std::string &name, bool create_suspended)
 : Name(name), TThread(create_suspended)
{

}

virtual __fastcall ~ThreadMutex(void)
{
}


void __fastcall Execute(void)
{
 while(!Terminated)
 {
 /*
//  Sleep(1);
  UGenericMutexExclusiveLocker lock1(mutex);
  UELockPtr<Locker> locker(mutex);
  outfile<<Name<<" pass 1"<<std::endl;
  {
   UGenericMutexExclusiveLocker lock2(mutex);
   outfile<<Name<<" pass 2"<<std::endl;
  }
  //Sleep(1);
  */
  UELockPtr<Locker> model1=GetLockerModel();
  model1->Write(Name," pass 1");
  {
   GetLockerModel()->Write(Name," pass 2");
  }
 }
}
};


int _tmain(int argc, _TCHAR* argv[])
{
 mutex=UCreateMutex();
 outfile.open("log.txt", ios::out | ios::trunc);
 ThreadMutex *thread1=new ThreadMutex("Thread1",false);
 ThreadMutex *thread2=new ThreadMutex("Thread2",false);
 ThreadMutex *thread3=new ThreadMutex("Thread3",false);

 getch();
 UDestroyMutex(mutex);
	return 0;
}
