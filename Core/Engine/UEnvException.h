#ifndef UEnv_Exception_h
#define UEnv_Exception_h

#include "../Utilities/UException.h"
#include "UContainer.h"

namespace RDK {

class UContExceptionWrapper: public UException
{
public:
std::string LogMessage;

std::string FileName;

int LineNumber;

UContExceptionWrapper(const UException &exception, UContainer* cont, const std::string &file_name, int line_number);
UContExceptionWrapper(const std::exception &exception, UContainer* cont, const std::string &file_name, int line_number);
/*
#ifdef BOOST_VERSION
UContExceptionWrapper(const boost::exception &exception, UContainer* cont, const std::string &file_name, int line_number);
#endif
*/
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


}

#endif

