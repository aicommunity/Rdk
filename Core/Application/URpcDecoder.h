#ifndef URpcDecoder_H
#define URpcDecoder_H

#include "UProject.h"
#include "URpcCommand.h"
#include "URpcDispatcherQueues.h"
#include <mutex>
#include <thread>

namespace RDK {

class URpcDispatcher;
class UApplication;

/// ������������ ������������� ������� ������ ������ RPC � ������������
/// ��������� �������������� �����
/// �� ������ ����� ��������� � ����� ���������� ����������� ������ ��������������
/// ��������� �������
class RDK_LIB_TYPE URpcDecoder: public URpcDispatcherQueues
{
protected: // �������� ����� ��������
URpcDispatcher* Dispatcher;

protected: // ������
/// ������� ��� ���������� ������ ������
std::mutex DispatchMutex;

std::thread DecoderThread;

bool ThreadTerminated;

public:
// --------------------------
// ������������ � �����������
// --------------------------
URpcDecoder(URpcDispatcher* dispatcher=0);
virtual ~URpcDecoder(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// ������������� ������ ���������
virtual void SetDispatcher(URpcDispatcher* dispatcher);

/// ������� ����� ����� ��������
virtual URpcDecoder* New(void)=0;

/// ���������, �������������� �� ������� �����������
/// �������, ��� ������� ��� ������������ ����� ������ ���������� false
virtual bool IsCmdSupported(const UEPtr<URpcCommand> &command) const=0;

/// ����� �����
virtual void Process(void);

/// ��������� �����
virtual void StopProcessThread(void);

/// ������������ ������������� � ����� ������� �� ������� ������
/// ���������� false ���� ������� �� ��������������
virtual bool ProcessCommand(const UEPtr<URpcCommand> &command);
virtual bool AProcessCommand(const UEPtr<URpcCommand> &command)=0;

/// ���������� ��������� �� ��������� ����������
UEPtr<UApplication> GetApplication(void);
// --------------------------
};

}

#endif

