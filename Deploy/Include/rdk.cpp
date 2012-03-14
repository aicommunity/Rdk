#ifndef RDK_CPP
#define RDK_CPP

#include "rdk.h"

#include "myrdk.cpp"
#include "../../Core/Engine/Engine.cpp"
#include "../../Core/Graphics/Graphics.cpp"

namespace RDK {

// ������� ������ ��������� � ���������� ��������� �� ����
UAContainerStorage* NewUStorage(void)
{
 return new UAContainerStorage;
}

// ������� ����� � ���������� ��������� �� ���
UAContainerEnvironment* NewUAContainerEnvironment(void)
{
 return new UAContainerEnvironment;
}

// ������� ������ � ���������� ��������� �� ����
UEngine* NewUEngine(void)
{
 return new UEngine;
}

// ������� ������ ��������� � ���������� ��������� �� ����
UAContainerStorage* CreateStorage(UAContainerStorage *storage)
{
 return storage;
}

// ������� ����� � ���������� ��������� �� ���.
// ���� ������ ��������� 'storage', �� ��������� ��� �� ������.
// ���� ���� 'isinit' == true, �� �������������� ��������� ������������ ������������
// ���� ��������� �� ������ external_libs != 0, ������������� �������������� ��������� ����� �������������
UAContainerEnvironment* CreateEnvironment(UAContainerEnvironment *environment, UAContainerStorage *storage, bool isinit, list<UAContainer*>* external_classes, list<UALibrary*>* external_libs)
{
 UAContainerEnvironment *env=environment;

 if(!storage)
  return env;

 if(!env->SetStorage(storage) || !isinit)
  return env;

 if(external_classes != 0)
 {
  list<UAContainer*>::iterator I,J;
  I=external_classes->begin();
  J=external_classes->end();
  while(I != J)
  {
   env->AddClass(*I);
   ++I;
  }
 }

 env->AddClassLibrary(&BCLLibrary);
 env->AddClassLibrary(&IOLibrary);

 if(external_libs != 0)
 {
  list<UALibrary*>::iterator I,J;
  I=external_libs->begin();
  J=external_libs->end();
  while(I != J)
  {
   env->AddClassLibrary(*I);
   ++I;
  }
 }

 env->BuildStorage();

 return env;
}

}
#endif





