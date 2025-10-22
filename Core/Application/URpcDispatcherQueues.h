#ifndef URpcDispatcherQueues_H
#define URpcDispatcherQueues_H

#include "UProject.h"
#include "URpcCommand.h"
#include <memory>

#include <mutex>
#include <condition_variable>
#include "../System/ModernBoostReplacement.h"


namespace RDK {

/// ��������� ���������� ��������� ������
class RDK_LIB_TYPE URpcDispatcherQueues
{
protected:
/// ������� ������, ��������� ���������
std::list<std::shared_ptr<URpcCommand> > CommandQueue;

/// ������� ������� �� �������, ��������� ��������
std::list<std::shared_ptr<URpcCommand> > ProcessedCommandQueue;

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
virtual bool PushCommand(const std::shared_ptr<URpcCommand> &command, unsigned &cmd_id);

/// ���������� ������� � ������� �� ���������
/// � ������ ������� ���������� false
virtual bool PushCommand(const std::shared_ptr<URpcCommand> &command);

/// ���������� ��������� �� ������� �� �� Id � ������� �������
/// ������������ ��������� ����� ����, ���� ������� ��� � �������
virtual std::shared_ptr<URpcCommand> FindProcessedCommand(unsigned cmd_id);

/// ������� ������� �� ������� ������� �� �� ��������������
/// ���������� true ���� ������� ���� � �������
virtual bool PopProcessedCommand(unsigned cmd_id);

/// ���������� ��������� �� ����� ������ �����
/// � ������� ��� �� �������
virtual std::shared_ptr<URpcCommand> PopProcessedCommand(void);

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
std::shared_ptr<URpcCommand> PopFromCommandQueue(void);

/// ������ ������� � ������� �������
void PushToProcessedQueue(const std::shared_ptr<URpcCommand> &command);
// --------------------------

};

}

#endif

