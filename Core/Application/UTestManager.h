#ifndef UTestManagerH
#define UTestManagerH

#include "../../Deploy/Include/rdk_cpp_initdll.h"

namespace RDK {

class UApplication;

class UTest
{

};

/// Менеджер тестов
class UTestManager
{
protected: // Данные
/// Тестируемое приложение
UEPtr<UApplication> Application;

public: // Методы
// --------------------
// Методы инициализации
// --------------------
/// Возвращает указатель на тестируемое приложение
UEPtr<UApplication> GetApplication(void);

/// Задает тестируемое приложение
virtual void SetApplication(const UEPtr<UApplication> &value);
// --------------------

// --------------------
// Методы тестирования
// --------------------
/// Загрузка тестов
virtual int LoadTests(const std::string &file_name);

/// Проводит тестирование
/// Записывает в выходной массив результаты тестов
/// Возвращает код ошибки тестирования
virtual int ProcessTests(void);
// --------------------


// --------------------
// Вспомогательные методы
// --------------------
protected:
/// Декодирует корневой файл описания тестов
/// Возвращает массив имен файлов с описаниями тестов
int DecodeMainTestDescriptionFile(std::vector<std::string> &test_file_names);

/// Декодирует тест
int DecodeTestFile(const std::string &test_file_name, UTest &test);
// --------------------


};


}

#endif
