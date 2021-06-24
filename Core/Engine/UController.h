/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UControllerH
#define UControllerH

#include <vector>
#include <map>
#include <typeinfo>
#include <list>
#include "UModule.h"
#include "UEPtr.h"
#include "../Math/MDMatrix.h"

namespace RDK {

class UContainer;
class UIProperty;

using namespace std;

class RDK_LIB_TYPE UController: public RDK::UModule
{
protected: // Параметры
// Флаг разрешения обновления данных контроллера
bool Enabled;

protected: // Данные
// Указатель на компоненты
UContainer* Component;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UController(void);
virtual ~UController(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Флаг разрешения обновления данных контроллера
bool IsEnabled(void) const;
bool IsEnabled(bool value);

// Возвращает имя компонента
std::string GetComponentName(void) const;
std::string GetComponentName(UContainer *level) const;
// --------------------------

// --------------------------
// Методы управления интерфейсом
// --------------------------
public:
// Связывает интерфейс с компонентом
virtual bool Link(UContainer* component, bool forchilds=false);

// Отвязывает интерфейс от компонента
virtual bool UnLink(bool forchilds=false);

// Обновляет интерфейс
virtual bool Update(void);
// --------------------------

// --------------------------
// Скрытые методы управления интерфейсом
// --------------------------
protected:
// Связывает интерфейс с компонентом
virtual bool ALink(UContainer* component, bool forchilds=false);

// Отвязывает интерфейс от компонента
virtual bool AUnLink(bool forchilds=false);

// Обновляет интерфейс
virtual bool AUpdate(void)=0;
// --------------------------
};

/// Описание точки съема данных
struct UControllerData: public UController
{
/// свойство-источник данных
UEPtr<UIProperty> Property;
/// Имя свойства
//std::string PropertyName;

/// Количество последних хранимых точек данных
int NumPoints;

/// Конструктор
UControllerData(void);

/// Задает размер хранимых данных
void SetNumPoints(int value);

/// Возвращает имя свойства
std::string GetPropertyName(void) const;

/// Обновляет хранимые данные добавляя новые
virtual bool AUpdate(void)=0;

/// Удаляет все хранимые данные
virtual void Clear(void)=0;

/// Настраивает точку съема данных
virtual bool Configure(UContainer *container, UEPtr<UIProperty> property)=0;

/// Возвращает тип данных
virtual const type_info& GetDataType(void) const;
};

struct UControllerDataReader: public UControllerData
{
/// Временной интервал на котором хранятся данные (секунды)
double TimeInterval;

/// Тип данных свойства
/// 0 - не определено
/// 1 - простое свойство типа double
/// 2 - простое свйоство типа int
/// 3 - свойство типа MDMatrix<double>
/// 4 - свойство типа MDMatrix<int>
/// 5 - свойство типа MDVector<double>
/// 6 - свойство типа MDVector<int>
int PropertyType;

/// Координаты матрицы по которым берутся данные
int MRow, MCol;

/// Данные
std::list<double> XData;
std::list<double> YData;

public:
/// Конструктор
UControllerDataReader(void);

/// Задает длину интервала в секундах
virtual void SetTimeInterval(double value);

/// Координаты матрицы по которым берутся данные
virtual void SetMatrixCoord(int row, int col);

// Отвязывает интерфейс от компонента
virtual bool AUnLink(bool forchilds=false);

/// Обновляет хранимые данные добавляя новые
virtual bool AUpdate(void);

/// Удаляет все хранимые данные
virtual void Clear(void);

/// Настраивает точку съема данных
virtual bool Configure(UContainer *container, UEPtr<UIProperty> property);

protected:
/// Задает размер хранимых данных
void SetNumPoints(int value);
};

/// Класс, регистрирующий данные, которые хранятся в матрице типа
/// MDMatrix<double>, в нулевой строке.
/// Формат хранения: моменты времени в которые происходили события
/// Длина вектора может быть произвольной
struct UControllerDataReaderTimeEvents: public UControllerDataReader
{
public:
UControllerDataReaderTimeEvents(void);

protected:
/// Координаты матрицы по которым берутся данные
virtual void SetMatrixCoord(int row, int col);

/// Обновляет хранимые данные добавляя новые
virtual bool AUpdate(void);
};
              /*

template <class T>
struct UDataSourceT: public UControllerData
{
/// Данные
std::map<double, MDMatrix<T> > Data;

UDataSourceT(void);

/// Обновляет хранимые данные добавляя новые
virtual bool AUpdate(void);

/// Считывает данные и очищает массив
virtual void ExtractData(std::map<double, MDMatrix<T> > &buffer);

/// Удаляет все хранимые данные
virtual void Clear(void);

/// Настраивает точку съема данных
virtual bool Configure(UContainer *container, const std::string &property_name);

/// Возвращает тип данных
virtual const type_info& GetDataType(void) const;
};

template<class T>
UDataSourceT<T>::UDataSourceT(void)
 : UControllerData()
{
}

/// Обновляет хранимые данные добавляя новые
template<class T>
bool UDataSourceT<T>::AUpdate(void)
{
 const MDMatrix<T> *data=Component->AccessPropertyData<MDMatrix<T> >(PropertyName);
 if(!data)
 {
  return false;
 }

 if(LastReadTime>=LastWriteTime)
 {
  Data.clear();
  LastReadTime=0.0;
 }


 Data[Component->GetTime().GetDoubleTime()]=*data;
 LastWriteTime=Component->GetTime().GetDoubleTime();
 return true;
};

/// Считывает данные и очищает массив
template<class T>
void UDataSourceT<T>::ExtractData(std::map<double, MDMatrix<T> > &buffer)
{
 buffer=Data;
 LastReadTime=Component->GetTime().GetDoubleTime();
}

/// Удаляет все хранимые данные
template<class T>
void UDataSourceT<T>::Clear(void)
{
 Data.clear();
 LastReadTime=LastWriteTime=0.0;
};

/// Настраивает точку съема данных
template<class T>
bool UDataSourceT<T>::Configure(UContainer *container, const std::string &property_name)
{
 if(PropertyName == property_name && Component == container)
  return true;

 UnLink();
 Clear();

 if(!Link(container))
  return false;

 PropertyName.clear();

 UEPtr<UIProperty> prop=Component->FindProperty(property_name);
 if(!prop)
 {
  UnLink();
  return false;
 }

 if(prop->GetLanguageType() != typeid(MDMatrix<T>))
 {
  UnLink();
  return false;
 }

 PropertyName=property_name;
 Enabled=true;
 return true;
};

/// Возвращает тип данных
template<class T>
const type_info& UDataSourceT<T>::GetDataType(void) const
{
 return typeid(MDMatrix<T>);
}

                  */
}

#endif
