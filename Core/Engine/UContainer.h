/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAContainerH
#define UAContainerH

#include "../Math/MVector.h"
#include "UComponent.h"
#include "UPropertyEndpoints.h"
#include "UTime.h"
#include "UController.h"
#include <unordered_map>
#include <typeindex>

namespace RDK {

class UContainer;

// Интерфейсный класс для работы с указателями
class RDK_LIB_TYPE UIPointer
{
protected:

public:
virtual ~UIPointer();
virtual UEPtr<UContainer> const Get(void) const=0;

virtual void Set(UEPtr<UContainer> source)=0;

virtual void Del(UEPtr<UContainer> source)=0;

// Проверяет, существует ли такой указатель в этом классе
// Возвращает 0 если да, и <0 если нет
virtual int Find(UEPtr<const UContainer> cont) const=0;

// Операторы
// -----------------
// -----------------
UIPointer& operator = (UEPtr<UContainer> source)
{
 Set(source);
 return *this;
};
// -----------------
};


// Переменная указателя свойства
struct RDK_LIB_TYPE UPVariable
{
// Id объекта
 UId Id;

 // Указатель на свойство
 UEPtr<UIPointer> Pointer;

// Конструкторы и деструкторы
// --------------------------
// --------------------------
UPVariable(void);
UPVariable(UId id, UEPtr<UIPointer> prop);
UPVariable(const UPVariable&) = default;
UPVariable& operator=(const UPVariable&) = default;
virtual ~UPVariable(void);
// --------------------------
};


class UInstancesStorageElement;

typedef long int IndexT;
//class UContainer;
typedef UContainer* PUAContainer;

// Массив указателей на контейнеры
typedef std::vector<UEPtr<UContainer> > UAContainerVector;

typedef std::map<UEPtr<UContainer>, NameT> UAStaticContainerMap;

class RDK_LIB_TYPE UContainer: public UComponent
{
public:
typedef std::unordered_map<NameT,UPVariable> PointerMapT;
typedef std::unordered_map<NameT,UPVariable>::iterator PointerMapIteratorT;
typedef std::unordered_map<NameT,UPVariable>::const_iterator PointerMapCIteratorT;

//friend class UStorage;
friend class UController;

private: // Таблица соответствий имен и Id компонент объекта
std::unordered_map<NameT,UId> CompsLookupTable;

// Таблица локальных указателей на дочерние компоненты
PointerMapT PointerLookupTable;

private: // Таблица компонент
// Таблица компонент
UAContainerVector Components;

// Index map for O(1) component lookup by ID
std::unordered_map<UId, size_t> ComponentsIdIndex;

// Cache for last successful dynamic_pointer_cast in GetComponent<T>
mutable UEPtr<UContainer> CachedComponent;
mutable UId CachedComponentId;
mutable std::type_index CachedComponentType;

/// Маска для фильтрации системных событий компонента
UAStaticContainerMap StaticComponents;

// Таблица контроллеров интерфейса
std::vector<UEPtr<UController> > Controllers;

public: // Имя объекта
UProperty<NameT, UContainer, ptSysParameter> Name;

// Id объекта
UProperty<UId, UContainer, ptSysParameter> Id;

// true - расчет объекта будет выполняться
// false - расчет объекта будет игнорироваться
UProperty<bool,  UContainer, ptPubParameter> Activity;

// Реальный шаг = 1./TimeStep
UProperty<UTime,  UContainer, ptSysParameter> TimeStep;

/// в миллисекундах.
/// Если время расчета превышено, то расчет последующих дочерних компонент
/// не выполняется
/// Если значение параметра <0, то нет ограничений
UProperty<long long, UContainer, ptPubParameter> MaxCalculationDuration;

/// в миллисекундах, по превышении которого выдается предупреждающее сообщение в лог.
/// Если значение параметра <0, то нет ограничений
UProperty<long long, UContainer, ptPubParameter> CalculationDurationThreshold;

public: // Координата компонента в пространстве сети
UProperty<RDK::MVector<double,3>, UContainer, ptPubParameter> Coord;

// (без учета времени обсчета дочерних объектов) (мс)
UProperty<unsigned long long, UContainer, ptPubSysState> StepDuration;

/// Маска для фильтрации системных событий компонента
UProperty<unsigned int, UContainer, ptPubParameter | static_cast<unsigned int>(pgSystem)> DebugSysEventsMask;

protected: // Время, прошедшее между двумя последними итерациями счета
unsigned long long InterstepsInterval;

// счетчиком текущего интервала ожидания.
// В противном случае 'CalcCounter' не ипользуется
int CalcCounter;

// Хранит величину шага счета родительского
// объекта или величину шага счета этого объекта
// если родительского объекта не существует
UTime OwnerTimeStep;

// Указатель на 0-й элемент вектора компонент
UEPtr<UContainer>* PComponents;

// Количество компонент в векторе компонент
int NumComponents;

// Cache for active (non-static) components to optimize Calculate() loop
// Updated when components are added/removed or static flag changes
std::vector<UEPtr<UContainer>> ActiveComponents;
bool ActiveComponentsCacheValid;

// Кэш для проверки TimeStep (оптимизация Calculate)
// Инвалидируется при изменении TimeStep
mutable bool CachedTimeStepEqual;
mutable bool CachedTimeStepLess;
mutable bool CachedTimeStepGreater;

//UEPtr<UInstancesStorageElement> ObjectIterator;
//UEPtr<UInstancesStorageElement> ObjectIterator;

// Последний использованный Id компонент
UId LastId;

// Флаг запроса на пропуск обсчета компонент в текущей итерации расчетов
bool SkipComponentCalculation;

// Флаг запроса на повторный обсчет компонент в текущей итерации расчетов сначала
bool ComponentReCalculation;

/// Время начала расчета этого компонента по системным часам
unsigned long long StartCalcTime;

/// Время завершения расчета этого компонента по системным часам
unsigned long long LastCalcTime;

/// Флаг, показывающий что компонент находится в движении
bool MovingFlag;

/// Может быть отрицательрным если память освобождалась.
/// Актуально если включен флаг MemoryMonitor
long long MemoryUsageDiff;

/// Может быть отрицательрным если кусок увеличился.
/// Актуально если включен флаг MemoryMonitor
long long MaxMemoryBlockDiff;

protected:
/// Список имен свойств, выводимых в подробный лог
std::vector<std::string> PropertiesForDetailedLog;

// Конструкторы и деструкторы
// --------------------------
// --------------------------
public:
UContainer(void);
// Удаляем конструктор копирования и оператор присваивания
// Копирование должно выполняться через метод Copy()
UContainer(const UContainer&) = delete;
UContainer& operator=(const UContainer&) = delete;
virtual ~UContainer(void);
// --------------------------

// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта
UEPtr<UContainer> GetOwner(void) const;

// Возвращает указатель на главного владельца этим объектом
UEPtr<UContainer> GetMainOwner(void) const;

// Возвращает хранилище компонент этого объекта
UEPtr<UStorage> const GetStorage(void) const;

// Проверяет, является ли объект owner владельцем этого объекта на каком-либо уровне иерархии
bool CheckOwner(UEPtr<UContainer> owner) const;

// Возвращает полный Id объекта (включая Id всех владельцев)
ULongId& GetFullId(ULongId &buffer) const;
ULongId GetFullId(void) const;

// Возвращает  'длинный' Id объекта (исключая имя владельца 'mainowner')
// Метод возвращает пустой вектор, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии
ULongId& GetLongId(UEPtr<UContainer> mainowner, ULongId &buffer) const;
ULongId GetLongId(UEPtr<UContainer> mainowner) const;
// Промежуточный вариант одноименного метода, возвращающего длинное имя
std::string& GetLongId(UEPtr<UContainer> mainowner, std::string &buffer) const;

// Возвращает true если передаваемый идентификатор объекта корректен, в противном случае возвращает false
bool CheckLongId(const ULongId &id) const;
// Промежуточный вариант одноименного метода, обрабатывающего длинное имя
bool CheckLongId(const std::string &id) const;

// Управление средой выполнения этого объекта
virtual bool SetEnvironment(UEPtr<UEnvironment> environment);

virtual bool SetLogger(UEPtr<UExceptionLogger> logger);

// Вызов обработчика исключений среды
virtual void ProcessException(UException &exception);

/// Записывает диагностическое или ошибочное сообщение в лог
virtual void LogMessage(int msg_level, const std::string &line, int error_event_number=0);
virtual void LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &method_name, int line, int error_event_number=0);

virtual void LogDebugSysMessage(unsigned long long debug_sys_msg_type, unsigned long long modifier);

/// Записывает значения выбранных свойств перед началом расчета компонента
virtual void LogPropertiesBeforeCalc(void);

/// Записывает значения выбранных свойств после завершения расчета компонента
virtual void LogPropertiesAfterCalc(void);

/// Возвращает состояние флага режима отладки
virtual bool CheckDebugMode(void) const;

/// Разделитель - запятая
void SetPropertiesForDetailedLog(const std::string &str);
// --------------------------

// Методы управления свойствами
// --------------------------
// Удаляет владельца объекта
// --------------------------
// Методы управления свойствами
// --------------------------
// Удаляет владельца объекта
void BreakOwner(void);

// Указатель устанавливается на число уровней дочерних компонент
// 'levels'. Если levels < 0 то устанавливается компонентам на всех уровнях
void SetMainOwner(UEPtr<UComponent> mainowner);
void SetMainOwner(UEPtr<UComponent> mainowner, int levels);

// Проверяет предлагаемый Id 'id' на уникальность в рамках данного, объекта.
bool CheckId(const UId &id);

// Генерирует уникальный Id
virtual UId GenerateId(void);

/// Находит свойство по имени и типу T, возвращает указатель или 0 если не найдено
template<typename T>
const UEPtr<UVBaseDataProperty<T> > FindPropertyEx(const NameT &name) const;

template<typename T>
UEPtr<UVBaseDataProperty<T> > FindPropertyEx(const NameT &name);

/// Подключает данные свойства source_property компонента source_component
/// к свойству destination_property данного контейнера
template<typename T>
bool AttachPropertyData(const NameT& destination_property, const NameT& source_component, const NameT &source_property);

template<typename T>
void DetachPropertyData(const NameT& destination_property);
// --------------------------

// Методы управления общедоступными свойствами
// --------------------------
// --------------------------
public:
// Координата компонента в пространстве сети
const RDK::MVector<double,3>& GetCoord(void) const;

// (без учета времени обсчета дочерних объектов) (мс)
unsigned long long GetStepDuration(void) const;

// (вместе со времени обсчета дочерних объектов) (мс)
unsigned long long GetFullStepDuration(void) const;

unsigned long long GetInterstepsInterval(void) const;

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double GetInstantPerformance(void) const;

// Устанавливает величину шага интегрирования
const UTime& GetTimeStep(void) const;

// Устанавливает величину шага интегрирования компоненту и всем его дочерним компонентам
bool SetGlobalTimeStep(UTime timestep);

/// Предназначено только для вызова из UEnvironment
void ChangeUseIndTimeStepMode(bool value);

inline const bool& GetActivity(void) const { return Activity.GetData(); }

UId GetId(void) const;

// Проверяет предлагаемое имя 'name' на уникальность в рамках данного объекта
bool CheckName(const NameT &name);

bool ValidateName(const NameT &name);

// Генерирует имя уникальное в компонентах этого объекта
virtual NameT& GenerateName(const NameT &prefix, NameT &namebuffer);

// Устанавливает имя объекта
const NameT& GetName(void) const;

// (включая имена всех владельцев)
// Возвращает полное имя объекта (включая имена всех владельцев)
NameT& GetFullName(NameT &buffer) const;
NameT GetFullName(void) const;

// Возвращает 'длинное' имя объекта (исключая имя владельца 'mainowner').
// Метод возвращает пустую строку, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии
NameT& GetLongName(const UEPtr<UContainer> &mainowner, NameT &buffer) const;
NameT GetLongName(const UEPtr<UContainer> &mainowner) const;

/// Максимально допустимое время расчета компонента вместе с дочерними компонентами
/// в миллисекундах.
/// Если время расчета превышено, то расчет последующих дочерних компонент
/// не выполняется
/// Если значение параметра <0, то нет ограничений
const long long& GetMaxCalculationDuration(void) const;

/// в миллисекундах, по превышении которого выдается предупреждающее сообщение в лог.
/// Если значение параметра <0, то нет ограничений
const long long& GetCalculationDurationThreshold(void) const;

const unsigned int& GetDebugSysEventsMask(void) const;

/// Может быть отрицательрным если память освобождалась.
/// Актуально если включен флаг MemoryMonitor
long long GetMemoryUsageDiff(void) const;

/// Может быть отрицательрным если кусок увеличился.
/// Актуально если включен флаг MemoryMonitor
long long GetMaxMemoryBlockDiff(void) const;

bool SetName(const NameT &name);
bool SetCoord(const RDK::MVector<double,3> &value);
virtual bool SetActivity(const bool &activity);

protected:

bool SetTimeStep(const UTime &timestep);
bool SetId(const UId &id);
bool SetDebugSysEventsMask(const unsigned int &value);
bool SetMaxCalculationDuration(const long long &value);
bool SetCalculationDurationThreshold(const long long& value);
// --------------------------

// Методы доступа к таблицам соотвествий
// --------------------------
// --------------------------
public:
// Возвращает имя дочернего компонента по его Id
const NameT& GetComponentName(const UId &id) const;

const UId& GetComponentId(const NameT &name, bool no_throw=false) const;

// Возвращает имя локального указателя по его Id
const NameT& GetPointerName(const UId &id) const;

// Возвращает Id локального указателя по его имени
const UId& GetPointerId(const NameT &name) const;

// и возвращает вектор компонент либо пустой вектор
// Осуществляет поиск всех компонент по заданному имени класса
// и возвращает вектор компонент либо пустой вектор
// find_all
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
const vector<UEPtr<UContainer> >& GetComponentsByClassName(const NameT &name, vector<UEPtr<UContainer> > &buffer, bool find_all=false);

// Осуществляет поиск всех компонент по заданному имени класса
// и возвращает вектор длинных имен компонент относительно текущего либо пустой вектор
// find_all
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
const vector<NameT>& GetComponentsNameByClassName(const NameT &name, vector<NameT> &buffer, bool find_all=false);

// find_all
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
// имена возвращются относительно уровня net
template<class T>
const vector<NameT>& GetComponentsNameByClassType(vector<NameT> &buffer, UEPtr<UContainer> net=0, bool find_all=false);
// --------------------------

public:
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual UContainer* New(void)=0;

//virtual UContainerDescription* NewDescription(void);
//virtual UContainerDescription* NewDescription(void);

// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual UEPtr<UContainer> Alloc(UEPtr<UStorage> stor, bool copystate=false);

// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;

// или вызов деструктора, если Storage == 0
// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
virtual void Free(void);

//UEPtr<UInstancesStorageElement> GetObjectIterator(void);
//void SetObjectIterator(UEPtr<UInstancesStorageElement> value);
//void SetObjectIterator(UEPtr<UInstancesStorageElement> value);

protected:
/// ������������ ���������� ���������� ������ ����������, �������������� ��� �����������
virtual void AUpdateInternalData(void);
// --------------------------

// Методы доступа к компонентам
// --------------------------
// --------------------------
public:
// --------------------------
// Методы доступа к компонентам
// --------------------------
// Возвращает число дочерних компонент
int GetNumComponents(void) const;

// Возвращает полное число дочерних компонент
// (включая все компоненты дочерних компонент)
int GetNumAllComponents(void) const;

bool CheckComponent(const NameT &name);
bool CheckComponentL(const NameT &name);

// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;

// объекте по короткому Id 'id'
// Если id == ForbiddenId то возвращает указатель на этот компонент
// Если nothrow == true то возвращает 0 и не кидает исключение
virtual UEPtr<UContainer> GetComponent(const UId &id, bool no_throw=false) const;

template<class T>
UEPtr<T> GetComponent(const UId &id, bool no_throw=false) const
{
 // Check cache first
 if(CachedComponent && CachedComponentId == id)
 {
  if(typeid(T) == CachedComponentType)
  {
   return static_pointer_cast<T>(CachedComponent);
  }
 }
 
 // Get base component
 UEPtr<UContainer> comp = GetComponent(id, no_throw);
 if(!comp)
  return UEPtr<T>(0);
 
 // Try dynamic cast
 UEPtr<T> result = dynamic_pointer_cast<T>(comp);
 if(result)
 {
  // Update cache
  CachedComponent = comp;
  CachedComponentId = id;
  CachedComponentType = typeid(T);
 }
 
 return result;
}

// объекте по короткому имени 'name'
virtual UEPtr<UContainer> GetComponent(const NameT &name, bool no_throw=false) const;

template<class T>
UEPtr<T> GetComponent(const NameT &name, bool no_throw=false) const
{
 // Get base component by name
 UEPtr<UContainer> comp = GetComponent(name, no_throw);
 if(!comp)
  return UEPtr<T>(0);
 
 // Check cache if ID matches
 if(CachedComponent && CachedComponent == comp)
 {
  if(typeid(T) == CachedComponentType)
  {
   return static_pointer_cast<T>(CachedComponent);
  }
 }
 
 // Try dynamic cast
 UEPtr<T> result = dynamic_pointer_cast<T>(comp);
 if(result)
 {
  // Update cache
  CachedComponent = comp;
  CachedComponentId = comp->Id;
  CachedComponentType = typeid(T);
 }
 
 return result;
}

// объекте по ДЛИННОМУ Id 'id'
// Если id[0] == ForbiddenId или Id имеет нулевой размер,
// то возвращает указатель на этот компонент
UEPtr<UContainer> GetComponentL(const ULongId &id, bool no_throw=false) const;

template<class T>
UEPtr<T> GetComponentL(const ULongId &id, bool no_throw=false) const
{
 // Get base component by long ID
 UEPtr<UContainer> comp = GetComponentL(id, no_throw);
 if(!comp)
  return UEPtr<T>(0);
 
 // Check cache if ID matches
 if(CachedComponent && CachedComponent == comp)
 {
  if(typeid(T) == CachedComponentType)
  {
   return static_pointer_cast<T>(CachedComponent);
  }
 }
 
 // Try dynamic cast
 UEPtr<T> result = dynamic_pointer_cast<T>(comp);
 if(result)
 {
  // Update cache
  CachedComponent = comp;
  CachedComponentId = comp->Id;
  CachedComponentType = typeid(T);
 }
 
 return result;
}

// объекте по ДЛИННОМУ имени 'name'
virtual UEPtr<UContainer> GetComponentL(const NameT &name, bool no_throw=false) const;

template<class T>
UEPtr<T> GetComponentL(const NameT &name, bool no_throw=false) const
{
 // Get base component by long name
 UEPtr<UContainer> comp = GetComponentL(name, no_throw);
 if(!comp)
  return UEPtr<T>(0);
 
 // Check cache if component matches
 if(CachedComponent && CachedComponent == comp)
 {
  if(typeid(T) == CachedComponentType)
  {
   return static_pointer_cast<T>(CachedComponent);
  }
 }
 
 // Try dynamic cast
 UEPtr<T> result = dynamic_pointer_cast<T>(comp);
 if(result)
 {
  // Update cache
  CachedComponent = comp;
  CachedComponentId = comp->Id;
  CachedComponentType = typeid(T);
 }
 
 return result;
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по порядковому индеку в списке компонент
// Метод возвращает 0, если индекс выходит за границы массива
UEPtr<UContainer> GetComponentByIndex(int index) const;

template<class T>
UEPtr<T> GetComponentByIndex(int index) const
{
 return dynamic_pointer_cast<T>(GetComponentByIndex(index));
}

// Добавляет дочерний компонент в этот объект
// Возвращает его Id или ForbiddenId если добавление неудачно
// Может быть передан указатель на локальную переменную
virtual void BeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual void AfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual UId AddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Таким образом 'name' - должно быть коротким именем
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
void DelComponent(const NameT &name, bool canfree=true);

// Принудительно удаляет все дочерние компоненты
void DelAllComponents(void);
protected:
void DelAllComponentsRaw(void);

public:
/// и имя 'name'
virtual void AddStaticComponent(const NameT &classname, const NameT &name, UEPtr<UContainer> comp);

/// Удаляет компонент из списка статических компонент
virtual void DelStaticComponent(UEPtr<UContainer> comp);

/// Удаляет все компоненты из списка статических компонент
virtual void DelAllStaticComponents(void);

/// с классом 'classname' и именем 'name'
virtual UEPtr<UContainer> FindStaticComponent(const NameT &classname, const NameT &name) const;

/// Если comp не принадлежит этому компоненту, или target имеет отличный от
/// этого компонента storage, или target не может принять в себя компонент
/// то возвращает false и не делает ничего
virtual bool MoveComponent(UEPtr<UContainer> comp, UEPtr<UContainer> target);

// Возвращает список имен и Id компонент, содержащихся непосредственно
// в этом объекте
// Память должна быть выделена
void GetComponentsList(vector<UId> &buffer) const;
void GetComponentsList(vector<NameT> &buffer) const;

// Копирует все компоненты этого объекта в объект 'comp', если возможно
// Если хранилище stor != 0 то используется оно
virtual void CopyComponents(UEPtr<UContainer> comp, UEPtr<UStorage> stor=0) const;

// Перемещает компонент с текущим индексом index или именем 'name' вверх или
// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
// на эту границу
virtual bool ChangeComponentPosition(int index, int step);
virtual bool ChangeComponentPosition(const NameT &name, int step);

// Применяется для изменения порядка расчета компонент
virtual bool SetComponentPosition(int index, int new_position);
virtual bool SetComponentPosition(const NameT &name, int new_position);

bool IsMoving(void) const;
// --------------------------

// Методы управления коммуникационными компонентами
// ----------------------
// ----------------------
public:
// Возвращает список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
virtual ULongIdVector& GetConnectorsList(ULongIdVector &buffer,
						  int sublevel=-1, UEPtr<UContainer> ownerlevel=0);

// Возвращает список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
virtual ULongIdVector& GetItemsList(ULongIdVector &buffer,
							int sublevel=-1, UEPtr<UContainer> ownerlevel=0);

// Возвращает список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
virtual ULongIdVector& GetNetsList(ULongIdVector &buffer,
							int sublevel=-1, UEPtr<UContainer> ownerlevel=0);
// ----------------------

// Методы управления локальными указателями
// --------------------------
// Устанавливает дочерний компонент 'id' в качестве заданного класса локальных указателей
// 'pointerid'
bool SetComponentAs(const UId &id, const UId &pointerid);

// Устанавливает дочерний компонент 'name' в качестве заданного класса локальных указателей
// 'pointername'
bool SetComponentAs(const NameT &name,const NameT &pointername);

// Сбрасывает отношение дочерниего компонента 'id' к заданному классу локальных указателей
// 'pointerid'
bool ResetComponentAs(const UId &id, const UId &pointerid);

// Сбрасывает отношение дочерниего компонента 'name' к заданному классу локальных указателей
// 'pointername'
bool ResetComponentAs(const NameT &name,const NameT &pointername);

// Сбрасывает отношение дочерниего компонента 'name' ко всем классам локальных указателей
bool ResetComponentAll(const UId &id);

// Сбрасывает отношение дочерниего компонента 'name' ко всем классам локальных указателей
bool ResetComponentAll(const NameT &name);

// Удаляет все компоненты относящиеся к заданному классу локальных указателей
void DelAllComponentsAs(const NameT &pointername, bool canfree=true);
// --------------------------

// Методы управления общими (shared) переменными
// --------------------------
// Метод инициализации общих переменных. Вызывается автоматически при добавлении
// объекта владельцу
// --------------------------
// Методы управления общими (shared) переменными
// --------------------------
// Метод инициализации общих переменных. Вызывается автоматически при добавлении
// объекта владельцу
virtual void SharesInit(void);

// объекта из владельца
// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
virtual void SharesUnInit(void);
// --------------------------

// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool Default(void);

/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
virtual bool DefaultAll(UContainer* cont, bool subcomps);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool Build(void);

// Сброс процесса счета без потери настроек
virtual bool Reset(void);

// Выполняет расчет этого объекта
virtual bool Calculate(void);

// Выполняет начальную инициализацию этого объекта
virtual void Init(void);

// Выполняет деинициализацию этого объекта
virtual void UnInit(void);

// Обновляет состояние MainOwner после расчета этого объекта
virtual void UpdateMainOwner(void);

// Обычно вызывается дочерним компонентом и прерывает обсчет цепочки дочерних
// компонент на этом шаге счета
virtual void ForceSkipComponentCalculation(void);

// Обычно вызывается дочерним компонентом и требует перерасчет цепочки дочерних
// компонент на этом шаге счета сначала
virtual void ForceComponentReCalculation(void);

/// Проверяет текущую длительность расчета этого компонента
/// и если она превышает MaxCalculationDuration и MaxCalculationDuration>=0
/// то прерывает обсчет остальной цепочки дочерних компонент
virtual bool CheckDurationAndSkipComponentCalculation(void);

virtual void UpdateComputationOrder(void);

/// Invalidates active components cache (called when component static flag changes)
void InvalidateActiveComponentsCache(void);

/// Adds component to active cache (incremental update)
void AddToActiveCache(UEPtr<UContainer> comp);

/// Removes component from active cache (incremental update)
void RemoveFromActiveCache(UEPtr<UContainer> comp);
// --------------------------

// Скрытые методы управления счетом
// --------------------------
protected:
virtual void AInit(void);
virtual void AUnInit(void);
// --------------------------

// Скрытые методы управления контроллерами интерфейса
// Удаление контроллеров лежит на вызывающем модуле
// --------------------------
protected:
// Добавляет новый контроллер
void AddController(UEPtr<UController> controller, bool forchilds=false);

// Удаляет контроллер из списка
void DelController(UEPtr<UController> controller, bool forchilds=false);

// Удаляет все контроллеры
void DelAllControllers(bool forchilds=false);

// Инициирует отключение всех контроллеров
void UnLinkAllControllers(bool forchilds=false);

// Проверяет, существует ли контроллер в списке
bool CheckController(UEPtr<UController> controller) const;

// Возвращает число контроллеров
size_t GetNumControllers(void) const;

// Возвращает контроллер по индексу
UEPtr<UController> GetController(int index);
// --------------------------

// Скрытые методы управления локальными указателями
// --------------------------
public:
// Добавляет указатель с именем 'name' в таблицу соотвествий
// параметров и назначает ей корректный индекс
// Должна вызываться в конструкторах классов
UId AddLookupPointer(const NameT &name, UEPtr<UIPointer> pointer);

protected:
// Удаляет указатель с ID 'id' из таблицы соотвествий
void DelLookupPointer(const NameT &name);

// Осуществляет поиск в таблице указателя, соответствующего заданному источнику
PointerMapCIteratorT FindLookupPointer(UEPtr<UContainer> source) const;
// --------------------------

// Скрытые методы управления таблицей компонент
// --------------------------
// Добавляет компонент 'comp' в таблицу компонент
void AddComponentTable(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Удаляет компонент 'comp' из таблицы компонент
void DelComponentTable(UEPtr<UContainer> comp);

// Updates cache of active (non-static) components for optimized Calculate() loop
void UpdateActiveComponentsCache(void);

/// Rebuild Id→index map after PComponents reorder (ChangeComponentPosition).
void RebuildComponentsIdIndex(void);
// --------------------------

// Скрытые методы управления общими (shared) переменными
// --------------------------
protected:
// Метод инициализации общих переменных. Вызывается автоматически при добавлении
// объекта владельцу
virtual void ASharesInit(void);

// объекта из владельца
// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
virtual void ASharesUnInit(void);
// --------------------------

protected:
// Скрытые методы управления таблицей соответсвий компонент
// --------------------------
// Обновляет таблицу соответствий компонент заменяя 'oldname'
// имя компонента на 'newname'
void ModifyLookupComponent(const NameT &oldname, const NameT &newname);

// Обновляет таблицу соответствий компонент устанавливая Id 'id'
// для компонента с именем 'name'
void SetLookupComponent(const NameT &name, const UId &id);

// Обновляет таблицу соответствий компонент удаляя запись
// компонента с именем 'name'
void DelLookupComponent(const NameT &name);
// --------------------------

// Скрытые методы управления компонентами
// --------------------------
// --------------------------
protected:
/// Обновляет или создаёт запись о статическом компоненте класса classname,
/// возвращает его идентификатор
UId UpdateStaticComponent(const NameT &classname, UEPtr<UContainer> comp);

// Метод предполагает, что компонент принадлежит объекту
// Удаляет компонент comp
virtual void BeforeDelComponent(UEPtr<UContainer> comp, bool canfree=true);
virtual void AfterDelComponent(UEPtr<UContainer> comp, bool canfree=true);
void DelComponent(UEPtr<UContainer> comp, bool canfree);

// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
// Может быть передан указатель на локальную переменную
virtual void ABeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual void AAfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual void ABeforeDelComponent(UEPtr<UContainer> comp, bool canfree);
virtual void AAfterDelComponent(UEPtr<UContainer> comp, bool canfree);
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------

// Скрытые методы управления счетом
// --------------------------
protected:
// Обновляет состояние MainOwner после расчета этого объекта
virtual void AUpdateMainOwner(void);
// --------------------------

/// Базовый класс для всех исключений контейнера.
/// Содержит сведения об объекте, в котором возникло исключение.
public: /* Базовый класс исключений */
class EIContainer
{
public: // Короткое имя компонента в котором сгенерировано исключение
std::string Name;

// Короткий идентификатор компонента в котором сгенерировано исключение
ULongId Id;

// Полное имя владельца компонента в котором сгенерировано исключение
std::string OwnerName;

// Полный идентификатор владельца компонента в котором сгенерировано исключение
ULongId OwnerId;

// Полное имя главного владельца компонента в котором сгенерировано исключение
std::string MainOwnerName;

// Полный идентификатор главного владельца компонента в котором сгенерировано исключение
ULongId MainOwnerId;


public: // --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
EIContainer(void);
explicit EIContainer(const UContainer *cont);
EIContainer(const EIContainer &copy);
virtual ~EIContainer(void);
// --------------------------

// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Id компонента не найден
struct EComponentIdNotExist: public EIdNotExist
{
explicit EComponentIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id компонента уже существует
struct EComponentIdAlreadyExist: public EIdAlreadyExist
{
explicit EComponentIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// Имя компонента не найдено
struct EComponentNameNotExist: public ENameNotExist
{
explicit EComponentNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Имя компонента уже существует
struct EComponentNameAlreadyExist: public ENameAlreadyExist
{
explicit EComponentNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

/// Имя компонента некорректно (нарушает правила допустимых имён)
struct EComponentNameInvalid: public ENameError
{
explicit EComponentNameInvalid(const std::string &name) : ENameError(name) {};
virtual std::string CreateLogMessage(void) const
{
 return std::string(" invalid component name: ") + Name;
}
};

// Id указателя не найден
struct EPointerIdNotExist: public EIdNotExist
{
explicit EPointerIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id указателя уже существует
struct EPointerIdAlreadyExist: public EIdAlreadyExist
{
explicit EPointerIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// Имя указателя не найдено
struct EPointerNameNotExist: public ENameNotExist
{
explicit EPointerNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Имя указателя уже существует
struct EPointerNameAlreadyExist: public ENameAlreadyExist
{
explicit EPointerNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// Ошибки добавления/удаления компонент
struct EAddComponentAlreadyHaveOwner: public EIdError
{
explicit EAddComponentAlreadyHaveOwner(UId id) : EIdError(id) {};
};

// Недопустимый тип дочернего компонента
struct EAddComponentHaveInvalidType: public EIdError
{
explicit EAddComponentHaveInvalidType(UId id) : EIdError(id) {};
};

// Интерфейсный класс для обработки ошибок счета компонент
class EICalculateContainer: public EIContainer
{
 public:  // Короткое имя дочернего компонента в котором произошла ошибка счета
 std::string SubName;

 // Короткий идентификатор дочернего компонента в котором произошла ошибка счета
 ULongId SubId;

 // Конструкторы и деструкторы
 // --------------------------
 // --------------------------
 EICalculateContainer(void);
 EICalculateContainer(const UContainer *cont, const UContainer *subcont);
 EICalculateContainer(const EICalculateContainer &copy);
 virtual ~EICalculateContainer(void);
 // --------------------------

 // Методы формирования лога
 // --------------------------
 // Формирует строку лога об исключении
 virtual std::string CreateLogMessage(void) const;
 // --------------------------
};


// Исключение - ошибка
class EComponentCalculate: public EError, public EICalculateContainer
{
public:
 // Конструкторы и деструкторы
 // --------------------------
 // --------------------------
 EComponentCalculate(void);
 EComponentCalculate(const UContainer *cont, const UContainer *subcont);
 EComponentCalculate(const EComponentCalculate &copy);
 virtual ~EComponentCalculate(void) throw();
 // --------------------------

// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключение - фатальная ошибка стороннего исключения
class EComponentSystemException: public ESystemException, public EICalculateContainer
{
 public:

 // Конструкторы и деструкторы
 // --------------------------
 // --------------------------
 EComponentSystemException(void);
 EComponentSystemException(const UContainer *cont, const UContainer *subcont, const std::string &info);
 EComponentSystemException(const EComponentSystemException &copy);
 virtual ~EComponentSystemException(void) throw();
 // --------------------------

// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};
/* **************************** */

};

bool PreparePropertyLogString(const UVariable& variable, unsigned int expected_type, std::string &result);


// и возвращает вектор длинных имен компонент относительно текущего либо пустой вектор
// find_all
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
template<class T>
const vector<NameT>& UContainer::GetComponentsNameByClassType(vector<NameT> &buffer, UEPtr<UContainer> net, bool find_all)
{
 int numComp=int(GetNumComponents());
 UEPtr<UContainer> comp;
 UEPtr<UContainer> root(net);
 string compName;

 if(!net)
  root=this;

 switch(find_all)
 {
  case false:
   for(int i=0; i<numComp; i++)
   {
	comp=GetComponentByIndex(i);
	if(dynamic_pointer_cast<T>(comp))
	{
	 compName=comp->GetLongName(root, compName);
	 buffer.push_back(compName);
	}
   }
   break;

  case true:
   for(int i=0; i<numComp; i++)
   {
	comp=GetComponentByIndex(i);
	comp->GetComponentsNameByClassType<T>(buffer, root, true);
	if(dynamic_pointer_cast<T>(comp))
	{
	 compName=comp->GetLongName(root, compName);
	 buffer.push_back(compName);
	}
   }
   break;
 }

 return buffer;
}

// ���������� ������� ����� �������� � ����� ������
template<typename T>
const UEPtr<UVBaseDataProperty<T> > UContainer::FindPropertyEx(const NameT &name) const
{
 UEPtr<UIProperty> property=FindProperty(name);
 if(!property)
  return 0;

 if(property->GetLanguageType() != typeid(T))
  return 0;

 return reinterpret_cast<const UVBaseDataProperty<T>*>(property.Get());
}

template<typename T>
UEPtr<UVBaseDataProperty<T> > UContainer::FindPropertyEx(const NameT &name)
{
 UEPtr<UIProperty> property=FindProperty(name);
 if(!property)
  return 0;

 if(property->GetLanguageType() != typeid(T))
  return 0;

 return reinterpret_cast<UVBaseDataProperty<T>*>(property.Get());
}


/// ���������� � �������� destination_property ������ �������� ������� ����������
template<typename T>
bool UContainer::AttachPropertyData(const NameT& destination_property, const NameT& source_component, const NameT &source_property)
{
 UEPtr<UVBaseDataProperty<T> > dest_prop=FindPropertyEx<T>(destination_property);
 if(!dest_prop)
  return false;
 UEPtr<UContainer> source_cont=GetComponentL(source_component,true);
 if(!source_cont)
  return false;

 UEPtr<UVBaseDataProperty<T> > source_prop=source_cont->FindPropertyEx<T>(source_property);
 if(!source_prop)
  return false;

 return dest_prop->AttachTo(source_prop);
}

/// ��������� �� �������� destination_property ������ �������� ������� ����������
template<typename T>
void UContainer::DetachPropertyData(const NameT& destination_property)
{
 UEPtr<UVBaseDataProperty<T> > dest=FindPropertyEx<T>(destination_property);
 if(dest)
  dest->DetachFrom();
}


/// ���������� ��� ������������� ����������
template<typename T,class OwnerT>
UItem* UVProperty<T, OwnerT>::GetItem(int index)
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return reinterpret_cast<UItem*>(ConnectedOutputs[index]->GetOwner());
 }
 return 0;
}

template<typename T,class OwnerT>
std::string UVProperty<T, OwnerT>::GetItemOutputName(int index) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetName();
 }
 return std::string();
}

/// ���������� ��� ������������� ����������
template<typename T,class OwnerT>
std::string UVProperty<T, OwnerT>::GetItemName(int index) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetOwner()->GetName();
 }
 return std::string();
}

template<typename T,class OwnerT>
std::string UVProperty<T, OwnerT>::GetItemFullName(int index) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetOwner()->GetFullName();
 }
 return std::string();
}

}

#include "UPointer.h"


#endif

