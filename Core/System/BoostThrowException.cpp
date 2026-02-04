#include <boost/throw_exception.hpp>
#include <cstdlib>

// Реализация обработчиков исключений Boost для конфигураций без исключений.
// В новых версиях Boost (через vcpkg) при отключённых исключениях (BOOST_NO_EXCEPTIONS)
// заголовок boost/throw_exception.hpp объявляет, но НЕ определяет функции
//   BOOST_NORETURN void throw_exception(std::exception const&);
//   BOOST_NORETURN void throw_exception(std::exception const&, boost::source_location const&);
// Ожидается, что пользователь предоставит эти реализации.
//
// Здесь мы даём минимальную реализацию, которая аварийно завершает процесс.

namespace boost {

BOOST_NORETURN void throw_exception(std::exception const& e)
{
    (void)e;
    std::terminate();
}

BOOST_NORETURN void throw_exception(std::exception const& e, boost::source_location const& loc)
{
    (void)e;
    (void)loc;
    std::terminate();
}

} // namespace boost


