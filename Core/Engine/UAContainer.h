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
#include "UADataComponent.h"
#include "UController.h"
#include "UTime.h"

namespace RDK {

class UAContainer;

// Класс описания локальных указателей
class UIPointer
{
protected: // Данные

public:
virtual UEPtr<UAContainer> const Get(void) const=0;

virtual void Set(UEPtr<UAContainer> source)=0;

virtual void Del(UEPtr<UAContainer> source)=0;

// Проверяет, существует ли такой указатель в этом классе
// Возвращает 0 если да, и <0 если нет
virtual int Find(UEPtr<const UAContainer> cont) const=0;

// -----------------
// Операторы
// -----------------
UIPointer& operator = (UEPtr<UAContainer> source)
{
 Set(source);
 return *this;
};
// -----------------
};


// Хранилище свойств указателя
struct UPVariable
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


class UAContainerStorage;
class UInstancesStorageElement;

typedef long int IndexT;
//class UAContainer;
typedef UAContainer* PUAContainer;

// Массив указателей на контейнеры
typedef std::vector<UEPtr<UAContainer> > UAContainerVector;

class UAContainer: public UADataComponent, public UTimeControl
{
public: // Типы данных
typedef std::map<NameT,UPVariable> PointerMapT;
typedef std::map<NameT,UPVariable>::iterator PointerMapIteratorT;
typedef std::map<NameT,UPVariable>::const_iterator PointerMapCIteratorT;

friend class UAContainerStorage;
friend class UController;

private: // Системные свойства
// Таблица соответствий имен и Id компонент объекта
std::map<NameT,UId> CompsLookupTable;

// Таблица локальных указателей на дочерние компоненты
PointerMapT PointerLookupTable;

private: // Системные свойства
// Таблица компонент
UAContainerVector Components;

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

public: // Физические свойства
// Координата компонента в пространстве сети
RDK::MVector<double> Coord;

// Время, затраченное на обработку объекта
// (без учета времени обсчета дочерних объектов) (мс)
long long StepDuration;

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
UEPtr<UInstancesStorageElement> ObjectIterator;

// Последний использованный Id компонент
UId LastId;

// Флаг запроса на пропуск обсчета компонент в текущей итерации расчетов
bool SkipComponentCalculation;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
public:
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
UEPtr<UAContainerStorage> const GetStorage(void) const;

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
// Промежуточный вариант одноименного метода, возвращающего длинное имя
std::string& GetLongId(UEPtr<UAContainer> mainowner, std::string &buffer) const;

// Возвращает true если передаваемый идентификатор объекта корректен, в противном случае возвращает false
bool CheckLongId(const ULongId &id) const;
// Промежуточный вариант одноименного метода, обрабатывающего длинное имя
bool CheckLongId(const std::string &id) const;
// --------------------------

// --------------------------
// Методы управления свойствами
// --------------------------
// Удаляет владельца объекта
void BreakOwner(void);

// Указатель устанавливается на число уровней дочерних компонент
// 'levels'. Если levels < 0 то устанавливается компонентам на всех уровнях
void SetMainOwner(UEPtr<UAComponent> mainowner);
void SetMainOwner(UEPtr<UAComponent> mainowner, int levels);

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

// Время, затраченное на обработку объекта
// (без учета времени обсчета дочерних объектов) (мс)
long long GetStepDuration(void) const;

// Время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
long long GetFullStepDuration(void) const;

// Устанавливает величину шага интегрирования
UTime GetTimeStep(void) const;
bool SetTimeStep(UTime timestep);

// Устанавливает флаг активности объекта
bool GetActivity(void) const;
bool SetActivity(bool activity);

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

// Создает экземпляр описания класса
//virtual UContainerDescription* NewDescription(void);

// Создает копию этого объекта с сохранением всех компонент
// и значений параметров.
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual UEPtr<UAContainer> Alloc(UEPtr<UAContainerStorage> stor=0, bool copystate=false);

// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual bool Copy(UEPtr<UAContainer> target, UEPtr<UAContainerStorage> stor=0, bool copystate=false) const;

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
virtual void BeforeAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);
virtual void AfterAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);
virtual UId AddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Таким образом 'id' - должно быть коротким Id
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
virtual void DelComponent(const UId &id, bool canfree=true);

// Удаляет дочерний компонент из этого объекта.
// Удаляемый компонент должен содержаться именно в этом объекте.
// Таким образом 'name' - должно быть коротким именем
// удаляемого объекта
// Если 'canfree' == true - предпринимается попытка вернуть объект в хранилище
// или удалить его. Иначе объект сохраняется в хранилище в состоянии занят
// либо повисает, если хранилище не установлено
void DelComponent(const NameT &name, bool canfree);

// Принудительно удаляет все дочерние компоненты
void DelAllComponents(void);

// Возвращает список имен и Id компонент, содержащихся непосредственно
// в этом объекте
// Память должна быть выделена
void GetComponentsList(vector<UId> &buffer) const;

// Копирует все компоненты этого объекта в объект 'comp', если возможно
// Если хранилище stor != 0 то используется оно
virtual void CopyComponents(UEPtr<UAContainer> comp, UEPtr<UAContainerStorage> stor=0) const;
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
virtual void UpdateMainOwner(void);

// Обычно вызывается дочерним компонентом и прерывает обсчет цепочки дочерних
// компонент на этом шаге счета
virtual void ForceSkipComponentCalculation(void);
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
PointerMapCIteratorT FindLookupPointer(UEPtr<UAContainer> source) const;
// --------------------------

// --------------------------
// Скрытые методы управления таблицей компонент
// --------------------------
// Добавляет компонент 'comp' в таблицу компонент
void AddComponentTable(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);

// Удаляет компонент 'comp' из таблицы компонент
void DelComponentTable(UEPtr<UAContainer> comp);
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
void ModifyLookupComponent(const NameT &oldname, const NameT newname);

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
// Удаляет компонент comp
// Метод предполагает, что компонент принадлежит объекту
virtual void BeforeDelComponent(UEPtr<UAContainer> comp, bool canfree=true);
virtual void AfterDelComponent(UEPtr<UAContainer> comp, bool canfree=true);
void DelComponent(UEPtr<UAContainer> comp, bool canfree);

// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
// Может быть передан указатель на локальную переменную
virtual void ABeforeAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);
virtual void AAfterAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);
virtual bool AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual void ABeforeDelComponent(UEPtr<UAContainer> comp, bool canfree);
virtual void AAfterDelComponent(UEPtr<UAContainer> comp, bool canfree);
virtual bool ADelComponent(UEPtr<UAContainer> comp);
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
EIContainer(const UAContainer *cont);
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
EComponentIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id компонента уже существует
struct EComponentIdAlreadyExist: public EIdAlreadyExist
{
EComponentIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// Имя компонента не найдено
struct EComponentNameNotExist: public ENameNotExist
{
EComponentNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Имя компонента уже существует
struct EComponentNameAlreadyExist: public ENameAlreadyExist
{
EComponentNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};


// Id указателя не найден
struct EPointerIdNotExist: public EIdNotExist
{
EPointerIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id указателя уже существует
struct EPointerIdAlreadyExist: public EIdAlreadyExist
{
EPointerIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// Имя указателя не найдено
struct EPointerNameNotExist: public ENameNotExist
{
EPointerNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Имя указателя уже существует
struct EPointerNameAlreadyExist: public ENameAlreadyExist
{
EPointerNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// Ошибки добавления/удаления компонент
// Id указателя уже существует
struct EAddComponentAlreadyHaveOwner: public EIdError
{
EAddComponentAlreadyHaveOwner(UId id) : EIdError(id) {};
};

// Недопустимый тип дочернего компонента
struct EAddComponentHaveInvalidType: public EIdError
{
EAddComponentHaveInvalidType(UId id) : EIdError(id) {};
};


// Исключение - ошибка
class EComponentCalculate: public EError, public EIContainer
{
 public: // Данные исключения
 // Короткое имя дочернего компонента в котором произошла ошибка счета
 std::string SubName;

 // Короткий идентификатор дочернего компонента в котором произошла ошибка счета
 ULongId SubId;

 // --------------------------
 // Конструкторы и деструкторы
 // --------------------------
 EComponentCalculate(void);
 EComponentCalculate(const UAContainer *cont, const UAContainer *subcont);
 EComponentCalculate(const EComponentCalculate &copy);
 virtual ~EComponentCalculate(void);
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

}

#include "UPointer.h"
#include "UShare.h"

#endif

