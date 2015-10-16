#ifndef UApplication_CPP
#define UApplication_CPP

#include "UApplication.h"

//#include "UAppCore.cpp"
//#include "UProject.cpp"
//#include "UIVisualController.cpp"
//#include "URpcDispatcherQueues.cpp"
//#include "URpcDispatcher.cpp"
//#include "URpcDecoder.cpp"
//#include "URpcDecoderInternal.cpp"
//#include "URpcCommand.cpp"
//#include "URpcCommandInternal.cpp"

              
#ifdef __BORLANDC__
//#include "Bcb/Application.bcb.cpp"
#endif

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UApplication::UApplication(void)
{
 Name="Application";
}

UApplication::~UApplication(void)
{

}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ������� �������
const std::string& UApplication::GetWorkDirectory(void) const
{
 return WorkDirectory;
}

bool UApplication::SetWorkDirectory(const std::string& value)
{
 WorkDirectory=value;
 return true;
}

// ������� ������� ��������� �������
bool UApplication::GetProjectOpenFlag(void) const
{
 return ProjectOpenFlag;
}

bool UApplication::SetProjectOpenFlag(bool value)
{
 ProjectOpenFlag=value;
 return true;
}
// --------------------------

// --------------------------
// ������ �������������
// --------------------------
/// ������������� ������ � ���������� ������
UEPtr<URpcDispatcher> UApplication::GetRpcDispatcher(void)
{
 return RpcDispatcher;
}

/// ������������� ����� ��������� ������
/// ��������������� �� ������������ ������ ���������� ����� �� ���������� �������
bool UApplication::SetRpcDispatcher(const UEPtr<URpcDispatcher> &value)
{
 if(RpcDispatcher == value)
  return true;

 if(RpcDispatcher)
  RpcDispatcher->SetApplication(0);
 RpcDispatcher=value;
 RpcDispatcher->SetApplication(this);
 return true;
}

/// ������������� ������ � ����������� ������
UEPtr<UEngineControl> UApplication::GetEngineControl(void)
{
 return EngineControl;
}

/// ������������� ����� ���������� ������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
bool UApplication::SetEngineControl(const UEPtr<UEngineControl> &value)
{
 if(EngineControl == value)
  return true;

 if(EngineControl)
  EngineControl->PauseEngine(-1);
 EngineControl=value;
 return true;
}

/// ������������� ������ � �������
UEPtr<UProject> UApplication::GetProject(void)
{
 return Project;
}

/// ������������� ����� ������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
bool UApplication::SetProject(const UEPtr<UProject> &value)
{
 if(Project == value)
  return true;

 // TODO: ����� �����-�� ����������� �������� �� ������ ��������.
 Project=value;
 return true;
}

/// ������������� ������ � ����������� ��������� �����
UEPtr<UServerControl> UApplication::GetServerControl(void) const
{
 return ServerControl;
}

/// ������������� ����� ���������� �������
/// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������
bool UApplication::SetServerControl(const UEPtr<UServerControl> &value)
{
 if(ServerControl == value)
  return true;

 // TODO: ����� �����-�� ����������� �������� �� ������ ��������
 if(ServerControl)
  ServerControl->SetApplication(0);
 ServerControl=value;
 ServerControl->SetApplication(this);
 return true;
}

/// �������������� ����������
bool UApplication::Init(void)
{
 return true;
}

/// ���������������� ����������
bool UApplication::UnInit(void)
{
 if(EngineControl)
  EngineControl->PauseEngine(-1);
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
/// ������� ������ (����� ���������� � ��������)
bool UApplication::CreateProject(const std::string &filename)
{
 return true;
}

/// ��������� ������
bool UApplication::OpenProject(const std::string &filename)
{
 return true;
}

/// ��������� ������
bool UApplication::SaveProject(void)
{
 return true;
}

bool UApplication::SaveProjectAs(const std::string &filename)
{
 return true;
}

/// ��������� ������
bool UApplication::CloseProject(void)
{
 return true;
}

/// ��������� ������ � ����� ������������
bool UApplication::CloneProject(const std::string &filename)
{
 return true;
}
// --------------------------


// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ������ �������
int UApplication::GetNumEngines(void) const
{
 return EngineControl->GetNumEngines();
}

bool UApplication::SetNumEngines(int num)
{
 if(!EngineControl->SetNumEngines(num))
  return false;

 if(!ServerControl->SetNumEngines(num))
  return false;

 return true;
}

bool UApplication::InsertEngine(int index)
{
 if(!EngineControl->InsertEngine(index))
  return false;

 if(!ServerControl->InsertEngine(index))
  return false;

 return true;
}

bool UApplication::DeleteEngine(int index)
{
 if(!EngineControl->DeleteEngine(index))
  return false;

 if(!ServerControl->DeleteEngine(index))
  return false;

 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
/// ��������� ��������� ���������� ������, ��� ����, ���� engine_index == -1
void UApplication::StartEngine(int engine_index)
{
 EngineControl->StartEngine(engine_index);
}

/// ������������� ��������� ���������� ������, ��� ����, ���� engine_index == -1
void UApplication::PauseEngine(int engine_index)
{
 EngineControl->PauseEngine(engine_index);
}

/// ���������� ��������� ���������� ������, ��� ����, ���� engine_index == -1
void UApplication::ResetEngine(int engine_index)
{
 EngineControl->ResetEngine(engine_index);
}

/// ������ ��� ������� ���������� ������, ��� ����, ���� engine_index == -1
void UApplication::StepEngine(int engine_index)
{
 EngineControl->StepEngine(engine_index);
}
// --------------------------


}

#endif

