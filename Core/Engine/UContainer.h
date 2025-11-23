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

// ����� �������� ��������� ����������
class RDK_LIB_TYPE UIPointer
{
protected: // ������

public:
virtual ~UIPointer();
virtual UEPtr<UContainer> const Get(void) const=0;

virtual void Set(UEPtr<UContainer> source)=0;

virtual void Del(UEPtr<UContainer> source)=0;

// Возвращает 0 если да, и <0 если нет
// ���������� 0 ���� ��, � <0 ���� ���
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


// ��������� ������� ���������
struct RDK_LIB_TYPE UPVariable
{
 // Id ���������
 UId Id;

 // ��������� �� ��������
 UEPtr<UIPointer> Pointer;

// Конструкторы и деструкторы
// --------------------------
// --------------------------
UPVariable(void);
UPVariable(UId id, UEPtr<UIPointer> prop);
virtual ~UPVariable(void);
// --------------------------
};


class UInstancesStorageElement;

typedef long int IndexT;
//class UContainer;
typedef UContainer* PUAContainer;

// ������ ���������� �� ����������
typedef std::vector<UEPtr<UContainer> > UAContainerVector;

// ������ ����������� ���������
typedef std::map<UEPtr<UContainer>, NameT> UAStaticContainerMap;

class RDK_LIB_TYPE UContainer: public UComponent
{
public: // ���� ������
typedef std::unordered_map<NameT,UPVariable> PointerMapT;
typedef std::unordered_map<NameT,UPVariable>::iterator PointerMapIteratorT;
typedef std::unordered_map<NameT,UPVariable>::const_iterator PointerMapCIteratorT;

//friend class UStorage;
friend class UController;

private: // Таблица соответствий имен и Id компонент объекта
// ������� ������������ ���� � Id ��������� �������
std::unordered_map<NameT,UId> CompsLookupTable;

// ������� ��������� ���������� �� �������� ����������
PointerMapT PointerLookupTable;

private: // Таблица компонент
// ������� ���������
UAContainerVector Components;

// Index map for O(1) component lookup by ID
std::unordered_map<UId, size_t> ComponentsIdIndex;

// Cache for last successful dynamic_pointer_cast in GetComponent<T>
mutable UEPtr<UContainer> CachedComponent;
mutable UId CachedComponentId;
mutable std::type_index CachedComponentType;

/// Маска для фильтрации системных событий компонента
/// ������� ����������� ���������
UAStaticContainerMap StaticComponents;

// ������� ������������ ����������
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

/// ����� ��������������� �������� ������ ��������� ���������� ����������
/// Маска для фильтрации системных событий компонента
UProperty<unsigned int, UContainer, ptPubParameter | static_cast<unsigned int>(pgSystem)> DebugSysEventsMask;

protected: // Время, прошедшее между двумя последними итерациями счета
// �����, ��������� ����� ����� ���������� ���������� �����
unsigned long long InterstepsInterval;

// счетчиком текущего интервала ожидания.
// В противном случае 'CalcCounter' не ипользуется
// � ��������� ������ 'CalcCounter' �� �����������
int CalcCounter;

// объекта или величину шага счета этого объекта
// если родительского объекта не существует
// ���� ������������� ������� �� ����������
UTime OwnerTimeStep;

// ��������� �� 0-� ������� ������� ���������
UEPtr<UContainer>* PComponents;

// ���������� ��������� � ������� ���������
int NumComponents;

//UEPtr<UInstancesStorageElement> ObjectIterator;
//UEPtr<UInstancesStorageElement> ObjectIterator;

// ��������� �������������� Id ���������
UId LastId;

// ���� ������� �� ������� ������� ��������� � ������� �������� ��������
bool SkipComponentCalculation;

// ���� ������� �� ��������� ������ ��������� � ������� �������� �������� �������
bool ComponentReCalculation;

// ����� ������ ����� ���������� �� ������� ����
unsigned long long StartCalcTime;

// ����� ��������� ����� ���������� �� ���������� ����
unsigned long long LastCalcTime;

/// ����, ������������ �� ����� ����������� ����������
bool MovingFlag;

/// Может быть отрицательрным если память освобождалась.
/// Актуально если включен флаг MemoryMonitor
/// ��������� ���� ������� ���� MemoryMonitor
long long MemoryUsageDiff;

/// Может быть отрицательрным если кусок увеличился.
/// Актуально если включен флаг MemoryMonitor
/// ��������� ���� ������� ���� MemoryMonitor
long long MaxMemoryBlockDiff;

protected:
/// ������ �������, ��������� � ��������� ���
std::vector<std::string> PropertiesForDetailedLog;

// Конструкторы и деструкторы
// --------------------------
// --------------------------
public:
UContainer(void);
virtual ~UContainer(void);
// --------------------------

// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта
// ���������� �������� ����� �������
UEPtr<UContainer> GetOwner(void) const;

// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UContainer> GetMainOwner(void) const;

// ���������� ��������� ��������� ����� �������
UEPtr<UStorage> const GetStorage(void) const;

// владельцем этого объекта на каком-либо уровне иерархии
// ���������� ����� ������� �� �����-���� ������ ��������
bool CheckOwner(UEPtr<UContainer> owner) const;

// (включая Id всех владельцев)
// (������� Id ���� ����������)
ULongId& GetFullId(ULongId &buffer) const;
ULongId GetFullId(void) const;

// (исключая имя владельца 'mainowner')
// Метод возвращает пустой вектор, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии
// ���������� ������� �� �� ����� ������ ��������
ULongId& GetLongId(UEPtr<UContainer> mainowner, ULongId &buffer) const;
ULongId GetLongId(UEPtr<UContainer> mainowner) const;
// ������������� ������� ������������ ������, ������������� ������� ���
std::string& GetLongId(UEPtr<UContainer> mainowner, std::string &buffer) const;

// ���������� true ���� ������������ ������������� ������� ���������, � ��������� ������ ���������� false
bool CheckLongId(const ULongId &id) const;
// ������������� ������� ������������ ������, ��������������� ������� ���
bool CheckLongId(const std::string &id) const;

// ���������� ������ ���������� ����� �������
virtual bool SetEnvironment(UEPtr<UEnvironment> environment);

// ��������� �� ������
virtual bool SetLogger(UEPtr<ULoggerEnv> logger);

// ����� ����������� ���������� �����
virtual void ProcessException(UException &exception);

// ����� ����������� ���������� ����� ��� ������� ������ ������ � ���
virtual void LogMessage(int msg_level, const std::string &line, int error_event_number=0);
virtual void LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &method_name, int line, int error_event_number=0);

virtual void LogDebugSysMessage(unsigned long long debug_sys_msg_type, unsigned long long modifier);

/// �������� �������� ��� ����� � ������ (�����, ���������, ���������)
virtual void LogPropertiesBeforeCalc(void);

/// �������� �������� ��� ������ �� ������� (������)
virtual void LogPropertiesAfterCalc(void);

/// ���������� ��������� ����� ������ �������
virtual bool CheckDebugMode(void) const;

/// Разделитель - запятая
/// ����������� - �������
void SetPropertiesForDetailedLog(const std::string &str);
// --------------------------

// Методы управления свойствами
// --------------------------
// Удаляет владельца объекта
// ������� ��������� �������
void BreakOwner(void);

// 'levels'. Если levels < 0 то устанавливается компонентам на всех уровнях
// 'levels'. ���� levels < 0 �� ��������������� ����������� �� ���� �������
void SetMainOwner(UEPtr<UComponent> mainowner);
void SetMainOwner(UEPtr<UComponent> mainowner, int levels);

// ��������� ������������ Id 'id' �� ������������ � ������ �������, �������.
bool CheckId(const UId &id);

// ���������� ���������� Id
virtual UId GenerateId(void);


// ���������� ������� ����� �������� � ����� ������
template<typename T>
const UEPtr<UVBaseDataProperty<T> > FindPropertyEx(const NameT &name) const;

template<typename T>
UEPtr<UVBaseDataProperty<T> > FindPropertyEx(const NameT &name);

/// ���������� � �������� destination_property ������ �������� ������� ����������
template<typename T>
bool AttachPropertyData(const NameT& destination_property, const NameT& source_component, const NameT &source_property);

/// ��������� �� �������� destination_property ������ �������� ������� ����������
template<typename T>
void DetachPropertyData(const NameT& destination_property);
// --------------------------

// Методы управления общедоступными свойствами
// --------------------------
// --------------------------
public:
// ���������� ���������� � ������������ ����
const RDK::MVector<double,3>& GetCoord(void) const;

// (без учета времени обсчета дочерних объектов) (мс)
// (��� ����� ������� ������� �������� ��������) (��)
unsigned long long GetStepDuration(void) const;

// (вместе со времени обсчета дочерних объектов) (мс)
// (������ �� ������� ������� �������� ��������) (��)
unsigned long long GetFullStepDuration(void) const;

// �����, ��������� ����� ����� ���������� ���������� �����
unsigned long long GetInterstepsInterval(void) const;

// полного затраченного времени к ожидаемому времени шага счета
// ������� ������������ ������� � ���������� ������� ���� �����
double GetInstantPerformance(void) const;

// ������������� �������� ���� ��������������
const UTime& GetTimeStep(void) const;

// ������������� �������� ���� �������������� ���������� � ���� ��� �������� �����������
bool SetGlobalTimeStep(UTime timestep);

/// Предназначено только для вызова из UEnvironment
/// ������������� ������ ��� ������ �� UEnvironment
void ChangeUseIndTimeStepMode(bool value);

// ������������� ���� ���������� �������
const bool& GetActivity(void) const;

// Id �������
UId GetId(void) const;

// ��������� ������������ ��� 'name' �� ������������ � ������ ������� �������
bool CheckName(const NameT &name);

// ��������� ������������ ��� 'name' �� �������������� ������������
bool ValidateName(const NameT &name);

// ���������� ��� ���������� � ����������� ����� �������
virtual NameT& GenerateName(const NameT &prefix, NameT &namebuffer);

// ������������� ��� �������
const NameT& GetName(void) const;

// (включая имена всех владельцев)
// (������� ����� ���� ����������)
NameT& GetFullName(NameT &buffer) const;
NameT GetFullName(void) const;

// (исключая имя владельца 'mainowner').
// Метод возвращает пустую строку, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии
// ���������� ������� �� �� ����� ������ ��������
NameT& GetLongName(const UEPtr<UContainer> &mainowner, NameT &buffer) const;
NameT GetLongName(const UEPtr<UContainer> &mainowner) const;

/// в миллисекундах.
/// Если время расчета превышено, то расчет последующих дочерних компонент
/// не выполняется
/// Если значение параметра <0, то нет ограничений
/// ���� �������� ��������� <0, �� ��� �����������
const long long& GetMaxCalculationDuration(void) const;

/// в миллисекундах, по превышении которого выдается предупреждающее сообщение в лог.
/// Если значение параметра <0, то нет ограничений
/// ���� �������� ��������� <0, �� ��� �����������
const long long& GetCalculationDurationThreshold(void) const;

/// ����� ��������������� �������� ������ ��������� ���������� ����������
const unsigned int& GetDebugSysEventsMask(void) const;

/// Может быть отрицательрным если память освобождалась.
/// Актуально если включен флаг MemoryMonitor
/// ��������� ���� ������� ���� MemoryMonitor
long long GetMemoryUsageDiff(void) const;

/// Может быть отрицательрным если кусок увеличился.
/// Актуально если включен флаг MemoryMonitor
/// ��������� ���� ������� ���� MemoryMonitor
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
// ���������� ��� ��������� ���������� �� ��� Id
const NameT& GetComponentName(const UId &id) const;

// ���������� Id ��������� ���������� �� ��� �����
const UId& GetComponentId(const NameT &name, bool nothrow=false) const;

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& GetPointerName(const UId &id) const;

// ���������� Id ���������� ��������� �� ��� �����
const UId& GetPointerId(const NameT &name) const;

// и возвращает вектор компонент либо пустой вектор
// find_all
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
// true -  ������ � ������� ���������� � ������
const vector<UEPtr<UContainer> >& GetComponentsByClassName(const NameT &name, vector<UEPtr<UContainer> > &buffer, bool find_all=false);

// и возвращает вектор длинных имен компонент относительно текущего либо пустой вектор
// find_all
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
// true -  ������ � ������� ���������� � ������
const vector<NameT>& GetComponentsNameByClassName(const NameT &name, vector<NameT> &buffer, bool find_all=false);

// и возвращает вектор длинных имен компонент относительно текущего либо пустой вектор
// find_all
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
// имена возвращются относительно уровня net
// ����� ����������� ������������ ������ net
template<class T>
const vector<NameT>& GetComponentsNameByClassType(vector<NameT> &buffer, UEPtr<UContainer> net=0, bool find_all=false);
// --------------------------

public:
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UContainer* New(void)=0;

//virtual UContainerDescription* NewDescription(void);
//virtual UContainerDescription* NewDescription(void);

// и значений параметров.
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
// � ��� �� ��������� ��� ������������� ���� ������
virtual UEPtr<UContainer> Alloc(UEPtr<UStorage> stor, bool copystate=false);

// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
// � ��� �� ��������� ��� ������������� ���� ������
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;

// или вызов деструктора, если Storage == 0
// ��� ����� �����������, ���� Storage == 0
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
// ���������� ����� �������� ���������
int GetNumComponents(void) const;

// (включая все компоненты дочерних компонент)
// (������� ��� ���������� �������� ���������)
int GetNumAllComponents(void) const;

bool CheckComponent(const NameT &name);
bool CheckComponentL(const NameT &name);

// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;

// объекте по короткому Id 'id'
// Если id == ForbiddenId то возвращает указатель на этот компонент
// Если nothrow == true то возвращает 0 и не кидает исключение
// ���� nothrow == true �� ���������� 0 � �� ������ ����������
virtual UEPtr<UContainer> GetComponent(const UId &id, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponent(const UId &id, bool nothrow=false) const
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
 UEPtr<UContainer> comp = GetComponent(id, nothrow);
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
// ������� �� ��������� ����� 'name'
virtual UEPtr<UContainer> GetComponent(const NameT &name, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponent(const NameT &name, bool nothrow=false) const
{
 // Get base component by name
 UEPtr<UContainer> comp = GetComponent(name, nothrow);
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
// �� ���������� ��������� �� ���� ���������
UEPtr<UContainer> GetComponentL(const ULongId &id, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponentL(const ULongId &id, bool nothrow=false) const
{
 // Get base component by long ID
 UEPtr<UContainer> comp = GetComponentL(id, nothrow);
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
// ������� �� �������� ����� 'name'
virtual UEPtr<UContainer> GetComponentL(const NameT &name, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponentL(const NameT &name, bool nothrow=false) const
{
 // Get base component by long name
 UEPtr<UContainer> comp = GetComponentL(name, nothrow);
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

// объекте по порядковому индеку в списке компонент
// Метод возвращает 0, если индекс выходит за границы массива
// ����� ���������� 0, ���� ������ ������� �� ������� �������
UEPtr<UContainer> GetComponentByIndex(int index) const;

template<class T>
UEPtr<T> GetComponentByIndex(int index) const
{
 return dynamic_pointer_cast<T>(GetComponentByIndex(index));
}

// Возвращает его Id или ForbiddenId если добавление неудачно
// Может быть передан указатель на локальную переменную
// ����� ���� ������� ��������� �� ��������� ����������
virtual void BeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual void AfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual UId AddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Удаляемый компонент должен содержаться именно в этом объекте.
// Таким образом 'id' - должно быть коротким Id
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
//virtual void DelComponent(const UId &id, bool canfree=true);
//virtual void DelComponent(const UId &id, bool canfree=true);

// Удаляемый компонент должен содержаться именно в этом объекте.
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
// ���� ��������, ���� ��������� �� �����������
void DelComponent(const NameT &name, bool canfree=true);

// ������������� ������� ��� �������� ����������
void DelAllComponents(void);
protected:
void DelAllComponentsRaw(void);

public:
/// и имя 'name'
/// � ��� 'name'
virtual void AddStaticComponent(const NameT &classname, const NameT &name, UEPtr<UContainer> comp);

/// ������� ��������� ��� ����������� ����������
virtual void DelStaticComponent(UEPtr<UContainer> comp);

/// ������� ��������� ��� ����������� ����������
virtual void DelAllStaticComponents(void);

/// с классом 'classname' и именем 'name'
/// � ������� 'classname' � ������ 'name'
virtual UEPtr<UContainer> FindStaticComponent(const NameT &classname, const NameT &name) const;

/// Если comp не принадлежит этому компоненту, или target имеет отличный от
/// этого компонента storage, или target не может принять в себя компонент
/// то возвращает false и не делает ничего
/// �� ���������� false � �� ������ ������
virtual bool MoveComponent(UEPtr<UContainer> comp, UEPtr<UContainer> target);

// в этом объекте
// Память должна быть выделена
// ������ ������ ���� ��������
void GetComponentsList(vector<UId> &buffer) const;
void GetComponentsList(vector<NameT> &buffer) const;

// Если хранилище stor != 0 то используется оно
// ���� ��������� stor != 0 �� ������������ ���
virtual void CopyComponents(UEPtr<UContainer> comp, UEPtr<UStorage> stor=0) const;

// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
// на эту границу
// �� ��� �������
virtual bool ChangeComponentPosition(int index, int step);
virtual bool ChangeComponentPosition(const NameT &name, int step);

// заданную позицию
// Применяется для изменения порядка расчета компонент
// ����������� ��� ��������� ������� ������� ���������
virtual bool SetComponentPosition(int index, int new_position);
virtual bool SetComponentPosition(const NameT &name, int new_position);

/// ����, ������������ �� ����� ����������� ����������
bool IsMoving(void) const;
// --------------------------

// Методы управления коммуникационными компонентами
// ----------------------
// ----------------------
public:
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
// ��������������� ������� ������ �� ������������.
virtual ULongIdVector& GetConnectorsList(ULongIdVector &buffer,
						  int sublevel=-1, UEPtr<UContainer> ownerlevel=0);

// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
// ��������������� ������� ������ �� ������������.
virtual ULongIdVector& GetItemsList(ULongIdVector &buffer,
							int sublevel=-1, UEPtr<UContainer> ownerlevel=0);

// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
// ��������������� ������� ������ �� ������������.
virtual ULongIdVector& GetNetsList(ULongIdVector &buffer,
							int sublevel=-1, UEPtr<UContainer> ownerlevel=0);
// ----------------------

// Методы управления локальными указателями
// --------------------------
// Устанавливает дочерний компонент 'id' в качестве заданного класса локальных указателей
// 'pointerid'
// 'pointerid'
bool SetComponentAs(const UId &id, const UId &pointerid);

// 'pointername'
// 'pointername'
bool SetComponentAs(const NameT &name,const NameT &pointername);

// 'pointerid'
// 'pointerid'
bool ResetComponentAs(const UId &id, const UId &pointerid);

// 'pointername'
// 'pointername'
bool ResetComponentAs(const NameT &name,const NameT &pointername);

// ���������� ��������� ���������� ���������� 'name' �� ���� ������� ��������� ����������
bool ResetComponentAll(const UId &id);

// ���������� ��������� ���������� ���������� 'name' �� ���� ������� ��������� ����������
bool ResetComponentAll(const NameT &name);

// ������� ��� ���������� ����������� � ��������� ������ ��������� ����������
void DelAllComponentsAs(const NameT &pointername, bool canfree=true);
// --------------------------

// Методы управления общими (shared) переменными
// --------------------------
// Метод инициализации общих переменных. Вызывается автоматически при добавлении
// объекта владельцу
// ������� ���������
virtual void SharesInit(void);

// объекта из владельца
// ������� �� ���������
virtual void SharesUnInit(void);
// --------------------------

// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool Default(void);

/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
/// ���� subcomps == true �� ����� ���������� ��������� ���� �������� ���������
virtual bool DefaultAll(UContainer* cont, bool subcomps);

// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
// � ������ �������� ������
virtual bool Build(void);

// ����� �������� ����� ��� ������ ��������
virtual bool Reset(void);

// ��������� ������ ����� �������
virtual bool Calculate(void);

// ��������� ��������� ������������� ����� �������
virtual void Init(void);

// ��������� ��������������� ����� �������
virtual void UnInit(void);

// ��������� ��������� MainOwner ����� ������� ����� �������
virtual void UpdateMainOwner(void);

// компонент на этом шаге счета
// ��������� �� ���� ���� �����
virtual void ForceSkipComponentCalculation(void);

// компонент на этом шаге счета сначала
// ��������� �� ���� ���� ����� �������
virtual void ForceComponentReCalculation(void);

/// и если она превышает MaxCalculationDuration и MaxCalculationDuration>=0
/// то прерывает обсчет остальной цепочки дочерних компонент
/// �� ��������� ������ ��������� ������� �������� ���������
virtual bool CheckDurationAndSkipComponentCalculation(void);

// ������������� ���������� � ��������� ������� �������
virtual void UpdateComputationOrder(void);
// --------------------------

// Скрытые методы управления счетом
// --------------------------
// --------------------------
protected:
virtual void AInit(void);
virtual void AUnInit(void);
// --------------------------

// Скрытые методы управления контроллерами интерфейса
// Удаление контроллеров лежит на вызывающем модуле
// --------------------------
// --------------------------
protected:
// ��������� ����� ����������
void AddController(UEPtr<UController> controller, bool forchilds=false);

// ������� ���������� �� ������
void DelController(UEPtr<UController> controller, bool forchilds=false);

// ������� ��� �����������
void DelAllControllers(bool forchilds=false);

// ���������� ���������� ���� ������������
void UnLinkAllControllers(bool forchilds=false);

// ���������, ���������� �� ���������� � ������
bool CheckController(UEPtr<UController> controller) const;

// ���������� ����� ������������
size_t GetNumControllers(void) const;

// ���������� ���������� �� �������
UEPtr<UController> GetController(int index);
// --------------------------

// Скрытые методы управления локальными указателями
// --------------------------
// --------------------------
public:
// параметров и назначает ей корректный индекс
// Должна вызываться в конструкторах классов
// ������ ���������� � ������������� �������
UId AddLookupPointer(const NameT &name, UEPtr<UIPointer> pointer);

protected:
// ������� ��������� � ID 'id' �� ������� �����������
void DelLookupPointer(const NameT &name);

// ������������ ����� � ������� ���������, ���������������� ��������� ���������
PointerMapCIteratorT FindLookupPointer(UEPtr<UContainer> source) const;
// --------------------------

// Скрытые методы управления таблицей компонент
// --------------------------
// Добавляет компонент 'comp' в таблицу компонент
// ��������� ��������� 'comp' � ������� ���������
void AddComponentTable(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ������� ��������� 'comp' �� ������� ���������
void DelComponentTable(UEPtr<UContainer> comp);
// --------------------------

// Скрытые методы управления общими (shared) переменными
// --------------------------
// --------------------------
protected:
// объекта владельцу
// ������� ���������
virtual void ASharesInit(void);

// объекта из владельца
// ������� �� ���������
virtual void ASharesUnInit(void);
// --------------------------

protected:
// Скрытые методы управления таблицей соответсвий компонент
// --------------------------
// Обновляет таблицу соответствий компонент заменяя 'oldname'
// имя компонента на 'newname'
// ��� ���������� �� 'newname'
void ModifyLookupComponent(const NameT &oldname, const NameT &newname);

// для компонента с именем 'name'
// ��� ���������� � ������ 'name'
void SetLookupComponent(const NameT &name, const UId &id);

// компонента с именем 'name'
// ���������� � ������ 'name'
void DelLookupComponent(const NameT &name);
// --------------------------

// Скрытые методы управления компонентами
// --------------------------
// --------------------------
protected:
/// ���������� ����������� �������� �� ���������� ������������ ����������
UId UpdateStaticComponent(const NameT &classname, UEPtr<UContainer> comp);

// Метод предполагает, что компонент принадлежит объекту
// ����� ������������, ��� ��������� ����������� �������
virtual void BeforeDelComponent(UEPtr<UContainer> comp, bool canfree=true);
virtual void AfterDelComponent(UEPtr<UContainer> comp, bool canfree=true);
void DelComponent(UEPtr<UContainer> comp, bool canfree);

// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
// Может быть передан указатель на локальную переменную
// ����� ���� ������� ��������� �� ��������� ����������
virtual void ABeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual void AAfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
// ���������� � ������ ���������
virtual void ABeforeDelComponent(UEPtr<UContainer> comp, bool canfree);
virtual void AAfterDelComponent(UEPtr<UContainer> comp, bool canfree);
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------

// Скрытые методы управления счетом
// --------------------------
// --------------------------
protected:
// ��������� ��������� MainOwner ����� ������� ����� �������
virtual void AUpdateMainOwner(void);
// --------------------------

public: /* Базовый класс исключений */
/* ������� ����� ���������� */
class EIContainer
{
public: // Короткое имя компонента в котором сгенерировано исключение
// �������� ��� ���������� � ������� ������������� ����������
std::string Name;

// �������� ������������� ���������� � ������� ������������� ����������
ULongId Id;

// ������ ��� ��������� ���������� � ������� ������������� ����������
std::string OwnerName;

// ������ ������������� ��������� ���������� � ������� ������������� ����������
ULongId OwnerId;

// ������ ��� �������� ��������� ���������� � ������� ������������� ����������
std::string MainOwnerName;

// ������ ������������� �������� ��������� ���������� � ������� ������������� ����������
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
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Id ���������� �� ������
struct EComponentIdNotExist: public EIdNotExist
{
explicit EComponentIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id ���������� ��� ����������
struct EComponentIdAlreadyExist: public EIdAlreadyExist
{
explicit EComponentIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ��� ���������� �� �������
struct EComponentNameNotExist: public ENameNotExist
{
explicit EComponentNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ��� ���������� ��� ����������
struct EComponentNameAlreadyExist: public ENameAlreadyExist
{
explicit EComponentNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// ��� ���������� �����������
struct EComponentNameInvalid: public ENameError
{
explicit EComponentNameInvalid(const std::string &name) : ENameError(name) {};
};

// Id ��������� �� ������
struct EPointerIdNotExist: public EIdNotExist
{
explicit EPointerIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id ��������� ��� ����������
struct EPointerIdAlreadyExist: public EIdAlreadyExist
{
explicit EPointerIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ��� ��������� �� �������
struct EPointerNameNotExist: public ENameNotExist
{
explicit EPointerNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ��� ��������� ��� ����������
struct EPointerNameAlreadyExist: public ENameAlreadyExist
{
explicit EPointerNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// Id указателя уже существует
// Id ��������� ��� ����������
struct EAddComponentAlreadyHaveOwner: public EIdError
{
explicit EAddComponentAlreadyHaveOwner(UId id) : EIdError(id) {};
};

// ������������ ��� ��������� ����������
struct EAddComponentHaveInvalidType: public EIdError
{
explicit EAddComponentHaveInvalidType(UId id) : EIdError(id) {};
};

// ������������ ����� ��� ��������� ������ ����� ���������
class EICalculateContainer: public EIContainer
{
 public: // Короткое имя дочернего компонента в котором произошла ошибка счета
 // �������� ��� ��������� ���������� � ������� ��������� ������ �����
 std::string SubName;

 // �������� ������������� ��������� ���������� � ������� ��������� ������ �����
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
 // ��������� ������ ���� �� ����������
 virtual std::string CreateLogMessage(void) const;
 // --------------------------
};


// ���������� - ������
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
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ���������� - ��������� ������ ���������� ����������
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
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};
/* **************************** */

};

/// ������� �������������� ������ ��� �����������
bool PreparePropertyLogString(const UVariable& variable, unsigned int expected_type, std::string &result);


// и возвращает вектор длинных имен компонент относительно текущего либо пустой вектор
// find_all
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
// true -  ������ � ������� ���������� � ������
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

/// ���������� ��� ������������� ������
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

/// ���������� ������ ��� ������������� ����������
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

