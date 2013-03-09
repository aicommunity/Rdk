/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef USerStorageBinaryBINARY_H
#define USerStorageBinaryBINARY_H

#include "USerStorage.h"

namespace RDK {

class USerStorageBinary: public USerStorage
{
private:
// Начальный максимальный размер очереди
enum { INIT_SIZE = 1024 };

protected: // Данные
// Данные очереди
unsigned char* m_pData;

// Число данных в очерди
int Size;

// Максимально доступный размер очереди
int MaxSize;

// Индекс первого элемента
int Front;

// Индекс последнего элемента
int Back;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Создает пустую очередь
USerStorageBinary(void);

// Создает очередь с резервированым максимальным размером
USerStorageBinary(int size);

// Создает очередь заполненную данными из вектора vec
USerStorageBinary(const unsigned char* vec, int nsize);

virtual ~USerStorageBinary(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает первый элемент
unsigned char&    front(void);

// Возвращает последний элемент
unsigned char&    back(void);

// Оператор доступа к элементам по индексу
// Индекс считается от начала очереди
unsigned char& operator [] (int i);

// Возвращает true если очередь пуста
bool empty(void);

// Возвращает число элементов данных в очереди
int size(void);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Добавляет элемент в конец
void push(const unsigned char& obj);

// Извлекает элемент из начала
void pop(void);

// Очищает очередь
void clear(void);


// Резервирует место для size_ элементов
void reserve(int size_);

// Заполняет очередь данными из вектора vec.
// 0-й элемент вектора становится началом очереди
void FromVec(const unsigned char* vec, int nsize);
// --------------------------

// --------------------------
// Скрытые методы управления данными
// --------------------------
protected:
// Метод масштабирования массива
void InternalResize(int size);
// --------------------------
};

}
#endif
