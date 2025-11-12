#ifndef URpcDispatcher_H
#define URpcDispatcher_H

#include "UProject.h"
#include "URpcCommand.h"
#include "URpcDecoder.h"
#include "URpcDispatcherQueues.h"

#include <thread>
#include <functional>
#include <mutex>


namespace RDK {

/// ��������� ������������� ����� ������� ��� ��������� ������
class RDK_LIB_TYPE URpcDispatcher: public URpcDispatcherQueues
{
protected:
/// ������ ���������, ��������������� �������
std::vector<std::shared_ptr<URpcDecoder> > Decoders;

/// ������� ������� �������
std::shared_ptr<URpcDecoder> CommonDecoder;

/// ��������� ����������
std::shared_ptr<UApplication> Application;

protected:
/// �������� ��������
std::shared_ptr<URpcDecoder> DecoderPrototype;

protected: // ������
/// ������� ��� ���������� ������ ������
std::mutex DispatchMutex;

std::jthread DispatcherThread;

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
virtual bool IsCmdSupported(const std::shared_ptr<URpcCommand> &command) const;

/// ������������� �������� ��������
/// �������� ����� ���� ������� ����������
virtual void SetDecoderPrototype(const std::shared_ptr<URpcDecoder> &decoder);

/// ���������� � ������������� ������� ������� �������
std::shared_ptr<URpcDecoder> GetCommonDecoder(void);
bool SetCommonDecoder(const std::shared_ptr<URpcDecoder> &decoder);

/// ��������� ����������
std::shared_ptr<UApplication> GetApplication(void);
bool SetApplication(std::shared_ptr<UApplication> application);

/// ������������ ��������������� ������� ������� ������
virtual void Dispatch(void);

/// ����� ��������� �����
virtual void StopDispatch(void);

/// �������� ������� ����������, ���������� ��������� ���������� � ������� �� �������
virtual bool SyncDispatchCommand(const std::shared_ptr<URpcCommand> &command, unsigned timeout);
// --------------------------

// --------------------------
// ��������������� ������ ����������
// --------------------------
protected:
/// ������������ ����� ���������������� ��������
/// ����� ������ ���������� � ����� ������
virtual void DispatchCommand(const std::shared_ptr<URpcCommand> &command);

public:
/// �������� � ������������ ������ ��������� � ����� �������
virtual void UpdateDecoders(void);
// --------------------------

};

}

#endif

