#ifndef URpcDecoderCommonH
#define URpcDecoderCommonH


#include "URpcDecoder.h"

namespace RDK {

///
class RDK_LIB_TYPE URpcDecoderCommon: public URpcDecoder
{

public:
// --------------------------
// ������������ � �����������
// --------------------------
URpcDecoderCommon(void);
virtual ~URpcDecoderCommon(void);
// --------------------------

// --------------------------
// ������ ���������� ���������
// --------------------------
/// ���������, �������������� �� ������� �����������
/// �������, ��� ������� ��� ������������ ����� ������ ���������� false
virtual bool IsCmdSupported(const UEPtr<URpcCommand> &command) const;

/// ������� ����� ����� ��������
virtual URpcDecoderCommon* New(void);

/// ������������ ������������� � ����� ������� �� ������� ������
/// ���������� false ���� ������� �� ��������������
virtual bool AProcessCommand(const UEPtr<URpcCommand> &command);

virtual const char* RemoteCall(const char *request, int &return_value, int &channel_index);
virtual std::string ARemoteCall(const std::string &cmd, RDK::USerStorageXML &xml, const std::string &component_name, int engine_index, int &return_value);
// --------------------------
};

}

#endif
