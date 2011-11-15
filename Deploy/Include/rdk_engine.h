#ifndef RDK_ENGINE_H
#define RDK_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------
// Методы инициализации
// ----------------------------
RDK_LIB_TYPE int LoadEngine(void *create_storage, void *create_environment, void *create_engine);

// Инициализирует движок (функция должна быть вызвана первой!)
RDK_LIB_TYPE int RDK_EngineInit(int predefined_structure);

// Инициализация библиотеки
RDK_LIB_TYPE int RDKInit(void);

// Деинициализация библиотеки
RDK_LIB_TYPE int RDKUnInit(void);

// Загружает набор предустановленных библиотек
RDK_LIB_TYPE int RDKLoadPredefinedLibraries(void);
// ----------------------------

// --------------------------
// Методы управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
RDK_LIB_TYPE int RDKStorage_GetNumClasses(void);

// Возвращает id классов в хранилище. Память должна быть выделена
RDK_LIB_TYPE void RDKStorage_GetClassesList(int *buffer);

// Возвращает имя класса по его id.
RDK_LIB_TYPE const char * RDKStorage_GetClassName(int id);

// Возвращает Id класса по его имени
RDK_LIB_TYPE int RDKStorage_GetClassId(const char *name);

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
RDK_LIB_TYPE bool RDKStorage_DelClass(int classid);

// Удаляет все свободные объекты из хранилища
RDK_LIB_TYPE void RDKStorage_FreeObjectsStorage(void);

// Удаляет все объекты из хранилища
RDK_LIB_TYPE void RDKStorage_ClearObjectsStorage(void);

// Вычисляет суммарное число объектов в хранилище
RDK_LIB_TYPE int RDKStorage_CalcNumObjects(void);
RDK_LIB_TYPE int RDKStorage_CalcNumObjectsById(int classid);
RDK_LIB_TYPE int RDKStorage_CalcNumObjectsByName(const char* classname);
// --------------------------

// --------------------------
// Методы управления средой
// ----------------------------
// Индекс предварительно заданной модели обработки
RDK_LIB_TYPE int RDKEnv_GetPredefinedStructure(void);
RDK_LIB_TYPE bool RDKEnv_SetPredefinedStructure(int value);

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
RDK_LIB_TYPE bool RDKEnv_IsStoragePresent(void);

// Возвращает состояние инициализации
RDK_LIB_TYPE bool RDKEnv_IsInit(void);

// Признак наличия сформированной структуры
RDK_LIB_TYPE bool RDKEnv_IsStructured(void);

// Инициализация среды
RDK_LIB_TYPE bool RDKEnv_Init(void);

// Деинициализация среды
RDK_LIB_TYPE bool RDKEnv_UnInit(void);

// Формирует предварительно заданную модель обработки
RDK_LIB_TYPE bool RDKEnv_CreateStructure(void);

// Уничтожает текущую модель обработки
RDK_LIB_TYPE bool RDKEnv_DestroyStructure(void);

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
RDK_LIB_TYPE void RDKEnv_Destroy(void);

// Загружает библиотеку по имени dll-файла
RDK_LIB_TYPE int RDKEnv_LoadStorageLibrary(const char *filename);

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
RDK_LIB_TYPE bool RDKEnv_DelClassLibraryByIndex(int index);

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
RDK_LIB_TYPE bool RDKEnv_DelClassLibraryByName(const char *name);

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
RDK_LIB_TYPE bool RDKEnv_DelAllClassLibraries(void);

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
RDK_LIB_TYPE bool RDKEnv_BuildStorage(void);

// Возвращает число библиотек
RDK_LIB_TYPE int RDKEnv_GetNumClassLibraries(void);

// Возвращает имя библиотеки по индексу
RDK_LIB_TYPE const char * RDKEnv_GetClassLibraryName(int index);

// Возвращает версию библиотеки по индексу
RDK_LIB_TYPE const char * RDKEnv_GetClassLibraryVersion(int index);

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
// Возвращает id нового класса в хранилище
RDK_LIB_TYPE int RDKEnv_CreateClass(const char* stringid);

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
RDK_LIB_TYPE int RDKEnv_Calculate(const char* stringid=0);
// ----------------------------

// ----------------------------
// Методы управления моделью
// ----------------------------
// Удаляет модель
RDK_LIB_TYPE int RDKModel_Destroy(void);

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
RDK_LIB_TYPE int RDKModel_Create(int classid);

// Очищает модель
RDK_LIB_TYPE int RDKModel_Clear(void);

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр контейнера с заданным 'classid'
// если stringid - пустая строка, то добавляет в саму модель
RDK_LIB_TYPE int RDKModel_AddComponent(char* stringid, int classid);

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр контейнера с заданным 'id'
// если stringid - пустая строка, то удаляет из самой модели
RDK_LIB_TYPE int RDKModel_DelComponent(char* stringid, int id);

// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
RDK_LIB_TYPE int RDKModel_GetNumComponents(char* stringid);

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
RDK_LIB_TYPE int RDKModel_GetComponentsList(char* stringid, int *buffer);

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
RDK_LIB_TYPE const char* RDKModel_GetComponentName(char* stringid);

// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
RDK_LIB_TYPE const char * RDKModel_GetComponentParameters(const char *stringid);

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
RDK_LIB_TYPE const char * RDKModel_GetComponentSelectedParameters(const char *stringid);

// устанавливает параметры компонента по идентификатору
RDK_LIB_TYPE bool RDKModel_SetComponentParameters(const char *stringid, const char* buffer);

// Связывает выбранные контейнеры друг с другом
RDK_LIB_TYPE int RDKModel_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number);

// Разрывает выбранную связь
RDK_LIB_TYPE int RDKModel_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number);

// Разрывает все связи
RDK_LIB_TYPE int RDKModel_BreakAllLinks(void);

// Разрывает все входные и выходные связи выбранного контейнера
RDK_LIB_TYPE int RDKModel_BreakAllComponentLinks(char* stringid);

// Разрывает все входные связи выбранного контейнера
RDK_LIB_TYPE int RDKModel_BreakAllComponentInputLinks(char* stringid);

// Разрывает все выходные связи выбранного контейнера
RDK_LIB_TYPE int RDKModel_BreakAllComponentOutputLinks(char* stringid);

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
RDK_LIB_TYPE const char * RDKModel_GetComponentInternalLinks(char* stringid);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
RDK_LIB_TYPE int RDKModel_SetComponentInternalLinks(char* stringid, char* buffer);

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
RDK_LIB_TYPE const char * RDKModel_GetComponentInputLinks(char* stringid);

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
RDK_LIB_TYPE const char * RDKModel_GetComponentOutputLinks(char* stringid);

// Возвращает состояние компонента по идентификатору
RDK_LIB_TYPE const char * RDKModel_GetComponentState(const char *stringid);

// Возвращает выборочные данные состояния компонента по идентификатору
RDK_LIB_TYPE const char * RDKModel_GetComponentSelectedState(const char *stringid);

// Устанавливает состояние компонента по идентификатору
RDK_LIB_TYPE bool RDKModel_SetComponentState(const char *stringid, const char* buffer);

// Возвращает число входов у компонента
RDK_LIB_TYPE int RDKModel_GetComponentNumInputs(const char *stringid);

// Возвращает размер входа компонента в числе элементов
RDK_LIB_TYPE int RDKModel_GetComponentInputSize(const char *stringid, int index);

// Возвращает размер элемента входа в байтах
RDK_LIB_TYPE int RDKModel_GetComponentInputElementSize(const char *stringid, int index);

// Возвращает размер входа компонента в байтах элементов
RDK_LIB_TYPE int RDKModel_GetComponentInputByteSize(const char *stringid, int index);

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
RDK_LIB_TYPE unsigned char* RDKModel_GetComponentInputData(const char *stringid, int index);

// Возвращает число выходов у компонента
RDK_LIB_TYPE int RDKModel_GetComponentNumOutputs(const char *stringid);

// Возвращает размер выхода компонента в числе элементов
RDK_LIB_TYPE int RDKModel_GetComponentOutputSize(const char *stringid, int index);

// Возвращает размер элемента выхода в байтах
RDK_LIB_TYPE int RDKModel_GetComponentOutputElementSize(const char *stringid, int index);

// Возвращает размер выхода компонента в байтах элементов
RDK_LIB_TYPE int RDKModel_GetComponentOutputByteSize(const char *stringid, int index);

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
RDK_LIB_TYPE unsigned char* RDKModel_GetComponentOutputData(const char *stringid, int index);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
RDK_LIB_TYPE const char * RDKModel_SaveComponent(const char *stringid);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
RDK_LIB_TYPE int RDKModel_LoadComponent(const char *stringid, char* buffer);

// Сохраняет все параметры компонента и его дочерних компонент в xml
RDK_LIB_TYPE const char * RDKModel_SaveComponentParameters(const char *stringid);

// Загружает все параметры компонента и его дочерних компонент из xml
RDK_LIB_TYPE int RDKModel_LoadComponentParameters(const char *stringid, char* buffer);

// Сохраняет состояние компонента и его дочерних компонент в xml
RDK_LIB_TYPE const char * RDKModel_SaveComponentState(const char *stringid);

// Загружает состояние компонента и его дочерних компонент из xml
RDK_LIB_TYPE int RDKModel_LoadComponentState(const char *stringid, char* buffer);
// --------------------------


#ifdef __cplusplus
}
#endif

#endif
