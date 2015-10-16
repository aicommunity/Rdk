#ifndef URpcDecoder_H
#define URpcDecoder_H

#include "UProject.h"
#include "URpcCommand.h"
#include "URpcDispatcherQueues.h"

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
boost::mutex DispatchMutex;

boost::thread DecoderThread;

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

