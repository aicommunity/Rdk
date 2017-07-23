#ifndef UTestManagerH
#define UTestManagerH

#include "../../Deploy/Include/rdk_cpp_initdll.h"
//#include "../../Libraries/Rtv-BasicLib/Core/RTVSupport.h"

namespace RDK {

class UApplication;

struct UPropertyTest
{
  std::string component;
  std::string property;
  std::string type;
  std::string delta;
  std::string value;
};

struct UTest
{
protected: // Данные
  /// Тестируемое приложение
  UEPtr<UApplication> Application;

  /// Длительность расчёта
  int calcDuration;

  /// Режим проведения тестов
  /// true - проведение количества итераций = calcDuration
  /// false - рассчет проводится в течении calcDuration миллисекунд
  bool stepsMode;

  /// Список загруженных тестов
  std::vector<UPropertyTest> propertyTests;

  /// Путь к фалу, содержащему тестовую конфигурацию
  std::string testsFileName;

  /// Путь к файлу конфигурации тестируемого проекта
  std::string testProjectFileName;

public: // Методы
  // --------------------
  // Методы инициализации
  // --------------------
  UTest(void);
  UTest(const UEPtr<UApplication> &value);

  /// Возвращает указатель на тестируемое приложение
  UEPtr<UApplication> GetApplication(void);

  /// Задает тестируемое приложение
  virtual void SetApplication(const UEPtr<UApplication> &value);

  UTest& operator = (const UTest &copy);
  // --------------------

  // --------------------
  // Методы тестирования
  // --------------------
  virtual int LoadTest(std::string testFile);

  /// Проводит тестирование
  /// Возвращает колличество неудачных проперти тестов
  virtual int ProcessTest(void);

  // --------------------
  // Вспомогательные методы
  // --------------------
protected:
  // Методы сравнения
  bool compareProperties(bool               value, std::string str, std::string delta);
  bool compareProperties(int                value, std::string str, std::string delta);
  bool compareProperties(unsigned int       value, std::string str, std::string delta);
  bool compareProperties(long long          value, std::string str, std::string delta);
  bool compareProperties(unsigned long long value, std::string str, std::string delta);
  bool compareProperties(float              value, std::string str, std::string delta);
  bool compareProperties(double             value, std::string str, std::string delta);
  bool compareProperties(std::string        value, std::string str, std::string);

//  bool compareProperties(std::vector<RTV::TZoneExt> value, std::string str, std::string delta);
};

/// Менеджер тестов
class UTestManager
{
protected: // Данные
/// Тестируемое приложение
UEPtr<UApplication> Application;

/// Список загруженных тестов
std::vector<UTest> tests;

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
/// Возвращает колличество неудачных тестов
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
