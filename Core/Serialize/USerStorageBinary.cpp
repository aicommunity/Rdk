/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef USER_STORAGE_BINARY_CPP
#define USER_STORAGE_BINARY_CPP

#include <string.h>
#include <fstream>
#include <iostream>
#include "USerStorageBinary.h"

namespace RDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
// ������� ������ �������
USerStorageBinary::USerStorageBinary(void)
{
 Size = 0;
 MaxSize = 0;
 m_pData=0;
 Front=0;
 Back=0;
};

// ������� ������� � �������������� ������������ ��������
USerStorageBinary::USerStorageBinary(int size)
{
 MaxSize = size;
 Size = 0;
 m_pData=0;
 InternalResize(size);
};

// ������� ������� ����������� ������� �� ������� vec
USerStorageBinary::USerStorageBinary(const unsigned char* vec, int nsize)
{
 Size = MaxSize = 0;
 m_pData=0;
 InternalResize(nsize);
 memcpy(m_pData,vec,nsize*sizeof(unsigned char));
 Front = 0;
 Back = Size-1;
};

USerStorageBinary::~USerStorageBinary(void)
{
 if(m_pData)
 {
  delete []m_pData;
  m_pData=0;
 }
};
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������ �������
unsigned char&    USerStorageBinary::front(void)
{
 return    m_pData[Front];
};

// ���������� ��������� �������
unsigned char&    USerStorageBinary::back(void)
{
 return    m_pData[Back];
};

// �������� ������� � ��������� �� �������
// ������ ��������� �� ������ �������
unsigned char& USerStorageBinary::operator [] (int i)
{
 if(Front+i<MaxSize)
  return m_pData[Front+i];
 else
  return m_pData[Front+i-MaxSize];
}

// ���������� true ���� ������� �����
bool USerStorageBinary::empty(void)
{
 return (size() == 0);
};

// ���������� ����� ��������� ������ � �������
int USerStorageBinary::size(void)
{
 return    Size;
};
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ��������� ������� � �����
void USerStorageBinary::push(const unsigned char& obj)
{
 if(empty())
 {
  // ���� �� ������� ���������
  if(!MaxSize)
  {
   InternalResize(INIT_SIZE);
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
    // �������� �������� ����������
    InternalResize(Size+MaxSize);
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
   unsigned char* temp=new unsigned char[MaxSize];
   for(int i=Front,j=0;i<MaxSize;i++,j++)
    temp[j]=m_pData[i];
   for(int i=0,j=Front+1;i<Front;i++,j++)
    temp[j]=m_pData[i];

   delete []m_pData;
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
void USerStorageBinary::pop(void)
{
 if(++Front == MaxSize)
  Front = 0;
 --Size;
};

// ������� �������
void USerStorageBinary::clear(void)
{
 Size = 0;
};


// ����������� ����� ��� size_ ���������
void USerStorageBinary::reserve(int size_)
{
 if(size() < size_)
 {
  MaxSize = size_;
  InternalResize(MaxSize);
 }
};

// ��������� ������� ������� �� ������� vec.
// 0-� ������� ������� ���������� ������� �������
void USerStorageBinary::FromVec(const unsigned char* vec, int nsize)
{
 InternalResize(nsize);
 memcpy(m_pData,vec,nsize*sizeof(unsigned char));
 Front = 0;
 Back = Size-1;
};
// --------------------------


// --------------------------
// ������� ������ ���������� �������
// --------------------------
// ����� ��������������� �������
void USerStorageBinary::InternalResize(int size)
{
 unsigned char *temp=new unsigned char[size];

 int minsize=(size<MaxSize)?size:MaxSize;
 if(MaxSize>0)
  memcpy(temp,m_pData,sizeof(unsigned char)*minsize);
 delete []m_pData;
 m_pData=temp;
 MaxSize=size;
}

// Modern file operations
bool USerStorageBinary::SaveToFile(const std::string& filepath)
{
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Write size first
        file.write(reinterpret_cast<const char*>(&Size), sizeof(Size));
        
        // Write data
        if (Size > 0 && m_pData) {
            file.write(reinterpret_cast<const char*>(m_pData), Size);
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool USerStorageBinary::LoadFromFile(const std::string& filepath)
{
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Read size first
        int fileSize;
        file.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
        if (file.fail()) {
            return false;
        }
        
        // Resize buffer
        if (fileSize > 0) {
            InternalResize(fileSize);
            file.read(reinterpret_cast<char*>(m_pData), fileSize);
            if (file.fail()) {
                return false;
            }
            Size = fileSize;
            Front = 0;
            Back = Size - 1;
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}
// --------------------------

}
#endif

