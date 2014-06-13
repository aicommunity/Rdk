#ifndef URpcDispatcherQueues_H
#define URpcDispatcherQueues_H

#include "UProject.h"
#include "URpcCommand.h"

#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>


namespace RDK {

/// ��������� ���������� ��������� ������
class URpcDispatcherQueues
{
protected:
/// ������� ������, ��������� ���������
std::list<UEPtr<URpcCommand> > CommandQueue;

/// ������� ������� �� �������, ��������� ��������
std::list<UEPtr<URpcCommand> > ProcessedCommandQueue;

protected: // ������
/// ������� ��� ���������� ������� � ������� ������
boost::mutex CommandQueueMutex;

/// ������� ��� ���������� ������� � ������� �������
boost::mutex ProcessedCommandQueueMutex;

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

