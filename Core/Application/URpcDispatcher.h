#ifndef URpcDispatcher_H
#define URpcDispatcher_H

#include "UProject.h"
#include "URpcCommand.h"
#include "URpcDecoder.h"

namespace RDK {

/// ��������� ������������� ����� ������� ��� ��������� ������
class URpcDispatcher
{
protected:
/// ������� ������, ��������� ���������
std::list<UEPtr<URpcCommand> > CommandQueue;

/// ������� ������� �� �������, ��������� ��������
std::list<UEPtr<URpcCommand> > ProcessedCommandQueue;

/// ������ ���������, ��������������� �������
std::vector<UEPtr<URpcDecoder> > Decoders;

protected:
/// �������� ��������
UEPtr<URpcDecoder> DecoderPrototype;


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

/// ���������� ������� � ������� �� ���������
/// ���������� Id ������� � cmd_id
/// � ������ ������� ���������� false
virtual bool PushCommand(const UEPtr<URpcCommand> &command, unsigned &cmd_id);

/// ���������� ��������� �� ������� �� �� Id � ������� �������
/// ������������ ��������� ����� ����, ���� ������� ��� � �������
virtual UEPtr<URpcCommand> FindProcessedCommand(unsigned cmd_id);

/// ������� ������� �� ������� ������� �� �� ��������������
/// ���������� true ���� ������� ���� � �������
virtual bool PopCommand(unsigned cmd_id);

/// ������������ ��������������� ������� ������� ������
virtual void Dispatch(void);

/// ������� �������
virtual void ClearQueue(void);

/// ������� ������� ��������� �������� �������
virtual void ClearProcessedQueue(void);

/// �������� � ������������ ������ ��������� � ����� �������
virtual void UpdateDecoders(int num_channels);
// --------------------------

};

}

#endif

