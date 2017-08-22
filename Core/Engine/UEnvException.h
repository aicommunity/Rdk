#ifndef UEnv_Exception_h
#define UEnv_Exception_h

#include "../Utilities/UException.h"
#include "UContainer.h"

namespace RDK {

class UExceptionWrapperStd: public UException
{
protected:
std::string LogMessage;

public:
explicit UExceptionWrapperStd(const std::exception &exception);
virtual ~UExceptionWrapperStd(void) throw();

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string CreateLogMessage(void) const;
// --------------------------
};

class UExceptionWrapperSEH: public UException
{
protected:
std::string LogMessage;

public:
explicit UExceptionWrapperSEH(const std::string &seh_info);
virtual ~UExceptionWrapperSEH(void) throw();

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string CreateLogMessage(void) const;
// --------------------------
};

class UExceptionWrapperBcb: public UException
{
protected:
std::string LogMessage;

public:
explicit UExceptionWrapperBcb(const std::string &seh_info);
virtual ~UExceptionWrapperBcb(void) throw();

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string CreateLogMessage(void) const;
// --------------------------
};

class UExceptionWrapperBoost: public UException
{
protected:
std::string LogMessage;

public:
explicit UExceptionWrapperBoost(const std::string &seh_info);
virtual ~UExceptionWrapperBoost(void) throw();

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string CreateLogMessage(void) const;
// --------------------------
};

class UExceptionWrapperOpenCv: public UException
{
protected:
std::string LogMessage;

public:
explicit UExceptionWrapperOpenCv(const std::string &seh_info);
virtual ~UExceptionWrapperOpenCv(void) throw();

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string CreateLogMessage(void) const;
// --------------------------
};

template<typename T>
void rdk_throw(T exception, const char* file_name, int line_number)
{
 exception.SetExFileName(file_name);
 exception.SetExLineNumber(line_number);
 throw exception;
}

template<typename T>
void rdk_throw(T exception, const char* file_name, int line_number, const char* component_name)
{
 if(file_name)
  exception.SetExFileName(file_name);
 exception.SetExLineNumber(line_number);

 if(component_name)
  exception.SetObjectName(component_name);

 throw exception;
}

#define RDK_THROW(e) rdk_throw(e,__FILE__,__LINE__,this->GetFullName().c_str())
#define RDK_RAW_THROW(e) rdk_throw(e,__FILE__,__LINE__)


}

#endif

