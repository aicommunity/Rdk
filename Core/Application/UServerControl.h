#ifndef UServerControlH
#define UServerControlH

#include "../../Deploy/Include/rdk.h"

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
std::vector< std::vector< RDK::ULongTime > > perf_data;

///����������� ������������������ ��� ���������� �������
std::vector< std::vector< RDK::ULongTime > > aver_perf_data;

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
/// ���������� ������ �������
/// ���������� ��������������� �������
/// ���������� �� UApplication
// --------------------------
virtual bool SetNumEngines(int num);
virtual bool ASetNumEngines(int num);

virtual bool InsertEngine(int index);
virtual bool AInsertEngine(int index);

virtual bool DeleteEngine(int index);
virtual bool ADeleteEngine(int index);
// --------------------------
};

}

#endif