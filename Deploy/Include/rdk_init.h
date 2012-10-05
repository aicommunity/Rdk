#ifndef RDK_INIT_H
#define RDK_INIT_H

#ifdef __cplusplus
extern "C"  {
#else
typedef int bool;
#endif

// Варианты типа свойства (битовая маска) pt - Property Type
// 0x1 - Параметр
// 0x2 - Переменная состояния
// 0x4 - Временная переменная
enum {ptParameter=1, ptState=2, ptTemp=4, ptAny=255};

// Варианты групп свойства (битовая маска) pg - Property Group
// 0x100 - Общедоступный
// 0x200 - Системный
// 0x400 - Входные данные
// 0x800 - Выходные данные
// 0x1000 - Флаг смены режима работы компонента
enum {pgPublic=0x100, pgSystem=0x200, pgInput=0x400, pgOutput=0x800, pgMode=0x1000, pgAny=0xFFFFFF};


// ----------------------------
// Функции инициализации
// ----------------------------
// Инициализирует движок (функция должна быть вызвана первой!)
RDK_LIB_TYPE int RDK_CALL EngineInit(int predefined_structure, void* exception_handler=0);

// Инициализирует графический движок (функция должна быть вызвана первой!)
RDK_LIB_TYPE int RDK_CALL GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height, bool reflectionx=false,
		void* exception_handler=0);

// Деинициализирует движок (функция автоматически вызывается при вызове инициализации)
RDK_LIB_TYPE int RDK_CALL EngineUnInit(void);
// ----------------------------

// --------------------------
// Функции управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
RDK_LIB_TYPE int RDK_CALL Storage_GetNumClasses(void);

// Возвращает id классов в хранилище. Память должна быть выделена
RDK_LIB_TYPE void RDK_CALL Storage_GetClassesList(int *buffer);

// Возвращает имя класса по его id.
RDK_LIB_TYPE const char * RDK_CALL Storage_GetClassName(int id);

// Возвращает Id класса по его имени
RDK_LIB_TYPE int RDK_CALL Storage_GetClassId(const char *name);

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
RDK_LIB_TYPE bool RDK_CALL Storage_DelClass(int classid);

// Удаляет все свободные объекты из хранилища
RDK_LIB_TYPE void RDK_CALL Storage_FreeObjectsStorage(void);

// Удаляет все объекты из хранилища
RDK_LIB_TYPE void RDK_CALL Storage_ClearObjectsStorage(void);

// Вычисляет суммарное число объектов в хранилище
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjects(void);
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjectsById(int classid);
RDK_LIB_TYPE int RDK_CALL Storage_CalcNumObjectsByName(const char* classname);

// Возвращает описание класса по его id в формате xml
RDK_LIB_TYPE const char* RDK_CALL Storage_GetClassDescription(int classid);

// Устанавливает описание класса по его id, считывая его из формата xml
RDK_LIB_TYPE bool RDK_CALL Storage_SetClassDescription(int classid, const char* description);

// Сохраняет описание всех классов в xml
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveClassesDescription(void);

// Загружает описание всех классов из xml
RDK_LIB_TYPE bool RDK_CALL Storage_LoadClassesDescription(const char* xmltext);

// Сохраняет общее описание всех классов в xml
RDK_LIB_TYPE const char* RDK_CALL Storage_SaveCommonClassesDescription(void);

// Загружает общее описание всех классов из xml
RDK_LIB_TYPE bool RDK_CALL Storage_LoadCommonClassesDescription(const char* xmltext);
// --------------------------

// --------------------------
// Функции управления средой
// ----------------------------
// Загружает библиотеку по имени dll-файла
RDK_LIB_TYPE int RDK_CALL Env_LoadStorageLibrary(const char *filename);

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
RDK_LIB_TYPE bool RDK_CALL Env_DelClassLibraryByIndex(int index);

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
RDK_LIB_TYPE bool RDK_CALL Env_DelClassLibraryByName(const char *name);

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
RDK_LIB_TYPE bool RDK_CALL Env_DelAllClassLibraries(void);

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
RDK_LIB_TYPE bool RDK_CALL Env_BuildStorage(void);

// Возвращает число библиотек
RDK_LIB_TYPE int RDK_CALL Env_GetNumClassLibraries(void);

// Возвращает имя библиотеки по индексу
RDK_LIB_TYPE const char * RDK_CALL Env_GetClassLibraryName(int index);

// Возвращает версию библиотеки по индексу
RDK_LIB_TYPE const char * RDK_CALL Env_GetClassLibraryVersion(int index);

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
// Возвращает id нового класса в хранилище
RDK_LIB_TYPE int RDK_CALL Env_CreateClass(const char* stringid);

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
RDK_LIB_TYPE int RDK_CALL Env_Calculate(const char* stringid);

// Расчет всей модели в реальном времени
RDK_LIB_TYPE void RDK_CALL Env_RTCalculate(void);

// Метод сброса счета
// Если stringid == 0 то сбрасывает всю модель целиком,
// иначе - только указанный компонент модели
RDK_LIB_TYPE int RDK_CALL Env_Reset(const char* stringid);

// Производит увеличение времени модели на требуемую величину
RDK_LIB_TYPE void RDK_CALL Env_IncreaseModelTimeByStep(void);


// ***********************************************
// Методы управления текущим компонентом
// !!! Следующие методы влияют на все
// методы, обращающиеся к компонентам по строковому id !!!
// ***********************************************
// Устанавливает текущий компонент (адресация относительно корня - модели)
RDK_LIB_TYPE int RDK_CALL Env_SelectCurrentComponent(const char *stringid);

// Сбрасывает текущий компонент в состояние по умолчению (модель)
RDK_LIB_TYPE int RDK_CALL Env_ResetCurrentComponent(const char *stringid);

// Меняет текущий компонент на его родителя (подъем на уровень вверх)
// Если уже на верхнем уровне, то не делает ничего
RDK_LIB_TYPE int RDK_CALL Env_UpCurrentComponent(void);

// Меняет текущий компонент на его дочерний на произвольном уровне вложенности
// (спуск на N уровней вниз относительно текущего компонента)
RDK_LIB_TYPE int RDK_CALL Env_DownCurrentComponent(const char *stringid);

// Возвращает длинное имя текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentComponentName(void);

// Возвращает длинный строковой id текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Env_GetCurrentComponentId(void);
// ***********************************************

// ***********************************************
// Вспомогательные функции управления средой. обычно вызов не требуется
// ***********************************************
// Индекс предварительно заданной модели обработки
RDK_LIB_TYPE int RDK_CALL Env_GetPredefinedStructure(void);
RDK_LIB_TYPE bool RDK_CALL Env_SetPredefinedStructure(int value);

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
RDK_LIB_TYPE bool RDK_CALL Env_IsStoragePresent(void);

// Возвращает состояние инициализации
RDK_LIB_TYPE bool RDK_CALL Env_IsInit(void);

// Признак наличия сформированной структуры
RDK_LIB_TYPE bool RDK_CALL Env_IsStructured(void);

// Инициализация среды
RDK_LIB_TYPE bool RDK_CALL Env_Init(void);

// Деинициализация среды
RDK_LIB_TYPE bool RDK_CALL Env_UnInit(void);

// Формирует предварительно заданную модель обработки
RDK_LIB_TYPE bool RDK_CALL Env_CreateStructure(void);

// Уничтожает текущую модель обработки
RDK_LIB_TYPE bool RDK_CALL Env_DestroyStructure(void);

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
RDK_LIB_TYPE void RDK_CALL Env_Destroy(void);
// ***********************************************
// ----------------------------


// --------------------------
// Функции управления средой видеообработки
// --------------------------
// Задает число входов среды
RDK_LIB_TYPE void RDK_CALL Env_SetNumInputImages(int number);

// Задает число выходов среды
RDK_LIB_TYPE void RDK_CALL Env_SetNumOutputImages(int number);

// Возвращает число входов среды
RDK_LIB_TYPE int RDK_CALL Env_GetNumInputImages(void);

// Возвращает число выходов среды
RDK_LIB_TYPE int RDK_CALL Env_GetNumOutputImages(void);

// Задает разрешение по умолчанию (рабочее разрешение)
RDK_LIB_TYPE void RDK_CALL Env_SetInputRes(int number, int width, int height);

// Задает данные изображения
RDK_LIB_TYPE void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

// Задает флаг отражения входного изображения вокруг горизонтальной оси
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value);

// Возвращает разрешение по умолчанию (рабочее разрешение)
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageWidth(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageHeight(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetInputImageColorModel(int number);

// Возвращает текущее выходное разрешение
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageWidth(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageHeight(int number);
RDK_LIB_TYPE int RDK_CALL Env_GetOutputImageColorModel(int number);

// Возвращает данные выходного изображения
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetInputImage(int index);
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetOutputImage(int index);
RDK_LIB_TYPE unsigned char* RDK_CALL Env_GetOutputImageY8(int index);
// --------------------------

// ----------------------------
// Методы управления моделью
// ----------------------------
// Удаляет модель
RDK_LIB_TYPE int RDK_CALL Model_Destroy(void);

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
RDK_LIB_TYPE int RDK_CALL Model_Create(int classid);

// Очищает модель
RDK_LIB_TYPE int RDK_CALL Model_Clear(void);

// Проверяет, существует ли модель
RDK_LIB_TYPE bool RDK_CALL Model_Check(void);

// Добавляет в выбранный компонент модели с идентификатором 'stringid' экземпляр компонента с заданным 'classid'
// если stringid - пустая строка, то добавляет в саму модель
// Возвращает имя компонента в случае успеха
RDK_LIB_TYPE const char* RDK_CALL Model_AddComponent(const char* stringid, int classid);

// Удаляет из выбранного компонента модели с идентификатором 'stringid' экземпляр компонента с заданным 'id'
// если stringid - пустая строка, то удаляет из самой модели
RDK_LIB_TYPE int RDK_CALL Model_DelComponent(const char* stringid, int id);

// Возвращает число всех компонент в заданном компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
RDK_LIB_TYPE int RDK_CALL Model_GetNumComponents(const char* stringid);

// Возвращает массив id всех компонент заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
RDK_LIB_TYPE int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer);

// Возвращает xml-список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
RDK_LIB_TYPE const char* RDK_CALL Model_GetConnectorsList(const char* stringid,
						  int sublevel=-1, const char* owner_level_stringid=0);

// Возвращает xml-список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
RDK_LIB_TYPE const char* RDK_CALL Model_GetItemsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// Возвращает xml-список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
RDK_LIB_TYPE const char* RDK_CALL Model_GetNetsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// Возвращает короткое имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentName(const char* stringid);

// Возвращает длинное имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid=0);

// Возвращает длинный id компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid=0);

// Возвращает имя класса компонента в хранилище по длинному 'stringid'
// если stringid - пустая строка, то возвращает имя класса модели
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentClassName(const char* stringid);

// Возвращает свойства компонента по идентификатору
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentProperties(const char *stringid, unsigned int type_mask);

// Возвращает свойства компонента по идентификатору с описаниями
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask);

// Возвращает выборочные свойства компонента по идентификатору
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedProperties(const char *stringid, unsigned int type_mask);

// Возвращает значение свойства компонента по идентификатору компонента и имени свойства
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentPropertyValue(const char *stringid, const char *paramname);

// Устанавливает свойства компонента по идентификатору
RDK_LIB_TYPE int RDK_CALL Model_SetComponentProperties(const char *stringid, const char* buffer);

// Устанавливает значение свойства компонента по идентификатору компонента и имени свойства
RDK_LIB_TYPE void RDK_CALL Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer);

// Возвращает параметры компонента по идентификатору
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameters(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// Возвращает параметры компонента по идентификатору с описаниями
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// Возвращает выборочные параметры компонента по идентификатору
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// Возвращает значение параметра компонента по идентификатору компонента и имени параметра
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname);

// Устанавливает параметры компонента по идентификатору
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer);

// Устанавливает значение параметра компонента по идентификатору компонента и имени параметра
// Deprecated
RDK_LIB_TYPE void RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer);

// Возвращает состояние компонента по идентификатору
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentState(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// Возвращает выборочные данные состояния компонента по идентификатору
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid);

// Возвращает значение параметра перменной состояния по идентификатору компонента и имени переменной
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename);

// Устанавливает состояние компонента по идентификатору
// Deprecated
RDK_LIB_TYPE bool RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer);

// Устанавливает значение переменной состояния компонента по идентификатору компонента и имени переменной
// Deprecated
RDK_LIB_TYPE void RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer);

// Связывает выбранные компоненты друг с другом
RDK_LIB_TYPE int RDK_CALL Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// Связывает все компоненты выбранного компонента по возрастанию id в формате: 0 выход к 0 входу
RDK_LIB_TYPE int RDK_CALL Model_ChainLinking(const char* stringid);

// Связывает все компоненты выбранного компонента параллельно, подключая их к необходимому числу выходов модели
// Используется для тестирования производительности
RDK_LIB_TYPE int RDK_CALL Model_ParallelLinking(const char* stringid);

// Разрывает выбранную связь
RDK_LIB_TYPE int RDK_CALL Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// Разрывает все связи
RDK_LIB_TYPE int RDK_CALL Model_BreakAllLinks(void);

// Разрывает все входные и выходные связи выбранного контейнера
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentLinks(const char* stringid);

// Разрывает все входные связи выбранного контейнера
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentInputLinks(const char* stringid);

// Разрывает все выходные связи выбранного контейнера
RDK_LIB_TYPE int RDK_CALL Model_BreakAllComponentOutputLinks(const char* stringid);

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid=0);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются с уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то применяется уровень текущего компонента
RDK_LIB_TYPE int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid=0);

// Возвращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid=0, bool sublevel=-1);

// Возвращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
RDK_LIB_TYPE const char * RDK_CALL Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid=0, bool sublevel=-1);

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
RDK_LIB_TYPE const char* RDK_CALL Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid=0);

// Возвращает число входов у компонента
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumInputs(const char *stringid);

// Возвращает размер входа компонента в числе элементов
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputDataSize(const char *stringid, int index);

// Возвращает размер элемента входа в байтах
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index);

// Возвращает размер входа компонента в байтах
RDK_LIB_TYPE int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index);

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index);

// Возвращает число выходов у компонента
RDK_LIB_TYPE int RDK_CALL Model_GetComponentNumOutputs(const char *stringid);

// Возвращает размер выхода компонента в числе элементов
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputDataSize(const char *stringid, int index);

// Возвращает размер элемента выхода в байтах
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputElementSize(const char *stringid, int index);

// Возвращает размер выхода компонента в байтах
RDK_LIB_TYPE int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index);

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
RDK_LIB_TYPE unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponent(const char *stringid, unsigned int params_type_mask=ptParameter | pgPublic);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponent(const char *stringid, char* buffer);

// Сохраняет все свойства компонента и его дочерних компонент в xml
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask);

// Загружает все свойства компонента и его дочерних компонент из xml
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer);

// Сохраняет все параметры компонента и его дочерних компонент в xml
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask=ptParameter | pgPublic);

// Загружает все параметры компонента и его дочерних компонент из xml
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentParameters(const char *stringid, char* buffer);

// Сохраняет состояние компонента и его дочерних компонент в xml
// Deprecated
RDK_LIB_TYPE const char * RDK_CALL Model_SaveComponentState(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// Загружает состояние компонента и его дочерних компонент из xml
// Deprecated
RDK_LIB_TYPE int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer);

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
RDK_LIB_TYPE const char* RDK_CALL Model_SaveComponentDrawInfo(const char *stringid);

// Управляет шагом счета модели по умолчанию
RDK_LIB_TYPE int RDK_CALL Model_GetDefaultTimeStep(void);
RDK_LIB_TYPE void RDK_CALL Model_SetDefaultTimeStep(int value);

// Управляет шагом счета компонента
RDK_LIB_TYPE int RDK_CALL Model_GetTimeStep(const char *stringid);
RDK_LIB_TYPE void RDK_CALL Model_SetTimeStep(const char *stringid, int value);

// Устанавливает шаг счета компонента и всех его дочерних компонент
RDK_LIB_TYPE void RDK_CALL Model_SetGlobalTimeStep(const char *stringid, int value);

// Возвращает текущее время модели
RDK_LIB_TYPE long long RDK_CALL Model_GetTime(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleTime(void);

// Устанавливает текущее время модели
RDK_LIB_TYPE bool RDK_CALL Model_SetTime(long long value);

// Возвращает реальное время
RDK_LIB_TYPE long long RDK_CALL Model_GetRealTime(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleRealTime(void);

// Устанавливает реальное время
RDK_LIB_TYPE bool RDK_CALL Model_SetRealTime(long long value);

// Увеличивает реальное время на заданную величину
RDK_LIB_TYPE bool RDK_CALL Model_IncreaseRealTime(long long value);

// Возвращает мгновенный шаг в реальном времени
RDK_LIB_TYPE long long RDK_CALL Model_GetRealTimeStep(void);
RDK_LIB_TYPE double RDK_CALL Model_GetDoubleRealTimeStep(void);

// Возвращает время расчета компонента без времени расчета дочерних компонент (мс)
RDK_LIB_TYPE long long RDK_CALL Model_GetStepDuration(const char *stringid);

// Возвращает время, затраченное на обработку объекта
// (вместе со времени расчета дочерних компонент) (мс)
RDK_LIB_TYPE long long RDK_CALL Model_GetFullStepDuration(const char *stringid);

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
RDK_LIB_TYPE double RDK_CALL Model_GetInstantPerformance(const char *stringid);
// --------------------------

// --------------------------
// Функции управления моделью видеообработки
// --------------------------
// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::UBitmap*
RDK_LIB_TYPE const /* RDK::UBitmap* */void* const RDK_CALL Model_GetComponentOutput(const char *stringid, int index);

// Возвращает указатель на выход с индексом 'index' компонента 'id'
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* const RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, int index);

// Возвращает указатель на вход с индексом 'index' компонента 'id'
RDK_LIB_TYPE const /*RDK::UBitmap* */ void* const RDK_CALL Model_GetComponentBitmapInput(const char *stringid, int index);

// Замещает изображение выхода с индексом 'index' компонента 'id'
RDK_LIB_TYPE void RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp);

// Замещает изображение входа с индексом 'index' компонента 'id'
RDK_LIB_TYPE void RDK_CALL Model_SetComponentBitmapInput(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp);
// --------------------------

// --------------------------
// Функции управления исключениями
// ----------------------------
// Управление функцией-обработчиком исключений
RDK_LIB_TYPE void* RDK_CALL Engine_GetExceptionHandler(void);
RDK_LIB_TYPE bool RDK_CALL Engine_SetExceptionHandler(void* value);

// Возвращает массив строк лога
RDK_LIB_TYPE const char* RDK_CALL Engine_GetLog(void);

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
RDK_LIB_TYPE const char* RDK_CALL Engine_GetUnreadLog(void);
// ----------------------------

#ifdef __cplusplus
}
#endif

#endif
