#ifndef UEXCEPTION_H
#define UEXCEPTION_H

#include <ctime>
#include <string>

namespace RDK {

/* ������� ����� ���������� */
class UException
{
protected: // ����� ������
// ��������� ���������� ����� ����������
//static long long LastNumber;

protected: // ������ ����������
// ���������� ����� ����������
//long long Number;

// ��� ����������
// 0 - ������������
// 1 - ���������
// 2 - ������, ��������� �������������
// 3 - ��������������
// 4 - ����������
int Type;

// ����� ������������� ����������
std::time_t Time;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UException(void);
UException(const UException &copy);
virtual ~UException(void);
// --------------------------

// --------------------------
// ������ ���������� ������ �������
// --------------------------
// ��������� ���������� ����� ����������
//static long long GetLastNumber(void);
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
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


/* ��������� ������ (��������� �� 0 ���������� � �.�.) */
struct EFatal: public UException
{
// --------------------------
// ������������ � �����������
// --------------------------
EFatal(void);
EFatal(const EFatal &copy);
virtual ~EFatal(void);
// --------------------------

};

/* ������, �������������� ������������� */
struct EError: public UException
{
// --------------------------
// ������������ � �����������
// --------------------------
EError(void);
EError(const EError &copy);
virtual ~EError(void);
// --------------------------

};

/* �������������� (�������� �� ������������� ������������� ��������) */
struct EWarning: public UException
{
// --------------------------
// ������������ � �����������
// --------------------------
EWarning(void);
EWarning(const EWarning &copy);
virtual ~EWarning(void);
// --------------------------

};

/* �������������� ���������, ������ ������� ������������ ������������� */
struct EInfo: public UException
{
// --------------------------
// ������������ � �����������
// --------------------------
EInfo(void);
EInfo(const EInfo &copy);
virtual ~EInfo(void);
// --------------------------
};

/* ������ �������������� ������ � ����� */
struct EStrToNumber: public EError
{
std::string Str; // ������

// --------------------------
// ������������ � �����������
// --------------------------
EStrToNumber(const std::string &str);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ����������, ��������� � ����������������
struct EIdError: public EError
{
// �������������, ��������� ����������
int Id;

// --------------------------
// ������������ � �����������
// --------------------------
EIdError(int id);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ����������, ��������� � �������
struct ENameError: public EError
{
// �������������, ��������� ����������
std::string Name;

// --------------------------
// ������������ � �����������
// --------------------------
ENameError(const std::string &name);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


// Id �� ������
struct EIdNotExist: public EIdError
{
EIdNotExist(int id) : EIdError(id) {};
};

// ��� �� �������
struct ENameNotExist: public ENameError
{
ENameNotExist(const std::string &name) : ENameError(name) {};
};

// Id ��� ����������
struct EIdAlreadyExist: public EIdError
{
EIdAlreadyExist(int id) : EIdError(id) {};
};

// ��� ��� ����������
struct ENameAlreadyExist: public ENameError
{
ENameAlreadyExist(const std::string &name) : ENameError(name) {};
};

// Id �� ��������� (forbidden id)
struct EForbiddenId: public EIdError
{
EForbiddenId(int id) : EIdError(id) {};
};

// Id �� ���������
struct EInvalidId: public EIdError
{
EInvalidId(int id) : EIdError(id) {};
};

// ����������, ��������� � ���������
struct EIndexError: public EError
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
struct EInvalidIndex: public EIndexError
{
EInvalidIndex(int index) : EIndexError(index) {};
};


struct ESystemException: public EFatal
{
// �������������� ���������� �� ����������
std::string Info;

// --------------------------
// ������������ � �����������
// --------------------------
ESystemException(void);
ESystemException(const std::string &info);
ESystemException(const ESystemException &copy);
virtual ~ESystemException(void);
// --------------------------


// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------

};

// ����������, ��������� � ���������� ������������ �������� �������
struct EFunctionReturnFalse: public EError
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
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ����������, ��������� � ���������� ������������ �������� �������
struct EFunctionReturnError: public EError
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

