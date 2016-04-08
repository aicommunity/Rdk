#ifndef UEXCEPTION_H
#define UEXCEPTION_H

#include <ctime>
#include <string>
#include "../../Deploy/Include/initdll_defs.h"

#define RDK_EX_UNKNOWN 0
#define RDK_EX_FATAL 1
#define RDK_EX_ERROR 2
#define RDK_EX_WARNING 3
#define RDK_EX_INFO 4
#define RDK_EX_DEBUG 5

#define RDK_SYS_DEBUG_CALC 1
#define RDK_SYS_DEBUG_RESET 2
#define RDK_SYS_DEBUG_PARAMETERS 4
#define RDK_SYS_DEBUG_STATES 8
#define RDK_SYS_DEBUG_INPUTS 16
#define RDK_SYS_DEBUG_OUTPUTS 32

#define RDK_SYS_MESSAGE_ENTER 1
#define RDK_SYS_MESSAGE_EXIT_OK 2
#define RDK_SYS_MESSAGE_EXIT_ININIT_FAIL 4

namespace RDK {

/* ������� ����� ���������� */
class RDK_LIB_TYPE UException: public std::exception
{
protected: // ������ ����������
// ���������� ����� ����������
//long long Number;

/// ��� ����������
/// 0 - ������������
/// 1 - ���������
/// 2 - ������, ��������� �������������
/// 3 - ��������������
/// 4 - ����������
/// 5 - �������
int Type;

// ����� ������������� ����������
std::time_t Time;

/// ��� ����� � ������� ��������� ����������
mutable std::string ExFileName;

/// ������ �� ������� ��������� ����������
mutable int ExLineNumber;

/// ��� ������� ���������������� ���������
mutable std::string ObjectName;

/// ��������� ����������
mutable std::string Message;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UException(void);
UException(const UException &copy);
virtual ~UException(void) throw();
// --------------------------

// --------------------------
// ������ ���������� ������ �������
// --------------------------
// --------------------------

// --------------------------
// ������ ��������� ������� ����������
// --------------------------
// ���������� ���������� ����� ����������
//long long GetNumber(void) const;

// ��� ����������
int GetType(void) const;

// ����� ������������� (���������) ����������
std::time_t GetTime(void) const;
void SetTime(std::time_t ex_time);

/// ��� ����� � ������� ��������� ����������
std::string GetExFileName(void) const;
void SetExFileName(const std::string &value);

/// ������ �� ������� ��������� ����������
int GetExLineNumber(void) const;
void SetExLineNumber(int value);

/// ��� ������� ���������������� ���������
std::string GetObjectName(void) const;
void SetObjectName(const std::string &value);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
/// ���������� ������ ���� �� ����������
virtual char const * what() const throw();

protected:
/// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


/* ��������� ������ (��������� �� 0 ���������� � �.�.) */
struct RDK_LIB_TYPE EFatal: public UException
{
// --------------------------
// ������������ � �����������
// --------------------------
EFatal(void);
EFatal(const EFatal &copy);
virtual ~EFatal(void) throw();
// --------------------------

};

/* ������, �������������� ������������� */
struct RDK_LIB_TYPE EError: public UException
{
// --------------------------
// ������������ � �����������
// --------------------------
EError(void);
EError(const EError &copy);
virtual ~EError(void) throw();
// --------------------------

};

/* �������������� (�������� �� ������������� ������������� ��������) */
struct RDK_LIB_TYPE EWarning: public UException
{
// --------------------------
// ������������ � �����������
// --------------------------
EWarning(void);
EWarning(const EWarning &copy);
virtual ~EWarning(void) throw();
// --------------------------

};

/* �������������� ���������, ������ ������� ������������ ������������� */
struct RDK_LIB_TYPE EInfo: public UException
{
// --------------------------
// ������������ � �����������
// --------------------------
EInfo(void);
EInfo(const EInfo &copy);
virtual ~EInfo(void) throw();
// --------------------------
};

/* ���������� ���������, ������ ������� ������������ ������������� */
struct RDK_LIB_TYPE EDebug: public UException
{
// --------------------------
// ������������ � �����������
// --------------------------
EDebug(void);
EDebug(const EDebug &copy);
virtual ~EDebug(void) throw();
// --------------------------
};

/* ������ �������������� ������ � ����� */
struct RDK_LIB_TYPE EStrToNumber: public EError
{
std::string Str; // ������

// --------------------------
// ������������ � �����������
// --------------------------
EStrToNumber(const std::string &str);
virtual ~EStrToNumber(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ����������, ��������� � ����������������
struct RDK_LIB_TYPE EIdError: public EError
{
// �������������, ��������� ����������
int Id;

// --------------------------
// ������������ � �����������
// --------------------------
EIdError(int id);
virtual ~EIdError(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ����������, ��������� � �������
struct RDK_LIB_TYPE ENameError: public EError
{
// �������������, ��������� ����������
std::string Name;

// --------------------------
// ������������ � �����������
// --------------------------
ENameError(const std::string &name);
virtual ~ENameError(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


// Id �� ������
struct RDK_LIB_TYPE EIdNotExist: public EIdError
{
EIdNotExist(int id) : EIdError(id) {};
};

// ��� �� �������
struct RDK_LIB_TYPE ENameNotExist: public ENameError
{
ENameNotExist(const std::string &name) : ENameError(name) {};
};

// Id ��� ����������
struct RDK_LIB_TYPE EIdAlreadyExist: public EIdError
{
EIdAlreadyExist(int id) : EIdError(id) {};
};

// ��� ��� ����������
struct RDK_LIB_TYPE ENameAlreadyExist: public ENameError
{
ENameAlreadyExist(const std::string &name) : ENameError(name) {};
};

// Id �� ��������� (forbidden id)
struct RDK_LIB_TYPE EForbiddenId: public EIdError
{
EForbiddenId(int id) : EIdError(id) {};
};

// Id �� ���������
struct RDK_LIB_TYPE EInvalidId: public EIdError
{
EInvalidId(int id) : EIdError(id) {};
};

// ����������, ��������� � ���������
struct RDK_LIB_TYPE EIndexError: public EError
{
// ������, ��������� ����������
int Index;

// --------------------------
// ������������ � �����������
// --------------------------
EIndexError(int index);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Id �� ���������
struct RDK_LIB_TYPE EInvalidIndex: public EIndexError
{
EInvalidIndex(int index) : EIndexError(index) {};
};


struct RDK_LIB_TYPE ESystemException: public EFatal
{
// �������������� ���������� �� ����������
std::string Info;

// --------------------------
// ������������ � �����������
// --------------------------
ESystemException(void);
ESystemException(const std::string &info);
ESystemException(const ESystemException &copy);
virtual ~ESystemException(void) throw();
// --------------------------


// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------

};

// ����������, ��������� � ���������� ������������ �������� �������
struct RDK_LIB_TYPE EFunctionReturnFalse: public EError
{
// ��� �����
std::string FileName;

// ��� ������� � ������� ��������� ������
std::string FunctionName;

// ������ � ���������
int Line;

// --------------------------
// ������������ � �����������
// --------------------------
EFunctionReturnFalse(const std::string &file_name, const std::string &function_name, int line);
virtual ~EFunctionReturnFalse(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ����������, ��������� � ���������� ������������ �������� �������
struct RDK_LIB_TYPE EFunctionReturnError: public EError
{
// ��� �����
std::string FileName;

// ��� ������� � ������� ��������� ������
std::string FunctionName;

// ������ � ���������
int Line;

// ��� ������
int Code;

// --------------------------
// ������������ � �����������
// --------------------------
EFunctionReturnError(const std::string &file_name, const std::string &function_name, int line, int code);
virtual ~EFunctionReturnError(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ���������� � ������� ������� ������ ��� ��������� ������
struct RDK_LIB_TYPE EStringFatal: public EFatal
{
std::string Str;

// --------------------------
// ������������ � �����������
// --------------------------
EStringFatal(const std::string &str);
virtual ~EStringFatal(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ���������� � ������� ������� ������ ��� ������
struct RDK_LIB_TYPE EStringError: public EError
{
std::string Str;

// --------------------------
// ������������ � �����������
// --------------------------
EStringError(const std::string &str);
virtual ~EStringError(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ���������� � ������� ������� ������ ��� ��������������
struct RDK_LIB_TYPE EStringWarning: public EWarning
{
std::string Str;

// --------------------------
// ������������ � �����������
// --------------------------
EStringWarning(const std::string &str);
virtual ~EStringWarning(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ���������� � ������� ������� ������ ��� �������������� ���������
struct RDK_LIB_TYPE EStringInfo: public EInfo
{
std::string Str;

// --------------------------
// ������������ � �����������
// --------------------------
EStringInfo(const std::string &str);
virtual ~EStringInfo(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ���������� � ������� ������� ������ ��� �������������� ���������
struct RDK_LIB_TYPE EStringDebug: public EDebug
{
std::string Str;

// --------------------------
// ������������ � �����������
// --------------------------
EStringDebug(const std::string &str);
virtual ~EStringDebug(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ����������, ������� ���������� ����������
struct RDK_LIB_TYPE UExceptionUnhandled: public EFatal
{
// ��� �����
std::string FileName;

// ��� ������� � ������� ��������� ������
std::string FunctionName;

// ������ � ���������
int Line;

// --------------------------
// ������������ � �����������
// --------------------------
UExceptionUnhandled(const std::string &file_name, int line, const std::string &function_name);
virtual ~UExceptionUnhandled(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


}

#endif

