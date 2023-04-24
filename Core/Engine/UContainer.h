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
#include "UProperty.h"
#include "ULocalProperty.h"
#include "UShare.h"
#include "UTime.h"
#include "UController.h"

namespace RDK {

class UContainer;

// Класс описания локальных указателей
class RDK_LIB_TYPE UIPointer
{
protected: // Данные

public:
virtual UEPtr<UContainer> const Get(void) const=0;

virtual void Set(UEPtr<UContainer> source)=0;

virtual void Del(UEPtr<UContainer> source)=0;

// Проверяет, существует ли такой указатель в этом классе
// Возвращает 0 если да, и <0 если нет
virtual int Find(UEPtr<const UContainer> cont) const=0;

// -----------------
// Операторы
// -----------------
UIPointer& operator = (UEPtr<UContainer> source)
{
 Set(source);
 return *this;
};
// -----------------
};


// Хранилище свойств указателя
struct RDK_LIB_TYPE UPVariable
{
 // Id указателя
 UId Id;

 // Указатель на свойство
 UEPtr<UIPointer> Pointer;

// --------------------------
// Конструкторы и деструкторы
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

// Массив указателей на контейнеры
typedef std::vector<UEPtr<UContainer> > UAContainerVector;

// Массив статических компонент
typedef std::map<UEPtr<UContainer>, NameT> UAStaticContainerMap;

class RDK_LIB_TYPE UContainer: public UComponent
{
public: // Типы данных
typedef std::map<NameT,UPVariable> PointerMapT;
typedef std::map<NameT,UPVariable>::iterator PointerMapIteratorT;
typedef std::map<NameT,UPVariable>::const_iterator PointerMapCIteratorT;

//friend class UStorage;
friend class UController;

private: // Системные свойства
// Таблица соответствий имен и Id компонент объекта
std::map<NameT,UId> CompsLookupTable;

// Таблица локальных указателей на дочерние компоненты
PointerMapT PointerLookupTable;

private: // Системные свойства
// Таблица компонент
UAContainerVector Components;

/// Таблица статических компонент
UAStaticContainerMap StaticComponents;

// Таблица контроллеров интерфейса
std::vector<UEPtr<UController> > Controllers;

protected: // Основные свойства

protected: // Общедоступные свойства
// Имя объекта
NameT Name;

// Id объекта
UId Id;

// Флаг активности объекта
// true - расчет объекта будет выполняться
// false - расчет объекта будет игнорироваться
bool Activity;

// Шаг счета в долях секунды
// Реальный шаг = 1./TimeStep
UTime TimeStep;

/// Максимально допустимое время расчета компонента вместе с дочерними компонентами
/// в миллисекундах.
/// Если время расчета превышено, то расчет последующих дочерних компонент
/// не выполняется
/// Если значение параметра <0, то нет ограничений
long long MaxCalculationDuration;

/// Время расчета компонента вместе с дочерними компонентами
/// в миллисекундах, по превышении которого выдается предупреждающее сообщение в лог.
/// Если значение параметра <0, то нет ограничений
long long CalculationDurationThreshold;

public: //
/// Флаг включения мониторинга утечки памяти
ULProperty<bool, UContainer, ptPubParameter | pgSystem> MemoryMonitor;


protected: // Физические свойства
// Координата компонента в пространстве сети
RDK::MVector<double,3> Coord;

// Время, затраченное на обработку объекта
// (без учета времени обсчета дочерних объектов) (мс)
unsigned long long StepDuration;

// Время, прошедшее между двумя последними итерациями счета
unsigned long long InterstepsInterval;

/// Флаги переопределения настроек вывода детальной отладочной информации
unsigned int DebugSysEventsMask;

protected: // Временные переменные
// Если 'TimeStep' > 'Owner->TimeStep' то 'CalcCounter' является
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

// Указатель на этот объект в хранилище
//UEPtr<UInstancesStorageElement> ObjectIterator;

// Последний использованный Id компонент
UId LastId;

// Флаг запроса на пропуск обсчета компонент в текущей итерации расчетов
bool SkipComponentCalculation;

// Флаг запроса на повторный обсчет компонент в текущей итерации расчетов сначала
bool ComponentReCalculation;

// Время начала счета компонента на текущем шаге
unsigned long long StartCalcTime;

// Время окончания счета компонента на предыдущем шаге
unsigned long long LastCalcTime;

/// Флаг, выставляемый на время перемещения компонента
bool MovingFlag;

/// Объем потребленной памяти за шаг расчета.
/// Может быть отрицательрным если память освобождалась.
/// Актуально если включен флаг MemoryMonitor
long long MemoryUsageDiff;

/// Изменение максимально длинного куска доступной памяти после шага расчета
/// Может быть отрицательрным если кусок увеличился.
/// Актуально если включен флаг MemoryMonitor
long long MaxMemoryBlockDiff;

protected:
/// Список свойств, выводимых в детальный лог
std::vector<std::string> PropertiesForDetailedLog;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
public:
UContainer(void);
virtual ~UContainer(void);
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта
UEPtr<UContainer> GetOwner(void) const;

// Возвращает указатель на главного владельца этим объектом
UEPtr<UContainer> GetMainOwner(void) const;

// Возвращает хранилище компонент этого объекта
UEPtr<UStorage> const GetStorage(void) const;

// Проверяет, является ли объект owner
// владельцем этого объекта на каком-либо уровне иерархии
bool CheckOwner(UEPtr<UContainer> owner) const;

// Возвращает полный Id объекта
// (включая Id всех владельцев)
ULongId& GetFullId(ULongId &buffer) const;
ULongId GetFullId(void) const;

// Возвращает  'длинный' Id объекта
// (исключая имя владельца 'mainowner')
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

// Указатель на логгер
virtual bool SetLogger(UEPtr<ULoggerEnv> logger);

// Вызов обработчика исключений среды
virtual void ProcessException(UException &exception);

// Вызов обработчика исключений среды для простой записи данных в лог
virtual void LogMessage(int msg_level, const std::string &line, int error_event_number=0);
virtual void LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &method_name, int line, int error_event_number=0);

virtual void LogDebugSysMessage(unsigned long long debug_sys_msg_type, unsigned long long modifier);

/// Логирует свойства при входе в расчет (входы, параметры, состояния)
virtual void LogPropertiesBeforeCalc(void);

/// Логирует свойства при выходе из расчета (выходы)
virtual void LogPropertiesAfterCalc(void);

/// Возвращает состояние флага режима отладки
virtual bool CheckDebugMode(void) const;

/// Формирует список свйоств для детального лога из строки
/// Разделитель - запятая
void SetPropertiesForDetailedLog(const std::string &str);
// --------------------------

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


// Возвращает базовый класс свойства с типом данных
template<typename T>
const UEPtr<UVBaseDataProperty<T> > FindPropertyEx(const NameT &name) const;

template<typename T>
UEPtr<UVBaseDataProperty<T> > FindPropertyEx(const NameT &name);

/// Подключает к свойству destination_property данные свойства другого компонента
template<typename T>
bool AttachPropertyData(const NameT& destination_property, const NameT& source_component, const NameT &source_property);

/// Отключает от свойства destination_property данные свойства другого компонента
template<typename T>
void DetachPropertyData(const NameT& destination_property);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
public:
// Координата компонента в пространстве сети
const RDK::MVector<double,3>& GetCoord(void) const;
bool SetCoord(const RDK::MVector<double,3> &value);

// Время, затраченное на обработку объекта
// (без учета времени обсчета дочерних объектов) (мс)
unsigned long long GetStepDuration(void) const;

// Время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
unsigned long long GetFullStepDuration(void) const;

// Время, прошедшее между двумя последними итерациями счета
unsigned long long GetInterstepsInterval(void) const;

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double GetInstantPerformance(void) const;

// Устанавливает величину шага интегрирования
const UTime& GetTimeStep(void) const;
bool SetTimeStep(const UTime &timestep);

// Устанавливает величину шага интегрирования компоненту и всем его дочерним компонентам
bool SetGlobalTimeStep(UTime timestep);

/// Переключает режим использования индивидуального TimeStep для компонента и всех дочерних компонент
/// Предназначено только для вызова из UEnvironment
void ChangeUseIndTimeStepMode(bool value);

// Устанавливает флаг активности объекта
const bool& GetActivity(void) const;
virtual bool SetActivity(const bool &activity);

// Id объекта
const UId& GetId(void) const;
bool SetId(const UId &id);

// Проверяет предлагаемое имя 'name' на уникальность в рамках данного объекта
bool CheckName(const NameT &name);

// Проверяет предлагаемое имя 'name' на синтаксическую корректность
bool ValidateName(const NameT &name);

// Генерирует имя уникальное в компонентах этого объекта
virtual NameT& GenerateName(const NameT &prefix, NameT &namebuffer);

// Устанавливает имя объекта
const NameT& GetName(void) const;
bool SetName(const NameT &name);


// Возвращает полное имя объекта
// (включая имена всех владельцев)
NameT& GetFullName(NameT &buffer) const;
NameT GetFullName(void) const;

// Возвращает  'длинное' имя объекта
// (исключая имя владельца 'mainowner').
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
bool SetMaxCalculationDuration(const long long &value);

/// Время расчета компонента вместе с дочерними компонентами
/// в миллисекундах, по превышении которого выдается предупреждающее сообщение в лог.
/// Если значение параметра <0, то нет ограничений
const long long& GetCalculationDurationThreshold(void) const;
bool SetCalculationDurationThreshold(const long long& value);

/// Флаги переопределения настроек вывода детальной отладочной информации
const unsigned int& GetDebugSysEventsMask(void) const;
bool SetDebugSysEventsMask(const unsigned int &value);

bool SetMemoryMonitor(const bool &value);

/// Объем потребленной памяти за шаг расчета.
/// Может быть отрицательрным если память освобождалась.
/// Актуально если включен флаг MemoryMonitor
long long GetMemoryUsageDiff(void) const;

/// Изменение максимально длинного куска доступной памяти после шага расчета
/// Может быть отрицательрным если кусок увеличился.
/// Актуально если включен флаг MemoryMonitor
long long GetMaxMemoryBlockDiff(void) const;
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
public:
// Возвращает имя дочернего компонента по его Id
const NameT& GetComponentName(const UId &id) const;

// Возвращает Id дочернего компонента по его имени
const UId& GetComponentId(const NameT &name, bool nothrow=false) const;

// Возвращает имя локального указателя по его Id
const NameT& GetPointerName(const UId &id) const;

// Возвращает Id локального указателя по его имени
const UId& GetPointerId(const NameT &name) const;

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

// Осуществляет поиск всех компонент приводимых к заданному C++ типу
// и возвращает вектор длинных имен компонент относительно текущего либо пустой вектор
// find_all
// false - искать в текущей компоненте
// true -  искать в текущей компоненте и глубже
// имена возвращются относительно уровня net
template<class T>
const vector<NameT>& GetComponentsNameByClassType(vector<NameT> &buffer, UEPtr<UContainer> net=0, bool find_all=false);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual UContainer* New(void)=0;

// Создает экземпляр описания класса
//virtual UContainerDescription* NewDescription(void);

// Создает копию этого объекта с сохранением всех компонент
// и значений параметров.
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual UEPtr<UContainer> Alloc(UEPtr<UStorage> stor, bool copystate=false);

// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
virtual void Free(void);

// Указатель на этот объект в хранилище
//UEPtr<UInstancesStorageElement> GetObjectIterator(void);
//void SetObjectIterator(UEPtr<UInstancesStorageElement> value);

protected:
/// Осуществляет обновление внутренних данных компонента, обеспечивающих его целостность
virtual void AUpdateInternalData(void);
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
public:
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

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому Id 'id'
// Если id == ForbiddenId то возвращает указатель на этот компонент
// Если nothrow == true то возвращает 0 и не кидает исключение
virtual UEPtr<UContainer> GetComponent(const UId &id, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponent(const UId &id, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponent(id,nothrow));
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому имени 'name'
virtual UEPtr<UContainer> GetComponent(const NameT &name, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponent(const NameT &name, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponent(name,nothrow));
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по ДЛИННОМУ Id 'id'
// Если id[0] == ForbiddenId или Id имеет нулевой размер,
// то возвращает указатель на этот компонент
UEPtr<UContainer> GetComponentL(const ULongId &id, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponentL(const ULongId &id, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponentL(id,nothrow));
}

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по ДЛИННОМУ имени 'name'
virtual UEPtr<UContainer> GetComponentL(const NameT &name, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponentL(const NameT &name, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponentL(name,nothrow));
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
// Таким образом 'id' - должно быть коротким Id
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
//virtual void DelComponent(const UId &id, bool canfree=true);

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
void DelComponent(const NameT &name, bool canfree=true);

// Принудительно удаляет все дочерние компоненты
void DelAllComponents(void);
protected:
void DelAllComponentsRaw(void);

public:
/// Добавляет компонент как статическую переменную задавая ему имя класса 'classname'
/// и имя 'name'
virtual void AddStaticComponent(const NameT &classname, const NameT &name, UEPtr<UContainer> comp);

/// Удаляет компонент как статическую переменную
virtual void DelStaticComponent(UEPtr<UContainer> comp);

/// Удаляет компонент как статическую переменную
virtual void DelAllStaticComponents(void);

/// Возвращает указатель на static компонент
/// с классом 'classname' и именем 'name'
virtual UEPtr<UContainer> FindStaticComponent(const NameT &classname, const NameT &name) const;

/// Перемещает компоненту в другой компонент
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

// Устанавливает компонент с текущим индексом index или именем 'name' на
// заданную позицию
// Применяется для изменения порядка расчета компонент
virtual bool SetComponentPosition(int index, int new_position);
virtual bool SetComponentPosition(const NameT &name, int new_position);

/// Флаг, выставляемый на время перемещения компонента
bool IsMoving(void) const;
// --------------------------

// ----------------------
// Методы управления коммуникационными компонентами
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

// --------------------------
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

// --------------------------
// Методы управления общими (shared) переменными
// --------------------------
// Метод инициализации общих переменных. Вызывается автоматически при добавлении
// объекта владельцу
virtual void SharesInit(void);

// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
virtual void SharesUnInit(void);
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool Default(void);

/// Метод сброса параметров на значения по умолчанию
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

// Устанавливает компоненты в требуемый порядок расчета
virtual void UpdateComputationOrder(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
virtual void AInit(void);
virtual void AUnInit(void);
// --------------------------

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

// --------------------------
// Скрытые методы управления таблицей компонент
// --------------------------
// Добавляет компонент 'comp' в таблицу компонент
void AddComponentTable(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Удаляет компонент 'comp' из таблицы компонент
void DelComponentTable(UEPtr<UContainer> comp);
// --------------------------

// --------------------------
// Скрытые методы управления общими (shared) переменными
// --------------------------
protected:
// Метод инициализации общих переменных. Вызывается автоматически при добавлении
// объекта владельцу
virtual void ASharesInit(void);

// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
virtual void ASharesUnInit(void);
// --------------------------

protected:
// --------------------------
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

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
protected:
/// Производит необходимые операции по добавлению статического компонента
UId UpdateStaticComponent(const NameT &classname, UEPtr<UContainer> comp);

// Удаляет компонент comp
// Метод предполагает, что компонент принадлежит объекту
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

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Обновляет состояние MainOwner после расчета этого объекта
virtual void AUpdateMainOwner(void);
// --------------------------

public: // Классы описания исключений
/* Базовый класс исключений */
class EIContainer
{
public: // Данные исключения
// Короткое имя компонента в котором сгенерировано исключение
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


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EIContainer(void);
explicit EIContainer(const UContainer *cont);
EIContainer(const EIContainer &copy);
virtual ~EIContainer(void);
// --------------------------

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

// Имя компонента недопустимо
struct EComponentNameInvalid: public ENameError
{
explicit EComponentNameInvalid(const std::string &name) : ENameError(name) {};
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
// Id указателя уже существует
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
 public: // Данные исключения
 // Короткое имя дочернего компонента в котором произошла ошибка счета
 std::string SubName;

 // Короткий идентификатор дочернего компонента в котором произошла ошибка счета
 ULongId SubId;

 // --------------------------
 // Конструкторы и деструкторы
 // --------------------------
 EICalculateContainer(void);
 EICalculateContainer(const UContainer *cont, const UContainer *subcont);
 EICalculateContainer(const EICalculateContainer &copy);
 virtual ~EICalculateContainer(void);
 // --------------------------

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
 // --------------------------
 // Конструкторы и деструкторы
 // --------------------------
 EComponentCalculate(void);
 EComponentCalculate(const UContainer *cont, const UContainer *subcont);
 EComponentCalculate(const EComponentCalculate &copy);
 virtual ~EComponentCalculate(void) throw();
 // --------------------------

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

 // --------------------------
 // Конструкторы и деструкторы
 // --------------------------
 EComponentSystemException(void);
 EComponentSystemException(const UContainer *cont, const UContainer *subcont, const std::string &info);
 EComponentSystemException(const EComponentSystemException &copy);
 virtual ~EComponentSystemException(void) throw();
 // --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};
/* **************************** */

};

/// Функция подготавливает строку для логирования
bool PreparePropertyLogString(const UVariable& variable, unsigned int expected_type, std::string &result);


// Осуществляет поиск всех компонент приводимых к заданному C++ типу
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

// Возвращает базовый класс свойства с типом данных
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


/// Подключает к свойству destination_property данные свойства другого компонента
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

/// Отключает от свойства destination_property данные свойства другого компонента
template<typename T>
void UContainer::DetachPropertyData(const NameT& destination_property)
{
 UEPtr<UVBaseDataProperty<T> > dest=FindPropertyEx<T>(destination_property);
 if(dest)
  dest->DetachFrom();
}


}

#include "UPointer.h"
#include "UShare.h"

#endif

