#ifndef VIDEO_SERVER_RPC_LIB_H
#define VIDEO_SERVER_RPC_LIB_H

#define RDK_RPC_CLIENT_NOT_CONNECTED 9999
#define RDK_RPC_LIBRARY_NOT_INIT 100000
#define RDK_RPC_RESPONSE_RECEIVE_TIMEOUT 10000
#define RDK_RPC_PACKET_READER_ACCESS_TIMEOUT 1000
#define RDK_RPC_RESPONSE_NOT_RECIEVED 100
#define RDK_RPC_UNSUCCESSFULL_DECODING 101

#define RDK_LIB_TYPE __declspec(dllexport)
#define RDK_CALL __cdecl

#ifndef RDK_PTZ_DEFINED
#define RDK_PTZ_DEFINED
/// Направления движения
enum TPtzDirection
{
Up=1,
Down=2,
Left=3,
Right=4,
UpLeft=5,
UpRight=6,
DownLeft=7,
DownRight=8
};
#endif

#ifdef __cplusplus
extern "C"  {
#else
typedef int bool;
#endif

/// Проверка состояния IdTCPClient
RDK_LIB_TYPE bool RDK_CALL Rpc_IsClientConnected(void);

/// Управление параметрами
RDK_LIB_TYPE const char* RDK_CALL Rpc_GetServerAnswerDebug(void);

RDK_LIB_TYPE bool RDK_CALL Rpc_SetServerAnswerDebug(const char* value);

/// Инициализация
RDK_LIB_TYPE int RDK_CALL Rpc_Init(void);

RDK_LIB_TYPE int RDK_CALL Rpc_UnInit(void);

/// Коммуникация с сервером
RDK_LIB_TYPE int RDK_CALL Rpc_Connect(const char* serverAddress, int serverPort);

RDK_LIB_TYPE int RDK_CALL Rpc_Disconnect(void);

/// timeOut, мс
/// 0 - работа без подтверждения
/// value - синхронный режим, время ожидания
RDK_LIB_TYPE int RDK_CALL Rpc_StartChannel(int channel_index, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_StopChannel(int channel_index, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_ResetChannel(int channel_index, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_GetNumChannels(int &results, int timeout);


/// Команды управления камерами
// ---------------------
// Функции инициализации
// ---------------------
/// Инициализирует библиотеку
/// Должна быть вызвана в первую очередь
//RDK_LIB_TYPE int RDK_CALL Ptz_InitLib(int channel_index, void* exception_handler=0);

/// Загрузка набора преднастроенных камер
//RDK_LIB_TYPE int RDK_CALL Ptz_LoadCameras(int channel_index, const char *file_name);

/// Сохранение нового набора преднастроенных камер
//RDK_LIB_TYPE int RDK_CALL Ptz_SaveCameras(int channel_index, const char *file_name);

/// Деинициализирует библиотеку
/// Должна быть вызвана перед выгрузкой библиотеки
//RDK_LIB_TYPE int RDK_CALL Ptz_UnInitLib(int channel_index);

/// Возвращает список текущих камер в виде строки разделяемой ','
RDK_LIB_TYPE int RDK_CALL Ptz_GetCameraNames(int channel_index, const char* &results, int timeout);

/// Возвращает список доступных типов (классов) камер в виде строки разделяемой ','
RDK_LIB_TYPE int RDK_CALL Ptz_GetCameraTypes(int channel_index, const char* &results, int timeout);

/// Возвращает тип (класс) камеры по имени
RDK_LIB_TYPE int RDK_CALL Ptz_GetCameraType(int channel_index, const char* camera_name, const char* &results, int timeout);

/// Добавляет новую камеру
/// Инициализирует камеру с новым именем camera_name создавая управление типа camera_type
RDK_LIB_TYPE int RDK_CALL Ptz_AddCamera(int channel_index, const char* camera_name, const char* camera_type, int timeout);

/// Удаляет камеру
RDK_LIB_TYPE int RDK_CALL Ptz_DelCamera(int channel_index, const char* camera_name, int timeout);

/// Удаляет все камеры
RDK_LIB_TYPE int RDK_CALL Ptz_DelAllCameras(int channel_index, int timeout);

/// Задает значение параметра компонента управления камерой
RDK_LIB_TYPE int RDK_CALL Ptz_SetCameraParameter(int channel_index, const char* camera_name, const char* param_name, const char* param_value, int timeout);

/// Считывает значение параметра компонента управления камерой
RDK_LIB_TYPE int RDK_CALL Ptz_GetCameraParameter(int channel_index, const char* camera_name, const char* param_name, const char* &results, int timeout);

/// Возвращает конфигурацию и состояние камеры
//RDK_LIB_TYPE TPtzCameraInfo* Ptz_GetPtzInfo(const char* camera_name);

/// Запрашивает текущее состояние камеры
/// Может быть ресурсоемкой операцией
//RDK_LIB_TYPE int RDK_CALL Ptz_ReadCurrentCameraState(int channel_index, const char* camera_name);

/// Возвращает список поддерживаемых команд в виде списка, разделенного ','
RDK_LIB_TYPE int RDK_CALL Ptz_GetImplementedCommands(int channel_index, const char* camera_name, const char* &results, int timeout);

/// Выполняет действия по подключению к физической камере
RDK_LIB_TYPE int RDK_CALL Ptz_CameraConnect(int channel_index, const char* camera_name, int timeout);

/// Выполняет действия по отключению от физической камеры
RDK_LIB_TYPE int RDK_CALL Ptz_CameraDisconnect(int channel_index, const char* camera_name, int timeout);

/// Возвращает true если заданная функция реализована
//RDK_LIB_TYPE bool RDK_CALL Ptz_IsCmdImplementedById(int channel_index, const char* camera_name, int cmd);
RDK_LIB_TYPE int RDK_CALL Ptz_IsCmdImplemented(int channel_index, const char* camera_name, const char* cmd, bool &results, int timeout);

/// Возвращает список поддерживаемых параметров, разделенных запятой
RDK_LIB_TYPE int RDK_CALL Ptz_GetImplementedMoveParamsList(int channel_index, const char* camera_name, const char* &results, int timeout);

/// Возвращает true если параметр поддерживается
RDK_LIB_TYPE int RDK_CALL Ptz_IsMoveParamImplemented(int channel_index, const char* camera_name, const char *param_name, bool results, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_SetMoveParamImplemented(int channel_index, const char* camera_name, const char *param_name, bool value, bool &results, int timeout);

/// Минимальное "родное" значение параметра
RDK_LIB_TYPE int RDK_CALL Ptz_GetMoveParamMinNativeValue(int channel_index, const char* camera_name, const char *param_name, double &results, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_SetMoveParamMinNativeValue(int channel_index, const char* camera_name, const char *param_name, double value, bool &results, int timeout);

/// Максимальное "родное" значение параметра
RDK_LIB_TYPE int RDK_CALL Ptz_GetMoveParamMaxNativeValue(int channel_index, const char* camera_name, const char *param_name, double &results, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_SetMoveParamMaxNativeValue(int channel_index, const char* camera_name, const char *param_name, double value, bool &results, int timeout);

/// Минимальное общепринятое значение параметра
RDK_LIB_TYPE int RDK_CALL Ptz_GetMoveParamMinValue(int channel_index, const char* camera_name, const char *param_name, double &results, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_SetMoveParamMinValue(int channel_index, const char* camera_name, const char *param_name, double value, bool &results, int timeout);

/// Максимальное общепринятое значение параметра
RDK_LIB_TYPE int RDK_CALL Ptz_GetMoveParamMaxValue(int channel_index, const char* camera_name, const char *param_name, double &results, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_SetMoveParamMaxValue(int channel_index, const char* camera_name, const char *param_name, double value, bool &results, int timeout);
// ---------------------
// Общие функции управления движением
// ---------------------
/// Прекращает текущее движение камеры.
RDK_LIB_TYPE int RDK_CALL Ptz_Stop(int channel_index, const char* camera_name, int timeout);

/// Возвращает камеру в начальное положение.
RDK_LIB_TYPE int RDK_CALL Ptz_GotoHome(int channel_index, const char* camera_name, int timeout);

/// Запоминает текущее положение камеры как некоторое предустановленное.
RDK_LIB_TYPE int RDK_CALL Ptz_PresetPoint(int channel_index, const char* camera_name, int i, int timeout);

/// Перемещает камеру в предустановленное положение.
RDK_LIB_TYPE int RDK_CALL Ptz_GotoPoint(int channel_index, const char* camera_name, int i, int timeout);

/// Удаляет сохраненное предустановленное положение
RDK_LIB_TYPE int RDK_CALL Ptz_RemovePoint(int channel_index, const char* camera_name, int i, int timeout);
/// -------------------------------
/// Функции чтения/записи параметров движения камеры
/// Список возможных имен параметров движения:
/// Pan, PanSpeed, Tilt, TiltSpeed, Zoom, ZoomSpeed, Focus, FocusSpeed,
/// Iris, IrisSpeed, Brightness, BrightnessSpeed
/// -------------------------------
// ---------------------
// Функции считывания состояния камеры в стандартизированных величинах
// ---------------------
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPTZPosition(int channel_index, const char* camera_name, double &pan, double &tilt, double &zoom, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPTPosition(int channel_index, const char* camera_name, double &pan, double &tilt, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPanPosition(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadTiltPosition(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadZoomPosition(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadFocusPosition(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadIrisPosition(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadBrightnessPosition(int channel_index, const char* camera_name, double &value, int timeout);
// ---------------------
// Функции считывания состояния камеры в родных для камеры величинах
// ---------------------
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPTZPositionNative(int channel_index, const char* camera_name, double &pan, double &tilt, double &zoom, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPTPositionNative(int channel_index, const char* camera_name, double &pan, double &tilt, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPanPositionNative(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadTiltPositionNative(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadZoomPositionNative(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadFocusPositionNative(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadIrisPositionNative(int channel_index, const char* camera_name, double &value, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadBrightnessPositionNative(int channel_index, const char* camera_name, double &value, int timeout);
// ---------------------
// Функции управления перемещением по скорости
// Скорость задается в станадартизованных библиотекой ед/с
// Непрерывно перемещает камеру в заданном знаком скорости направлении
// ---------------------
/// Перемещает камеру одновременно по 3 направлениям Pan, Tilt, Zoom, со
/// скоростями соответственно pan_speed, tilt_speed, zoom_speed, если камера поддерживает такой режим.
RDK_LIB_TYPE int RDK_CALL Ptz_MovePTZ(int channel_index, const char* camera_name, double pan_speed=0, double tilt_speed=0, double zoom_speed=0, int timeout=100);

/// Перемещает камеру одновременно по 2 направлениям Pan, Tilt со
/// скоростями соответственно pan_speed, tilt_speed, zoom_speed, если камера поддерживает такой режим.
RDK_LIB_TYPE int RDK_CALL Ptz_MovePT(int channel_index, const char* camera_name, double pan_speed=0, double tilt_speed=0, int timeout=100);

/// Перемещает камеру в направлении TPtzDirection со скоростью speed
/// (здесь предполагается, что speed >=0).
RDK_LIB_TYPE int RDK_CALL Ptz_MoveDirection(int channel_index, const char* camera_name, TPtzDirection direction, double speed, int timeout);

/// Перемещает камеру по горизонтальной оси с скоростью speed.
RDK_LIB_TYPE int RDK_CALL Ptz_MovePan(int channel_index, const char* camera_name, double speed, int timeout);

/// Перемещает камеру по вертикальной оси с скоростью speed.
RDK_LIB_TYPE int RDK_CALL Ptz_MoveTilt(int channel_index, const char* camera_name, double speed, int timeout);

/// Изменение поля зрения камеры  с скоростью speed.
RDK_LIB_TYPE int RDK_CALL Ptz_MoveZoom(int channel_index, const char* camera_name, double speed, int timeout);

/// Изменение фокусировки камеры  с скоростью speed.
RDK_LIB_TYPE int RDK_CALL Ptz_MoveFocus(int channel_index, const char* camera_name, double speed, int timeout);

/// Изменение величины диафрагмы камеры  с скоростью speed.
RDK_LIB_TYPE int RDK_CALL Ptz_MoveIris(int channel_index, const char* camera_name, double speed, int timeout);

/// Изменение величины к-та усиления камеры  с скоростью speed.
RDK_LIB_TYPE int RDK_CALL Ptz_MoveBrightness(int channel_index, const char* camera_name, double speed, int timeout);
// ---------------------
// Функции управления перемещением по скорости
// Скорость задается в родных для камеры единицах
/// Непрерывно перемещает камеру в заданном знаком скорости направлении
// ---------------------
RDK_LIB_TYPE int RDK_CALL Ptz_MovePTZNative(int channel_index, const char* camera_name, double pan_speed=0, double tilt_speed=0, double zoom_speed=0, int timeout=100);
RDK_LIB_TYPE int RDK_CALL Ptz_MovePTNative(int channel_index, const char* camera_name, double pan_speed=0, double tilt_speed=0, int timeout=100);
RDK_LIB_TYPE int RDK_CALL Ptz_MoveDirectionNative(int channel_index, const char* camera_name, TPtzDirection direction, double speed, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_MovePanNative(int channel_index, const char* camera_name, double speed, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_MoveTiltNative(int channel_index, const char* camera_name, double speed, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_MoveZoomNative(int channel_index, const char* camera_name, double speed, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_MoveFocusNative(int channel_index, const char* camera_name, double speed, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_MoveIrisNative(int channel_index, const char* camera_name, double speed, int timeout);
RDK_LIB_TYPE int RDK_CALL Ptz_MoveBrightnessNative(int channel_index, const char* camera_name, double speed, int timeout);
/// -----------------------------------
// Функции абсолютного позиционирования
// Параметры задаются в станадартизованных библиотекой ед/с
// Перемещает камеру в заданное положение
/// -----------------------------------
/// Перемещает камеру в заданное положение по 3 переменным.
/// Параметры положения pan, tilt, zoom могут иметь отрицательное значение
/// Параметры скорости pan_speed, tilt_speed, zoom_speed предполагаются всегда положительными
/// Скорость <0 означает перемещение с текущей заданной скоростью
//RDK_LIB_TYPE int RDK_CALL Ptz_SetPTZ(int channel_index, const char* camera_name, double pan, double tilt, double zoom, double pan_speed=-1.0, double tilt_speed=-1.0, double zoom_speed=-1.0);

/// Перемещает камеру по горизонтальной оси в положение value со скоростью speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_SetPan(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// Перемещает камеру по вертикальной оси в положение value со скоростью speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_SetTilt(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// Изменяет поле зрения камеры в значение value со скоростью speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_SetZoom(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// Изменяет значение фокусировки камеры до величины value со скоростью speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_SetFocus(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// Изменяет значение диафрагмы камеры до величины value со скоростью speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_SetIris(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// Изменяет значение к-та усиления камеры до величины value со скоростью speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_SetBrightness(int channel_index, const char* camera_name, double value, double speed=-1.0);
/// -----------------------------------

/// -----------------------------------
// Функции абсолютного позиционирования
// Параметры задаются в родных для камеры единицах
/// -----------------------------------
/// Перемещает камеру в заданном направлении
/// Параметры положения pan, tilt, zoom могут иметь отрицательное значение
/// Параметры скорости pan_speed, tilt_speed, zoom_speed предполагаются всегда положительными
/// Скорость <0 означает перемещение с текущей заданной скоростью
//RDK_LIB_TYPE int RDK_CALL Ptz_SetPTZNative(int channel_index, const char* camera_name, double pan, double tilt, double zoom, double pan_speed=-1.0, double tilt_speed=-1.0, double zoom_speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_SetPanNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_SetTiltNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_SetZoomNative(int channel_index, const char* camera_name, double value, double speed=-1.0);

//RDK_LIB_TYPE int RDK_CALL Ptz_SetFocusNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_SetIrisNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_SetBrightnessNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
/// -----------------------------------

// ---------------------
// Функции управления вспомогательными параметрами камеры
// Эти функции управляют параметрами, представляющими собой флаг
// включения/выключения некоторых режимов (функций).
// Соответственно, значения аргумента value во всех функциях обычно принимаются
// равными 0 (функция выключена), 1 (функция включена).
// ---------------------
/// Дворники
RDK_LIB_TYPE int RDK_CALL Ptz_Rain(int channel_index, const char* camera_name, double value, int timeout);

/// Подсветка
RDK_LIB_TYPE int RDK_CALL Ptz_Light(int channel_index, const char* camera_name, double value, int timeout);

/// Автофокусировка
RDK_LIB_TYPE int RDK_CALL Ptz_AutoFocus(int channel_index, const char* camera_name, double value, int timeout);

/// Автодиафрагма
RDK_LIB_TYPE int RDK_CALL Ptz_AutoIris(int channel_index, const char* camera_name, double value, int timeout);

/// Автоподстройка к-та усиления
RDK_LIB_TYPE int RDK_CALL Ptz_AutoBrightness(int channel_index, const char* camera_name, double value, int timeout);
// ---------------------
// Функции считывания состояния камеры в родных для камеры величинах
// ---------------------
#ifdef __cplusplus
}
#endif

#endif

