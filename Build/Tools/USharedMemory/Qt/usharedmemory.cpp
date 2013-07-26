#include "usharedmemory.h"
#include <vector>
#include <qsharedmemory.h>

int NumPipes=0;

std::vector<QSharedMemory*> MemoryPage;

/// Возвращает число доступных каналов обмена
int CALLING_CONVERSION GetNumPipes(void)
{
 return NumPipes;
}

/// Задает число каналов обмена данными
/// Эта функция не должна вызываться во время обмена!
void CALLING_CONVERSION SetNumPipes(int value)
{
 if(NumPipes == value)
  return;

 if(value<0)
  return;

 for(int i=value;i<NumPipes;i++)
 {
  if(MemoryPage[i])
  {
   MemoryPage[i]->detach();
   delete MemoryPage[i];
   MemoryPage[i]=0;
  }
 }
 MemoryPage.resize(value);

 for(int i=NumPipes;i<value;i++)
 {
  MemoryPage[i]=new QSharedMemory;
 }
 NumPipes=value;
}

/// Задает размер памяти канала 'pipe_index'
/// Также выбирает способ подключения 'mode', master (1) или slave (0)
/// (Ответственность за распределение ролей лежит на программисте)
/// Параметр 'pipe_uid' определяет уникальный строковой идентификатор общей области памяти для этого канала.
/// (Ответственность за уникальность идентификатора лежитна программисте)
/// Эта функция не должна вызываться во время обмена по выбранному каналу!
int CALLING_CONVERSION InitPipe(int pipe_index, int &pipe_byte_size, int mode, const char *pipe_uid)
{
 if(pipe_index<0 || pipe_index>NumPipes)
  return -1;

 if(mode != 0 && mode != 1)
  return -2;

 if(!MemoryPage[pipe_index])
  return -3;

 MemoryPage[pipe_index]->setKey(pipe_uid);
 if(mode == 1)
 {
  if(pipe_byte_size<=0)
   return -4;
  if(!MemoryPage[pipe_index]->create(pipe_byte_size))
   return -5;
 }
 else
 if(mode == 0)
 {
  if(!MemoryPage[pipe_index]->attach())
   return -6;
  pipe_byte_size=MemoryPage[pipe_index]->size();
 }

 return 0;
}

/// Отключается от канала
int CALLING_CONVERSION UnInitPipe(int pipe_index)
{
 if(pipe_index<0 || pipe_index>NumPipes)
  return -1;

 if(!MemoryPage[pipe_index])
  return -3;

 if(!MemoryPage[pipe_index]->detach())
  return -7;

 return 0;
}

/// Проверяет, инициализирован ли канал
/// Sic! Возвращает 0 в случае, если канали инициализирован
/// или сообщение об ошибке < 0.
int CALLING_CONVERSION IsPipeInit(int pipe_index)
{
 if(pipe_index<0 || pipe_index>NumPipes)
  return -1;

 if(!MemoryPage[pipe_index])
  return -3;

 if(!MemoryPage[pipe_index]->isAttached())
  return -15;

 return 0;
}


/// Возвращает размер общей памяти канала
int CALLING_CONVERSION GetPipeSize(int pipe_index)
{
 if(pipe_index<0 || pipe_index>NumPipes)
  return -1;

 if(!MemoryPage[pipe_index])
  return -3;

 return MemoryPage[pipe_index]->size();
}

/// Записывает данные в общую память, копируя их из буфера 'buffer'
/// Возвращает число записанных байт
int CALLING_CONVERSION WriteData(int pipe_index, const char* buffer, int buffer_byte_size)
{
 if(pipe_index<0 || pipe_index>NumPipes)
  return -1;

 QSharedMemory* p=MemoryPage[pipe_index];

 if(!p)
  return -3;

 if(!p->isAttached())
  return -10;

 int size=0;
 if(p->lock())
 {
  size=(p->size()<buffer_byte_size)?p->size():buffer_byte_size;
  memcpy(p->data(),buffer,size);
  p->unlock();
 }
 else
  return -4;

 return size;
}

/// Читает данные из общей памяти, копируя их в буфер 'buffer', максимум buffer_byte_size
/// Память должна быть выделена
/// Возвращает число прочитанных байт
int CALLING_CONVERSION ReadData(int pipe_index, char* buffer, int buffer_byte_size)
{
 if(pipe_index<0 || pipe_index>NumPipes)
  return -1;

 QSharedMemory* p=MemoryPage[pipe_index];

 if(!p)
  return -3;

 if(!p->isAttached())
  return -10;

 int size=0;
 if(p->lock())
 {
  size=(p->size()>buffer_byte_size)?buffer_byte_size:p->size();
  memcpy(buffer,p->data(),size);
  p->unlock();
 }
 else
  return -4;

 return size;
}
