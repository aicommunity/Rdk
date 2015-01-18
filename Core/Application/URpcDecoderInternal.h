#ifndef URpcDecoderInternal_H
#define URpcDecoderInternal_H

#include "URpcDecoder.h"

namespace RDK {

/// 
class RDK_LIB_TYPE URpcDecoderInternal: public URpcDecoder
{

public:
// --------------------------
// ������������ � �����������
// --------------------------
URpcDecoderInternal(void);
virtual ~URpcDecoderInternal(void);
// --------------------------

// --------------------------
// ������ ���������� ���������
// --------------------------
/// ���������, �������������� �� ������� �����������
/// �������, ��� ������� ��� ������������ ����� ������ ���������� false
virtual bool IsCmdSupported(const UEPtr<URpcCommand> &command) const;

/// ������� ����� ����� ��������
virtual URpcDecoderInternal* New(void);

/// ������������ ������������� � ����� ������� �� ������� ������
/// ���������� false ���� ������� �� ��������������
virtual bool AProcessCommand(const UEPtr<URpcCommand> &command);
// --------------------------
};

}

#endif

