//---------------------------------------------------------------------------

#ifndef UEngineControlQtH
#define UEngineControlQtH
//---------------------------------------------------------------------------
#include <vector>
#include <map>
#include <QTimer>
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../GUI/BCB/TUVisualController.h"
#include "TServerBroadcasterCommonUnit.h"

class RDK_LIB_TYPE UEngineControlVcl: public RDK::UEngineControl
{
protected:
/// ������ ��� ������������� ������
QTimer *Timer;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UEngineControlVcl(void);
virtual ~UEngineControlVcl(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// �������� ������ ����� �������
virtual RDK::UEngineControlThread* CreateEngineThread(RDK::UEngineControl* engine_control, int channel_index);

/// �������� ������ ����� �������
virtual RDK::UEngineStateThread* CreateEngineStateThread(RDK::UEngineControl* engine_control);

/// ��������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void StartChannel(int channel_index);

/// ������������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
virtual void PauseChannel(int channel_index);
// --------------------------

protected:
/// ������� �������
void TimerTimer(void);
};

#endif
