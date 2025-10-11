#ifndef URpcDispatcherQueues_H
#define URpcDispatcherQueues_H

#include "UProject.h"
#include "URpcCommand.h"
#include "../Engine/UEPtr.h"

#include <mutex>
#include <condition_variable>
#include "../System/ModernBoostReplacement.h"


namespace RDK {

/// ��������� ���������� ��������� ������
class RDK_LIB_TYPE URpcDispatcherQueues
{
protected:
/// ������� ������, ��������� ���������
std::list<UEPtr<URpcCommand> > CommandQueue;

/// ������� ������� �� �������, ��������� ��������
std::list<UEPtr<URpcCommand> > ProcessedCommandQueue;

protected: // ������
/// ������� ��� ���������� ������� � ������� ������
std::mutex CommandQueueMutex;

/// ������� ��� ���������� ������� � ������� �������
std::mutex ProcessedCommandQueueMutex;

public:
// --------------------------
// ������������ � �����������
// --------------------------
URpcDispatcherQueues(void);
virtual ~URpcDispatcherQueues(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// ���������� ������� � ������� �� ���������
/// ���������� Id ������� � cmd_id
/// � ������ ������� ���������� false
virtual bool PushCommand(const UEPtr<URpcCommand> &command, unsigned &cmd_id);

/// ���������� ������� � ������� �� ���������
/// � ������ ������� ���������� false
virtual bool PushCommand(const UEPtr<URpcCommand> &command);

/// ���������� ��������� �� ������� �� �� Id � ������� �������
/// ������������ ��������� ����� ����, ���� ������� ��� � �������
virtual UEPtr<URpcCommand> FindProcessedCommand(unsigned cmd_id);

/// ������� ������� �� ������� ������� �� �� ��������������
/// ���������� true ���� ������� ���� � �������
virtual bool PopProcessedCommand(unsigned cmd_id);

/// ���������� ��������� �� ����� ������ �����
/// � ������� ��� �� �������
virtual UEPtr<URpcCommand> PopProcessedCommand(void);

/// ���������� true ���� ������� ������� �� �����
virtual bool CheckProcessedCommand(void);

/// ������� �������
virtual void ClearQueue(void);

/// ������� ������� ��������� �������� �������
virtual void ClearProcessedQueue(void);
// --------------------------

// --------------------------
// ��������������� ������ ����������
// --------------------------
public:
/// ���������� ��������� �� ������� ������� �� CommandQueue � ������� �� �� �������
UEPtr<URpcCommand> PopFromCommandQueue(void);

/// ������ ������� � ������� �������
void PushToProcessedQueue(const UEPtr<URpcCommand> &command);
// --------------------------

};

}

#endif

