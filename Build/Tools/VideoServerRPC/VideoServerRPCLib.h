#ifndef VIDEO_SERVER_RPC_LIB_H
#define VIDEO_SERVER_RPC_LIB_H

#define RDK_RPC_RESPONSE_RECEIVE_TIMEOUT 10000

#define RDK_LIB_TYPE __declspec(dllexport)
#define RDK_CALL __cdecl

#ifdef __cplusplus
extern "C"  {
#else
typedef int bool;
#endif

/// Управление параметрами
//RDK_LIB_TYPE int RDK_CALL Rpc_GetResponseTimeOut(void);

//RDK_LIB_TYPE bool RDK_CALL Rpc_SetResponseTimeOut(const int &value);

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


/// Команды управления камерами
RDK_LIB_TYPE int RDK_CALL Rpc_CameraLeft(int channel_index, const char* cameraname, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_CameraRight(int channel_index, const char* cameraname, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_CameraUp(int channel_index, const char* cameraname, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_CameraDown(int channel_index, const char* cameraname, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_CameraStop(int channel_index, const char* cameraname, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_CameraZoomIn(int channel_index, const char* cameraname, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_CameraZoomOut(int channel_index, const char* cameraname, int timeout);

#ifdef __cplusplus
}
#endif

#endif

