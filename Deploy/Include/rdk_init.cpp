#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include "rdk_init.h"
//#include "rdk_gengine.h"


RDK::UEngine* PEngine=0;
RDK::UAEnvironment* PEnvironment=0;
RDK::UAStorage* PStorage=0;

/*****************************************************************************/
RDK::UAStorage* CreateNewStorage(void)
{
 return new RDK::UAStorage;
}

extern RDK::UAEnvironment* CreateNewEnvironment(void)
{
 return new RDK::UAEnvironment;
}

extern RDK::UEngine* CreateNewEngine(void)
{
 return new RDK::UEngine;
}

RDK::UAStorage* CreateNewGStorage(void)
{
 return new RDK::UBAStorage;
}

RDK::UAEnvironment* CreateNewGEnvironment(void)
{
 return new RDK::UBAEnvironment;
}

RDK::UEngine* CreateNewGEngine(void)
{
 return new RDK::UBEngine;
}
/*****************************************************************************/

// ----------------------------
// Методы инициализации
// ----------------------------
int RDK_EngineInit(int predefined_structure)
{
 LoadEngine((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine);
 RDKInit();
 RDKLoadPredefinedLibraries();

 Env_SetPredefinedStructure(predefined_structure);
 Env_CreateStructure();

 return 0;
}

int RDK_GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height)
{
 LoadEngine((void*)CreateNewGStorage, (void*)CreateNewGEnvironment, (void*)CreateNewGEngine);
 RDKInit();
 RDKLoadPredefinedLibraries();

 // Задает число входов среды
 Env_SetNumInputImages(num_inputs);
 Env_SetNumOutputImages(num_outputs);

 // Задает разрешение по умолчанию (рабочее разрешение)
 for(int i=0;i<num_inputs;i++)
  Env_SetInputRes(i, input_width, input_height);

 Env_SetPredefinedStructure(predefined_structure);
 Env_CreateStructure();

 return 0;
}
// ----------------------------

// --------------------------
// Методы управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
int Storage_GetNumClasses(void)
{
 return PEngine->Storage_GetNumClasses();
}

// Возвращает id классов в хранилище. Память должна быть выделена
void Storage_GetClassesList(int *buffer)
{
 PEngine->Storage_GetClassesList(buffer);
}

// Возвращает имя класса по его id.
const char * Storage_GetClassName(int id)
{
 return PEngine->Storage_GetClassName(id);
}

// Возвращает Id класса по его имени
int Storage_GetClassId(const char *name)
{
 return PEngine->Storage_GetClassId(name);
}

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
bool Storage_DelClass(int classid)
{
 return PEngine->Storage_DelClass(classid);
}

// Удалаяет все свободные объекты из хранилища
void Storage_FreeObjectsStorage(void)
{
 PEngine->Storage_FreeObjectsStorage();
}

// Удаляет все объекты из хранилища
void Storage_ClearObjectsStorage(void)
{
 PEngine->Storage_ClearObjectsStorage();
}

// Вычисляет суммарное число объектов в хранилище
int Storage_CalcNumObjects(void)
{
 return PEngine->Storage_CalcNumObjects();
}

int Storage_CalcNumObjectsById(int classid)
{
 return PEngine->Storage_CalcNumObjectsById(classid);
}

int Storage_CalcNumObjectsByName(const char* classname)
{
 return PEngine->Storage_CalcNumObjectsByName(classname);
}
// ----------------------------

// ----------------------------
// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
int Env_GetPredefinedStructure(void)
{
 return PEngine->Env_GetPredefinedStructure();
}

bool Env_SetPredefinedStructure(int value)
{
 return PEngine->Env_SetPredefinedStructure(value);
}

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool Env_IsStoragePresent(void)
{
 return PEngine->Env_IsStoragePresent();
}

// Возвращает состояние инициализации
bool Env_IsInit(void)
{
 return PEngine->Env_IsInit();
}

// Признак наличия сформированной структуры
bool Env_IsStructured(void)
{
 return PEngine->Env_IsStructured();
}

// Инициализация среды
bool Env_Init(void)
{
 return PEngine->Env_Init();
}

// Деинициализация среды
bool Env_UnInit(void)
{
 return PEngine->Env_UnInit();
}

// Формирует предварительно заданную модель обработки
bool Env_CreateStructure(void)
{
 return PEngine->Env_CreateStructure();
}

// Уничтожает текущую модель обработки
bool Env_DestroyStructure(void)
{
 return PEngine->Env_DestroyStructure();
}

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
void Env_Destroy(void)
{
 return PEngine->Env_Destroy();
}

// Загружает библиотеку по имени dll-файла
int Env_LoadStorageLibrary(const char *filename)
{
 return PEngine->Env_LoadStorageLibrary(filename);
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool Env_DelClassLibraryByIndex(int index)
{
 return PEngine->Env_DelClassLibraryByIndex(index);
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool Env_DelClassLibraryByName(const char *name)
{
 return PEngine->Env_DelClassLibraryByName(name);
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool Env_DelAllClassLibraries(void)
{
 return PEngine->Env_DelAllClassLibraries();
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool Env_BuildStorage(void)
{
 return PEngine->Env_BuildStorage();
}

// Возвращает число библиотек
int Env_GetNumClassLibraries(void)
{
 return PEngine->Env_GetNumClassLibraries();
}

// Возвращает имя библиотеки по индексу
const char * Env_GetClassLibraryName(int index)
{
 return PEngine->Env_GetClassLibraryName(index);
}

// Возвращает версию библиотеки по индексу
const char * Env_GetClassLibraryVersion(int index)
{
 return PEngine->Env_GetClassLibraryVersion(index);
}

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
// Возвращает id нового класса в хранилище
int Env_CreateClass(const char* stringid)
{
 return PEngine->Env_CreateClass(stringid);
}

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
int Env_Calculate(const char* stringid)
{
 return PEngine->Env_Calculate(stringid);
}
// --------------------------

// --------------------------
// Методы управления моделью
// ----------------------------
// Удаляет модель
int Model_Destroy(void)
{
 return PEngine->Model_Destroy();
}

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
int Model_Create(int classid)
{
 return PEngine->Model_Create(classid);
}

// Очищает модель
int Model_Clear(void)
{
 return PEngine->Model_Clear();
}

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр контейнера с заданным 'classid'
// если stringid - пустая строка, то добавляет в саму модель
int Model_AddComponent(char* stringid, int classid)
{
 return PEngine->Model_AddComponent(stringid, classid);
}

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр контейнера с заданным 'id'
// если stringid - пустая строка, то удаляет из самой модели
int Model_DelComponent(char* stringid, int id)
{
 return PEngine->Model_DelComponent(stringid, id);
}

// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
int Model_GetNumComponents(char* stringid)
{
 return PEngine->Model_GetNumComponents(stringid);
}

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
int Model_GetComponentsList(char* stringid, int *buffer)
{
 return PEngine->Model_GetComponentsList(stringid, buffer);
}

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
const char* Model_GetComponentName(char* stringid)
{
 return PEngine->Model_GetComponentName(stringid);
}

// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * Model_GetComponentParameters(const char *stringid)
{
 return PEngine->Model_GetComponentParameters(stringid);
}

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * Model_GetComponentSelectedParameters(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedParameters(stringid);
}

// устанавливает параметры компонента по идентификатору
bool Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentParameters(stringid, buffer);
}

// Связывает выбранные контейнеры друг с другом
int Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}

// Разрывает выбранную связь
int Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_BreakLink(stringid1, output_number, stringid2, input_number);
}

// Разрывает все связи
int Model_BreakAllLinks(void)
{
 return PEngine->Model_BreakAllLinks();
}


// Разрывает все входные и выходные связи выбранного контейнера
int Model_BreakAllComponentLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentLinks(stringid);
}

// Разрывает все входные связи выбранного контейнера
int Model_BreakAllComponentInputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentInputLinks(stringid);
}

// Разрывает все выходные связи выбранного контейнера
int Model_BreakAllComponentOutputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentOutputLinks(stringid);
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
const char * Model_GetComponentInternalLinks(char* stringid)
{
 return PEngine->Model_GetComponentInternalLinks(stringid);
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
int Model_SetComponentInternalLinks(char* stringid,char* buffer)
{
 return PEngine->Model_SetComponentInternalLinks(stringid,buffer);
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
const char * Model_GetComponentInputLinks(char* stringid)
{
 return PEngine->Model_GetComponentInputLinks(stringid);
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
const char * Model_GetComponentOutputLinks(char* stringid)
{
 return PEngine->Model_GetComponentOutputLinks(stringid);
}

// Возвращает состояние компонента по идентификатору
// Память для buffer должна быть выделена!
const char * Model_GetComponentState(const char *stringid)
{
 return PEngine->Model_GetComponentState(stringid);
}

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
const char * Model_GetComponentSelectedState(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedState(stringid);
}

// Устанавливает состояние компонента по идентификатору
bool Model_SetComponentState(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentState(stringid, buffer);
}

// Возвращает число входов у компонента
int Model_GetComponentNumInputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumInputs(stringid);
}

// Возвращает размер входа компонента в числе элементов
int Model_GetComponentInputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputSize(stringid, index);
}

// Возвращает размер элемента входа в байтах
int Model_GetComponentInputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputElementSize(stringid, index);
}

// Возвращает размер входа компонента в байтах элементов
int Model_GetComponentInputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputByteSize(stringid, index);
}

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
unsigned char* Model_GetComponentInputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputData(stringid, index);
}

// Возвращает число выходов у компонента
int Model_GetComponentNumOutputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumOutputs(stringid);
}

// Возвращает размер выхода компонента в числе элементов
int Model_GetComponentOutputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputSize(stringid, index);
}

// Возвращает размер элемента выхода в байтах
int Model_GetComponentOutputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputElementSize(stringid, index);
}

// Возвращает размер выхода компонента в байтах элементов
int Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputByteSize(stringid, index);
}

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
unsigned char* Model_GetComponentOutputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputData(stringid, index);
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
const char * Model_SaveComponent(const char *stringid)
{
 return PEngine->Model_SaveComponent(stringid);
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int Model_LoadComponent(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponent(stringid, buffer);
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
const char * Model_SaveComponentParameters(const char *stringid)
{
 return PEngine->Model_SaveComponentParameters(stringid);
}

// Загружает все параметры компонента и его дочерних компонент из xml
int Model_LoadComponentParameters(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentParameters(stringid, buffer);
}

// Сохраняет состояние компонента и его дочерних компонент в xml
const char * Model_SaveComponentState(const char *stringid)
{
 return PEngine->Model_SaveComponentState(stringid);
}

// Загружает состояние компонента и его дочерних компонент из xml
int Model_LoadComponentState(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentState(stringid, buffer);
}
// --------------------------

// --------------------------
// Методы управления исключениями
// ----------------------------
// Возвращает массив строк лога
const char* RDKEngine_GetLog(void)
{
 return PEngine->GetLog();
}
// ----------------------------


// --------------------------
// Методы управления средой
// --------------------------
// Задает число входов среды
void Env_SetNumInputImages(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetNumInputImages(number);
}

// Задает число выходов среды
void Env_SetNumOutputImages(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetNumOutputImages(number);
}

// Задает число входов среды
int Env_GetNumInputImages(void)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetNumInputImages();
}

// Задает число выходов среды
int Env_GetNumOutputImages(void)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetNumInputImages();
}

// Задает разрешение по умолчанию (рабочее разрешение)
void Env_SetInputRes(int number, int width, int height)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetInputRes(number, width, height);
}

void Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetInputImage(number, image, width, height,cmodel);
}


// Возвращает разрешение по умолчанию (рабочее разрешение)
int Env_GetInputImageWidth(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImageWidth(number);
}

int Env_GetInputImageHeight(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImageHeight(number);
}

int Env_GetInputImageColorModel(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImageColorModel(number);
}

// Возвращает текущее выходное разрешение
int Env_GetOutputImageWidth(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageWidth(number);
}

int Env_GetOutputImageHeight(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageHeight(number);
}

int Env_GetOutputImageColorModel(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageColorModel(number);
}

unsigned char* Env_GetInputImage(int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImage(index);
}

unsigned char* Env_GetOutputImage(int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImage(index);
}

unsigned char* Env_GetOutputImageY8(int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageY8(index);
}

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const RDK::UBitmap* const Env_GetComponentOutput(const char *stringid, int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetComponentOutput(stringid, index);
}
// --------------------------

/* ************************************************************************** */

// Менеджер DLL
class RDKDllManager
{
public:
// Массив хранилищ
std::vector<RDK::UAContainerStorage*> StorageList;

// Массив сред
std::vector<RDK::UAContainerEnvironment*> EnvironmentList;

// Массив движков
std::vector<RDK::UEngine*> EngineList;

// ----------------------------------------------------------
// Глобальные указатели на функции создания хранилища и среды
// ----------------------------------------------------------
// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
typedef RDK::UAContainerStorage* (*PCreateNewStorage)(void);
PCreateNewStorage FuncCreateNewStorage;

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
typedef RDK::UAContainerEnvironment* (*PCreateNewEnvironment)(void);
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
// --------------------------
};

// Экземпляр менеджера
RDKDllManager DllManager;

extern RDK::UEngine* PEngine;
extern RDK::UAEnvironment* PEnvironment;
extern RDK::UAStorage* PStorage;


// Менеджер DLL
// --------------------------
// Конструкторы и деструкторы
// --------------------------
RDKDllManager::RDKDllManager(void)
{
 FuncCreateNewStorage=&RDK::NewUStorage;
 FuncCreateNewEnvironment=&RDK::NewUAContainerEnvironment;
 FuncCreateNewEngine=&RDK::NewUEngine;
}

RDKDllManager::~RDKDllManager(void)
{
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
// --------------------------



// ----------------------------
// Внутренние методы инициализации
// ----------------------------
// Деинициализация движка
int Engine_Destroy(void)
{
 PEngine=0;
 PEnvironment=0;
 PStorage=0;
 return 0;
}

int RDKInit(void)
{
  if(!PEngine->Init())
  {
   Engine_Destroy();
   return 3;
  }

  PEnvironment=PEngine->GetEnvironment();
  if(PEnvironment)
   PStorage=PEnvironment->GetStorage();
  else
  {
   Engine_Destroy();
   return 4;
  }

  if(!PStorage)
  {
   Engine_Destroy();
   return 5;
  }

 return 0;
}

int RDKUnInit(void)
{
 if(!PEngine->UnInit())
  return 1;

 return 0;
}

// Загружает набор предустановленных библиотек
int RDKLoadPredefinedLibraries(void)
{
 return PEngine->LoadPredefinedLibraries();
}

// Обработчик исключений библиотеки
// Должен быть вызван в глобальном обработчике пользовательского ПО
int RDKExceptionDispatcher(void *exception)
{
 if(!PEngine)
  return 1;

 RDK::UException *exc=reinterpret_cast<RDK::UException*>(exception);
 PEngine->ProcessException(exc);

 return 0;
}

// Деинициализация движка
extern int Engine_Destroy(void);

// Инициализация движка
int Engine_Create(RDK::UEngine *engine, const char *inifilename, void *pCreateNewStorage, void *pCreateNewEnvironment)
{
 if(!engine)
  return 1;

 if(PEngine != engine)
 {
  Engine_Destroy();

  PEngine=engine;
 }

 if(inifilename)
 {
  if(!PEngine->SetOptionsFileName(inifilename))
  {
   Engine_Destroy();
   return 2;
  }

  if(!PEngine->SetFuncCreateNewStorage(reinterpret_cast<RDK::UEngine::PCreateNewStorage>(pCreateNewStorage)))
   return 10;

  if(!PEngine->SetFuncCreateNewEnvironment(reinterpret_cast<RDK::UEngine::PCreateNewEnvironment>(pCreateNewEnvironment)))
   return 11;
 }
 return 0;
}


int LoadEngine(void *create_storage, void *create_environment, void *create_engine)
{
 if(!DllInit(create_storage, create_environment, create_engine))
  return -2;

 RDK::UEngine* pengine=dynamic_cast<RDK::UEngine*>(AddNewEngine());

 if(!pengine)
  return -3;

 pengine->Default();

 if(Engine_Create(pengine, "options.ini",(void*)AddNewStorage,(void*)AddNewEnvironment))
  return -4;

 return 0;
}

// Инициализация dll
bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine)
{
 // Инициализация dll
 RDK::DLLDllInit=DllInit;

 // Указатель на функцию возвращающую число хранилищ в библиотеке
 RDK::DLLGetNumStorages=GetNumStorages;

 // Указатель на функцию возвращающую число сред в библиотеке
 RDK::DLLGetNumEnvironments=GetNumEnvironments;

 // Указатель на функцию возвращающую число движков в библиотеке
 RDK::DLLGetNumEngines=GetNumEngines;

 // Возвращает хранилище по индексу
 RDK::DLLGetStorage=GetStorage;

 // Возвращает среду по индексу
 RDK::DLLGetEnvironment=GetEnvironment;

 // Возвращает движок по индексу
 RDK::DLLGetEngine=GetEngine;

 // Создает новое хранилище и помещает в конец массива
 // Возвращает указатель на хранилище
 RDK::DLLAddNewStorage=AddNewStorage;

 // Создает новую среду и помещает в конец массива
 // Возвращает указатель на среду
 RDK::DLLAddNewEnvironment=AddNewEnvironment;

 // Создает новый движок и помещает в конец массива
 // Возвращает указатель на движок
 RDK::DLLAddNewEngine=AddNewEngine;

 return DllManager.Init(reinterpret_cast<RDKDllManager::PCreateNewStorage>(pfstorage),
                        reinterpret_cast<RDKDllManager::PCreateNewEnvironment>(pfenvironment),
                        reinterpret_cast<RDKDllManager::PCreateNewEngine>(pfengine));
}

// Возвращает число хранилищ в библиотеке
size_t GetNumStorages(void)
{
 return DllManager.StorageList.size();
}

// Возвращает число сред в библиотеке
size_t GetNumEnvironments(void)
{
 return DllManager.EnvironmentList.size();
}

// Возвращает число движков в библиотеке
size_t GetNumEngines(void)
{
 return DllManager.EngineList.size();
}

// Возвращает хранилище по индексу
RDK::UAContainerStorage* GetStorage(size_t i)
{
 if(i>=DllManager.StorageList.size())
  return 0;

 return DllManager.StorageList[i];
}

// Возвращает среду по индексу
RDK::UAContainerEnvironment* GetEnvironment(size_t i)
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
RDK::UAContainerStorage* AddNewStorage(void)
{
 if(!DllManager.FuncCreateNewStorage)
  return 0;

 RDK::UAContainerStorage* storage=DllManager.FuncCreateNewStorage();
 if(storage)
 {
  CreateStorage(storage);
  DllManager.StorageList.push_back(storage);
 }
 return storage;
}

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
RDK::UAContainerEnvironment* AddNewEnvironment(RDK::UAContainerStorage *storage,bool isinit, std::list<RDK::UAContainer*>* external_classes, std::list<RDK::UALibrary*>* external_libs)
{
 if(!DllManager.FuncCreateNewEnvironment)
  return 0;

 RDK::UAContainerEnvironment* environment=DllManager.FuncCreateNewEnvironment();
 if(environment)
 {
  CreateEnvironment(environment, storage,isinit,external_classes, external_libs);
  DllManager.EnvironmentList.push_back(environment);
 }
 return environment;
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
// ----------------------------




#endif

