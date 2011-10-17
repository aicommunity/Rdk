#ifndef RDK_ENGINE_CPP
#define RDK_ENGINE_CPP

#include "rdk.h"
#include "rdk_engine.h"


RDK::UEngine* PEngine=0;
RDK::UAEnvironment* PEnvironment=0;
RDK::UAStorage* PStorage=0;

// ----------------------------
// Методы инициализации
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
// ----------------------------

// --------------------------
// Методы управления средой
// --------------------------
// Методы управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
int RDKStorage_GetNumClasses(void)
{
 return PEngine->Storage_GetNumClasses();
}

// Возвращает id классов в хранилище. Память должна быть выделена
void RDKStorage_GetClassesList(int *buffer)
{
 PEngine->Storage_GetClassesList(buffer);
}

// Возвращает имя класса по его id.
const char * RDKStorage_GetClassName(int id)
{
 return PEngine->Storage_GetClassName(id);
}

// Возвращает Id класса по его имени
int RDKStorage_GetClassId(const char *name)
{
 return PEngine->Storage_GetClassId(name);
}

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
bool RDKStorage_DelClass(int classid)
{
 return PEngine->Storage_DelClass(classid);
}

// Удалаяет все свободные объекты из хранилища
void RDKStorage_FreeObjectsStorage(void)
{
 PEngine->Storage_FreeObjectsStorage();
}

// Удаляет все объекты из хранилища
void RDKStorage_ClearObjectsStorage(void)
{
 PEngine->Storage_ClearObjectsStorage();
}

// Вычисляет суммарное число объектов в хранилище
int RDKStorage_CalcNumObjects(void)
{
 return PEngine->Storage_CalcNumObjects();
}

int RDKStorage_CalcNumObjectsById(int classid)
{
 return PEngine->Storage_CalcNumObjectsById(classid);
}

int RDKStorage_CalcNumObjectsByName(const char* classname)
{
 return PEngine->Storage_CalcNumObjectsByName(classname);
}

// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
int RDKEnv_GetPredefinedStructure(void)
{
 return PEngine->Env_GetPredefinedStructure();
}

bool RDKEnv_SetPredefinedStructure(int value)
{
 return PEngine->Env_SetPredefinedStructure(value);
}

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool RDKEnv_IsStoragePresent(void)
{
 return PEngine->Env_IsStoragePresent();
}

// Возвращает состояние инициализации
bool RDKEnv_IsInit(void)
{
 return PEngine->Env_IsInit();
}

// Признак наличия сформированной структуры
bool RDKEnv_IsStructured(void)
{
 return PEngine->Env_IsStructured();
}

// Инициализация среды
bool RDKEnv_Init(void)
{
 return PEngine->Env_Init();
}

// Деинициализация среды
bool RDKEnv_UnInit(void)
{
 return PEngine->Env_UnInit();
}

// Формирует предварительно заданную модель обработки
bool RDKEnv_CreateStructure(void)
{
 return PEngine->Env_CreateStructure();
}

// Уничтожает текущую модель обработки
bool RDKEnv_DestroyStructure(void)
{
 return PEngine->Env_DestroyStructure();
}

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
void RDKEnv_Destroy(void)
{
 return PEngine->Env_Destroy();
}

// Загружает библиотеку по имени dll-файла
int RDKEnv_LoadStorageLibrary(const char *filename)
{
 return PEngine->Env_LoadStorageLibrary(filename);
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool RDKEnv_DelClassLibraryByIndex(int index)
{
 return PEngine->Env_DelClassLibraryByIndex(index);
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool RDKEnv_DelClassLibraryByName(const char *name)
{
 return PEngine->Env_DelClassLibraryByName(name);
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool RDKEnv_DelAllClassLibraries(void)
{
 return PEngine->Env_DelAllClassLibraries();
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool RDKEnv_BuildStorage(void)
{
 return PEngine->Env_BuildStorage();
}

// Возвращает число библиотек
int RDKEnv_GetNumClassLibraries(void)
{
 return PEngine->Env_GetNumClassLibraries();
}

// Возвращает имя библиотеки по индексу
const char * RDKEnv_GetClassLibraryName(int index)
{
 return PEngine->Env_GetClassLibraryName(index);
}

// Возвращает версию библиотеки по индексу
const char * RDKEnv_GetClassLibraryVersion(int index)
{
 return PEngine->Env_GetClassLibraryVersion(index);
}

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
// Возвращает id нового класса в хранилище
int RDKEnv_CreateClass(const char* stringid)
{
 return PEngine->Env_CreateClass(stringid);
}

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
int RDKEnv_Calculate(const char* stringid)
{
 return PEngine->Env_Calculate(stringid);
}

// Методы управления моделью
// ----------------------------
// Удаляет модель
int RDKModel_Destroy(void)
{
 return PEngine->Model_Destroy();
}

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
int RDKModel_Create(int classid)
{
 return PEngine->Model_Create(classid);
}

// Очищает модель
int RDKModel_Clear(void)
{
 return PEngine->Model_Clear();
}

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр контейнера с заданным 'classid'
// если stringid - пустая строка, то добавляет в саму модель
int RDKModel_AddComponent(char* stringid, int classid)
{
 return PEngine->Model_AddComponent(stringid, classid);
}

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр контейнера с заданным 'id'
// если stringid - пустая строка, то удаляет из самой модели
int RDKModel_DelComponent(char* stringid, int id)
{
 return PEngine->Model_DelComponent(stringid, id);
}

// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
int RDKModel_GetNumComponents(char* stringid)
{
 return PEngine->Model_GetNumComponents(stringid);
}

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
int RDKModel_GetComponentsList(char* stringid, int *buffer)
{
 return PEngine->Model_GetComponentsList(stringid, buffer);
}

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
const char* RDKModel_GetComponentName(char* stringid)
{
 return PEngine->Model_GetComponentName(stringid);
}

// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDKModel_GetComponentParameters(const char *stringid)
{
 return PEngine->Model_GetComponentParameters(stringid);
}

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDKModel_GetComponentSelectedParameters(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedParameters(stringid);
}

// устанавливает параметры компонента по идентификатору
bool RDKModel_SetComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentParameters(stringid, buffer);
}

// Связывает выбранные контейнеры друг с другом
int RDKModel_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}

// Разрывает выбранную связь
int RDKModel_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_BreakLink(stringid1, output_number, stringid2, input_number);
}

// Разрывает все связи
int RDKModel_BreakAllLinks(void)
{
 return PEngine->Model_BreakAllLinks();
}


// Разрывает все входные и выходные связи выбранного контейнера
int RDKModel_BreakAllComponentLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentLinks(stringid);
}

// Разрывает все входные связи выбранного контейнера
int RDKModel_BreakAllComponentInputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentInputLinks(stringid);
}

// Разрывает все выходные связи выбранного контейнера
int RDKModel_BreakAllComponentOutputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentOutputLinks(stringid);
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
const char * RDKModel_GetComponentInternalLinks(char* stringid)
{
 return PEngine->Model_GetComponentInternalLinks(stringid);
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
int RDKModel_SetComponentInternalLinks(char* stringid,char* buffer)
{
 return PEngine->Model_SetComponentInternalLinks(stringid,buffer);
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
const char * RDKModel_GetComponentInputLinks(char* stringid)
{
 return PEngine->Model_GetComponentInputLinks(stringid);
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
const char * RDKModel_GetComponentOutputLinks(char* stringid)
{
 return PEngine->Model_GetComponentOutputLinks(stringid);
}

// Возвращает состояние компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDKModel_GetComponentState(const char *stringid)
{
 return PEngine->Model_GetComponentState(stringid);
}

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDKModel_GetComponentSelectedState(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedState(stringid);
}

// Устанавливает состояние компонента по идентификатору
bool RDKModel_SetComponentState(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentState(stringid, buffer);
}

// Возвращает число входов у компонента
int RDKModel_GetComponentNumInputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumInputs(stringid);
}

// Возвращает размер входа компонента в числе элементов
int RDKModel_GetComponentInputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputSize(stringid, index);
}

// Возвращает размер элемента входа в байтах
int RDKModel_GetComponentInputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputElementSize(stringid, index);
}

// Возвращает размер входа компонента в байтах элементов
int RDKModel_GetComponentInputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputByteSize(stringid, index);
}

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
unsigned char* RDKModel_GetComponentInputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputData(stringid, index);
}

// Возвращает число выходов у компонента
int RDKModel_GetComponentNumOutputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumOutputs(stringid);
}

// Возвращает размер выхода компонента в числе элементов
int RDKModel_GetComponentOutputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputSize(stringid, index);
}

// Возвращает размер элемента выхода в байтах
int RDKModel_GetComponentOutputElementSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputElementSize(stringid, index);
}

// Возвращает размер выхода компонента в байтах элементов
int RDKModel_GetComponentOutputByteSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputByteSize(stringid, index);
}

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
unsigned char* RDKModel_GetComponentOutputData(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputData(stringid, index);
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
const char * RDKModel_SaveComponent(const char *stringid)
{
 return PEngine->Model_SaveComponent(stringid);
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int RDKModel_LoadComponent(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponent(stringid, buffer);
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
const char * RDKModel_SaveComponentParameters(const char *stringid)
{
 return PEngine->Model_SaveComponentParameters(stringid);
}

// Загружает все параметры компонента и его дочерних компонент из xml
int RDKModel_LoadComponentParameters(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentParameters(stringid, buffer);
}

// Сохраняет состояние компонента и его дочерних компонент в xml
const char * RDKModel_SaveComponentState(const char *stringid)
{
 return PEngine->Model_SaveComponentState(stringid);
}

// Загружает состояние компонента и его дочерних компонент из xml
int RDKModel_LoadComponentState(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentState(stringid, buffer);
}
// --------------------------

#endif


