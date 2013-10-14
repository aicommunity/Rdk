#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include <exception>
#include "rdk_init.h"
#include "rdk.h"
#include "rdk_rpc.cpp"


// Менеджер DLL
class RDKDllManager
{
public:
// Массив хранилищ
std::vector<RDK::UStorage*> StorageList;

// Массив сред
std::vector<RDK::UEnvironment*> EnvironmentList;

// Массив движков
std::vector<RDK::UEngine*> EngineList;

// ----------------------------------------------------------
// Глобальные указатели на функции создания хранилища и среды
// ----------------------------------------------------------
// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
typedef RDK::UStorage* (*PCreateNewStorage)(void);
PCreateNewStorage FuncCreateNewStorage;

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
typedef RDK::UEnvironment* (*PCreateNewEnvironment)(void);
PCreateNewEnvironment FuncCreateNewEnvironment;

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
typedef RDK::UEngine* (*PCreateNewEngine)(void);
PCreateNewEngine FuncCreateNewEngine;
// ----------------------------------------------------------

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
RDKDllManager(void);
~RDKDllManager(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
bool Init(PCreateNewStorage fCreateNewStorage,
            PCreateNewEnvironment fCreateNewEnvironment,
			PCreateNewEngine fCreateNewEngine);

/// Возвращает число движков
int GetNumEngines(void) const;

/// Создает требуемое число пустых движков
int SetNumEngines(int num);

/// Создаает требуемый движок
/// (если движок уже инициализирован, то не делает ничего
int EngineCreate(int index);

/// Уничтожает требуемый движок
/// (если движок уже уничтожен, то не делает ничего
int EngineDestroy(int index);
// --------------------------
};

// Экземпляр менеджера
RDKDllManager DllManager;


// --------------------------------------
// Объявления дополнительных функций
// --------------------------------------
/*
// Возвращает хранилище по индексу
RDK::UStorage* GetStorage(size_t i);

// Возвращает среду по индексу
RDK::UEnvironment*  GetEnvironment(size_t i);

// Возвращает движок по индексу
RDK::UEngine*  GetEngine(size_t i);

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
RDK::UStorage*  AddNewStorage(void);

// Удаляет существующее хранилище
void DelStorage(RDK::UStorage* env);

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
RDK::UEnvironment*  AddNewEnvironment(void);

// Удаляет существующую среду
void DelEnvironment(RDK::UEnvironment* env);

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
RDK::UEngine*  AddNewEngine(void);

// Удаляет существующий движок
void DelEngine(RDK::UEngine* engine);
						  */
// Инициализация библиотеки
//int Init(void* exception_handler);

// Деинициализация библиотеки
//int UnInit(void);

// Инициализация dll
bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine);
// --------------------------------------

RDK::UEPtr<RDK::UEngine> PEngine=0;
RDK::UEPtr<RDK::UEnvironment> PEnvironment=0;
RDK::UEPtr<RDK::UStorage> PStorage=0;

int SelectedEngineIndex=0;

/*****************************************************************************/
extern RDK::UStorage* CreateNewStorage(void);

extern RDK::UEnvironment* CreateNewEnvironment(void);

extern RDK::UEngine* CreateNewEngine(void);
/*****************************************************************************/

// ----------------------------
// Функции RPC
// С помощью этих функций возможно вызвать любую функицю библиотеки,
// возвращающую строковые данные
// ----------------------------
/// Выполняет запрос и возвращает xml-описание ответа
/// request - xml описание запроса
/// return_value - возвращаемое значение для тех функций, которые его имеют
/// для остальных возвращает 0
const char* RDK_CALL RemoteCall(const char *request, int &return_value)
{
 return RDK::RemoteCallInternal(request, return_value);
}
// ----------------------------

// ----------------------------
// Методы инициализации
// ----------------------------
// Возвращает число дивжков
int RDK_CALL GetNumEngines(void)
{
 return DllManager.GetNumEngines();
}

// Создает требуемое число движков
// num > 0
int RDK_CALL SetNumEngines(int num)
{
 if(num<=0)
  return 1;

 if(num == GetNumEngines())
  return 0;

 if(!DllInit((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine))
  return -2;


 int res=DllManager.SetNumEngines(num);
 if(res != 0)
  return res;

 if(SelectedEngineIndex>=num)
  SelectedEngineIndex=0;

 if(num>0)
 {
  PEngine=DllManager.EngineList[SelectedEngineIndex];
  PEnvironment=DllManager.EnvironmentList[SelectedEngineIndex];
  PStorage=DllManager.StorageList[SelectedEngineIndex];
 }

 RpcReturnString.resize(num);
 return 0;
}

// Возвращает индекс текущего выбранного движка
int RDK_CALL GetSelectedEngineIndex(void)
{
 return SelectedEngineIndex;
}


// Настраивает обычный интерфейс на работу с заданным движком
// В случае удаления движка, интерфейс автоматически перенастраивается на 0 движок
int RDK_CALL SelectEngine(int index)
{
 if(index<0 || index>=GetNumEngines())
  return 1;

 if(SelectedEngineIndex == index)
  return 0;

 SelectedEngineIndex=index;
 PEngine=DllManager.EngineList[index];
 PEnvironment=DllManager.EnvironmentList[index];
 PStorage=DllManager.StorageList[index];

 return 0;
}

int RDK_CALL EngineInit(int predefined_structure, void* exception_handler)
{
 int res=0;
 if(GetNumEngines()<=SelectedEngineIndex)
  res=SetNumEngines(SelectedEngineIndex+1);

 if(res != 0)
  return res;

 res=MEngineInit(SelectedEngineIndex, predefined_structure, exception_handler);

 if(res != 0)
  return res;

 PEngine=DllManager.EngineList[SelectedEngineIndex];
 PEnvironment=DllManager.EnvironmentList[SelectedEngineIndex];
 PStorage=DllManager.StorageList[SelectedEngineIndex];

 return 0;
}

int RDK_CALL MEngineInit(int engine_index, int predefined_structure, void* exception_handler)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 MEngineUnInit(engine_index);

 int res=0;

 res=DllManager.EngineCreate(engine_index);
 if(res != 0)
  return res;

 MEngine_SetExceptionHandler(engine_index, exception_handler);

 MEnv_SetPredefinedStructure(engine_index, predefined_structure);
 MEnv_CreateStructure(engine_index);
 MEnv_Init(engine_index);

 return 0;
}


int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx,
		void* exception_handler)
{
 int res=0;
 if(GetNumEngines()<=SelectedEngineIndex)
  res=SetNumEngines(SelectedEngineIndex+1);

 if(res != 0)
  return res;

 res=MGraphicalEngineInit(SelectedEngineIndex, predefined_structure, num_inputs,
		num_outputs, input_width, input_height, reflectionx, exception_handler);
 if(res != 0)
  return res;

 PEngine=DllManager.EngineList[SelectedEngineIndex];
 PEnvironment=DllManager.EnvironmentList[SelectedEngineIndex];
 PStorage=DllManager.StorageList[SelectedEngineIndex];

 return 0;
}

int RDK_CALL MGraphicalEngineInit(int engine_index, int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx,
		void* exception_handler)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 MEngineUnInit(engine_index);
// Init(exception_handler);

 int res=0;

 res=DllManager.EngineCreate(SelectedEngineIndex);
 if(res != 0)
  return res;

 MEngine_SetExceptionHandler(engine_index, exception_handler);

 // Задает число входов среды
 MEnv_SetNumInputImages(engine_index, num_inputs);
 MEnv_SetNumOutputImages(engine_index, num_outputs);

 // Задает разрешение по умолчанию (рабочее разрешение)
 for(int i=0;i<num_inputs;i++)
  MEnv_SetInputRes(engine_index, i, input_width, input_height);

 MEnv_SetReflectionXFlag(engine_index, reflectionx);

 MEnv_SetPredefinedStructure(engine_index, predefined_structure);
 MEnv_CreateStructure(engine_index);
 MEnv_Init(engine_index);

 return 0;
}

// Деинициализирует движок (функция автоматически вызывается при вызове инициализации)
int RDK_CALL EngineUnInit(void)
{
 if(PEngine)
  if(!Env_UnInit())
   return 1;

 return DllManager.EngineDestroy(SelectedEngineIndex);
}

int RDK_CALL MEngineUnInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 if(DllManager.EngineList[engine_index])
  if(!MEnv_UnInit(engine_index))
   return 1;

 return DllManager.EngineDestroy(engine_index);
}


/// Проверяет инициализирован ли движок
bool RDK_CALL IsEngineInit(void)
{
 return (PEngine)?true:false;
}

bool RDK_CALL MIsEngineInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;

 return (DllManager.EngineList[engine_index])?true:false;
}
// ----------------------------

// --------------------------
// Методы управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
int RDK_CALL Storage_GetNumClasses(void)
{
 return PEngine->Storage_GetNumClasses();
}

// Возвращает id классов в хранилище. Память должна быть выделена
void RDK_CALL Storage_GetClassesList(int *buffer)
{
 PEngine->Storage_GetClassesList(buffer);
}

// Возвращает имена классов в хранилище в виде строки разделенной запятыми
const char * RDK_CALL Storage_GetClassesNameList(void)
{
 return PEngine->Storage_GetClassesNameList();
}

// Возвращает имя класса по его id.
const char * RDK_CALL Storage_GetClassName(int id)
{
 return PEngine->Storage_GetClassName(id);
}

// Возвращает Id класса по его имени
int RDK_CALL Storage_GetClassId(const char *name)
{
 return PEngine->Storage_GetClassId(name);
}

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
bool RDK_CALL Storage_DelClass(int classid)
{
 return PEngine->Storage_DelClass(classid);
}

// Удалаяет все свободные объекты из хранилища
void RDK_CALL Storage_FreeObjectsStorage(void)
{
 PEngine->Storage_FreeObjectsStorage();
}

// Удаляет все объекты из хранилища
void RDK_CALL Storage_ClearObjectsStorage(void)
{
 PEngine->Storage_ClearObjectsStorage();
}

// Вычисляет суммарное число объектов в хранилище
int RDK_CALL Storage_CalcNumObjects(void)
{
 return PEngine->Storage_CalcNumObjects();
}

int RDK_CALL Storage_CalcNumObjectsById(int classid)
{
 return PEngine->Storage_CalcNumObjectsById(classid);
}

int RDK_CALL Storage_CalcNumObjectsByName(const char* classname)
{
 return PEngine->Storage_CalcNumObjectsByName(classname);
}

// Возвращает описание класса по его id в формате xml
const char* RDK_CALL Storage_GetClassDescription(const char* classname)
{
 return PEngine->Storage_GetClassDescription(classname);
}

// Устанавливает описание класса по его id, считывая его из формата xml
bool RDK_CALL Storage_SetClassDescription(const char* classname, const char* description)
{
 return PEngine->Storage_SetClassDescription(classname, description);
}

// Сохраняет описание всех классов в xml
const char* RDK_CALL Storage_SaveClassesDescription(void)
{
 return PEngine->Storage_SaveClassesDescription();
}

// Загружает описание всех классов из xml
bool RDK_CALL Storage_LoadClassesDescription(const char* xmltext)
{
 return PEngine->Storage_LoadClassesDescription(xmltext);
}

// Сохраняет общее описание всех классов в xml
const char* RDK_CALL Storage_SaveCommonClassesDescription(void)
{
 return PEngine->Storage_SaveCommonClassesDescription();
}

// Загружает общее описание всех классов из xml
bool RDK_CALL Storage_LoadCommonClassesDescription(const char* xmltext)
{
 return PEngine->Storage_LoadCommonClassesDescription(xmltext);
}

// Сохраняет описание всех классов в xml включая общее описание
const char* RDK_CALL Storage_SaveAllClassesDescription(void)
{
 return PEngine->Storage_SaveAllClassesDescription();
}

// Загружает описание всех классов из xml включая общее описание
bool RDK_CALL Storage_LoadAllClassesDescription(const char* xmltext)
{
 return PEngine->Storage_LoadAllClassesDescription(xmltext);
}

// Загружает библиотеку по имени dll-файла
int RDK_CALL Storage_LoadStorageLibrary(const char *filename)
{
 return PEngine->Storage_LoadStorageLibrary(filename);
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool RDK_CALL Storage_DelClassLibraryByIndex(int index)
{
 return PEngine->Storage_DelClassLibraryByIndex(index);
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool RDK_CALL Storage_DelClassLibraryByName(const char *name)
{
 return PEngine->Storage_DelClassLibraryByName(name);
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool RDK_CALL Storage_DelAllClassLibraries(void)
{
 return PEngine->Storage_DelAllClassLibraries();
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool RDK_CALL Storage_BuildStorage(void)
{
 return PEngine->Storage_BuildStorage();
}

// Возвращает число библиотек
int RDK_CALL Storage_GetNumClassLibraries(void)
{
 return PEngine->Storage_GetNumClassLibraries();
}

// Возвращает список библиотек в виде строки, разделенной запятыми
const char* RDK_CALL Storage_GetClassLibrariesList(void)
{
 return PEngine->Storage_GetClassLibrariesList();
}

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// library_name - имя библиотеки
const char* RDK_CALL Storage_GetLibraryClassNames(const char *library_name)
{
 return PEngine->Storage_GetLibraryClassNames(library_name);
}

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// index - индекс библиотеки
const char* RDK_CALL Storage_GetLibraryClassNamesByIndex(int index)
{
 return PEngine->Storage_GetLibraryClassNamesByIndex(index);
}

// Возвращает имя библиотеки по индексу
const char * RDK_CALL Storage_GetClassLibraryNameByIndex(int index)
{
 return PEngine->Storage_GetClassLibraryNameByIndex(index);
}

// Возвращает версию библиотеки по индексу
const char * RDK_CALL Storage_GetClassLibraryVersionByIndex(int index)
{
 return PEngine->Storage_GetClassLibraryVersionByIndex(index);
}

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
int RDK_CALL Storage_CreateClass(const char* stringid, const char *classname)
{
 return PEngine->Storage_CreateClass(stringid, classname);
}
// ----------------------------

// ----------------------------
// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
int RDK_CALL Env_GetPredefinedStructure(void)
{
 return PEngine->Env_GetPredefinedStructure();
}

int RDK_CALL MEnv_GetPredefinedStructure(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Env_GetPredefinedStructure();
}

bool RDK_CALL Env_SetPredefinedStructure(int value)
{
 return PEngine->Env_SetPredefinedStructure(value);
}

bool RDK_CALL MEnv_SetPredefinedStructure(int engine_index, int value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_SetPredefinedStructure(value);
}

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool RDK_CALL Env_IsStoragePresent(void)
{
 return PEngine->Env_IsStoragePresent();
}

bool RDK_CALL MEnv_IsStoragePresent(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_IsStoragePresent();
}

// Возвращает состояние инициализации
bool RDK_CALL Env_IsInit(void)
{
 return PEngine->Env_IsInit();
}

bool RDK_CALL MEnv_IsInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_IsInit();
}


// Признак наличия сформированной структуры
bool RDK_CALL Env_IsStructured(void)
{
 return PEngine->Env_IsStructured();
}

bool RDK_CALL MEnv_IsStructured(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_IsStructured();
}

// Инициализация среды
bool RDK_CALL Env_Init(void)
{
 return PEngine->Env_Init();
}

bool RDK_CALL MEnv_Init(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_Init();
}

// Деинициализация среды
bool RDK_CALL Env_UnInit(void)
{
 return PEngine->Env_UnInit();
}

bool RDK_CALL MEnv_UnInit(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_UnInit();
}

// Формирует предварительно заданную модель обработки
bool RDK_CALL Env_CreateStructure(void)
{
 return PEngine->Env_CreateStructure();
}

bool RDK_CALL MEnv_CreateStructure(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_CreateStructure();
}

// Уничтожает текущую модель обработки
bool RDK_CALL Env_DestroyStructure(void)
{
 return PEngine->Env_DestroyStructure();
}

bool RDK_CALL MEnv_DestroyStructure(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->Env_DestroyStructure();
}

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
void RDK_CALL Env_Destroy(void)
{
 return PEngine->Env_Destroy();
}

void RDK_CALL MEnv_Destroy(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_Destroy();
}

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
int RDK_CALL Env_Calculate(const char* stringid)
{
 return PEngine->Env_Calculate(stringid);
}

int RDK_CALL MEnv_Calculate(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Env_Calculate(stringid);
}


// Расчет всей модели в реальном времени
void RDK_CALL Env_RTCalculate(void)
{
 PEngine->Env_RTCalculate();
}

void RDK_CALL MEnv_RTCalculate(int engine_index)
{
 DllManager.EngineList[engine_index]->Env_RTCalculate();
}

// Метод сброса счета
// Если stringid == 0 то сбрасывает всю модель целиком,
// иначе - только указанный компонент модели
int RDK_CALL Env_Reset(const char* stringid)
{
 return PEngine->Env_Reset(stringid);
}

int RDK_CALL MEnv_Reset(int engine_index, const char* stringid)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Env_Reset(stringid);
}

// Производит увеличение времени модели на требуемую величину
void RDK_CALL Env_IncreaseModelTimeByStep(void)
{
 return PEngine->Env_IncreaseModelTimeByStep();
}

/// Устанавливает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
int RDK_CALL Env_SetMinInterstepsInterval(long long value)
{
 return PEngine->Env_SetMinInterstepsInterval(value);
}

int RDK_CALL MEnv_SetMinInterstepsInterval(int engine_index, long long value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Env_SetMinInterstepsInterval(value);
}

/// Возвращает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
long long RDK_CALL Env_GetMinInterstepsInterval(void)
{
 return PEngine->Env_GetMinInterstepsInterval();
}

long long RDK_CALL Env_GetMinInterstepsInterval(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;
 return DllManager.EngineList[engine_index]->Env_GetMinInterstepsInterval();
}

// Возвращает имя текущего каталога для хранения данных
const char* RDK_CALL Env_GetCurrentDataDir(void)
{
 return PEngine->Env_GetCurrentDataDir();
}

// Устанавливает имя текущего каталога для хранения данных
int RDK_CALL Env_SetCurrentDataDir(const char *dir)
{
 return PEngine->Env_SetCurrentDataDir(dir);
}

// ***********************************************
// Методы управления текущим компонентом
// !!! Следующие методы влияют на все
// методы, обращающиеся к компонентам по строковому id !!!
// ***********************************************
// Устанавливает текущий компонент (адресация относительно корня - модели)
int RDK_CALL Env_SelectCurrentComponent(const char *stringid)
{
 return PEngine->Env_SelectCurrentComponent(stringid);
}

// Сбрасывает текущий компонент в состояние по умолчению (модель)
int RDK_CALL Env_ResetCurrentComponent(const char *stringid)
{
 return PEngine->Env_ResetCurrentComponent(stringid);
}

// Меняет текущий компонент на его родителя (подъем на уровень вверх)
// Если уже на верхнем уровне, то не делает ничего
int RDK_CALL Env_UpCurrentComponent(void)
{
 return PEngine->Env_UpCurrentComponent();
}

// Меняет текущий компонент на его дочерний на произвольном уровне вложенности
// (спуск на N уровней вниз относительно текущего компонента)
int RDK_CALL Env_DownCurrentComponent(const char *stringid)
{
 return PEngine->Env_DownCurrentComponent(stringid);
}

// Возвращает длинное имя текущего компонента
const char* RDK_CALL Env_GetCurrentComponentName(void)
{
 return PEngine->Env_GetCurrentComponentName();
}

// Возвращает длинный строковой id текущего компонента
const char* RDK_CALL Env_GetCurrentComponentId(void)
{
 return PEngine->Env_GetCurrentComponentId();
}
// ***********************************************
// --------------------------

// --------------------------
// Методы управления моделью
// ----------------------------
// Удаляет модель
int RDK_CALL Model_Destroy(void)
{
 return PEngine->Model_Destroy();
}

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
int RDK_CALL Model_Create(const char *classname)
{
 return PEngine->Model_Create(classname);
}

int RDK_CALL MModel_Create(int engine_index, const char *classname)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;
 return DllManager.EngineList[engine_index]->Model_Create(classname);
}

// Очищает модель
int RDK_CALL Model_Clear(void)
{
 return PEngine->Model_Clear();
}

// Проверяет, существует ли модель
bool RDK_CALL Model_Check(void)
{
 return PEngine->Model_Check();
}

bool RDK_CALL MModel_Check(int engine_index)
{
 return DllManager.EngineList[engine_index]->Model_Check();
}

// Проверяет, существует ли в модели компонент с именем stringid)
bool RDK_CALL Model_CheckComponent(const char* stringid)
{
 return PEngine->Model_CheckComponent(stringid);
}

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'classname'
// если stringid - пустая строка, то добавляет в саму модель
// Возвращает имя компонента в случае успеха
const char* RDK_CALL Model_AddComponent(const char* stringid, const char *classname)
{
 return PEngine->Model_AddComponent(stringid, classname);
}

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'name'
// если stringid - пустая строка, то удаляет из самой модели
int RDK_CALL Model_DelComponent(const char* stringid, const char *name)
{
 return PEngine->Model_DelComponent(stringid, name);
}

// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
int RDK_CALL Model_GetNumComponents(const char* stringid)
{
 return PEngine->Model_GetNumComponents(stringid);
}

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer)
{
 return PEngine->Model_GetComponentsList(stringid, buffer);
}

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ','
const char* RDK_CALL Model_GetComponentsNameList(const char* stringid)
{
 return PEngine->Model_GetComponentsNameList(stringid);
}

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ',' и имеющих имя класса 'class_name'
// Если find_all == true то поиск ведется и во всех сабкомпонентах
const char* RDK_CALL Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all)
{
 return PEngine->Model_FindComponentsByClassName(stringid,class_name,find_all);
}

// Перемещает компонент с текущим индексом index или именем 'name' вверх или
// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
// на эту границу
int RDK_CALL Model_ChangeComponentPosition(const char* stringid, int step)
{
 return PEngine->Model_ChangeComponentPosition(stringid, step);
}

// Возвращает xml-список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
const char* RDK_CALL Model_GetConnectorsList(const char* stringid,
						  int sublevel, const char* owner_level_stringid)
{
 return PEngine->Model_GetConnectorsList(stringid, sublevel, owner_level_stringid);
}

// Возвращает xml-список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
const char* RDK_CALL Model_GetItemsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{
 return PEngine->Model_GetItemsList(stringid, sublevel, owner_level_stringid);
}

// Возвращает xml-список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
const char* RDK_CALL Model_GetNetsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{
 return PEngine->Model_GetNetsList(stringid, sublevel, owner_level_stringid);
}

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
const char* RDK_CALL Model_GetComponentName(const char* stringid)
{
 return PEngine->Model_GetComponentName(stringid);
}

// Возвращает длинное имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
const char* RDK_CALL Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid)
{
 return PEngine->Model_GetComponentLongName(stringid,owner_level_stringid);
}


// Возвращает длинный id компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
const char* RDK_CALL Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid)
{
 return PEngine->Model_GetComponentLongId(stringid,owner_level_stringid);
}

// Возвращает имя класса компонента в хранилище по длинному 'stringid'
// если stringid - пустая строка, то возвращает имя класса модели
const char* RDK_CALL Model_GetComponentClassName(const char* stringid)
{
 return PEngine->Model_GetComponentClassName(stringid);
}

// Возвращает список свойств компонента разделенный запятыми
const char* RDK_CALL Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentPropertiesList(stringid,type_mask);
}

// Возвращает список имен и индексов свойств компонента разделенный запятыми
// каждый элемент имеет вид имя_свойства:индекс_входа(выхода)
const char* RDK_CALL Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentPropertiesLookupList(stringid,type_mask);
}

// Возвращает свойства компонента по идентификатору
const char * RDK_CALL Model_GetComponentProperties(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentProperties(stringid,type_mask);
}

// Возвращает свойства компонента по идентификатору с описаниями
const char * RDK_CALL Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentPropertiesEx(stringid, type_mask);
}

// Возвращает выборочные свойства компонента по идентификатору
const char * RDK_CALL Model_GetComponentSelectedProperties(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentSelectedProperties(stringid);
}

// Возвращает значение свойства компонента по идентификатору компонента и имени свойства
const char * RDK_CALL Model_GetComponentPropertyValue(const char *stringid, const char *paramname)
{
 return PEngine->Model_GetComponentPropertyValue(stringid,paramname);
}

// Устанавливает свойства компонента по идентификатору
int RDK_CALL Model_SetComponentProperties(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentProperties(stringid, buffer);
}

// Устанавливает значение свойства компонента по идентификатору компонента и имени свойства
void RDK_CALL Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetComponentPropertyValue(stringid,paramname,buffer);
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
void RDK_CALL Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetGlobalComponentPropertyValue(stringid,class_stringid, paramname,buffer);
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
void RDK_CALL Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetGlobalOwnerComponentPropertyValue(stringid, class_stringid, class_owner_stringid, paramname,buffer);
}

// Возвращает указатель void* на данные свойства компонента
const void* RDK_CALL Model_GetComponentPropertyData(const char *stringid, const char *property_name)
{
 return PEngine->Model_GetComponentPropertyData(stringid, property_name);
}

// Копирует данные 'data' в заданное свойство компонента
int RDK_CALL Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data)
{
 return PEngine->Model_SetComponentPropertyData(stringid, property_name, data);
}

// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentParameters(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentProperties(stringid,type_mask & 0xFFFFFF01);
}

const char * RDK_CALL MModel_GetComponentParameters(int engine_index, const char *stringid, unsigned int type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_GetComponentProperties(stringid,type_mask & 0xFFFFFF01);
}

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedProperties(stringid);
}

// Возвращает параметры компонента по идентификатору с описаниями
const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentPropertiesEx(stringid, type_mask & 0xFFFFFF01);
}

// Возвращает значение параметра компонента по идентификатору компонента и имени параметра
const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname)
{
 return PEngine->Model_GetComponentPropertyValue(stringid,paramname);
}

const char * RDK_CALL MModel_GetComponentParameterValue(int engine_index, const char *stringid, const char *paramname)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_GetComponentPropertyValue(stringid,paramname);
}

// Устанавливает параметры компонента по идентификатору
int RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_SetComponentParameters(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.EngineList[engine_index]->Model_SetComponentProperties(stringid, buffer);
}

// Устанавливает значение параметра компонента по идентификатору компонента и имени параметра
void RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetComponentPropertyValue(stringid,paramname,buffer);
}

// Связывает выбранные контейнеры друг с другом
int RDK_CALL Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return PEngine->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}

// Связывает все компоненты выбранного компонента по возрастанию id в формате: 0 выход к 0 входу
int RDK_CALL Model_ChainLinking(const char* stringid)
{
 return PEngine->Model_ChainLinking(stringid);
}

// Связывает все компоненты выбранного компонента параллельно, подключая их к необходимому числу выходов модели
// Используется для тестирования производительности
int RDK_CALL Model_ParallelLinking(const char* stringid)
{
 return PEngine->Model_ParallelLinking(stringid);
}

// Разрывает выбранную связь
int RDK_CALL Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return PEngine->Model_BreakLink(stringid1, output_number, stringid2, input_number);
}

// Разрывает все связи
int RDK_CALL Model_BreakAllLinks(void)
{
 return PEngine->Model_BreakAllLinks();
}

// Разрывает связь ко входу connector_index коннектора 'connectorid'
int RDK_CALL Model_BreakConnectorLink(const char* connectorname, int connector_index)
{
 return PEngine->Model_BreakConnectorLink(connectorname, connector_index);
}

// Разрывает все входные и выходные связи выбранного контейнера
int RDK_CALL Model_BreakAllComponentLinks(const char* stringid)
{
 return PEngine->Model_BreakAllComponentLinks(stringid);
}

// Разрывает все входные связи выбранного контейнера
int RDK_CALL Model_BreakAllComponentInputLinks(const char* stringid)
{
 return PEngine->Model_BreakAllComponentInputLinks(stringid);
}

// Разрывает все выходные связи выбранного контейнера
int RDK_CALL Model_BreakAllComponentOutputLinks(const char* stringid)
{
 return PEngine->Model_BreakAllComponentOutputLinks(stringid);
}

// Проверяет, существует ли заданна связь
bool RDK_CALL Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return PEngine->Model_CheckLink(stringid1, output_number, stringid2, input_number);
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid)
{
 return PEngine->Model_GetComponentInternalLinks(stringid, owner_level_stringid);
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются с уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то применяется уровень текущего компонента
int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid)
{
 return PEngine->Model_SetComponentInternalLinks(stringid,buffer, owner_level_stringid);
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * RDK_CALL Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 return PEngine->Model_GetComponentInputLinks(stringid,owner_level_stringid, sublevel);
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * RDK_CALL Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 return PEngine->Model_GetComponentOutputLinks(stringid,owner_level_stringid, sublevel);
}

// Возращает все связи между двумя компонентами в виде xml в буфер buffer
// включая связи этого компонента
// если 'sublevel' == -1, то возвращает также все связи между объектом и любым дочерним компонентом
// второго объекта. Работает симметрично в обе стороны.
// если 'sublevel' == 0, то возвращает связи только между этими объектами
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char* RDK_CALL Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid)
{
 return PEngine->Model_GetComponentPersonalLinks(stringid,owner_level_stringid);
}

// Возвращает состояние компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentState(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_GetComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

const char * RDK_CALL MModel_GetComponentState(int engine_index, const char *stringid, unsigned int type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_GetComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedProperties(stringid);
}

// Возвращает значение переменной состояния компонента по идентификатору компонента и имени переменной
const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename)
{
 return PEngine->Model_GetComponentPropertyValue(stringid,statename);
}

const char * RDK_CALL MModel_GetComponentStateValue(int engine_index, const char *stringid, const char *statename)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_GetComponentPropertyValue(stringid,statename);
}

// Устанавливает состояние компонента по идентификатору
int RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_SetComponentState(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.EngineList[engine_index]->Model_SetComponentProperties(stringid, buffer);
}

// Устанавливает значение переменной состояния компонента по идентификатору компонента и имени переменной
void RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer)
{
 PEngine->Model_SetComponentPropertyValue(stringid,statename,buffer);
}

// Возвращает число входов у компонента
int RDK_CALL Model_GetComponentNumInputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumInputs(stringid);
}

// Возвращает размер входа компонента в числе элементов
int RDK_CALL Model_GetComponentInputDataSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputDataSize(stringid, index);
}

// Возвращает размер элемента входа в байтах
int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputElementSize(stringid, index);
}

// Возвращает размер входа компонента в байтах элементов
int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputByteSize(stringid, index);
}

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputData(stringid, index);
}

// Возвращает число выходов у компонента
int RDK_CALL Model_GetComponentNumOutputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumOutputs(stringid);
}

// Возвращает размер выхода компонента в числе элементов
int RDK_CALL Model_GetComponentOutputDataSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputDataSize(stringid, index);
}

// Возвращает размер элемента выхода в байтах
int RDK_CALL Model_GetComponentOutputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputElementSize(stringid, index);
}

// Возвращает размер выхода компонента в байтах элементов
int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputByteSize(stringid, index);
}

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputData(stringid, index);
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
const char * RDK_CALL Model_SaveComponent(const char *stringid, unsigned int params_type_mask)
{
 return PEngine->Model_SaveComponent(stringid, params_type_mask);
}

const char * RDK_CALL MModel_SaveComponent(int engine_index, const char *stringid, unsigned int params_type_mask)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->Model_SaveComponent(stringid, params_type_mask);
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int RDK_CALL Model_LoadComponent(const char *stringid, const char* buffer)
{
 return PEngine->Model_LoadComponent(stringid, buffer);
}

int RDK_CALL MModel_LoadComponent(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.EngineList[engine_index]->Model_LoadComponent(stringid, buffer);
}

// Сохраняет все свойства компонента и его дочерних компонент в xml
const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_SaveComponentProperties(stringid, type_mask);
}

// Загружает все свойства компонента и его дочерних компонент из xml
int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentProperties(stringid, buffer);
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF01);
}

// Загружает все параметры компонента и его дочерних компонент из xml
int RDK_CALL Model_LoadComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_LoadComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_LoadComponentParameters(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 1000;

 return DllManager.EngineList[engine_index]->Model_LoadComponentProperties(stringid, buffer);
}

// Сохраняет состояние компонента и его дочерних компонент в xml
const char * RDK_CALL Model_SaveComponentState(const char *stringid, unsigned int type_mask)
{
 return PEngine->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

// Загружает состояние компонента и его дочерних компонент из xml
int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentProperties(stringid, buffer);
}

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
const char* RDK_CALL Model_SaveComponentDrawInfo(const char *stringid)
{
 return PEngine->Model_SaveComponentDrawInfo(stringid);
}

// Управляет шагом счета модели по умолчанию
int RDK_CALL Model_GetDefaultTimeStep(void)
{
 return PEngine->Model_GetDefaultTimeStep();
}

void RDK_CALL Model_SetDefaultTimeStep(int value)
{
 PEngine->Model_SetDefaultTimeStep(value);
}

// Управляет шагом счета компонента
int RDK_CALL Model_GetTimeStep(const char *stringid)
{
 return PEngine->Model_GetTimeStep(stringid);
}

void RDK_CALL Model_SetTimeStep(const char *stringid, int value)
{
 PEngine->Model_SetTimeStep(stringid, value);
}

// Устанавливает шаг счета компонента и всех его дочерних компонент
void RDK_CALL Model_SetGlobalTimeStep(const char *stringid, int value)
{
 PEngine->Model_SetGlobalTimeStep(stringid, value);
}

// Возвращает текущее время модели
long long RDK_CALL Model_GetTime(void)
{
 return PEngine->Model_GetTime();
}

double RDK_CALL Model_GetDoubleTime(void)
{
 return PEngine->Model_GetDoubleTime();
}

// Устанавливает текущее время модели
bool RDK_CALL Model_SetTime(long long value)
{
 return PEngine->Model_SetTime(value);
}

// Возвращает реальное время
long long RDK_CALL Model_GetRealTime(void)
{
 return PEngine->Model_GetRealTime();
}

double RDK_CALL Model_GetDoubleRealTime(void)
{
 return PEngine->Model_GetDoubleRealTime();
}

// Устанавливает реальное время
bool RDK_CALL Model_SetRealTime(long long value)
{
 return PEngine->Model_SetRealTime(value);
}

// Увеличивает реальное время на заданную величину
bool RDK_CALL Model_IncreaseRealTime(long long value)
{
 return PEngine->Model_IncreaseRealTime(value);
}

// Возвращает мгновенный шаг в реальном времени
long long RDK_CALL Model_GetRealTimeStep(void)
{
 return PEngine->Model_GetRealTimeStep();
}

double RDK_CALL Model_GetDoubleRealTimeStep(void)
{
 return PEngine->Model_GetDoubleRealTimeStep();
}

// Возвращает время расчета компонента без времени расчета дочерних компонент (мс)
long long RDK_CALL Model_GetStepDuration(const char *stringid)
{
 return PEngine->Model_GetStepDuration(stringid);
}

long long RDK_CALL MModel_GetStepDuration(int engine_index, const char *stringid)
{
 return DllManager.EngineList[engine_index]->Model_GetStepDuration(stringid);
}

// Возвращает время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
long long RDK_CALL Model_GetFullStepDuration(const char *stringid)
{
 return PEngine->Model_GetFullStepDuration(stringid);
}

long long RDK_CALL MModel_GetFullStepDuration(int engine_index, const char *stringid)
{
 return DllManager.EngineList[engine_index]->Model_GetFullStepDuration(stringid);
}

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double RDK_CALL Model_GetInstantPerformance(const char *stringid)
{
 return PEngine->Model_GetInstantPerformance(stringid);
}

double RDK_CALL MModel_GetInstantPerformance(int engine_index, const char *stringid)
{
 return DllManager.EngineList[engine_index]->Model_GetInstantPerformance(stringid);
}

// Время, прошедшее между двумя последними итерациями счета
long long RDK_CALL Model_GetInterstepsInterval(const char *stringid)
{
 return PEngine->Model_GetInterstepsInterval(stringid);
}

long long RDK_CALL MModel_GetInterstepsInterval(int engine_index, const char *stringid)
{
 return DllManager.EngineList[engine_index]->Model_GetInterstepsInterval(stringid);
}
// --------------------------

// --------------------------
// Методы управления исключениями
// ----------------------------
// Управление функцией-обработчиком исключений
void* RDK_CALL Engine_GetExceptionHandler(void)
{
 return reinterpret_cast<void*>(PEngine->GetExceptionHandler());
}

void* RDK_CALL MEngine_GetExceptionHandler(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return reinterpret_cast<void*>(DllManager.EngineList[engine_index]->GetExceptionHandler());
}

bool RDK_CALL Engine_SetExceptionHandler(void* value)
{
 return PEngine->SetExceptionHandler(reinterpret_cast<RDK::UEnvironment::PExceptionHandler>(value));
}

bool RDK_CALL MEngine_SetExceptionHandler(int engine_index, void* value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return false;
 return DllManager.EngineList[engine_index]->SetExceptionHandler(reinterpret_cast<RDK::UEnvironment::PExceptionHandler>(value));
}

// Возвращает массив строк лога
const char* RDK_CALL Engine_GetLog(void)
{
 return PEngine->GetLog();
}

const char* RDK_CALL MEngine_GetLog(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->GetLog();
}

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* RDK_CALL Engine_GetUnreadLog(void)
{
 return PEngine->GetUnreadLog();
}

const char* RDK_CALL MEngine_GetUnreadLog(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index]->GetUnreadLog();
}
// ----------------------------


// --------------------------
// Методы управления средой
// --------------------------
// Задает число входов среды
void RDK_CALL Env_SetNumInputImages(int number)
{
 return PEngine->Env_SetNumInputImages(number);
}

void RDK_CALL MEnv_SetNumInputImages(int engine_index, int number)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_SetNumInputImages(number);
}

// Задает число выходов среды
void RDK_CALL Env_SetNumOutputImages(int number)
{
 return PEngine->Env_SetNumOutputImages(number);
}

void RDK_CALL MEnv_SetNumOutputImages(int engine_index, int number)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_SetNumOutputImages(number);
}

// Задает число входов среды
int RDK_CALL Env_GetNumInputImages(void)
{
 return PEngine->Env_GetNumInputImages();
}

// Задает число выходов среды
int RDK_CALL Env_GetNumOutputImages(void)
{
 return PEngine->Env_GetNumInputImages();
}

// Задает разрешение по умолчанию (рабочее разрешение)
void RDK_CALL Env_SetInputRes(int number, int width, int height)
{
 return PEngine->Env_SetInputRes(number, width, height);
}

void RDK_CALL MEnv_SetInputRes(int engine_index, int number, int width, int height)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_SetInputRes(number, width, height);
}

// Задает данные изображения
void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
 return PEngine->Env_SetInputImage(number, image, width, height,cmodel);
}

// Задает флаг отражения входного изображения вокруг горизонтальной оси
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value)
{
 return PEngine->Env_SetReflectionXFlag(value);
}

RDK_LIB_TYPE void MEnv_SetReflectionXFlag(int engine_index, bool value)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return;

 return DllManager.EngineList[engine_index]->Env_SetReflectionXFlag(value);
}


// Возвращает разрешение по умолчанию (рабочее разрешение)
int RDK_CALL Env_GetInputImageWidth(int number)
{
 return PEngine->Env_GetInputImageWidth(number);
}

int RDK_CALL Env_GetInputImageHeight(int number)
{
 return PEngine->Env_GetInputImageHeight(number);
}

int RDK_CALL Env_GetInputImageColorModel(int number)
{
 return PEngine->Env_GetInputImageColorModel(number);
}

// Возвращает текущее выходное разрешение
int RDK_CALL Env_GetOutputImageWidth(int number)
{
 return PEngine->Env_GetOutputImageWidth(number);
}

int RDK_CALL Env_GetOutputImageHeight(int number)
{
 return PEngine->Env_GetOutputImageHeight(number);
}

int RDK_CALL Env_GetOutputImageColorModel(int number)
{
 return PEngine->Env_GetOutputImageColorModel(number);
}

unsigned char* RDK_CALL Env_GetInputImage(int index)
{
 return PEngine->Env_GetInputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImage(int index)
{
 return PEngine->Env_GetOutputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImageY8(int index)
{
 return PEngine->Env_GetOutputImageY8(index);
}
// --------------------------

// --------------------------
// Методы управления графической моделью
// --------------------------
// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::MDMatrix*
// если выход не содержит данных такого типа, то возвращает 0
const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name)
{
 return (PEngine)->Model_GetComponentOutputAsMatrix(stringid, property_name);
}

const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrixByIndex(const char *stringid, int  index)
{
 return (PEngine)->Model_GetComponentOutputAsMatrix(stringid, index);
}

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const /* RDK::UBitmap* */ void* RDK_CALL Model_GetComponentOutput(const char *stringid, const char *property_name)
{
 return PEngine->Model_GetComponentOutput(stringid, property_name);
}

const /* RDK::UBitmap* */ void* RDK_CALL Model_GetComponentOutputByIndex(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutput(stringid, index);
}

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, const char *property_name)
{
 return PEngine->Model_GetComponentBitmapOutput(stringid, property_name);
}

const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutputByIndex(const char *stringid, int index)
{
 return PEngine->Model_GetComponentBitmapOutput(stringid, index);
}

// Возвращает указатель на вход с индексом 'index' компонента 'id'
const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInput(const char *stringid, const char *property_name)
{
 return PEngine->Model_GetComponentBitmapInput(stringid, property_name);
}

const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInputByIndex(const char *stringid, int index)
{
 return PEngine->Model_GetComponentBitmapInput(stringid, index);
}

// Замещает изображение выхода с индексом 'index' компонента 'id'
void RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 PEngine->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL MModel_SetComponentBitmapOutput(int engine_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 DllManager.EngineList[engine_index]->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL Model_SetComponentBitmapOutputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 PEngine->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL MModel_SetComponentBitmapOutputByIndex(int engine_index, const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 DllManager.EngineList[engine_index]->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

// Замещает изображение входа с индексом 'index' компонента 'id'
void RDK_CALL Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 PEngine->Model_SetComponentBitmapInput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

void RDK_CALL Model_SetComponentBitmapInputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 PEngine->Model_SetComponentBitmapInput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}
// --------------------------

/* ************************************************************************** */

extern RDK::UEPtr<RDK::UEngine> PEngine;
extern RDK::UEPtr<RDK::UEnvironment> PEnvironment;
extern RDK::UEPtr<RDK::UStorage> PStorage;


// Менеджер DLL
// --------------------------
// Конструкторы и деструкторы
// --------------------------
RDKDllManager::RDKDllManager(void)
{
 FuncCreateNewStorage=0;//&RDK::NewUStorage;
 FuncCreateNewEnvironment=0;//&RDK::NewUAContainerEnvironment;
 FuncCreateNewEngine=0;//&RDK::NewUEngine;
}

RDKDllManager::~RDKDllManager(void)
{
 EngineUnInit();

 for(size_t i=0;i<EnvironmentList.size();i++)
  if(EnvironmentList[i])
   delete EnvironmentList[i];

 EnvironmentList.resize(0);

 for(size_t i=0;i<StorageList.size();i++)
  if(StorageList[i])
   delete StorageList[i];

 StorageList.resize(0);

 for(size_t i=0;i<EngineList.size();i++)
  if(EngineList[i])
   delete EngineList[i];

 EngineList.resize(0);
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
bool RDKDllManager::Init(PCreateNewStorage fCreateNewStorage,
							PCreateNewEnvironment fCreateNewEnvironment,
							PCreateNewEngine fCreateNewEngine)
{
 FuncCreateNewStorage=fCreateNewStorage;
 FuncCreateNewEnvironment=fCreateNewEnvironment;
 FuncCreateNewEngine=fCreateNewEngine;

 return true;
}

/// Возвращает число движков
int RDKDllManager::GetNumEngines(void) const
{
 return int(EngineList.size());
}

/// Создает требуемое число пустых движков
int RDKDllManager::SetNumEngines(int num)
{
 if(num<0)
  return 1;

 for(int i=num;i<int(EngineList.size());i++)
  EngineDestroy(i);

 EngineList.resize(num);
 StorageList.resize(num);
 EnvironmentList.resize(num);
 return 0;
}

/// Создаает требуемый движок
/// (если движок уже инициализирован, то не делает ничего
int RDKDllManager::EngineCreate(int index)
{
 if(index<0 || index>=GetNumEngines())
  return 1;

 if(EngineList[index])
  return 0;

 EngineList[index]=FuncCreateNewEngine();
 if(!EngineList[index])
 {
  EngineDestroy(index);
  return 2;
 }

 try
 {
  StorageList[index]=FuncCreateNewStorage();
  if(!StorageList[index])
  {
   EngineDestroy(index);
   return 3;
  }

  EnvironmentList[index]=FuncCreateNewEnvironment();
  if(!EnvironmentList[index])
  {
   EngineDestroy(index);
   return 4;
  }

  EngineList[index]->Default();
  if(!EngineList[index]->Init(StorageList[index],EnvironmentList[index]))
  {
   EngineDestroy(index);
   return 5;
  }
 }
 catch (RDK::UException &exception)
 {
  EngineList[index]->ProcessException(exception);
 }
 return 0;
}

/// Уничтожает требуемый движок
/// (если движок уже уничтожен, то не делает ничего
int RDKDllManager::EngineDestroy(int index)
{
 if(index<0 || index>=GetNumEngines())
  return 1;

 if(EngineList[index])
 {
  delete EngineList[index];
  EngineList[index]=0;
 }

 if(EnvironmentList[index])
 {
  delete EnvironmentList[index];
  EnvironmentList[index]=0;
 }

 if(StorageList[index])
 {
  delete StorageList[index];
  StorageList[index]=0;
 }
 return 0;
}
// --------------------------



// ----------------------------
// Внутренние методы инициализации
// ----------------------------
/*
int Init(void* exception_handler)
{
 UnInit();
 if(!DllInit((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine))
  return -2;

 PEngine=dynamic_cast<RDK::UEngine*>(AddNewEngine());

 if(!PEngine)
  return -3;

 PEngine->Default();
 Engine_SetExceptionHandler(exception_handler);

 try {
  PEnvironment=AddNewEnvironment();
  PStorage=AddNewStorage();
  if(!PEnvironment)
  {
   UnInit();
   return 3;
  }

  if(!PStorage)
  {
   UnInit();
   return 4;
  }

  if(!PEngine->Init(PStorage,PEnvironment))
  {
   UnInit();
   return 3;
  }
 }
 catch (RDK::UException &exception)
 {
  PEngine->ProcessException(exception);
 }
 return 0;
}

int UnInit(void)
{
 try
 {
  if(PEngine)
  {
   PEngine->UnInit();
  }
 }
 catch (RDK::UException &exception)
 {
  PEngine->ProcessException(exception);
 }

 DelEngine(PEngine);
 PEngine=0;
 DelEnvironment(PEnvironment);
 PEnvironment=0;
 DelStorage(PStorage);
 PStorage=0;

 return 0;
}
        */
// Обработчик исключений библиотеки
// Должен быть вызван в глобальном обработчике пользовательского ПО
int RDK_CALL ExceptionDispatcher(void *exception)
{
 if(!PEngine)
  return 1;

 if(!exception)
  return 2;

 RDK::UException *exc=reinterpret_cast<RDK::UException*>(exception);
 PEngine->ProcessException(*exc);

 return 0;
}

// Инициализация dll
bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine)
{
 return DllManager.Init(reinterpret_cast<RDKDllManager::PCreateNewStorage>(pfstorage),
						reinterpret_cast<RDKDllManager::PCreateNewEnvironment>(pfenvironment),
						reinterpret_cast<RDKDllManager::PCreateNewEngine>(pfengine));
}
	   /*
// Возвращает число хранилищ в библиотеке
int GetNumStorages(void)
{
 return DllManager.StorageList.size();
}

// Возвращает число сред в библиотеке
int GetNumEnvironments(void)
{
 return DllManager.EnvironmentList.size();
}                     */
 /*
// Возвращает число движков в библиотеке
int GetNumEngines(void)
{
 return DllManager.EngineList.size();
}
	  */      /*
// Возвращает хранилище по индексу
RDK::UStorage* GetStorage(size_t i)
{
 if(i>=DllManager.StorageList.size())
  return 0;

 return DllManager.StorageList[i];
}

// Возвращает среду по индексу
RDK::UEnvironment* GetEnvironment(size_t i)
{
 if(i>=DllManager.EnvironmentList.size())
  return 0;

 return DllManager.EnvironmentList[i];
}

// Возвращает движок по индексу
RDK::UEngine* GetEngine(size_t i)
{
 if(i>=DllManager.EngineList.size())
  return 0;

 return DllManager.EngineList[i];
}


// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
RDK::UStorage* AddNewStorage(void)
{
 if(!DllManager.FuncCreateNewStorage)
  return 0;

 RDK::UStorage* storage=DllManager.FuncCreateNewStorage();
 if(storage)
 {
//  CreateStorage(storage);
  DllManager.StorageList.push_back(storage);
 }
 return storage;
}

// Удаляет существующее хранилище
void DelStorage(RDK::UStorage* storage)
{
 if(storage)
 {
  for(size_t i=0;i<DllManager.StorageList.size();i++)
   if(DllManager.StorageList[i] == storage)
   {
	DllManager.StorageList.erase(DllManager.StorageList.begin()+i);
	delete storage;
   }
 }
}

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
RDK::UEnvironment* AddNewEnvironment(void)
{
 if(!DllManager.FuncCreateNewEnvironment)
  return 0;

 RDK::UEnvironment* environment=DllManager.FuncCreateNewEnvironment();
 if(environment)
 {
//  CreateEnvironment(environment, storage,isinit,external_classes, external_libs);
  DllManager.EnvironmentList.push_back(environment);
 }
 return environment;
}

// Удаляет существующую среду
void DelEnvironment(RDK::UEnvironment* env)
{
 if(env)
 {
  for(size_t i=0;i<DllManager.EnvironmentList.size();i++)
   if(DllManager.EnvironmentList[i] == env)
   {
	DllManager.EnvironmentList.erase(DllManager.EnvironmentList.begin()+i);
	delete env;
   }
 }
}

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
RDK::UEngine* AddNewEngine(void)
{
 if(!DllManager.FuncCreateNewEngine)
  return 0;

 RDK::UEngine* engine=DllManager.FuncCreateNewEngine();
 if(engine)
 {
  DllManager.EngineList.push_back(engine);
 }
 return engine;
}

// Удаляет существую среду
void DelEngine(RDK::UEngine* engine)
{
 if(engine)
 {
  for(size_t i=0;i<DllManager.EngineList.size();i++)
   if(DllManager.EngineList[i] == engine)
   {
	DllManager.EngineList.erase(DllManager.EngineList.begin()+i);
	delete engine;
   }
 }
}                    */
// ----------------------------




#endif

