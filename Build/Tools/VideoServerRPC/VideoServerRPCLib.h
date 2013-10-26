#ifndef VIDEO_SERVER_RPC_LIB_H
#define VIDEO_SERVER_RPC_LIB_H

#define RDK_RPC_RESPONSE_RECEIVE_TIMEOUT 10000
#define RDK_RPC_PACKET_READER_ACCESS_TIMEOUT 1000
#define RDK_RPC_RESPONSE_NOT_RECIEVED 100
#define RDK_RPC_UNSUCCESSFULL_DECODING 101

#define RDK_LIB_TYPE __declspec(dllexport)
#define RDK_CALL __cdecl

#ifdef __cplusplus
extern "C"  {
#else
typedef int bool;
#endif

/// ���������� �����������
//RDK_LIB_TYPE int RDK_CALL Rpc_GetResponseTimeOut(void);

//RDK_LIB_TYPE bool RDK_CALL Rpc_SetResponseTimeOut(const int &value);

RDK_LIB_TYPE const char* RDK_CALL Rpc_GetServerAnswerDebug(void);

RDK_LIB_TYPE bool RDK_CALL Rpc_SetServerAnswerDebug(const char* value);

/// �������������
RDK_LIB_TYPE int RDK_CALL Rpc_Init(void);

RDK_LIB_TYPE int RDK_CALL Rpc_UnInit(void);

/// ������������ � ��������
RDK_LIB_TYPE int RDK_CALL Rpc_Connect(const char* serverAddress, int serverPort);

RDK_LIB_TYPE int RDK_CALL Rpc_Disconnect(void);

/// timeOut, ��
/// 0 - ������ ��� �������������
/// value - ���������� �����, ����� ��������
RDK_LIB_TYPE int RDK_CALL Rpc_StartChannel(int channel_index, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_StopChannel(int channel_index, int timeout);

RDK_LIB_TYPE int RDK_CALL Rpc_ResetChannel(int channel_index, int timeout);


/// ������� ���������� ��������
// ---------------------
// ������� �������������
// ---------------------
/// �������������� ����������
/// ������ ���� ������� � ������ �������
//RDK_LIB_TYPE int RDK_CALL Ptz_InitLib(int channel_index, void* exception_handler=0);

/// �������� ������ ��������������� �����
//RDK_LIB_TYPE int RDK_CALL Ptz_LoadCameras(int channel_index, const char *file_name);

/// ���������� ������ ������ ��������������� �����
//RDK_LIB_TYPE int RDK_CALL Ptz_SaveCameras(int channel_index, const char *file_name);

/// ���������������� ����������
/// ������ ���� ������� ����� ��������� ����������
//RDK_LIB_TYPE int RDK_CALL Ptz_UnInitLib(int channel_index);

/// ���������� ������ ������� ����� � ���� ������ ����������� ','
RDK_LIB_TYPE const char* RDK_CALL Ptz_GetCameraNames(int channel_index);

/// ���������� ������ ��������� ����� (�������) ����� � ���� ������ ����������� ','
//RDK_LIB_TYPE const char* RDK_CALL Ptz_GetCameraTypes(int channel_index);

/// ���������� ��� (�����) ������ �� �����
RDK_LIB_TYPE const char* RDK_CALL Ptz_GetCameraType(int channel_index, const char* camera_name);

/// ��������� ����� ������
/// �������������� ������ � ����� ������ camera_name �������� ���������� ���� camera_type
//RDK_LIB_TYPE int RDK_CALL Ptz_AddCamera(int channel_index, const char* camera_name, const char* camera_type);

/// ������� ������
//RDK_LIB_TYPE int RDK_CALL Ptz_DelCamera(int channel_index, const char* camera_name);

/// ������� ��� ������
//RDK_LIB_TYPE int RDK_CALL Ptz_DelAllCameras(int channel_index);

/// ������ �������� ��������� ���������� ���������� �������
RDK_LIB_TYPE int RDK_CALL Ptz_SetCameraParameter(int channel_index, const char* camera_name, const char* param_name, const char* param_value);

/// ��������� �������� ��������� ���������� ���������� �������
RDK_LIB_TYPE const char* Ptz_GetCameraParameter(int channel_index, const char* camera_name, const char* param_name);

/// ���������� ������������ � ��������� ������
//RDK_LIB_TYPE TPtzCameraInfo* Ptz_GetPtzInfo(const char* camera_name);

/// ����������� ������� ��������� ������
/// ����� ���� ������������ ���������
//RDK_LIB_TYPE int RDK_CALL Ptz_ReadCurrentCameraState(int channel_index, const char* camera_name);

/// ���������� ������ �������������� ������ � ���� ������, ������������ ','
RDK_LIB_TYPE const char* Ptz_GetImplementedCommands(int channel_index, const char* camera_name);

/// ��������� �������� �� ����������� � ���������� ������
RDK_LIB_TYPE int RDK_CALL Ptz_CameraConnect(int channel_index, const char* camera_name);

/// ��������� �������� �� ���������� �� ���������� ������
RDK_LIB_TYPE int RDK_CALL Ptz_CameraDisconnect(int channel_index, const char* camera_name);

/// ���������� true ���� �������� ������� �����������
//RDK_LIB_TYPE bool RDK_CALL Ptz_IsCmdImplemented(int channel_index, const char* camera_name, int cmd);
//RDK_LIB_TYPE bool RDK_CALL Ptz_IsCmdImplemented(int channel_index, const char* camera_name, const char* cmd);
/// -------------------------------
/// ������� ������/������ ���������� �������� ������
/// ������ ��������� ���� ���������� ��������:
/// Pan, PanSpeed, Tilt, TiltSpeed, Zoom, ZoomSpeed, Focus, FocusSpeed,
/// Iris, IrisSpeed, Brightness, BrightnessSpeed
/// -------------------------------
/// ���������� ������ �������������� ����������, ����������� �������
RDK_LIB_TYPE const char* RDK_CALL Ptz_GetImplementedMoveParamsList(int channel_index, const char* camera_name);

/// ���������� true ���� �������� ��������������
RDK_LIB_TYPE bool RDK_CALL Ptz_IsMoveParamImplemented(int channel_index, const char* camera_name, const char *param_name);
RDK_LIB_TYPE bool RDK_CALL Ptz_SetMoveParamImplemented(int channel_index, const char* camera_name, const char *param_name, bool value);

/// ���������� true ���� �������� ��������������
RDK_LIB_TYPE double RDK_CALL Ptz_GetMoveParamMinNativeValue(int channel_index, const char* camera_name, const char *param_name);
RDK_LIB_TYPE bool RDK_CALL Ptz_SetMoveParamMinNativeValue(int channel_index, const char* camera_name, const char *param_name, double value);

/// ������������ "������" �������� ���������
RDK_LIB_TYPE double RDK_CALL Ptz_GetMoveParamMaxNativeValue(int channel_index, const char* camera_name, const char *param_name);
RDK_LIB_TYPE bool RDK_CALL Ptz_SetMoveParamMaxNativeValue(int channel_index, const char* camera_name, const char *param_name, double value);

/// ����������� ������������ �������� ���������
RDK_LIB_TYPE double RDK_CALL Ptz_GetMoveParamMinValue(int channel_index, const char* camera_name, const char *param_name);
RDK_LIB_TYPE bool RDK_CALL Ptz_SetMoveParamMinValue(int channel_index, const char* camera_name, const char *param_name, double value);

/// ������������ ������������ �������� ���������
RDK_LIB_TYPE double RDK_CALL Ptz_GetMoveParamMaxValue(int channel_index, const char* camera_name, const char *param_name);
RDK_LIB_TYPE bool RDK_CALL Ptz_SetMoveParamMaxValue(int channel_index, const char* camera_name, const char *param_name, double value);
// ---------------------
// ����� ������� ���������� ���������
// ---------------------
/// ���������� ������� �������� ������.
RDK_LIB_TYPE int RDK_CALL Ptz_Stop(int channel_index, const char* camera_name, int timeout);

/// ���������� ������ � ��������� ���������.
RDK_LIB_TYPE int RDK_CALL Ptz_GotoHome(int channel_index, const char* camera_name);

/// ���������� ������� ��������� ������ ��� ��������� �����������������.
RDK_LIB_TYPE int RDK_CALL Ptz_PresetPoint(int channel_index, const char* camera_name, int i);

/// ���������� ������ � ����������������� ���������.
RDK_LIB_TYPE int RDK_CALL Ptz_GotoPoint(int channel_index, const char* camera_name, int i);

/// ������� ����������� ����������������� ���������
RDK_LIB_TYPE int RDK_CALL Ptz_RemovePoint(int channel_index, const char* camera_name, int i);
// ---------------------
// ������� ���������� ��������� ������ � ������������������� ���������
// ---------------------
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPTZPosition(int channel_index, const char* camera_name, double &pan, double &tilt, double &zoom);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPTPosition(int channel_index, const char* camera_name, double &pan, double &tilt);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPanPosition(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadTiltPosition(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadZoomPosition(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadFocusPosition(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadIrisPosition(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadBrightnessPosition(int channel_index, const char* camera_name, double &value);
// ---------------------
// ������� ���������� ��������� ������ � ������ ��� ������ ���������
// ---------------------
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPTZPositionNative(int channel_index, const char* camera_name, double &pan, double &tilt, double &zoom);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPTPositionNative(int channel_index, const char* camera_name, double &pan, double &tilt);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadPanPositionNative(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadTiltPositionNative(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadZoomPositionNative(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadFocusPositionNative(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadIrisPositionNative(int channel_index, const char* camera_name, double &value);
RDK_LIB_TYPE int RDK_CALL Ptz_ReadBrightnessPositionNative(int channel_index, const char* camera_name, double &value);
// ---------------------
// ������� ���������� ������������ �� ��������
// �������� �������� � ������������������ ����������� ��/�
// ���������� ���������� ������ � �������� ������ �������� �����������
// ---------------------
/// ���������� ������ ������������ �� 3 ������������ Pan, Tilt, Zoom, ��
/// ���������� �������������� pan_speed, tilt_speed, zoom_speed, ���� ������ ������������ ����� �����.
RDK_LIB_TYPE int RDK_CALL Ptz_Move(int channel_index, const char* camera_name, double pan_speed=0, double tilt_speed=0, double zoom_speed=0);

/// ���������� ������ � ����������� TPtzDirection �� ��������� speed
/// (����� ��������������, ��� speed >=0).
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveDirection(int channel_index, const char* camera_name, TPtzDirection direction, double speed);

/// ���������� ������ �� �������������� ��� � ��������� speed.
RDK_LIB_TYPE int RDK_CALL Ptz_MovePan(int channel_index, const char* camera_name, double speed, int timeout);

/// ���������� ������ �� ������������ ��� � ��������� speed.
RDK_LIB_TYPE int RDK_CALL Ptz_MoveTilt(int channel_index, const char* camera_name, double speed, int timeout);

/// ��������� ���� ������ ������  � ��������� speed.
RDK_LIB_TYPE int RDK_CALL Ptz_MoveZoom(int channel_index, const char* camera_name, double speed, int timeout);

/// ��������� ����������� ������  � ��������� speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveFocus(int channel_index, const char* camera_name, double speed);

/// ��������� �������� ��������� ������  � ��������� speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveIris(int channel_index, const char* camera_name, double speed);

/// ��������� �������� �-�� �������� ������  � ��������� speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveBrightness(int channel_index, const char* camera_name, double speed);
// ---------------------
// ������� ���������� ������������ �� ��������
// �������� �������� � ������ ��� ������ ��������
/// ���������� ���������� ������ � �������� ������ �������� �����������
// ---------------------
RDK_LIB_TYPE int RDK_CALL Ptz_MoveNative(int channel_index, const char* camera_name, double pan_speed=0, double tilt_speed=0, double zoom_speed=0);
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveDirectionNative(int channel_index, const char* camera_name, TPtzDirection direction, double speed);
RDK_LIB_TYPE int RDK_CALL Ptz_MovePanNative(int channel_index, const char* camera_name, double speed);
RDK_LIB_TYPE int RDK_CALL Ptz_MoveTiltNative(int channel_index, const char* camera_name, double speed);
RDK_LIB_TYPE int RDK_CALL Ptz_MoveZoomNative(int channel_index, const char* camera_name, double speed);
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveFocusNative(int channel_index, const char* camera_name, double speed);
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveIrisNative(int channel_index, const char* camera_name, double speed);
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveBrightnessNative(int channel_index, const char* camera_name, double speed);
/// -----------------------------------
// ������� ����������� ����������������
// ��������� �������� � ������������������ ����������� ��/�
// ���������� ������ � �������� ���������
/// -----------------------------------
/// ���������� ������ � �������� ��������� �� 3 ����������.
/// ��������� ��������� pan, tilt, zoom ����� ����� ������������� ��������
/// ��������� �������� pan_speed, tilt_speed, zoom_speed �������������� ������ ��������������
/// �������� <0 �������� ����������� � ������� �������� ���������
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveAbs(int channel_index, const char* camera_name, double pan, double tilt, double zoom, double pan_speed=-1.0, double tilt_speed=-1.0, double zoom_speed=-1.0);

/// ���������� ������ �� �������������� ��� � ��������� value �� ��������� speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_MovePanAbs(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// ���������� ������ �� ������������ ��� � ��������� value �� ��������� speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveTiltAbs(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// �������� ���� ������ ������ � �������� value �� ��������� speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveZoomAbs(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// �������� �������� ����������� ������ �� �������� value �� ��������� speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_SetFocus(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// �������� �������� ��������� ������ �� �������� value �� ��������� speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_SetIris(int channel_index, const char* camera_name, double value, double speed=-1.0);

/// �������� �������� �-�� �������� ������ �� �������� value �� ��������� speed.
//RDK_LIB_TYPE int RDK_CALL Ptz_SetBrightness(int channel_index, const char* camera_name, double value, double speed=-1.0);
/// -----------------------------------

/// -----------------------------------
// ������� ����������� ����������������
// ��������� �������� � ������ ��� ������ ��������
/// -----------------------------------
/// ���������� ������ � �������� �����������
/// ��������� ��������� pan, tilt, zoom ����� ����� ������������� ��������
/// ��������� �������� pan_speed, tilt_speed, zoom_speed �������������� ������ ��������������
/// �������� <0 �������� ����������� � ������� �������� ���������
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveAbsNative(int channel_index, const char* camera_name, double pan, double tilt, double zoom, double pan_speed=-1.0, double tilt_speed=-1.0, double zoom_speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_MovePanAbsNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveTiltAbsNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveZoomAbsNative(int channel_index, const char* camera_name, double value, double speed=-1.0);

//RDK_LIB_TYPE int RDK_CALL Ptz_SetFocusNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_SetIrisNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
//RDK_LIB_TYPE int RDK_CALL Ptz_SetBrightnessNative(int channel_index, const char* camera_name, double value, double speed=-1.0);
/// -----------------------------------

// ---------------------
// ������� ���������� ���������������� ����������� ������
// ��� ������� ��������� �����������, ��������������� ����� ����
// ���������/���������� ��������� ������� (�������).
// ��������������, �������� ��������� value �� ���� �������� ������ �����������
// ������� 0 (������� ���������), 1 (������� ��������).
// ---------------------
/// ��������
//RDK_LIB_TYPE int RDK_CALL Ptz_Rain(int channel_index, const char* camera_name, double value);

/// ���������
//RDK_LIB_TYPE int RDK_CALL Ptz_Light(int channel_index, const char* camera_name, double value);

/// ���������������
//RDK_LIB_TYPE int RDK_CALL Ptz_AutoFocus(int channel_index, const char* camera_name, double value);

/// �������������
//RDK_LIB_TYPE int RDK_CALL Ptz_AutoIris(int channel_index, const char* camera_name, double value);

/// �������������� �-�� ��������
//RDK_LIB_TYPE int RDK_CALL Ptz_AutoBrightness(int channel_index, const char* camera_name, double value);
// ---------------------
// ������� ���������� ��������� ������ � ������ ��� ������ ���������
// ---------------------
#ifdef __cplusplus
}
#endif

#endif

