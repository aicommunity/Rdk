/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2008.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UITEM_DATA_H
#define UITEM_DATA_H


// Отключение предупреждений в VS
//#pragma warning( disable : 4355)
//#pragma warning( disable : 4800)

//#pragma warning( disable: 4786 )

namespace RDK {

// Данные
class RDK_LIB_TYPE UItemData
{
friend class UADItem;
public: // Размер единичного элемента данных по умолчанию
static int DefaultDataSize;
public: // Данные
// Указатель на данные
union
{
  void *Void;
  void **PVoid;
  char *Char;
  unsigned char *UChar;
  short *Short;
  unsigned short *UShort;
  int *Int;
  unsigned int *UInt;
  long long *Long;
  unsigned long long *ULong;
  float *Float;
  double *Double;
  long double *LDouble;
};

protected: // Параметры
// Размер вектора данных
int Size;

// Размер данных в байтах
int ByteSize;

// Размер единичного данного в байтах
int DataSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UItemData(void);
UItemData(const UItemData &copy);
~UItemData(void);
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Размер вектора данных
virtual int GetSize(void) const;

// Размер данных в байтах
int GetByteSize(void) const;

// Размер единичного данного в байтах
int GetDataSize(void) const;
// --------------------------

// --------------------------
// Методы
// --------------------------
// Выделение памяти
void Resize(int size);

// Выделение памяти и заполнение заданным байтом
void Assign(int size, char ch);

// Выделение памяти и заполнение заданной последовательностью
// длиной ByteSize
void Assign(int size, const void *data);

// Выделение памяти и заполнение заданной последовательностью
// длиной size с элементами длиной datasize
// Массив записывается начиная с индекса shift
// Выделяется память разером datasize*size+shift*datasize*shift
void Assign(int size, int datasize, const void *data, int shift=0);

// Заполнение заданной последовательностью
// в предположении что память уже выделена
// Массив записывается начиная с индекса shift
void Assign(int size, const void *data, int shift=0) const;

// Устанавливает размер единичного данного в байтах
bool SetDataSize(int size);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UItemData& operator = (const UItemData &data);

// Оператор расширения массива
// Предполагает, что добавляемый массив имеет тот же формат
// Добавление происходит прямым копированием области памяти
UItemData& operator += (const UItemData &data);

// Оператор доступа
//inline char operator [] (int i);
// --------------------------
};



}

#endif

