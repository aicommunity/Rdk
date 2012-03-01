#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include "rdk_init.h"
#include "rdk.h"
//#include "rdk_gengine.h"

// --------------------------------------
// Объявления дополнительных функций
// --------------------------------------
// Возвращает хранилище по индексу
RDK_LIB_TYPE RDK::UAContainerStorage* RDK_CALL GetStorage(size_t i);

// Возвращает среду по индексу
RDK_LIB_TYPE RDK::UAContainerEnvironment*  RDK_CALL GetEnvironment(size_t i);

// Возвращает движок по индексу
RDK_LIB_TYPE RDK::UEngine*  RDK_CALL GetEngine(size_t i);

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
RDK_LIB_TYPE RDK::UAContainerStorage*  RDK_CALL AddNewStorage(void);

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
RDK_LIB_TYPE RDK::UAContainerEnvironment*  RDK_CALL AddNewEnvironment(RDK::UAContainerStorage *storage=0,bool isinit=true, std::list<RDK::UAContainer*>* external_classes=0, std::list<RDK::UALibrary*>* external_libs=0);

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
RDK_LIB_TYPE RDK::UEngine*  RDK_CALL AddNewEngine(void);
// --------------------------------------

RDK::UEPtr<RDK::UEngine> PEngine=0;
RDK::UEPtr<RDK::UAEnvironment> PEnvironment=0;
RDK::UEPtr<RDK::UAStorage> PStorage=0;

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
int RDK_CALL EngineInit(int predefined_structure)
{
 LoadEngine((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine);
 Init();
 LoadPredefinedLibraries();

 Env_SetPredefinedStructure(predefined_structure);
 Env_CreateStructure();

 return 0;
}

int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx)
{
 LoadEngine((void*)CreateNewGStorage, (void*)CreateNewGEnvironment, (void*)CreateNewGEngine);
 Init();
 LoadPredefinedLibraries();

 // Задает число входов среды
 Env_SetNumInputImages(num_inputs);
 Env_SetNumOutputImages(num_outputs);

 // Задает разрешение по умолчанию (рабочее разрешение)
 for(int i=0;i<num_inputs;i++)
  Env_SetInputRes(i, input_width, input_height);

 Env_SetReflectionXFlag(reflectionx);

 Env_SetPredefinedStructure(predefined_structure);
 Env_CreateStructure();

 return 0;
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
const char* RDK_CALL Storage_GetClassDescription(int classid)
{
 return PEngine->Storage_GetClassDescription(classid);
}

// Устанавливает описание класса по его id, считывая его из формата xml
bool RDK_CALL Storage_SetClassDescription(int classid, const char* description)
{
 return PEngine->Storage_SetClassDescription(classid, description);
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
// ----------------------------

// ----------------------------
// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
int RDK_CALL Env_GetPredefinedStructure(void)
{
 return PEngine->Env_GetPredefinedStructure();
}

bool RDK_CALL Env_SetPredefinedStructure(int value)
{
 return PEngine->Env_SetPredefinedStructure(value);
}

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool RDK_CALL Env_IsStoragePresent(void)
{
 return PEngine->Env_IsStoragePresent();
}

// Возвращает состояние инициализации
bool RDK_CALL Env_IsInit(void)
{
 return PEngine->Env_IsInit();
}

// Признак наличия сформированной структуры
bool RDK_CALL Env_IsStructured(void)
{
 return PEngine->Env_IsStructured();
}

// Инициализация среды
bool RDK_CALL Env_Init(void)
{
 return PEngine->Env_Init();
}

// Деинициализация среды
bool RDK_CALL Env_UnInit(void)
{
 return PEngine->Env_UnInit();
}

// Формирует предварительно заданную модель обработки
bool RDK_CALL Env_CreateStructure(void)
{
 return PEngine->Env_CreateStructure();
}

// Уничтожает текущую модель обработки
bool RDK_CALL Env_DestroyStructure(void)
{
 return PEngine->Env_DestroyStructure();
}

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
void RDK_CALL Env_Destroy(void)
{
 return PEngine->Env_Destroy();
}

// Загружает библиотеку по имени dll-файла
int RDK_CALL Env_LoadStorageLibrary(const char *filename)
{
 return PEngine->Env_LoadStorageLibrary(filename);
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool RDK_CALL Env_DelClassLibraryByIndex(int index)
{
 return PEngine->Env_DelClassLibraryByIndex(index);
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool RDK_CALL Env_DelClassLibraryByName(const char *name)
{
 return PEngine->Env_DelClassLibraryByName(name);
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool RDK_CALL Env_DelAllClassLibraries(void)
{
 return PEngine->Env_DelAllClassLibraries();
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool RDK_CALL Env_BuildStorage(void)
{
 return PEngine->Env_BuildStorage();
}

// Возвращает число библиотек
int RDK_CALL Env_GetNumClassLibraries(void)
{
 return PEngine->Env_GetNumClassLibraries();
}

// Возвращает имя библиотеки по индексу
const char * RDK_CALL Env_GetClassLibraryName(int index)
{
 return PEngine->Env_GetClassLibraryName(index);
}

// Возвращает версию библиотеки по индексу
const char * RDK_CALL Env_GetClassLibraryVersion(int index)
{
 return PEngine->Env_GetClassLibraryVersion(index);
}

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
// Возвращает id нового класса в хранилище
int RDK_CALL Env_CreateClass(const char* stringid)
{
 return PEngine->Env_CreateClass(stringid);
}

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
int RDK_CALL Env_Calculate(const char* stringid)
{
 return PEngine->Env_Calculate(stringid);
}
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
int RDK_CALL Model_Create(int classid)
{
 return PEngine->Model_Create(classid);
}

// Очищает модель
int RDK_CALL Model_Clear(void)
{
 return PEngine->Model_Clear();
}

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр контейнера с заданным 'classid'
// если stringid - пустая строка, то добавляет в саму модель
int RDK_CALL Model_AddComponent(char* stringid, int classid)
{
 return PEngine->Model_AddComponent(stringid, classid);
}

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр контейнера с заданным 'id'
// если stringid - пустая строка, то удаляет из самой модели
int RDK_CALL Model_DelComponent(char* stringid, int id)
{
 return PEngine->Model_DelComponent(stringid, id);
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

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
const char* RDK_CALL Model_GetComponentName(const char* stringid)
{
 return PEngine->Model_GetComponentName(stringid);
}

// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentParameters(const char *stringid)
{
 return PEngine->Model_GetComponentParameters(stringid);
}

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedParameters(stringid);
}

// Возвращает параметры компонента по идентификатору с описаниями
const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid)
{
 return PEngine->Model_GetComponentParametersEx(stringid);
}

// Возвращает значение параметра компонента по идентификатору компонента и имени параметра
const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname)
{
 return PEngine->Model_GetComponentParameterValue(stringid,paramname);
}

// Устанавливает параметры компонента по идентификатору
bool RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentParameters(stringid, buffer);
}

// Устанавливает значение параметра компонента по идентификатору компонента и имени параметра
void RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer)
{
 PEngine->Model_SetComponentParameterValue(stringid,paramname,buffer);
}

// Связывает выбранные контейнеры друг с другом
int RDK_CALL Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}

// Разрывает выбранную связь
int RDK_CALL Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 return PEngine->Model_BreakLink(stringid1, output_number, stringid2, input_number);
}

// Разрывает все связи
int RDK_CALL Model_BreakAllLinks(void)
{
 return PEngine->Model_BreakAllLinks();
}


// Разрывает все входные и выходные связи выбранного контейнера
int RDK_CALL Model_BreakAllComponentLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentLinks(stringid);
}

// Разрывает все входные связи выбранного контейнера
int RDK_CALL Model_BreakAllComponentInputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentInputLinks(stringid);
}

// Разрывает все выходные связи выбранного контейнера
int RDK_CALL Model_BreakAllComponentOutputLinks(char* stringid)
{
 return PEngine->Model_BreakAllComponentOutputLinks(stringid);
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
const char * RDK_CALL Model_GetComponentInternalLinks(char* stringid)
{
 return PEngine->Model_GetComponentInternalLinks(stringid);
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
int RDK_CALL Model_SetComponentInternalLinks(char* stringid,char* buffer)
{
 return PEngine->Model_SetComponentInternalLinks(stringid,buffer);
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
const char * RDK_CALL Model_GetComponentInputLinks(char* stringid)
{
 return PEngine->Model_GetComponentInputLinks(stringid);
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
const char * RDK_CALL Model_GetComponentOutputLinks(char* stringid)
{
 return PEngine->Model_GetComponentOutputLinks(stringid);
}

// Возвращает состояние компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentState(const char *stringid)
{
 return PEngine->Model_GetComponentState(stringid);
}

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid)
{
 return PEngine->Model_GetComponentSelectedState(stringid);
}

// Возвращает значение переменной состояния компонента по идентификатору компонента и имени переменной
const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename)
{
 return PEngine->Model_GetComponentStateValue(stringid,statename);
}

// Устанавливает состояние компонента по идентификатору
bool RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer)
{
 return PEngine->Model_SetComponentState(stringid, buffer);
}

// Устанавливает значение переменной состояния компонента по идентификатору компонента и имени переменной
void RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer)
{
 PEngine->Model_SetComponentStateValue(stringid,statename,buffer);
}

// Возвращает число входов у компонента
int RDK_CALL Model_GetComponentNumInputs(const char *stringid)
{
 return PEngine->Model_GetComponentNumInputs(stringid);
}

// Возвращает размер входа компонента в числе элементов
int RDK_CALL Model_GetComponentInputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentInputSize(stringid, index);
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
int RDK_CALL Model_GetComponentOutputSize(const char *stringid, int index)
{
 return PEngine->Model_GetComponentOutputSize(stringid, index);
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
const char * RDK_CALL Model_SaveComponent(const char *stringid)
{
 return PEngine->Model_SaveComponent(stringid);
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int RDK_CALL Model_LoadComponent(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponent(stringid, buffer);
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
const char * RDK_CALL Model_SaveComponentParameters(const char *stringid)
{
 return PEngine->Model_SaveComponentParameters(stringid);
}

// Загружает все параметры компонента и его дочерних компонент из xml
int RDK_CALL Model_LoadComponentParameters(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentParameters(stringid, buffer);
}

// Сохраняет состояние компонента и его дочерних компонент в xml
const char * RDK_CALL Model_SaveComponentState(const char *stringid)
{
 return PEngine->Model_SaveComponentState(stringid);
}

// Загружает состояние компонента и его дочерних компонент из xml
int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer)
{
 return PEngine->Model_LoadComponentState(stringid, buffer);
}
// --------------------------

// --------------------------
// Методы управления исключениями
// ----------------------------
// Управление функцией-обработчиком исключений
void* RDK_CALL Engine_GetExceptionHandler(void)
{
 return PEngine->GetExceptionHandler();
}

bool RDK_CALL Engine_SetExceptionHandler(void* value)
{
 return PEngine->SetExceptionHandler(reinterpret_cast<RDK::UEngine::PExceptionHandler>(value));
}

// Возвращает массив строк лога
const char* RDK_CALL Engine_GetLog(void)
{
 return PEngine->GetLog();
}
// ----------------------------


// --------------------------
// Методы управления средой
// --------------------------
// Задает число входов среды
void RDK_CALL Env_SetNumInputImages(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetNumInputImages(number);
}

// Задает число выходов среды
void RDK_CALL Env_SetNumOutputImages(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetNumOutputImages(number);
}

// Задает число входов среды
int RDK_CALL Env_GetNumInputImages(void)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetNumInputImages();
}

// Задает число выходов среды
int RDK_CALL Env_GetNumOutputImages(void)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetNumInputImages();
}

// Задает разрешение по умолчанию (рабочее разрешение)
void RDK_CALL Env_SetInputRes(int number, int width, int height)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetInputRes(number, width, height);
}

// Задает данные изображения
void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetInputImage(number, image, width, height,cmodel);
}

// Задает флаг отражения входного изображения вокруг горизонтальной оси
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_SetReflectionXFlag(value);
}


// Возвращает разрешение по умолчанию (рабочее разрешение)
int RDK_CALL Env_GetInputImageWidth(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetInputImageWidth(number);
}

int RDK_CALL Env_GetInputImageHeight(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetInputImageHeight(number);
}

int RDK_CALL Env_GetInputImageColorModel(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetInputImageColorModel(number);
}

// Возвращает текущее выходное разрешение
int RDK_CALL Env_GetOutputImageWidth(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImageWidth(number);
}

int RDK_CALL Env_GetOutputImageHeight(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImageHeight(number);
}

int RDK_CALL Env_GetOutputImageColorModel(int number)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImageColorModel(number);
}

unsigned char* RDK_CALL Env_GetInputImage(int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetInputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImage(int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImageY8(int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Env_GetOutputImageY8(index);
}
// --------------------------

// --------------------------
// Методы управления графической моделью
// --------------------------
// Возвращает указатель на выход с индексом 'index' компонента 'id'
const /* RDK::UBitmap* */ void* const RDK_CALL Model_GetComponentOutput(const char *stringid, int index)
{
 return RDK::dynamic_pointer_cast<RDK::UBEngine>(PEngine)->Model_GetComponentOutput(stringid, index);
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

extern RDK::UEPtr<RDK::UEngine> PEngine;
extern RDK::UEPtr<RDK::UAEnvironment> PEnvironment;
extern RDK::UEPtr<RDK::UAStorage> PStorage;


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

int RDK_CALL Init(void)
{
 try {
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

 PEnvironment->Default();
 }
 catch (RDK::Exception * exception)
 {
  PEngine->ProcessException(exception);
 }
 return 0;
}

int RDK_CALL UnInit(void)
{
 try
 {
  if(!PEngine->UnInit())
   return 1;
 }
 catch (RDK::Exception * exception)
 {
  PEngine->ProcessException(exception);
 }

 return 0;
}

// Загружает набор предустановленных библиотек
int RDK_CALL LoadPredefinedLibraries(void)
{
 return PEngine->LoadPredefinedLibraries();
}

// Обработчик исключений библиотеки
// Должен быть вызван в глобальном обработчике пользовательского ПО
int RDK_CALL ExceptionDispatcher(void *exception)
{
 if(!PEngine)
  return 1;

 RDK::Exception *exc=reinterpret_cast<RDK::Exception*>(exception);
 PEngine->ProcessException(exc);

 return 0;
}

// Деинициализация движка
extern int Engine_Destroy(void);

// Инициализация движка
int RDK_CALL Engine_Create(RDK::UEngine *engine, const char *inifilename, void *pCreateNewStorage, void *pCreateNewEnvironment)
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


int RDK_CALL LoadEngine(void *create_storage, void *create_environment, void *create_engine)
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
bool RDK_CALL DllInit(void* pfstorage,void* pfenvironment,void* pfengine)
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
int RDK_CALL GetNumStorages(void)
{
 return DllManager.StorageList.size();
}

// Возвращает число сред в библиотеке
int RDK_CALL GetNumEnvironments(void)
{
 return DllManager.EnvironmentList.size();
}

// Возвращает число движков в библиотеке
int RDK_CALL GetNumEngines(void)
{
 return DllManager.EngineList.size();
}

// Возвращает хранилище по индексу
RDK::UAContainerStorage* RDK_CALL GetStorage(size_t i)
{
 if(i>=DllManager.StorageList.size())
  return 0;

 return DllManager.StorageList[i];
}

// Возвращает среду по индексу
RDK::UAContainerEnvironment* RDK_CALL GetEnvironment(size_t i)
{
 if(i>=DllManager.EnvironmentList.size())
  return 0;

 return DllManager.EnvironmentList[i];
}

// Возвращает движок по индексу
RDK::UEngine* RDK_CALL GetEngine(size_t i)
{
 if(i>=DllManager.EngineList.size())
  return 0;

 return DllManager.EngineList[i];
}


// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
RDK::UAContainerStorage* RDK_CALL AddNewStorage(void)
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
RDK::UAContainerEnvironment* RDK_CALL AddNewEnvironment(RDK::UAContainerStorage *storage,bool isinit, std::list<RDK::UAContainer*>* external_classes, std::list<RDK::UALibrary*>* external_libs)
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
RDK::UEngine* RDK_CALL AddNewEngine(void)
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

