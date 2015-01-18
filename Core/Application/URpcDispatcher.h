#ifndef URpcDispatcher_H
#define URpcDispatcher_H

#include "UProject.h"
#include "URpcCommand.h"
#include "URpcDecoder.h"
#include "URpcDispatcherQueues.h"

#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>


namespace RDK {

/// ��������� ������������� ����� ������� ��� ��������� ������
class RDK_LIB_TYPE URpcDispatcher: public URpcDispatcherQueues
{
protected:
/// ������ ���������, ��������������� �������
std::vector<UEPtr<URpcDecoder> > Decoders;

protected:
/// �������� ��������
UEPtr<URpcDecoder> DecoderPrototype;

protected: // ������
/// ������� ��� ���������� ������ ������
boost::mutex DispatchMutex;

boost::thread DispatcherThread;

bool ThreadTerminated;

public:
// --------------------------
// ������������ � �����������
// --------------------------
URpcDispatcher(void);
virtual ~URpcDispatcher(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// ���������, �������������� �� �������� ������� �����������
/// ������� �������������� �������, ����� ������ false
virtual bool IsCmdSupported(const UEPtr<URpcCommand> &command) const;

/// ������������� �������� ��������
/// �������� ����� ���� ������� ����������
virtual void SetDecoderPrototype(const UEPtr<URpcDecoder> &decoder);

/// ������������ ��������������� ������� ������� ������
virtual void Dispatch(void);

/// �������� ������� ����������, ���������� ��������� ���������� � ������� �� �������
virtual bool SyncDispatchCommand(const UEPtr<URpcCommand> &command, unsigned timeout);
// --------------------------

// --------------------------
// ��������������� ������ ����������
// --------------------------
protected:
/// ������������ ����� ���������������� ��������
/// ����� ������ ���������� � ����� ������
virtual void DispatchCommand(const UEPtr<URpcCommand> &command);

public:
/// �������� � ������������ ������ ��������� � ����� �������
virtual void UpdateDecoders(void);
// --------------------------

};

}

#endif

