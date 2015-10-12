//---------------------------------------------------------------------------

#ifndef UBroadcasterInterfaceH
#define UBroadcasterInterfaceH

#include "UIVisualController.h"
#include "../Engine/UTime.h"

namespace RDK {

class UEngineControl;

class RDK_LIB_TYPE UBroadcasterInterface
{
public:
// ---------------------------
// ������ ������� � ����������
// ---------------------------
virtual bool GetBroadcastEnableFlag(void) const=0;
virtual bool SetBroadcastEnableFlag(bool value)=0;
// ---------------------------

// ---------------------------
// ������ ����������
// ---------------------------
/// ���������� ������������ ������
/// ������ ������ ���� ����� �� ����������� ��� ��������� ��������
virtual UEngineControl *GetEngineControl(void)=0;
virtual bool SetEngineControl(UEngineControl *engine_control)=0;

/// ������� ���������� ���������� � ������� �� �������� � ������������ � �����������
virtual bool AddMetadata(int channel_index, double time_stamp)=0;

/// ���������� ��������� �������� ����������
virtual bool SendMetadata(void)=0;

virtual bool RegisterToEngineControl(void)=0;

virtual bool UnRegisterFromEngineControl(void)=0;
// ---------------------------
};

}
#endif
