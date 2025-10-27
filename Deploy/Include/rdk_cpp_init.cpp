#ifndef RDK_CPP_INIT_CPP
#define RDK_CPP_INIT_CPP

#include "rdk_cpp_init.h"
#include "rdk_init.cpp"

namespace RDK {

// --------------------------
// ������ ������� � ���� ��� ����������
// --------------------------
// ���������� ������ �� ������ ����
const RDK::UVersion& RDK_CALL GetCoreVersion(void)
{
 return RdkCoreManager.GetVersion();
}

// ���������� ������ �� ��������� ����
std::shared_ptr<URdkCoreManager> RDK_CALL GetCore(void)
{
 return std::shared_ptr<URdkCoreManager>(&RdkCoreManager, [](URdkCoreManager*){}); // Non-owning deleter
}

// ���������� ��������� �� ������
// GetLogger функции удалены - используется glog

// ���������� ������ �� ��������� ������������ ����
std::shared_ptr<UEngine>& RDK_CALL GetEngine(void)
{
 static std::shared_ptr<UEngine> wrapper;
 wrapper = std::shared_ptr<UEngine>(RdkCoreManager.GetEngine(), [](UEngine*){});
 return wrapper;
}

std::shared_ptr<UEngine> RDK_CALL GetEngine(int channel_index)
{
 return std::shared_ptr<UEngine>(RdkCoreManager.GetEngine(channel_index), [](UEngine*){});
}


// ���������� ������ �� ��������� ����� ����������
std::shared_ptr<UEnvironment>& RDK_CALL GetEnvironment(void)
{
 static std::shared_ptr<UEnvironment> wrapper;
 wrapper = std::shared_ptr<UEnvironment>(RdkCoreManager.GetEnvironment(), [](UEnvironment*){});
 return wrapper;
}

std::shared_ptr<UEnvironment> RDK_CALL GetEnvironment(int channel_index)
{
 return std::shared_ptr<UEnvironment>(RdkCoreManager.GetEnvironment(channel_index), [](UEnvironment*){});
}

// ���������� ������ �� ��������� ���������
std::shared_ptr<UStorage>& RDK_CALL GetStorage(void)
{
 static std::shared_ptr<UStorage> wrapper;
 wrapper = std::shared_ptr<UStorage>(RdkCoreManager.GetStorage(), [](UStorage*){});
 return wrapper;
}

std::shared_ptr<UStorage> RDK_CALL GetStorage(int channel_index)
{
 return std::shared_ptr<UStorage>(RdkCoreManager.GetStorage(channel_index), [](UStorage*){});
}

// ���������� ��������� �� ������� ������
std::shared_ptr<UContainer> RDK_CALL GetModel(void)
{
 return RdkCoreManager.GetModel();
}

std::shared_ptr<UContainer> RDK_CALL GetModel(int channel_index)
{
 return RdkCoreManager.GetModel(channel_index);
}
// --------------------------

// --------------------------
// ������ ������� � ������� � �����������
// --------------------------
// ���������� ������ �� ��������� ����
RDK::UELockPtr<URdkCoreManager> RDK_CALL GetCoreLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<URdkCoreManager>(0,&RdkCoreManager);
#else
 return RDK::UELockPtr<URdkCoreManager>(RdkCoreManager.GetGlobalMutex(),&RdkCoreManager);
#endif
}

// ���������� ������ �� ��������� ������������ ����
RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(void)
{
 return RdkCoreManager.GetEngineLock();
}

RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLock(int channel_index)
{
 return RdkCoreManager.GetEngineLock(channel_index);
}

RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLockTimeout(unsigned timeout)
{
 return RdkCoreManager.GetEngineLockTimeout(timeout);
}

RDK::UELockPtr<RDK::UEngine> RDK_CALL GetEngineLockTimeout(int channel_index, unsigned timeout)
{
 return RdkCoreManager.GetEngineLockTimeout(channel_index, timeout);
}

// ���������� ������ �� ��������� ����� ����������
RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(void)
{
 return RdkCoreManager.GetEnvironmentLock();
}

RDK::UELockPtr<RDK::UEnvironment> RDK_CALL GetEnvironmentLock(int channel_index)
{
 return RdkCoreManager.GetEnvironmentLock(channel_index);
}

// ���������� ������ �� ��������� ���������
RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(void)
{
 return RdkCoreManager.GetStorageLock();
}

RDK::UELockPtr<RDK::UStorage> RDK_CALL GetStorageLock(int channel_index)
{
 return RdkCoreManager.GetStorageLock(channel_index);
}

// ���������� ��������� �� ������� ������
RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(void)
{
 return RdkCoreManager.GetModelLock();
}

RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLock(int channel_index)
{
 return RdkCoreManager.GetModelLock(channel_index);
}

RDK::UELockPtr<RDK::UContainer> RDK_CALL GetModelLockTimeout(int channel_index, unsigned timeout)
{
 return RdkCoreManager.GetModelLockTimeout(channel_index,timeout);
}
// --------------------------


// --------------------------
// ������ ������� � �������
// --------------------------
/// ���������� ������ �� ������
/// (�� ���������������!)
RDK::UBitmapFontCollection& GetFonts(void)
{
 return RdkCoreManager.GetFonts();
}
// --------------------------

// --------------------------
// ������������ � �����������
// --------------------------
EEnginePropertyNotFound::EEnginePropertyNotFound(const std::string &component_name, const std::string &property_name)
: ComponentName(component_name), PropertyName(property_name)
{

}

EEnginePropertyNotFound::~EEnginePropertyNotFound(void) throw()
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string EEnginePropertyNotFound::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" ComponentName=")+ComponentName+
			std::string(" PropertyName=")+PropertyName;
}
// --------------------------

// --------------------------
// ������������ � �����������
// --------------------------
EEnginePropertyDecodeLoadFail::EEnginePropertyDecodeLoadFail(const std::string &xml_data, const std::string &variable_type)
: XmlData(xml_data), VariableType(variable_type)
{

}

EEnginePropertyDecodeLoadFail::~EEnginePropertyDecodeLoadFail(void) throw()
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string EEnginePropertyDecodeLoadFail::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" XmlData=")+XmlData+
			std::string(" VariableType=")+VariableType;
}
// --------------------------

}


#endif

