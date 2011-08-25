/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, Sergey Vishnevskiy, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UQUEUE_H
#define UQUEUE_H

#include <vector>
#include <list>
#include <string>

namespace RDK {

using namespace std;

template<typename Ty>
class UQueue
{
private:
// ��������� ������������ ������ �������
enum { INIT_SIZE = 1024 };

protected: // ������
// ������ �������
vector<Ty> m_pData;

// ����� ������ � ������
unsigned int Size;

// ����������� ��������� ������ �������
unsigned int MaxSize;

// ������ ������� ��������
unsigned int Front;

// ������ ���������� ��������
unsigned int Back;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
// ������� ������ �������
UQueue(void)
{
 Size = 0;
 MaxSize = 0;
};

// ������� ������� � �������������� ������������ ��������
UQueue(size_t size)
{
 MaxSize = size;
 Size = 0;
 m_pData.resize(size);
};

// ������� ������� ����������� ������� �� ������� vec
UQueue(const std::vector<Ty>& vec)
{
 Size = MaxSize = vec.size();
 m_pData=vec;
 Front = 0;
 Back = Size-1;
};

~UQueue(void)
{
};
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������ �������
Ty&    front(void)
{
 return    m_pData[Front];
};

// ���������� ��������� �������
Ty&    back(void)
{
 return    m_pData[Back];
};

// �������� ������� � ��������� �� �������
// ������ ��������� �� ������ �������
Ty& operator [] (int i)
{
 if(Front+i<MaxSize)
  return m_pData[Front+i];
 else
  return m_pData[Front+i-MaxSize];
}

// ���������� true ���� ������� �����
bool empty(void)
{
 return (size() == 0);
};

// ���������� ����� ��������� ������ � �������
size_t size(void)
{
 return    Size;
};
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ��������� ������� � �����
void push(const Ty& obj)
{
 if(empty())
 {
  // ���� �� ������� ���������
  if(!MaxSize)
  {
   m_pData.resize(INIT_SIZE);
   MaxSize = INIT_SIZE;
  }
  Front = Back = 0;
 }
 else
 {
  // ���� ����� �� ����� ����� ������
  if(++Back == MaxSize)
  {
   // ���� ��� ������ ������������
   if(Size == MaxSize)
   {
    // �������� ����� ����, ������ ���
    // �������� �������� �� ��������
    m_pData.resize(m_pData.size()+MaxSize);
    Front = 0;
    Back = MaxSize;
    MaxSize *= 2;
   }
   else
    // ������ ��� ���� � ������
    Back = 0;
  }
  else
  if(Back == Front)
  {
   // ���� ����� �� ��� ������ ���������
   // � ������� �� ��� �����
   vector<Ty> temp;
   temp.resize(temp.size()+MaxSize);
   for(size_t i=Front,j=0;i<MaxSize;i++,j++)
    temp[j]=m_pData[i];
   for(size_t i=0,j=Front+1;i<Front;i++,j++)
    temp[j]=m_pData[i];

   m_pData=temp;

   Front = 0;
   Back = MaxSize;
   MaxSize *= 2;
  }
 }

 ++Size;
 m_pData[Back] = obj;
};

// ��������� ������� �� ������
void pop(void)
{
 if(++Front == MaxSize)
  Front = 0;
 --Size;
};

// ������� �������
void clear(void)
{
 Size = 0;
};


// ����������� ����� ��� size_ ���������
void reserve(size_t size_)
{
 if(size() < size_)
 {
  MaxSize = size_;
  m_pData.resize(MaxSize);
 }
};

// ��������� ������� ������� �� ������� vec.
// 0-� ������� ������� ���������� ������� �������
void FromVec(const std::vector<Ty>& vec)
{
 Size = MaxSize = vec.size();
 m_pData=vec;
 Front = 0;
 Back = Size-1;
};
// --------------------------
};

}
#endif
