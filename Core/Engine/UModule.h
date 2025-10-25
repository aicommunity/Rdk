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
#include <memory>
#include <stdexcept>
#include <iostream>

namespace RDK {

// Forward declaration
class UComponent;

class RDK_LIB_TYPE UModule
{
protected: // ����� �������� ���������
/// ���� ���������� ������� � �����
/// ���� false, �� ����� ��������� ����� ����� ������ ����� build
bool Ready;

/// ���� ������������� �������
/// ���� false - ������ ����� ������������������
bool InitFlag;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UModule(void);
virtual ~UModule(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
/// ���� ���������� ������� � �����
bool IsReady(void) const;
/// ���� ���������� ������� � ��������� �������������
bool IsInit(void) const;
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
/// �������������� �������� �� ��������� � ����� �������� �����
virtual bool BeforeDefault(void);
virtual bool AfterDefault(void);
virtual bool Default(void);

/// ������������ ������ ���������� ��������� �������
/// ����� ��������� ����������
/// ������������� �������� ����� Reset() � ���������� Ready � true
/// � ������ �������� ������
virtual bool BeforeBuild(void);
virtual bool AfterBuild(void);
virtual bool Build(void);

/// ����� �������� ����� ��� ������ ��������
virtual bool BeforeReset(void);
virtual bool AfterReset(void);
virtual bool Reset(void);

/// ��������� ������ ����� �������
virtual bool BeforeCalculate(void);
virtual bool AfterCalculate(void);
virtual bool Calculate(void);

/// ��������� ������������� ����� �������
virtual void BeforeInit(void);
virtual void AfterInit(void);
virtual void Init(void);

/// ��������� ��������������� ����� �������
virtual void BeforeUnInit(void);
virtual void AfterUnInit(void);
virtual void UnInit(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// ����������� � �������� �������
// --------------------------
protected:
/// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ABeforeDefault(void);
virtual bool AAfterDefault(void);
virtual bool ADefault(void);

/// ������������ ������ ���������� ��������� �������
/// ����� ��������� ����������
/// ������������� �������� ����� Reset() � ���������� Ready � true
/// � ������ �������� ������
virtual bool ABeforeBuild(void);
virtual bool AAfterBuild(void);
virtual bool ABuild(void);

/// ����� �������� �����.
virtual bool ABeforeReset(void);
virtual bool AAfterReset(void);
virtual bool AReset(void);

/// ��������� ������ ����� �������
virtual bool ABeforeCalculate(void);
virtual bool AAfterCalculate(void);
virtual bool ACalculate(void);

/// ��������� ������������� ����� �������
virtual void ABeforeInit(void);
virtual void AAfterInit(void);
virtual void AInit(void);

/// ��������� ��������������� ����� �������
virtual void ABeforeUnInit(void);
virtual void AAfterUnInit(void);
virtual void AUnInit(void);
// --------------------------
};

// Helper для безопасного получения shared_ptr производного типа
// ТРЕБУЕТ: объект должен быть создан через std::make_shared
template<typename Derived, typename Base>
std::shared_ptr<Derived> safe_shared_cast(Base* obj) {
    // Проверяем, наследуется ли Derived от UComponent
    if constexpr (std::is_base_of<UComponent, Derived>::value) {
        if (auto* component = dynamic_cast<UComponent*>(obj)) {
            try {
                return std::static_pointer_cast<Derived>(
                    component->shared_from_this()
                );
            } catch (const std::bad_weak_ptr&) {
                // КРИТИЧЕСКАЯ ОШИБКА: объект не управляется shared_ptr
                throw std::runtime_error(
                    "FATAL: Object not managed by shared_ptr. "
                    "All UComponent objects must be created via std::make_shared!"
                );
            }
        }
    }
    
    // Проверяем, наследуется ли Derived от UModule (включая UComponent, который наследуется от UModule)
    if constexpr (std::is_base_of<UModule, Derived>::value) {
        if (auto* module = dynamic_cast<UModule*>(obj)) {
            // Для UModule объектов, которые не наследуются от UComponent,
            // мы не можем использовать shared_from_this, поэтому создаем shared_ptr с NonOwningDeleter
            // ВНИМАНИЕ: Это временное решение для обратной совместимости
            return std::shared_ptr<Derived>(static_cast<Derived*>(obj), [](Derived*){});
        }
    }
    
    // Объект не наследуется от UComponent или UModule - создаем shared_ptr с пустым deleter'ом
    return std::shared_ptr<Derived>(static_cast<Derived*>(obj), [](Derived*){});
}

}

#endif

