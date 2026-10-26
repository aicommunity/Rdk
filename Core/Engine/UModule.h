/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UMODULE_H
#define UMODULE_H

#include "../../Deploy/Include/initdll_defs.h"
#include <cstdint>
#include <memory>

namespace RDK {

class UModule;
/// Process-local observation of native lifecycle, independent of parameters and
/// component storage. It does not own the component or represent a checkpoint.
class RDK_LIB_TYPE UModuleLifecycleObservation
{
public:
 UModuleLifecycleObservation()=default;
 bool IsAlive() const;
 bool IsCurrent() const;
private:
 struct State;
 std::shared_ptr<State> ObservedState;
 std::uint64_t Revision=0;
 UModuleLifecycleObservation(std::shared_ptr<State> state,std::uint64_t revision);
 friend class UModule;
};

class RDK_LIB_TYPE UModule
{
protected: // Флаги основных состояний
/// Флаг готовности объекта к счету
/// Если false, то перед итерацией счета будет вызван метод build
bool Ready;

/// Флаг инициализации объекта
/// если false - объект будет проинициализирован
bool InitFlag;

private:
 std::shared_ptr<UModuleLifecycleObservation::State> LifecycleState;
protected:
 /// For native overrides that enter lifecycle directly, without before-hooks.
 void AdvanceLifecycleObservation();

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UModule(void);
UModule(const UModule& source);
UModule& operator=(const UModule& source);
virtual ~UModule(void);

/// Invalidated before native Default/Build/Reset/Init/UnInit hooks, or on C++
/// assignment/destruction. Calculate and no-op Build/Init do not invalidate it.
/// Never serialized; copying a module creates an independent identity.
UModuleLifecycleObservation ObserveLifecycle() const;
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
/// Флаг готовности объекта к счету
inline bool IsReady(void) const
{
 return Ready;
}
/// Флаг готовности объекта к начальной инициализации
inline bool IsInit(void) const
{
 return InitFlag;
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
/// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool BeforeDefault(void);
virtual bool AfterDefault(void);
virtual bool Default(void);

/// Обеспечивает сборку внутренней структуры объекта
/// после настройки параметров
/// Автоматически вызывает метод Reset() и выставляет Ready в true
/// в случае успешной сборки
virtual bool BeforeBuild(void);
virtual bool AfterBuild(void);
virtual bool Build(void);

/// Сброс процесса счета без потери настроек
virtual bool BeforeReset(void);
virtual bool AfterReset(void);
virtual bool Reset(void);

/// Выполняет расчет этого объекта
virtual bool BeforeCalculate(void);
virtual bool AfterCalculate(void);
virtual bool Calculate(void);

/// Выполняет инициализацию этого объекта
virtual void BeforeInit(void);
virtual void AfterInit(void);
virtual void Init(void);

/// Выполняет деинициализацию этого объекта
virtual void BeforeUnInit(void);
virtual void AfterUnInit(void);
virtual void UnInit(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// Реализуются в конечных модулях
// --------------------------
protected:
/// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ABeforeDefault(void);
virtual bool AAfterDefault(void);
virtual bool ADefault(void);

/// Обеспечивает сборку внутренней структуры объекта
/// после настройки параметров
/// Автоматически вызывает метод Reset() и выставляет Ready в true
/// в случае успешной сборки
virtual bool ABeforeBuild(void);
virtual bool AAfterBuild(void);
virtual bool ABuild(void);

/// Сброс процесса счета.
virtual bool ABeforeReset(void);
virtual bool AAfterReset(void);
virtual bool AReset(void);

/// Выполняет расчет этого объекта
virtual bool ABeforeCalculate(void);
virtual bool AAfterCalculate(void);
virtual bool ACalculate(void);

/// Выполняет инициализацию этого объекта
virtual void ABeforeInit(void);
virtual void AAfterInit(void);
virtual void AInit(void);

/// Выполняет деинициализацию этого объекта
virtual void ABeforeUnInit(void);
virtual void AAfterUnInit(void);
virtual void AUnInit(void);
// --------------------------
};

}

#endif

