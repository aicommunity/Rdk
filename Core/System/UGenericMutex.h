// ---------------------------------------------------------------------------

#ifndef GenericMutexH
#define GenericMutexH

#include "../../Deploy/Include/initdll_defs.h"
#include <map>
#include <string>

#define RDK_MUTEX_TIMEOUT 0xFFFFFFFF

#ifdef RDK_MUTEX_DEADLOCK_DEBUG
struct RDK_LIB_TYPE TUThreadInfo
{
std::string Name;

int Pid;
};

extern RDK_LIB_TYPE std::map<int,TUThreadInfo> GlobalThreadInfoMap;
#endif

/**
 * @brief Базовый класс для реализации мьютексов с поддержкой shared/exclusive блокировок
 * 
 * Предоставляет интерфейс для синхронизации доступа к разделяемым ресурсам.
 * Поддерживает два типа блокировок:
 * - Shared lock: множественные потоки могут одновременно читать
 * - Exclusive lock: только один поток может писать
 */
class RDK_LIB_TYPE UGenericMutex
{
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
public:
int DebugId;
#endif

protected:


public:
UGenericMutex() noexcept;
virtual ~UGenericMutex() noexcept;

/**
 * @brief Блокирует мьютекс для чтения (shared lock)
 * @param timeout Таймаут в миллисекундах. RDK_MUTEX_TIMEOUT для бесконечного ожидания
 * @return true если блокировка успешна, false при таймауте или ошибке
 */
virtual bool shared_lock(unsigned timeout=RDK_MUTEX_TIMEOUT)=0;

/**
 * @brief Разблокирует мьютекс после shared lock
 * @return true если разблокировка успешна, false при ошибке
 */
virtual bool shared_unlock() noexcept=0;

/**
 * @brief Блокирует мьютекс для записи (exclusive lock)
 * @param timeout Таймаут в миллисекундах. RDK_MUTEX_TIMEOUT для бесконечного ожидания
 * @return true если блокировка успешна, false при таймауте или ошибке
 */
virtual bool exclusive_lock(unsigned timeout=RDK_MUTEX_TIMEOUT)=0;

/**
 * @brief Разблокирует мьютекс после exclusive lock
 * @return true если разблокировка успешна, false при ошибке
 */
virtual bool exclusive_unlock() noexcept=0;
};

/**
 * @brief Базовый класс для реализации событий (events)
 * 
 * Предоставляет интерфейс для синхронизации потоков через события.
 */
class RDK_LIB_TYPE UGenericEvent
{
protected:

public:
UGenericEvent() noexcept;
virtual ~UGenericEvent() noexcept;

/**
 * @brief Устанавливает событие в сигнальное состояние
 * @return true если операция успешна
 */
virtual bool set() noexcept=0;

/**
 * @brief Сбрасывает событие в несигнальное состояние
 * @return true если операция успешна
 */
virtual bool reset() noexcept=0;

/**
 * @brief Ожидает установки события
 * @param wait_time Таймаут ожидания в миллисекундах
 * @return true если событие установлено, false при таймауте
 */
virtual bool wait(unsigned wait_time)=0;
};

RDK_LIB_TYPE UGenericMutex* UCreateMutex(void);
RDK_LIB_TYPE void UDestroyMutex(UGenericMutex* mutex);

RDK_LIB_TYPE UGenericEvent* UCreateEvent(void);
RDK_LIB_TYPE UGenericEvent* UCreateEvent(bool initial_state);
RDK_LIB_TYPE void UDestroyEvent(UGenericEvent* event);

// ---------------------------------------------------------------------------
/**
 * @brief RAII класс для автоматической блокировки/разблокировки мьютекса в exclusive режиме
 * 
 * Гарантирует разблокировку мьютекса при выходе из области видимости.
 * Копирование запрещено.
 */
class RDK_LIB_TYPE UGenericMutexExclusiveLocker
{
private:
UGenericMutex *m_mutex;
bool m_locked;

public:
/**
 * @brief Конструктор - блокирует мьютекс в exclusive режиме
 * @param m Указатель на мьютекс. Может быть nullptr (в этом случае блокировка не выполняется)
 * @throws Может выбросить исключение при неудачной блокировке
 */
explicit UGenericMutexExclusiveLocker(UGenericMutex *m);

/**
 * @brief Деструктор - автоматически разблокирует мьютекс
 */
~UGenericMutexExclusiveLocker() noexcept;

/**
 * @brief Проверяет, успешно ли была выполнена блокировка
 * @return true если мьютекс заблокирован, false если блокировка не удалась или m был nullptr
 */
bool is_locked() const noexcept { return m_locked; }

private:
// Запрещаем копирование и присваивание
UGenericMutexExclusiveLocker(const UGenericMutexExclusiveLocker &m) = delete;
UGenericMutexExclusiveLocker& operator = (const UGenericMutexExclusiveLocker &m) = delete;
};

/**
 * @brief RAII класс для автоматической блокировки/разблокировки мьютекса в shared режиме
 * 
 * Гарантирует разблокировку мьютекса при выходе из области видимости.
 * Копирование запрещено.
 */
class RDK_LIB_TYPE UGenericMutexSharedLocker
{
private:
UGenericMutex *m_mutex;
bool m_locked;

public:
/**
 * @brief Конструктор - блокирует мьютекс в shared режиме
 * @param m Указатель на мьютекс. Может быть nullptr (в этом случае блокировка не выполняется)
 * @throws Может выбросить исключение при неудачной блокировке
 */
explicit UGenericMutexSharedLocker(UGenericMutex *m);

/**
 * @brief Деструктор - автоматически разблокирует мьютекс
 */
~UGenericMutexSharedLocker() noexcept;

/**
 * @brief Проверяет, успешно ли была выполнена блокировка
 * @return true если мьютекс заблокирован, false если блокировка не удалась или m был nullptr
 */
bool is_locked() const noexcept { return m_locked; }

private:
// Запрещаем копирование и присваивание
UGenericMutexSharedLocker(const UGenericMutexSharedLocker &m) = delete;
UGenericMutexSharedLocker& operator = (const UGenericMutexSharedLocker &m) = delete;
};
// ---------------------------------------------------------------------------

#endif
