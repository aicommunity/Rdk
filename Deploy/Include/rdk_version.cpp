#include <sstream>
#include "rdk_version.h"

namespace RDK {

RDK::UVersion GlobalVersion;

/// --------------------------
///  онструкторы и деструкторы
/// --------------------------
UVersion::UVersion(void)
 : Major(CoreMajorVer()), Minor(CoreMinorVer()), Revision(CoreRevisionVer()),
   Boost(BoostVersion()), OpenCv(OpenCvVersion()),
   CompilerName(RDK::CompilerName()), CompilerVersion(RDK::CompilerVersion())
{

}

UVersion::UVersion(const UVersion &copy)
 : Major(copy.Major), Minor(copy.Minor), Revision(copy.Revision),
   Boost(copy.Boost), OpenCv(copy.OpenCv),
   CompilerName(copy.CompilerName), CompilerVersion(copy.CompilerVersion)
{

}

UVersion::UVersion(int major, int minor, int revision)
    : Major(major), Minor(minor), Revision(revision)
{

}

UVersion::UVersion(int major, int minor, int revision, const std::string &boost, const std::string &opencv)
    : Major(major), Minor(minor), Revision(revision),
      Boost(boost), OpenCv(opencv)
{

}
/// --------------------------

/// --------------------------
/// ¬ывод и сравнение
/// --------------------------
/// ѕреобразует версию в строку
std::string UVersion::ToString(void) const
{
 std::stringstream tempstr;
 tempstr<<Major<<"."<<Minor<<" ("<<Revision<<")";
 return tempstr.str();
}

/// ѕреобразует версию в строку со всеми зависимост€ми
std::string UVersion::ToStringFull(void) const
{
 std::stringstream tempstr;
 tempstr<<Major<<"."<<Minor<<" ("<<Revision<<") boost="<<Boost;
 if(!OpenCv.empty())
  tempstr<<" opencv="<<OpenCv;
 tempstr<<" Compiler: "<<CompilerName<<" ("<<CompilerVersion<<")";
 return tempstr.str();
}

/// —равнивает версию с другой (только основную версию)
/// возвращает >0 если верси€ €дра больше,
/// возвращает <0 если верси€ €дра меньше,
/// возвращает 0 в случае совпадени€.
int UVersion::CompareCore(const UVersion &ver) const
{
 return CompareCore(ver.Major,ver.Minor, ver.Revision);
}

int UVersion::CompareCore(int major, int minor, int revision) const
{
 if(Major < major)
  return -1;
 if(Major > major)
  return 1;
 if(Major == major)
 {
  if(Minor < minor)
   return -1;
  if(Minor > minor)
   return 1;
  if(Minor == minor)
  {
   if(Revision < revision)
    return -1;
   if(Revision > revision)
    return 1;
  }
 }
 return 0;
}
/*
/// —равнивает версию с другой (вместе с зависимост€ми)
/// возвращает >0 если верси€ €дра больше,
/// возвращает <0 если верси€ €дра меньше,
/// возвращает 0 в случае совпадени€.
int UVersion::CompareFull(const UVersion &ver)
{
 int res=CompareCore(ver);
 if(!res)
  return res;

 if(Boost < ver.Boost)
  return -1;

 if(Boost > ver.Boost)
  return 1;

 if(Boost == ver.Boost)
 {
  if(OpenCv < ver.OpenCv)
   return -1;

  if(OpenCv > ver.OpenCv)
   return 1;
 }

 if(Boost == ver.Boost)
 {
  if(OpenCv < ver.OpenCv)
   return -1;

  if(OpenCv > ver.OpenCv)
   return 1;
 }
}

int UVersion::CompareFull(int major, int minor, int revision, const std::string &boost, const std::string &opencv)
{

}*/

/// —равнивает версию с другой (только основную версию) на полное совпадение
bool UVersion::IsEqualCore(const UVersion &ver) const
{
 return IsEqualCore(ver.Major,ver.Minor,ver.Revision);
}

bool UVersion::IsEqualCore(int major, int minor, int revision) const
{
 return !((Major != major) | (Minor != minor) | (Revision != revision));
}

/// —равнивает версию с другой (вместе с зависимост€ми) на полное совпадение
bool UVersion::IsEqualFull(const UVersion &ver)
{
 return IsEqualFull(ver.Major,ver.Minor,ver.Revision, ver.Boost, ver.OpenCv, ver.CompilerName, ver.CompilerVersion);
}

bool UVersion::IsEqualFull(int major, int minor, int revision, const std::string &boost, const std::string &opencv,
                           const std::string &compiler_name, const std::string &compiler_version)
{
 return !(!IsEqualCore(major, minor, revision) | (Boost != boost) | (OpenCv != opencv) | (CompilerName != compiler_name) | (CompilerVersion != compiler_version));
}
/// --------------------------


// ----------------------------
// ‘ункции определени€ версий
// ----------------------------
/// ƒоступ к глобальной переменной версии
const RDK::UVersion& GetGlobalVersion(void)
{
 return GlobalVersion;
}

/// ¬озвращает мажорную версию €дра
int CoreMajorVer(void)
{
 return RDK_MAJOR_VERSION;
}

/// ¬озвращает минорную версию €дра
int CoreMinorVer(void)
{
 return RDK_MINOR_VERSION;
}

/// ¬озвращает версию патча €дра
int CoreRevisionVer(void)
{
 return RDK_CORE_CVS_REVISION;
}

/// ¬озвращает полную вер сию €дра в виде строки
const char* CoreVer(void)
{
 static char version[100];
 std::stringstream tempstr;
 tempstr<<CoreMajorVer()<<"."<<CoreMinorVer()<<" ("<<CoreRevisionVer()<<")";
 strcpy_s(version,tempstr.str().c_str());
 return version;
}

/// —равнивает версию €дра с переданной
/// возвращает >0 если верси€ €дра больше,
/// возвращает <0 если верси€ €дра меньше,
/// возвращает 0 в случае совпадени€.
int CoreCompareVer(int major, int minor, int patch)
{
 if(CoreMajorVer() < major)
  return -1;
 if(CoreMajorVer() > major)
  return 1;
 if(CoreMajorVer() == major)
 {
  if(CoreMinorVer() < minor)
   return -1;
  if(CoreMinorVer() > minor)
   return 1;
  if(CoreMinorVer() == minor)
  {
   if(CoreRevisionVer() < patch)
    return -1;
   if(CoreRevisionVer() > patch)
    return 1;
  }
 }
 return 0;
}

/// ¬озвращает им€ компил€тора €дра
const char* CompilerName(void)
{
#if defined(__clang__)
 return "CLANG";
#elif defined(__ICC) || defined(__INTEL_COMPILER)
 return "Intel ICC/ICPC";

#elif defined(__GNUC__) || defined(__GNUG__)
 return "GNU GCC/G++";

#elif defined(__HP_cc) || defined(__HP_aCC)
 return "HP C/aCC";

#elif defined(__IBMC__) || defined(__IBMCPP__)
 return "IBM XL C/C++";

#elif defined(_MSC_VER)
  return "MSVC";

#elif defined(__PGI)
  return "Portland Group PGCC/PGCPP";

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  return "Oracle Solaris Studio";

#elif defined(__BORLANDC__)
  return "Borland C++ Builder";

#endif
}

/// ¬озвращает версию компил€тора €дра
const char* CompilerVersion(void)
{
#if defined(__clang__)
 return __clang_version__;
#elif defined(__ICC) || defined(__INTEL_COMPILER)
 return __VERSION__;

#elif defined(__GNUC__) || defined(__GNUG__)
 return __VERSION__;

#elif defined(__HP_cc) || defined(__HP_aCC)
 return __HP_aCC;

#elif defined(__IBMC__) || defined(__IBMCPP__)
 return __xlc__;

#elif defined(_MSC_VER)
    static char version[100];
    std::stringstream tempstr;
    tempstr<<_MSC_FULL_VER;
    strcpy(version,tempstr.str().c_str());
    return version;

#elif defined(__PGI)
  return __PGIC__; // Only major build. __PGIC_MINOR, and __PGIC_PATCHLEVEL__  don't supported here

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  return __SUNPRO_CC;

#elif defined(__BORLANDC__)
    static char version[100];
    std::stringstream tempstr;
    tempstr<<__BORLANDC__;
    strcpy(version,tempstr.str().c_str());
    return version;

#endif
}

/// ¬озвращает версию opencv (если используетс€)
const char* OpenCvVersion(void)
{
#ifdef CV_VERSION
 return CV_VERSION;
#else
 return "";
#endif
}
// ----------------------------

}
