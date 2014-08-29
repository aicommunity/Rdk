/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UADITEM_H
#define UADITEM_H

#include <typeinfo>
#include "UItem.h"
#include "UItemData.h"

namespace RDK {

class RDK_LIB_TYPE UADItem: public UItem
{
friend class UItemData;
private: // Хранилище выходных данных
// Вектор выходных данных
vector<UItemData> OutputData;

private: // Хранилище входных связей
// Массив указателей на вектора входов
//vector<UEPtr<const UItemData> > InputData;
vector<UItemData*> InputData;

protected: // Переменные быстрого доступа к даннным входов. Read only!
// Суммарное число всех входов
size_t FullInputDataSize;

// Минимальный размер вектора входов
size_t MinInputDataSize;

// Максимальный размер вектора входов
size_t MaxInputDataSize;

protected: // Переменные быстрого доступа к даннным выходов. Read only!
// Указатель на первый элемент массива указателей на вектора входов
UItemData* POutputData;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UADItem(void);
virtual ~UADItem(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает вектор выходных данных
inline const UItemData& GetOutputData(int index) const
{ return POutputData[index]; };

// Возвращает указатель на вектор входов InputData по указателю на item
// Возвращает 0 если citem == 0 или не найден в списке подключений
const UEPtr<const UItemData>& GetInputData(const UEPtr<UItem> &citem) const;

// Возвращает указатель на вектор входов InputData по индексу
// Проверяет индекс на корректность и возвращает 0, если такого входа нет фактически
const UEPtr<const UItemData>& GetInputData(size_t index) const;

// Возвращает размер вектора входов InputData по индексу
// Проверяет индекс на корректность и возвращает 0, если такого входа нет фактически
size_t GetInputDataSize(size_t index) const;

// Возвращает суммарный размер всех векторов входов
size_t GetFullInputDataSize(void) const;
// --------------------------

// ----------------------
// Методы управления выходными данными
// ----------------------
// Размер вектора выходных данных
inline size_t GetOutputDataSize(int index) const
{ return OutputData[index].Size; };
virtual bool SetOutputDataSize(int index, int size, bool nobuild=false);

// Возвращает размер вектора выходных данных в байтах
inline size_t GetByteOutputDataSize(int index) const
{ return OutputData[index].ByteSize; };

// Размер единичного данного вектора выходных данных в байтах
size_t GetOutputDataElementSize(int index) const
{ return OutputData[index].DataSize; };
bool SetOutputDataElementSize(int index, int size);

// Заполняет заданный выходной вектор данными
void FillOutputData(int index, const void *data=0);

// Заполняет все выходные вектора заданными данными
void FillOutputData(const void *data=0);
// ----------------------

// --------------------------
// Методы специфического доступа к  выходным данным
// --------------------------
// Инициализирует выход как указатель на объект
bool SetOutputDataAsPointer(int index, void *pointer);

// Возвращает данные выхода как указателя на объект
void* GetOutputDataAsPointer(int index);

// Возвращает данные входа как указателя на объект
void* GetInputDataAsPointer(int index);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Создает экземпляр описания класса
virtual UContainerDescription* ANewDescription(UComponentDescription* description);
// --------------------------

// ----------------------
// Методы доступа к системным свойствам
// ----------------------
protected:
// Размер выходных векторов
vector<size_t> GetOutputDataSize(void) const;
bool SetOutputDataSize(const vector<size_t> &value);

// Размер единичного данного вектора выходов в байтах
vector<size_t> GetOutputDataElementSize(void) const;
bool SetOutputDataElementSize(const vector<size_t> &value);
// ----------------------

// ----------------------
// Методы доступа к компонентам
// ----------------------
// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;
// ----------------------

// ----------------------
// Коммуникационные методы
// ----------------------
protected:
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Возвращает false если na уже подключен к этому входу.
// При успешном подключении c_index содержит реальный индекс подключенного входа
virtual bool ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index);
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

// Разрывает связь с элементом сети подключенным ко входу 'index'
virtual void DisconnectFromIndex(int c_index);
virtual void DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index);
//virtual void DisconnectFromIndex(const NameT &connector_property_name, int index=0);
// ----------------------

// --------------------------
// Методы управления счетом
// --------------------------
public:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool Default(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool Build(void);

// Сброс процесса счета без потери настроек
virtual bool Reset(void);
// --------------------------

// ----------------------
// Вспомогательные методы
// ----------------------
private:
// Обновляет входной массив по данным подключенного ко входу компонента
//void UpdateInputData(int index);

// Обновляет указатели на массивы входов и выходов
void UpdatePointers(void);

// Вычисляет минимальный и максимальный размер векторов входов
void CalcMinMaxInputDataSize(void);
// ----------------------
};


}


#endif

