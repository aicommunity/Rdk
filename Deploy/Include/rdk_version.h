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
/// Мажорная версия ядра
int Major;

/// Минорная версия ядра
int Minor;

/// Ревизия CVS
int Revision;

/// Версия boost
std::string Boost;

/// Версия OpenCv;
std::string OpenCv;

/// Имя компилятора
std::string CompilerName;

/// Версия компилятора
std::string CompilerVersion;

public: // Методы
/// --------------------------
/// Конструкторы и деструкторы
/// --------------------------
UVersion(void);
UVersion(const UVersion &copy);
UVersion(int major, int minor, int revision);
UVersion(int major, int minor, int revision, const std::string &boost, const std::string &opencv);
/// --------------------------

/// --------------------------
/// Вывод и сравнение
/// --------------------------
/// Преобразует версию в строку
std::string ToString(void) const;

/// Преобразует версию в строку со всеми зависимостями
std::string ToStringFull(void) const;

/// Сравнивает версию с другой (только основную версию)
/// возвращает >0 если версия ядра больше,
/// возвращает <0 если версия ядра меньше,
/// возвращает 0 в случае совпадения.
int CompareCore(const UVersion &ver) const;
int CompareCore(int major, int minor, int revision) const;

/// Сравнивает версию с другой (вместе с зависимостями)
/// возвращает >0 если версия ядра больше,
/// возвращает <0 если версия ядра меньше,
/// возвращает 0 в случае совпадения.
//int CompareFull(const UVersion &ver);
//int CompareFull(int major, int minor, int revision, const std::string &boost, const std::string &opencv);

/// Сравнивает версию с другой (только основную версию) на полное совпадение
bool IsEqualCore(const UVersion &ver) const;
bool IsEqualCore(int major, int minor, int revision) const;

/// Сравнивает версию с другой (вместе с зависимостями) на полное совпадение
bool IsEqualFull(const UVersion &ver);
bool IsEqualFull(int major, int minor, int revision, const std::string &boost, const std::string &opencv,
                 const std::string &compiler_name, const std::string &compiler_version);
/// --------------------------
};


// ----------------------------
// Функции доступа к отдельным полям определения версий
// ----------------------------
/// Доступ к глобальной переменной версии
const RDK::UVersion& GetGlobalVersion(void);

/// Возвращает мажорную версию ядра
int CoreMajorVer(void);

/// Возвращает минорную версию ядра
int CoreMinorVer(void);

/// Возвращает версию патча ядра
int CoreRevisionVer(void);

/// Возвращает полную версию ядра в виде строки
const char* CoreVer(void);

/// Сравнивает версию ядра с переданной
/// возвращает >0 если версия ядра больше,
/// возвращает <0 если версия ядра меньше,
/// возвращает 0 в случае совпадения.
int CoreCompareVer(int major, int minor, int patch);

/// Возвращает имя компилятора ядра
const char* CompilerName(void);

/// Возвращает версию компилятора ядра
const char* CompilerVersion(void);

/// Возвращает версию boost
const char* BoostVersion(void);

/// Возвращает версию opencv (если используется)
const char* OpenCvVersion(void);
// ----------------------------

}
#endif
