#ifndef UApplication_H
#define UApplication_H

#include "UAppCore.h"
#include "UProject.h"
#include "UIVisualController.h"
#include "URpcDispatcher.h"
#include "URpcDecoder.h"
#include "URpcDecoderInternal.h"
#include "URpcCommand.h"
#include "URpcCommandInternal.h"

#ifdef __BORLANDC__
#include "Bcb/Application.bcb.h"
#endif

namespace RDK {

class UApplication
{
protected: // ������ ����������
/// ������� �������
std::string WorkDirectory;

protected: // ������ ����������
/// ��������� ������
UEPtr<URpcDispatcher> RpcDispatcher;

/// �������� ������
UEPtr<UProject> Project;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UApplication(void);
virtual ~UApplication(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ������� �������
const std::string& GetWorkDirectory(void) const;
bool SetWorkDirectory(const std::string& value);
// --------------------------

// --------------------------
// ������ �������������
// --------------------------
/// ������������� ������ � ���������� ������
virtual UEPtr<URpcDispatcher> GetRpcDispatcher(void);

/// ������������� ����� ��������� ������
/// ��������������� �� ������������ ������ ���������� ����� �� ���������� �������
virtual bool SetRpcDispatcher(const UEPtr<URpcDispatcher> &value);

/// �������������� ����������
virtual bool Init(void);

/// ���������������� ����������
virtual bool UnInit(void);
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
/// ������� ������ (����� ���������� � ��������)
virtual bool CreateProject(const std::string &filename);

/// ��������� ������
virtual bool OpenProject(const std::string &filename);

/// ��������� ������
virtual bool SaveProject(void);
virtual bool SaveProjectAs(const std::string &filename);

/// ��������� ������
virtual bool CloseProject(void);

/// ��������� ������ � ����� ������������
virtual bool CloneProject(const std::string &filename);
// --------------------------
};

}

#endif

