/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef USerStorageH
#define USerStorageH

namespace RDK {

class USerStorage
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Создает пустую очередь
USerStorage(void);
virtual ~USerStorage(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Сохранение значения в хранилище сериализации
/*template<typename T>
bool Save(const T &value);

// Загрузка значения из хранилища сериализации
template<typename T>
bool Load(T &value);        */
// --------------------------

// --------------------------
// Скрытые методы управления данными
// --------------------------
protected:
// --------------------------
};

}
#endif
