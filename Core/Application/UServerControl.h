#ifndef UServerControlH
#define UServerControlH

#include "../../Deploy/Include/rdk.h"
#include "UIVisualController.h"
#include "UServerTransportTcp.h"
//��������� �����?
#include "URpcDispatcherQueues.h"
#include "URpcDispatcher.h"
#include "URpcDecoder.h"
#include "URpcDecoderInternal.h"
#include "URpcDecoderCommon.h"
#include "URpcCommand.h"
#include "URpcCommandInternal.h"

namespace RDK {

class UApplication;

class RDK_LIB_TYPE UServerControl: public UAppController
{
friend class UApplication;
protected: // ���������
/// ���� ���������� ������� ������� ��� ������
bool AutoStartFlag;

/// ��� �������
std::string ServerName;

/// ������������� �������
std::string ServerId;

/// ����� ����� ���������� ������ ������������������
int AverageIterations;

std::string MetaComponentName;

std::string MetaComponentStateName;

///����� ��� ���������� ���������� ������
std::string DebugFolder;

///������ ���� �� ���� ������
std::string DebugOutputPath;

protected: // ������
/// ��������� �� ��������� ����������
UEPtr<UApplication> Application;

/// ��������� �� ��������� ����������
UEPtr<UServerTransport> ServerTransport;

/// ��������� �� ��������� ���������� ��� ������ � �������� HTTP
UEPtr<UServerTransport> ServerTransportHttp;

/// ��������� ������
UEPtr<URpcDispatcher> RpcDispatcher;

/// ���� ��������� �������������
bool InitFlag;

protected: // ������ ��� ������ ������������������ � ���������� ���������� ����������
/// ���������� ��������� ������������������, ��
std::vector<std::vector<RDK::ULongTime> > ModelPerformanceResults;
std::vector<std::vector<RDK::ULongTime> > TransportPerformanceResults;

// ������ ����������� ������ � ������ ������ ������������������
int PerformancePushIndex;

std::vector<RDK::ULongTime> ModelAvg, TransportAvg;

///������ ������������������ �� ������� �� �������
//std::vector< std::vector< RDK::ULongTime > > perf_data;

///����������� ������������������ ��� ���������� �������
//std::vector< std::vector< RDK::ULongTime > > aver_perf_data;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UServerControl(void);
virtual ~UServerControl(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
/// ���� ���������� ������� ������� ��� ������
bool GetAutoStartFlag(void) const;
bool SetAutoStartFlag(bool value);

/// ��� �������
const std::string& GetServerName(void) const;
bool SetServerName(const std::string& value);

/// ������������� �������
const std::string& GetServerId(void) const;
bool SetServerId(const std::string& value);

/// ����� ����� ���������� ������ ������������������
int GetAverageIterations(void) const;
bool SetAverageIterations(int value);

const std::string& GetMetaComponentName(void) const;
bool SetMetaComponentName(const std::string& value);

const std::string& GetMetaComponentStateName(void) const;
bool SetMetaComponentStateName(const std::string& value);

///����� ��� ���������� ���������� ������
const std::string& GetDebugFolder(void) const;
bool SetDebugFolder(const std::string& value);

///������ ���� �� ���� ������
const std::string& GetDebugOutputPath(void) const;
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ���������� ��������� �� ��������� ����������
UEPtr<UApplication> GetApplication(void);
bool SetApplication(UEPtr<UApplication> value);
// --------------------------
/// ���������� ��������� �� ��������� ����������
UEPtr<UServerTransport> GetServerTransport(void);
bool SetServerTransport(UEPtr<UServerTransport> value);

UEPtr<UServerTransport> GetServerTransportHttp(void);
bool SetServerTransportHttp(UEPtr<UServerTransport> value);
// --------------------------

/// ������������� ������ � ���������� ������
virtual UEPtr<URpcDispatcher> GetRpcDispatcher(void);

/// ������������� ����� ��������� ������
/// ��������������� �� ������������ ������ ���������� ����� �� ���������� �������
virtual bool SetRpcDispatcher(const UEPtr<URpcDispatcher> &value);

// --------------------------
// ������ ��� ������ ������������������ � ���������� ���������� ����������
// --------------------------
/// ���������� ��������� ������������������, ��
const std::vector<std::vector<RDK::ULongTime> >& GetModelPerformanceResults(void) const;
const std::vector<std::vector<RDK::ULongTime> >& GetTransportPerformanceResults(void) const;

/// ������� �������� ������������ ������� �� ������ ������ � ������
const std::vector<RDK::ULongTime>& GetModelAvg(void) const;
const std::vector<RDK::ULongTime>& GetTransportAvg(void) const;
// --------------------------


// --------------------------
// ������ ����������
// --------------------------
/// ������������� (����������� ������)
virtual void Init(void);

/// ��������������� (����������� ���������)
virtual void UnInit(void);

/// �������� ��������� �������������
bool IsInit(void) const;
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
/// ������������ ��������� �������� ����������
virtual int RegisterMetadataReceiver(const std::string &address, int port);

/// ������� ��������� �������� ����������
virtual int UnRegisterMetadataReceiver(const std::string &address, int port);
// --------------------------

// --------------------------
// ����� ������ ���������� ������������
// --------------------------
// ��������� ��������� ���������� � xml
virtual void SaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void LoadParameters(RDK::USerStorageXML &xml);
// --------------------------

protected:
// --------------------------
// ��������������� ������
// --------------------------
/// ������������ ������������������ �������
/// � ��������� �� ���� ���� ������������������
void CalculatePerformance(void);

// �����, ���������� ����� ������ ������
virtual void AfterReset(void);

// �����, ���������� ����� ���� �������
virtual void AfterCalculate(void);
// --------------------------

// --------------------------
// ��������������� ������ (��������� �������������� � �����������)
// --------------------------

public:
/// ���������� ����� �� �������
virtual void SendCommandResponse(UServerTransport *transport, std::string &client_address, RDK::UParamT &dest, std::vector<RDK::UParamT> &binary_data);
/// ���������� ����� �� �������
//virtual void ASendCommandResponse(RDK::UParamT &dest, std::vector<RDK::UParamT> &binary_data);

virtual void ProcessCommandQueue(UServerTransport *transport);

virtual void ProcessIncomingData(std::string &bind,UServerTransport *transport);

protected:


/// �������� ������ � ������
void ConvertStringToVector(const std::string &source, RDK::UParamT &dest);

/// �������� ������ � ������
void ConvertVectorToString(const RDK::UParamT &source, std::string &dest);

// --------------------------
/// ���������� ������ �������
/// ���������� ��������������� �������
/// ���������� �� UApplication
// --------------------------
virtual bool SetNumChannels(int number);
virtual bool ASetNumChannels(int num);

virtual bool InsertChannel(int index);
virtual bool AInsertChannel(int index);

virtual bool DeleteChannel(int index);
virtual bool ADeleteChannel(int index);
// --------------------------
};

}

#endif
