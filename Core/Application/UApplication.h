#ifndef UApplication_H
#define UApplication_H

#include "UAppCore.h"
#include "UProject.h"
#include "UIVisualController.h"
#include "URpcDispatcherQueues.h"
#include "URpcDispatcher.h"
#include "URpcDecoder.h"
#include "URpcDecoderInternal.h"
#include "URpcDecoderCommon.h"
#include "URpcCommand.h"
#include "URpcCommandInternal.h"
#include "UEngineControl.h"
#include "UEngineControlThread.h"
#include "UEngineStateThread.h"
#include "UBroadcasterInterface.h"
#include "UServerControl.h"

#ifdef __BORLANDC__
#include "Bcb/Application.bcb.h"
#endif

namespace RDK {

class UEngineControl;
class UServerControl;

class RDK_LIB_TYPE UApplication: public UAppController
{
protected: // ������ ����������
/// ������� �������
std::string WorkDirectory;

// ������� ������� ��������� �������
bool ProjectOpenFlag;

protected: // ������ ����������
/// ��������� ������
UEPtr<URpcDispatcher> RpcDispatcher;

/// �������� ������
UEPtr<UProject> Project;

/// ���������� ������
UEPtr<UEngineControl> EngineControl;

/// ���������� ��������� �����
UEPtr<UServerControl> ServerControl;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UApplication(void);
virtual ~UApplication(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ������� �������
const std::string& GetWorkDirectory(void) const;
bool SetWorkDirectory(const std::string& value);

// ������� ������� ��������� �������
bool GetProjectOpenFlag(void) const;
bool SetProjectOpenFlag(bool value);
// --------------------------

// --------------------------
// ������ �������������
// --------------------------
/// ������������� ������ � ���������� ������
virtual UEPtr<URpcDispatcher> GetRpcDispatcher(void);

/// ������������� ����� ��������� ������
/// ��������������� �� ������������ ������ ���������� ����� �� ���������� �������
virtual bool SetRpcDispatcher(const UEPtr<URpcDispatcher> &value);

/// ������������� ������ � ����������� ������
virtual UEPtr<UEngineControl> GetEngineControl(void);

/// ������������� ����� ���������� ������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
virtual bool SetEngineControl(const UEPtr<UEngineControl> &value);

/// ������������� ������ � �������
virtual UEPtr<UProject> GetProject(void);

/// ������������� ����� ������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
virtual bool SetProject(const UEPtr<UProject> &value);

/// ������������� ������ � ����������� ��������� �����
virtual UEPtr<UServerControl> GetServerControl(void) const;

/// ������������� ����� ���������� �������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
virtual bool SetServerControl(const UEPtr<UServerControl> &value);

/// �������������� ����������
virtual bool Init(void);

/// ���������������� ����������
virtual bool UnInit(void);
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
/// ������� ������ (����� ���������� � ��������)
virtual bool CreateProject(const std::string &filename);

/// ��������� ������
virtual bool OpenProject(const std::string &filename);

/// ��������� ������
virtual bool SaveProject(void);
virtual bool SaveProjectAs(const std::string &filename);

/// ��������� ������
virtual bool CloseProject(void);

/// ��������� ������ � ����� ������������
virtual bool CloneProject(const std::string &filename);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ������ �������
int GetNumEngines(void) const;
bool SetNumEngines(int num);
bool InsertEngine(int index);
bool DeleteEngine(int index);
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
/// ��������� ��������� ���������� ������, ��� ����, ���� engine_index == -1
virtual void StartEngine(int engine_index);

/// ������������� ��������� ���������� ������, ��� ����, ���� engine_index == -1
virtual void PauseEngine(int engine_index);

/// ���������� ��������� ���������� ������, ��� ����, ���� engine_index == -1
virtual void ResetEngine(int engine_index);

/// ������ ��� ������� ���������� ������, ��� ����, ���� engine_index == -1
virtual void StepEngine(int engine_index);
// --------------------------
};

}

#endif

