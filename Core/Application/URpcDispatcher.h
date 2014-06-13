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
class URpcDispatcher: public URpcDispatcherQueues
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
/// ������������� �������� ��������
/// �������� ����� ���� ������� ����������
virtual void SetDecoderPrototype(const UEPtr<URpcDecoder> &decoder);

/// ������������ ��������������� ������� ������� ������
virtual void Dispatch(void);

/// �������� ������� ����������, ���������� ��������� ���������� � ������� �� �������
virtual bool SyncDispatchCommand(const UEPtr<URpcCommand> &command);
// --------------------------

// --------------------------
// ��������������� ������ ����������
// --------------------------
protected:
/// ������������ ����� ���������������� ��������
/// ����� ������ ���������� � ����� ������
virtual void DispatchCommand(const UEPtr<URpcCommand> &command);

/// �������� � ������������ ������ ��������� � ����� �������
virtual void UpdateDecoders(void);
// --------------------------

};

}

#endif

