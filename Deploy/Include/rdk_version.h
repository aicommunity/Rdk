#ifndef RDK_VERSION_H
#define RDK_VERSION_H


#define RDK_MAJOR_VERSION 1
#define RDK_MINOR_VERSION 0

#ifdef RDK_CORE_VERSION
#define RDK_CORE_CVS_REVISION RDK_CORE_VERSION
#else
#define RDK_CORE_CVS_REVISION 0
#endif

#include <string>

namespace RDK {

struct UVersion
{
/// �������� ������ ����
int Major;

/// �������� ������ ����
int Minor;

/// ������� CVS
int Revision;

/// ������ boost
std::string Boost;

/// ������ OpenCv;
std::string OpenCv;

/// ��� �����������
std::string CompilerName;

/// ������ �����������
std::string CompilerVersion;

public: // ������
/// --------------------------
/// ������������ � �����������
/// --------------------------
UVersion(void);
UVersion(const UVersion &copy);
UVersion(int major, int minor, int revision);
UVersion(int major, int minor, int revision, const std::string &boost, const std::string &opencv);
/// --------------------------

/// --------------------------
/// ����� � ���������
/// --------------------------
/// ����������� ������ � ������
std::string ToString(void) const;

/// ����������� ������ � ������ �� ����� �������������
std::string ToStringFull(void) const;

/// ���������� ������ � ������ (������ �������� ������)
/// ���������� >0 ���� ������ ���� ������,
/// ���������� <0 ���� ������ ���� ������,
/// ���������� 0 � ������ ����������.
int CompareCore(const UVersion &ver) const;
int CompareCore(int major, int minor, int revision) const;

/// ���������� ������ � ������ (������ � �������������)
/// ���������� >0 ���� ������ ���� ������,
/// ���������� <0 ���� ������ ���� ������,
/// ���������� 0 � ������ ����������.
//int CompareFull(const UVersion &ver);
//int CompareFull(int major, int minor, int revision, const std::string &boost, const std::string &opencv);

/// ���������� ������ � ������ (������ �������� ������) �� ������ ����������
bool IsEqualCore(const UVersion &ver) const;
bool IsEqualCore(int major, int minor, int revision) const;

/// ���������� ������ � ������ (������ � �������������) �� ������ ����������
bool IsEqualFull(const UVersion &ver);
bool IsEqualFull(int major, int minor, int revision, const std::string &boost, const std::string &opencv,
                 const std::string &compiler_name, const std::string &compiler_version);
/// --------------------------
};


// ----------------------------
// ������� ������� � ��������� ����� ����������� ������
// ----------------------------
/// ������ � ���������� ���������� ������
const RDK::UVersion& GetGlobalVersion(void);

/// ���������� �������� ������ ����
int CoreMajorVer(void);

/// ���������� �������� ������ ����
int CoreMinorVer(void);

/// ���������� ������ ����� ����
int CoreRevisionVer(void);

/// ���������� ������ ������ ���� � ���� ������
const char* CoreVer(void);

/// ���������� ������ ���� � ����������
/// ���������� >0 ���� ������ ���� ������,
/// ���������� <0 ���� ������ ���� ������,
/// ���������� 0 � ������ ����������.
int CoreCompareVer(int major, int minor, int patch);

/// ���������� ��� ����������� ����
const char* CompilerName(void);

/// ���������� ������ ����������� ����
const char* CompilerVersion(void);

/// ���������� ������ boost
const char* BoostVersion(void);

/// ���������� ������ opencv (���� ������������)
const char* OpenCvVersion(void);
// ----------------------------

}
#endif
