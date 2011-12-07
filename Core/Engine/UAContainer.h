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

#include <vector>
#include <map>
#include "UAComponent.h"
#include "UEnvSupport.h"
#include "UController.h"
#include "../Math/MVector.h"

namespace RDK {

class UAContainerStorage;
class UInstancesStorageElement;

typedef long int IndexT;
typedef int UTime;
typedef long long ULongTime;

//class UAContainer;
typedef UAContainer* PUAContainer;

// Массив указателей на контейнеры
typedef std::vector<UEPtr<UAContainer> > UAContainerVector;

extern const UTime DefaultTimeStep;


class UAContainer: public UAComponent
{
public: // Типы данных
typedef std::map<NameT,UVariable> VariableMapT;
typedef std::map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

typedef std::map<NameT,UPVariable> PointerMapT;
typedef std::map<NameT,UPVariable>::iterator PointerMapIteratorT;
typedef std::map<NameT,UPVariable>::const_iterator PointerMapCIteratorT;

friend class UAContainerStorage;
friend class UController;

public: // Классы описания исключений
 /* Базовый класс описания исключений */
 class IException
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
 IException(void);
 IException(const UAContainer *cont);
 IException(const IException &copy);
 virtual ~IException(void);
 // --------------------------
 };
/* **************************** */


private: // Системные свойства
// Таблица соответствий имен и Id компонент объекта
std::map<NameT,UId> CompsLookupTable;

// Таблица соответствий имен и Id параметров объекта
VariableMapT PropertiesLookupTable;

// Таблица соответствий имен и Id данных состояния объекта
VariableMapT StateLookupTable;

// Таблица локальных указателей на дочерние компоненты
PointerMapT PointerLookupTable;

// Таблица соответствий <компонента в таблице Components, локальный указатель>
//std::map<UAContainer*,UAContainer**> ComponentPointers;

private: // Глобальные свойства
// Текущее время модели в микросекундах
static ULongTime Time;

// Текущее время модели в секундах
static double DoubleTime;

// Реальное время в микросекундах
static ULongTime RealTime;

// Реальное время в секундах
static double DoubleRealTime;

// Мгновенный шаг в реальном времени в микросекундах
static ULongTime RealTimeStep;

// Мгновенный шаг в реальном времени в секундах
static double DoubleRealTimeStep;

private: // Системные свойства

// Таблица компонент
UAContainerVector Components;

// Таблица контроллеров интерфейса
std::vector<UController*> Controllers;

// Таблица соответствий <компонента в таблице Components, локальный указатель>
//std::map<UAContainer*,UAContainer**> ComponentPointers;

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

public: // Физические свойства
// Координата компонента в пространстве сети
RDK::MVector<double> Coord;


protected: // Временные переменные
// Если 'TimeStep' > 'Owner->TimeStep' то 'CalcCounter' является
// счетчиком текущего интервала ожидания.
// В противном случае 'CalcCounter' не ипользуется
UTime CalcCounter;

// Хранит величину шага счета родительского
// объекта или величину шага счета этого объекта
// если родительского объекта не существует
UTime OwnerTimeStep;

// Указатель на 0-й элемент вектора компонент
UEPtr<UAContainer>* PComponents;

// Количество компонент в векторе компонент
int NumComponents;

// Указатель на этот объект в хранилище
UInstancesStorageElement* ObjectIterator;

// Последний использованный Id компонент
UId LastId;

public: // Открытые методы
// --------------------------
// Методы управления глобальными свойствами
// --------------------------
// Возвращает текущее время модели
static const ULongTime& GetTime(void);
static const double& GetDoubleTime(void);

// Устанавливает текущее время модели
static bool SetTime(ULongTime value);

// Возвращает реальное время
static const ULongTime& GetRealTime(void);
static const double& GetDoubleRealTime(void);

// Устанавливает реальное время
static bool SetRealTime(ULongTime value);

// Возвращает мгновенный шаг в реальном времени
static const ULongTime& GetRealTimeStep(void);
static const double& GetDoubleRealTimeStep(void);
// --------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAContainer(void);
virtual ~UAContainer(void);
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает владелца этого объекта
UEPtr<UAContainer> GetOwner(void) const;

// Возвращает указатель на главного владельца этим объектом
UEPtr<UAContainer> GetMainOwner(void) const;

// Возвращает хранилище компонент этого объекта
UAContainerStorage* const GetStorage(void) const;

// Проверяет, является ли объект owner
// владельцем этого объекта на каком-либо уровне иерархии
bool CheckOwner(UEPtr<UAContainer> owner) const;

// Возвращает полный Id объекта
// (включая Id всех владельцев)
ULongId& GetFullId(ULongId &buffer=ULongIdemp) const;

// Возвращает  'длинный' Id объекта
// (исключая имя владельца 'mainowner')
// Метод возвращает пустой вектор, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии
ULongId& GetLongId(UEPtr<UAContainer> mainowner, ULongId &buffer=ULongIdemp) const;
// --------------------------

// --------------------------
// Методы управления свойствами
// --------------------------
// Удаляет владельца объекта
bool BreakOwner(void);

// Указатель устанавливается на число уровней дочерних компонент
// 'levels'. Если levels < 0 то устанавливается компонентам на всех уровнях
bool SetMainOwner(UEPtr<UAComponent> mainowner);
bool SetMainOwner(UEPtr<UAComponent> mainowner, int levels);

// Проверяет предлагаемый Id 'id' на уникальность в рамках данного, объекта.
bool CheckId(const UId &id);

// Генерирует уникальный Id
virtual UId GenerateId(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
public:
// Координата компонента в пространстве сети
RDK::MVector<double> GetCoord(void) const;
bool SetCoord(RDK::MVector<double> value);

// Устанавливает величину шага интегрирования
UTime GetTimeStep(void) const;
bool SetTimeStep(UTime timestep);

// Устанавливает флаг активности объекта
bool GetActivity(void) const;
bool SetActivity(bool activity);

UId GetClass(void) const;
bool SetClass(UId value);

// Id объекта
const UId& GetId(void) const;
bool SetId(const UId &id);

// Проверяет предлагаемое имя 'name' на уникальность в рамках данного объекта
bool CheckName(const NameT &name);

// Генерирует имя уникальное в компонентах этого объекта
virtual NameT& GenerateName(const NameT &prefix, NameT &namebuffer);

// Устанавливает имя объекта
const NameT& GetName(void) const;
bool SetName(const NameT &name);


// Возвращает полное имя объекта
// (включая имена всех владельцев)
NameT& GetFullName(NameT &buffer) const;

// Возвращает  'длинное' имя объекта
// (исключая имя владельца 'mainowner').
// Метод возвращает пустую строку, если 'mainowner' - не является
// владельцем объекта ни на каком уровне иерархии
NameT& GetLongName(const UEPtr<UAContainer> mainowner, NameT &buffer) const;
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
public:
// Возвращает имя дочернего компонента по его Id
const NameT& GetComponentName(const UId &id) const;

// Возвращает Id дочернего компонента по его имени
const UId& GetComponentId(const NameT &name) const;

// Возвращает имя параметра по его Id
const NameT& GetPropertyName(const UId &id) const;

// Возвращает Id параметра по его имени
const UId& GetPropertyId(const NameT &name) const;

// Возвращает полное имя параметра без префикса NMSDK, и суффикса '*'
NameT GetPropertyLongName(const NameT &name) const;
NameT GetPropertyLongName(const UId &id) const;

// Возвращает имя переменной состояния по его Id
const NameT& GetStateName(const UId &id) const;

// Возвращает Id переменной состояния по его имени
const UId& GetStateId(const NameT &name) const;

// Возвращает полное имя переменной состояния без префикса NMSDK, и суффикса '*'
NameT GetStateLongName(const NameT &name) const;
NameT GetStateLongName(const UId &id) const;

// Возвращает имя локального указателя по его Id
const NameT& GetPointerName(const UId &id) const;

// Возвращает Id локального указателя по его имени
const UId& GetPointerId(const NameT &name) const;
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual UAContainer* New(void)=0;

// Создает копию этого объекта с сохранением всех компонент
// и значений параметров.
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual UEPtr<UAContainer> Alloc(UAContainerStorage *stor=0, bool copystate=false);

// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual bool Copy(UEPtr<UAContainer> target, UAContainerStorage *stor=0, bool copystate=false) const;

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
virtual void Free(void);
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Возвращает число дочерних компонент
int GetNumComponents(void) const;

// Возвращает полное число дочерних компонент
// (включая все компоненты дочерних компонент)
int GetNumAllComponents(void) const;

// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
virtual bool CheckComponentType(UEPtr<UAContainer> comp) const;

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому Id 'id'
// Если id == ForbiddenId то возвращает указатель на этот компонент
virtual UEPtr<UAContainer> GetComponent(const UId &id) const;

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по короткому имени 'name'
virtual UEPtr<UAContainer> GetComponent(const NameT &name) const;

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по ДЛИННОМУ Id 'id'
// Если id[0] == ForbiddenId или Id имеет нулевой размер,
// то возвращает указатель на этот компонент
UEPtr<UAContainer> GetComponentL(const ULongId &id) const;

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по ДЛИННОМУ имени 'name'
virtual UEPtr<UAContainer> GetComponentL(const NameT &name) const;

// Возвращает указатель на дочерний компонент, хранимый в этом
// объекте по порядковому индеку в списке компонент
// Метод возвращает 0, если индекс выходит за границы массива
UEPtr<UAContainer> GetComponentByIndex(int index) const;

// Добавляет дочерний компонент в этот объект
// Возвращает его Id или ForbiddenId если добавление неудачно
// Может быть передан указатель на локальную переменную
virtual UId AddComponent(UEPtr<UAContainer> comp, UIPointer* pointer=0);

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Таким образом 'id' - должно быть коротким Id
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
virtual bool DelComponent(const UId &id, bool canfree=true);

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Таким образом 'name' - должно быть коротким именем
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
bool DelComponent(const NameT &name, bool canfree);

// Принудительно удаляет все дочерние компоненты
void DelAllComponents(void);

// Возвращает список имен и Id компонент, содержащихся непосредственно
// в этом объекте
// Память должна быть выделена
void GetComponentsList(UId *buffer) const;

// Копирует все компоненты этого объекта в объект 'comp', если возможно
// Если хранилище stor != 0 то используется оно
virtual bool CopyComponents(UEPtr<UAContainer> comp, UAContainerStorage* stor=0) const;
// --------------------------


// --------------------------
// Методы доступа к параметрам
// --------------------------
// Возвращает значение параметра по Id 'id'
UVariableData* GetProperty(const UId &id, UVariableData *values) const;

// Возвращает значение параметра по имени 'name'
UVariableData* GetProperty(const NameT &name, UVariableData *values) const;

// Устанавливает значение параметра по Id 'id'
bool SetProperty(const UId &id, UVariableData *values);

// Устанавливает значение параметра по имени 'name'
bool SetProperty(const NameT &name, UVariableData *values);

// Возвращает список Id параметров, содержащихся непосредственно
// в этом объекте
const UAContainer::VariableMapT& GetPropertiesList(void) const;

// Ищет имя свойства по указателю на него
const NameT& FindPropertyName(const UIProperty *prop) const;

// Копирует все параметры этого объекта в объект 'comp', если возможно.
virtual bool CopyProperties(UEPtr<UAContainer> comp) const;
// --------------------------

// ----------------------
// Методы управления коммуникационными компонентами
// ----------------------
public:
// Возвращает список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
virtual ULongIdVector& GetConnectorsList(ULongIdVector &buffer,
						  int sublevel=-1, UEPtr<UAContainer> ownerlevel=0);

// Возвращает список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
virtual ULongIdVector& GetItemsList(ULongIdVector &buffer,
							int sublevel=-1, UEPtr<UAContainer> ownerlevel=0);

// Возвращает список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
virtual ULongIdVector& GetNetsList(ULongIdVector &buffer,
							int sublevel=-1, UEPtr<UAContainer> ownerlevel=0);
// ----------------------

// --------------------------
// Методы доступа к переменным состояния
// --------------------------
// Возвращает значение переменной состояния по Id 'id'
virtual UVariableData* GetState(const UId &id, UVariableData *values) const;

// Возвращает значение переменной состояния по имени 'name'
UVariableData* GetState(const NameT &name, UVariableData *values) const;

// Устанавливает значение переменной состояния по Id 'id'
virtual bool SetState(const UId &id, UVariableData *values);

// Устанавливает значение переменной состояния по имени 'name'
bool SetState(const NameT &name, UVariableData *values);

// Возвращает список имен и Id переменных состояния, содержащихся непосредственно
// в этом объекте
const UAContainer::VariableMapT& GetStateList(void) const;

// Ищет имя свойства по указателю на него
const NameT& FindStateName(const UIProperty *prop) const;

// Копирует все переменные состояния этого объекта в объект 'comp', если возможно.
virtual bool CopyState(UEPtr<UAContainer> comp) const;
// --------------------------

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
virtual bool SharesInit(void);

// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
virtual bool SharesUnInit(void);
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool Default(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool Build(void);

// Сброс процесса счета без потери настроек
virtual bool Reset(void);

// Выполняет расчет этого объекта
virtual bool Calculate(void);

// Обновляет состояние MainOwner после расчета этого объекта
virtual bool UpdateMainOwner(void);
// --------------------------

// --------------------------
// Скрытые методы управления контроллерами интерфейса
// Удаление контроллеров лежит на вызывающем модуле
// --------------------------
protected:
// Добавляет новый контроллер
bool AddController(UController *controller, bool forchilds=false);

// Удаляет контроллер из списка
bool DelController(UController *controller, bool forchilds=false);

// Удаляет все контроллеры
bool DelAllControllers(bool forchilds=false);

// Инициирует отключение всех контроллеров
bool UnLinkAllControllers(bool forchilds=false);

// Проверяет, существует ли контроллер в списке
bool CheckController(UController *controller) const;

// Возвращает число контроллеров
bool GetNumControllers(void) const;

// Возвращает контроллер по индексу
UController* GetController(int index);
// --------------------------

protected:
// --------------------------
// Скрытые методы управления состоянием
// --------------------------
public:
// Добавляет переменную состояния с именем 'name' в таблицу соотвествий
// параметров и назначает ей корректный индекс
// Должна вызываться в конструкторах классов
UId AddLookupState(const NameT &name,UIProperty *property, bool delenable=true);

protected:
// Удаляет переменную состояния с именем 'name' из таблицы соотвествий
bool DelLookupState(const NameT &name);

// Удаляет всю таблицу соответствий
void ClearLookupStateTable(void);

// Возвращает полное имя переменной состояния без префикса NMSDK, и суффикса '*'
NameT GetStateLongName(const UIProperty &property) const;
// --------------------------

// --------------------------
// Скрытые методы управления локальными указателями
// --------------------------
public:
// Добавляет указатель с именем 'name' в таблицу соотвествий
// параметров и назначает ей корректный индекс
// Должна вызываться в конструкторах классов
UId AddLookupPointer(const NameT &name, UIPointer *pointer);

protected:
// Удаляет указатель с ID 'id' из таблицы соотвествий
bool DelLookupPointer(const NameT &name);

// Возвращает полное имя указателя без префикса RDK, и суффикса '*'
NameT GetPointerLongName(const UIPointer &pointer) const;

// Осуществляет поиск в таблице указателя, соответствующего заданному источнику
PointerMapCIteratorT FindLookupPointer(UEPtr<UAContainer> source) const;
// --------------------------

// --------------------------
// Скрытые методы управления таблицей компонент
// --------------------------
// Добавляет компонент 'comp' в таблицу компонент
void AddComponentTable(UEPtr<UAContainer> comp, UIPointer* pointer=0);

// Удаляет компонент 'comp' из таблицы компонент
void DelComponentTable(UEPtr<UAContainer> comp);
// --------------------------

// --------------------------
// Скрытые методы управления общими (shared) переменными
// --------------------------
protected:
// Метод инициализации общих переменных. Вызывается автоматически при добавлении
// объекта владельцу
virtual bool ASharesInit(void);

// Метод деинициализации общих переменных. Вызывается автоматически при удалении
// объекта из владельца
virtual bool ASharesUnInit(void);
// --------------------------


protected:
// --------------------------
// Скрытые методы управления таблицей соответсвий компонент
// --------------------------
// Обновляет таблицу соответствий компонент заменяя 'oldname'
// имя компонента на 'newname'
bool ModifyLookupComponent(const NameT &oldname, const NameT newname);

// Обновляет таблицу соответствий компонент устанавливая Id 'id'
// для компонента с именем 'name'
void SetLookupComponent(const NameT &name, const UId &id);

// Обновляет таблицу соответствий компонент удаляя запись
// компонента с именем 'name'
void DelLookupComponent(const NameT &name);
// --------------------------

// --------------------------
// Скрытые методы управления параметрами
// --------------------------
public:
// Добавляет параметр с именем 'name' в таблицу соотвествий
// параметров и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
UId AddLookupProperty(const NameT &name, UIProperty *property, bool delenable=true);

protected:
// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
bool DelLookupProperty(const NameT &name);

// Удаляет всю таблицу соответствий
void ClearLookupPropertyTable(void);

// Возвращает полное имя параметра без префикса RDK, и суффикса '*'
NameT GetPropertyLongName(const UIProperty &property) const;
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
protected:
// Удаляет компонент comp
// Метод предполагает, что компонент принадлежит объекту
bool DelComponent(UEPtr<UAContainer> comp, bool canfree);

// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
// Может быть передан указатель на локальную переменную
virtual bool AAddComponent(UEPtr<UAContainer> comp, UIPointer* pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual bool ADelComponent(UEPtr<UAContainer> comp);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Обновляет состояние MainOwner после расчета этого объекта
virtual bool AUpdateMainOwner(void);
// --------------------------
};



}

#include "UProperty.h"
#include "UPointer.h"   
#include "UShare.h"
#include "ULocalProperty.h"

#endif
