#ifndef UProjectDeployerH
#define UProjectDeployerH

//#include "../../Deploy/Include/rdk.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "UIVisualController.h"

namespace RDK{
class UApplication;

class RDK_LIB_TYPE UProjectDeployer: public UAppController
{
friend class UApplication;
protected: // ���������

protected: // ������
/// ��������� �� ��������� ����������
UEPtr<UApplication> Application;
protected: // ���������

protected: // ������

//������ ��� �����:
std::string database_address;
std::string database_name;
std::string database_login;
std::string database_password;

std::string ftp_remote_path;

std::string temp_project_deployment_path;

/*
//�������� �������������� (�� �������� ����� ��������� ������!)
//���� � ������� (���-� 1)
std::string project_path;
std::string project_url;
//��������� �������
int project_gt_id;
int project_sln_id;
int project_weigts_id;
int project_script_id;
*/

public: // ������

// --------------------------
// ������ ������� � ������
// --------------------------
/// ���������� ��������� �� ��������� ����������
UEPtr<UApplication> GetApplication(void);
bool SetApplication(UEPtr<UApplication> value);
// --------------------------

///��������� �������� ������ ��� ���������� ������,
/// �������� ������������ (�� ���� ���� ������ � ���� ������)
virtual int StartProjectDeployment(int task_id, bool standalone=false);


///����������� � ������� ������:
/// 1. ����������� �� ��������� ���������
/// 2. ������� � �������� ������ � ��������� ���� � �����?
/// 3. �������
virtual int PrepareProject(std::string &response);
/// �������� ��������� ���������� �������
virtual int GetPreparationResult(std::string &response);
///������� �������������� ������
virtual int OpenPreparedProject(std::string &response);
///��������� �������������� ������
virtual int RunPreparedProject();

///������ ���-�� ��������� � ����
virtual void SetDatabaseAccess(const std::string &db_address, const std::string &db_name, const std::string &db_login, const std::string &db_password);
/// ����������� ����������
virtual void AConnectToDatabase();
///
//virtual void SetProjectIndices(int gt_id, int sln_id, int weights_id, int script_id);

virtual int GetDeploymentState();
virtual int GetStageCap();
virtual int GetStageProgress();

virtual std::string GetLastError();
virtual std::string GetProjectFileName();

virtual void SetFtpRemotePath(const std::string &path);
virtual void SetTempProjectDeploymentPath(const std::string &path);

virtual std::string GetTempProjectDeploymentPath();

///���������� ��������� ������ ������� (������ -2/0/1 ������� � ����)
virtual int GetCalculationState();
///���������� ��������� ��������� ���������� �������
/// ���������� false ��� ������ ��������� ���������
/// @state - ������ ��������� �������
/// @frame_id - ������ �������� �����
virtual bool GetCaptureState(int &state, unsigned long long& frame_id, unsigned long long& max_frame_id);
///������������ ������������ � ���������� ������ ���
/// ���������� false ���� ���� ��������� ������, ����� true
/// @error - ����� ������ �� ���� ����������
virtual bool ProcessCalculationLog(std::string &error);
///���������� true ���� ��������� ������/������ ������ �� ������ ��������
/// �� ���������� ����� ��������� � false ���� ���
//virtual bool CaptureProcessingFinished(int frame_number);
///��������� ������ �������, �������� ��������������� ��������� ������� � ���� ������
virtual bool FinishCalculation() {return false;};
///��������� ���������� ������� (���������� ����� Results) � ��������������� ����� ���������� ���������,
/// ��������� ������� �������� � �������� ������ � ��������� ���������
virtual bool UploadCalculationResults() {return false;};
///���������� �������� �������, ������� ������� �� ���� ������ ������������
/// ������� ���������� ������, ��������� ��������� ���������� ������� � �.�.
virtual bool CloseSolver() {return false;};
///�������� ��������� ��������
virtual int GetUploadState(){return -1;};

/// ������ ��� ������� ��� ����
virtual void SetStandaloneTask(int task);
virtual int GetStandaloneTask();
// --------------------------
// ������������ � �����������
// --------------------------
UProjectDeployer(void);
virtual ~UProjectDeployer(void);
// --------------------------

// --------------------------
// ������ ��������������� ������
// --------------------------

/// ������ �������� ����� �� ���������� ���������, �������� ��������
/// ������ ������������ ������� ��� ����������� �� ���� ����������
//virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);
};

}//namespace RDK
#endif
