#include "UEnvException.h"

namespace RDK {

UExceptionWrapperStd::UExceptionWrapperStd(const std::exception &exception)
{
 Type=1;
 LogMessage=typeid(exception).name()+std::string(" ")+exception.what();
}

UExceptionWrapperStd::~UExceptionWrapperStd(void) throw()
{
}

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UExceptionWrapperStd::CreateLogMessage(void) const
{
 return UException::CreateLogMessage()+LogMessage;
}
// --------------------------

UExceptionWrapperSEH::UExceptionWrapperSEH(const std::string &seh_info)
{
 Type=1;
 LogMessage=seh_info;
}

UExceptionWrapperSEH::~UExceptionWrapperSEH(void) throw()
{
}

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UExceptionWrapperSEH::CreateLogMessage(void) const
{
 return UException::CreateLogMessage()+LogMessage;
}
// --------------------------

UExceptionWrapperBcb::UExceptionWrapperBcb(const std::string &seh_info)
{
 Type=1;
 LogMessage=seh_info;
}

UExceptionWrapperBcb::~UExceptionWrapperBcb(void) throw()
{
}


// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UExceptionWrapperBcb::CreateLogMessage(void) const
{
 return UException::CreateLogMessage()+LogMessage;
}
// --------------------------

UExceptionWrapperBoost::UExceptionWrapperBoost(const std::string &seh_info)
{
 Type=1;
 LogMessage=seh_info;
}

UExceptionWrapperBoost::~UExceptionWrapperBoost(void) throw()
{
}


// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UExceptionWrapperBoost::CreateLogMessage(void) const
{
 return UException::CreateLogMessage()+LogMessage;
}
// --------------------------

UExceptionWrapperOpenCv::UExceptionWrapperOpenCv(const std::string &seh_info)
{
 Type=1;
 LogMessage=seh_info;
}

UExceptionWrapperOpenCv::~UExceptionWrapperOpenCv(void) throw()
{
}

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UExceptionWrapperOpenCv::CreateLogMessage(void) const
{
 return UException::CreateLogMessage()+LogMessage;
}
// --------------------------
			 /*
#ifdef BOOST_VERSION
UContExceptionWrapper::UContExceptionWrapper(const boost::exception &exception, UContainer* cont, const std::string &file_name, int line_number)
 : FileName(file_name), LineNumber(line_number)
{
 Type=1;
 LogMessage=UException::CreateLogMessage()+std::string(" ");
 LogMessage+=typeid(exception).name()+std::string(" ")+exception.what()+std::string("| Src: ")+FileName+std::string(":")+sntoa(LineNumber);
 if(cont)
 {
  std::string buf;
  LogMessage+=std::string("> ")+cont->GetFullName(buf);
 }
}
#endif
		 */


}
