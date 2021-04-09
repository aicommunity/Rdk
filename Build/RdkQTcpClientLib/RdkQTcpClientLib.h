#ifndef RDKQTCPCLIENTLIB_H
#define RDKQTCPCLIENTLIB_H

#define RDK_RPC_SERVER_NOT_EXIST 9996
#define RDK_RPC_CONNECTION_ERROR 9997
#define RDK_RPC_SERVER_NOT_ACTIVE 9998
#define RDK_RPC_CLIENT_NOT_CONNECTED 9999
#define RDK_RPC_LIBRARY_NOT_INIT 100000
#define RDK_RPC_LIBRARY_RECEIVER_STOPPED 100001
#define RDK_RPC_RESPONSE_RECEIVE_TIMEOUT 10000
#define RDK_RPC_PACKET_READER_ACCESS_TIMEOUT 1000
#define RDK_RPC_RESPONSE_NOT_RECIEVED 100
#define RDK_RPC_UNSUCCESSFULL_DECODING 101
#define RDK_RPC_SEND_TRANSPORT_EXCEPTION 938311
#define RDK_RPC_RECEIVE_TRANSPORT_EXCEPTION 938312
#define RDK_RPC_SEND_UNLOCK_WAIT_TIMEOUT 938313
#define RDK_RPC_SEND_TRANSPORT_UNHANDLED_EXCEPTION 938314
#define RDK_RPC_SERVER_RESULT_CODE_NOT_FOUND 1038314

/*
#if defined TEST
#define TEST_COMMON_DLLSPEC Q_DECL_EXPORT
#else
#define TEST_COMMON_DLLSPEC Q_DECL_IMPORT
#endif
*/
/*
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
*/

int Rtcl_SumTest(int a, int b);

/// Вспомогательные методы
/// Очистка списка пришедших пакетов от ответов сервера на асинхронные команды и команды с вышедшем таймаутом ожидания ответа
//bool  Rpc_EraseUnresponcedPackets(int server_index);

/// Проверка состояния клиента
bool  Rpc_IsClientConnected(int server_index);

/// Выделение имени камеры и индекса канала из составного имени компоненты
bool Rpc_CameraNameSeparator(const char* &source, const char* &camera_name, int &channel_index);

/// Управление отладочными данными
const char*  Rpc_GetServerAnswerDebug(int server_index);

bool  Rpc_SetServerAnswerDebug(int server_index, const char* value);

/// Инициализация
int  Rpc_Init(int num_serverPtz_MoveZoom);

int  Rpc_UnInit(void);

bool Rpc_IsInit();

/// Коммуникация с сервером
int  Rpc_Ping(int server_index, int timeout);

int  Rpc_Connect(int server_index, const char* serverAddress, int serverPort);

int  Rpc_Disconnect(int server_index);

int  Rpc_AddServer(void);

int  Rpc_DelServer(int server_index);

int  Rpc_LastServerId();

///////////////

int  Rpc_StartChannel(int server_index, int channel_index, int timeout);

int  Rpc_StopChannel(int server_index, int channel_index, int timeout);

int  Rpc_StartTraining(int server_index, int channel_index, int timeout);

int  Rpc_ResetChannel(int server_index, int channel_index, int timeout);

int  Rpc_GetNumChannels(int server_index, int &results, int timeout);

int  Rpc_SetNumChannels(int server_index, int num_channels, int timeout);

int  Rpc_GetChannelName(int server_index, int channel_index, const char* &result, int timeout);

int  Rpc_GetDeploymentState(int server_index, int &dp_state, int& dp_progress, int& dp_cap, const char *&last_error, int timeout);

int Rpc_SetChannelName(int server_index, int channel_index, const char* channel_name, int timeout);

int Rpc_GetLastError(int server_index, const char* &result, int timeout);

 int Rpc_SaveProject(int server_index, int timeout);

 int Rpc_LoadProject(int server_index, const char* project_path, int timeout);

 int Rpc_DeployProject(int server_index, int task_index, int &resp, int timeout=-1);

 int Rpc_PrepareProject(int server_index, const char* &verbose_response, int timeout);

 int Rpc_GetPreparationResult(int server_index, const char* &verbose_response, int timeout);

 int Rpc_OpenPreparedProject(int server_index, const char* &verbose_response, int timeout);

 int Rpc_RunPreparedProject(int server_index, int timeout);

 int Rpc_GetCalculationState(int server_index,
                            int& calculation_state,
                            int& capture_state,
                            unsigned long long& capture_frid,
                            unsigned long long& capture_maxfrid,
                            const char* &message,
                            int timeout);

 int Rpc_FinishCalculation(int server_index, bool& result, const char* &last_error, int timeout);

 int Rpc_UploadCalculationResults(int server_index, bool& result, const char* &last_error, int timeout);

 int Rpc_GetUploadState(int server_index, int& upload_state, const char* &last_error, int timeout);

 int Rpc_CloseSolver(int server_index, bool& result, const char* &last_error, int timeout);


#endif // RDKQTCPCLIENTLIB_H
