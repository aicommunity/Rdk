#ifndef RDK_CPP_INIT_CPP
#define RDK_CPP_INIT_CPP

#include "rdk_cpp_init.h"
#include "rdk_init.cpp"

// ���������� ������ �� ��������� ������������ ����
RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void)
{
 return DllManager.GetEngine();
}

RDK::UEPtr<RDK::UEngine> RDK_CALL GetEngine(int engine_index)
{
 return DllManager.GetEngine(engine_index);
}


// ���������� ������ �� ��������� ����� ����������
RDK::UEPtr<RDK::UEnvironment>& RDK_CALL GetEnvironment(void)
{
 return DllManager.GetEnvironment();
}

RDK::UEPtr<RDK::UEnvironment> RDK_CALL GetEnvironment(int engine_index)
{
 return DllManager.GetEnvironment(engine_index);
}

// ���������� ������ �� ��������� ���������
RDK::UEPtr<RDK::UStorage>& RDK_CALL GetStorage(void)
{
 return DllManager.GetStorage();
}

RDK::UEPtr<RDK::UStorage> RDK_CALL GetStorage(int engine_index)
{
 return DllManager.GetStorage(engine_index);
}

// ���������� ��������� �� ������� ������
RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(void)
{
 return DllManager.GetModel();
}

RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(int engine_index)
{
 return DllManager.GetModel(engine_index);
}


// --------------------------
// ������ ������� � ������� � �����������
// --------------------------
// ���������� ������ �� ��������� ������������ ����
RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(void)
{
 return DllManager.GetEngineLock(0);
}

RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(int engine_index)
{
 return DllManager.GetEngineLock(engine_index);
}

// ���������� ������ �� ��������� ����� ����������
RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(void)
{
 return DllManager.GetEnvironmentLock(0);
}

RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(int engine_index)
{
 return DllManager.GetEnvironmentLock(engine_index);
}

// ���������� ������ �� ��������� ���������
RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(void)
{
 return DllManager.GetStorageLock(0);
}

RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(int engine_index)
{
 return DllManager.GetStorageLock(engine_index);
}

// ���������� ��������� �� ������� ������
RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(void)
{
 return DllManager.GetModelLock(0);
}

RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(int engine_index)
{
 return DllManager.GetModelLock(engine_index);
}
// --------------------------

#endif
