#include "UEnvException.h"

namespace RDK {

UContExceptionWrapper::UContExceptionWrapper(const UException &exception, UContainer* cont, const std::string &file_name, int line_number)
 : FileName(file_name), LineNumber(line_number)
{
 Type=exception.GetType();
 Time=exception.GetTime();
 LogMessage=exception.CreateLogMessage()+std::string("| Src: ")+FileName+std::string(":")+sntoa(LineNumber);
 LogMessage+=std::string("| Src: ")+FileName+std::string(":")+sntoa(LineNumber);
 if(cont)
 {
  std::string buf;
  LogMessage+=std::string("> ")+cont->GetFullName(buf);
 }
}

UContExceptionWrapper::UContExceptionWrapper(const std::exception &exception, UContainer* cont, const std::string &file_name, int line_number)
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

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UContExceptionWrapper::CreateLogMessage(void) const
{
 return LogMessage;
}
// --------------------------


}
