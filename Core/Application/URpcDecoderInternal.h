#ifndef URpcDecoderInternal_H
#define URpcDecoderInternal_H

#include "URpcDecoder.h"

namespace RDK {

/// 
class URpcDecoderInternal: public URpcDecoder
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
/// ������� ����� ����� ��������
virtual URpcDecoderInternal* New(void);

/// ������������ ������������� � ����� ������� �� ������� ������
/// ���������� false ���� ������� �� ��������������
virtual bool ProcessCommand(UEPtr<URpcCommand> &command);
// --------------------------
};

}

#endif

