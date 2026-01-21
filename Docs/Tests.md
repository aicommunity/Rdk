# Тестирование Rdk Core

## RU

### Обзор

Описание структуры и стратегии тестирования Rdk Core.

### Структура тестов

#### Юнит-тесты (`Rdk/Tests/Unit/`)

- **Engine/UComponent/** - тесты компонентов
  - `Test_UComponent_Lifecycle.cpp` - тесты жизненного цикла
  - `Test_UComponent_Properties.cpp` - тесты свойств

- **Engine/UProperty/** - тесты свойств
  - `Test_UProperty_Basic.cpp` - базовые тесты
  - `Test_UProperty_Connections.cpp` - тесты соединений
  - `Test_UProperty_Containers.cpp` - тесты контейнеров
  - `Test_UProperty_Performance.cpp` - тесты производительности

- **Engine/UContainer/** - тесты контейнеров
  - `Test_UContainer_Components.cpp` - тесты компонентов в контейнерах

- **Engine/UStorage/** - тесты хранилища
  - `Test_UStorage_Objects.cpp` - тесты объектов хранилища

- **Engine/Logging/** - тесты логирования
  - `Test_Logging_Sinks.cpp` - тесты sink'ов логирования

- **Graphics/** - тесты графики
- **Serialize/** - тесты сериализации

#### Интеграционные тесты (`Rdk/Tests/Integration/`)

- **Application/** - тесты приложения
- **Core/** - тесты ядра

### Запуск тестов

```bash
cd build
ctest
# или
cmake --build . --target test
```

### См. также

- [Docs/Performance-And-Testing/Testing-Strategy.md](../../Docs/Performance-And-Testing/Testing-Strategy.md) - общая стратегия тестирования
- [Reports/33-34](../../Reports/) - результаты тестирования

---

## EN

### Overview

Description of Rdk Core test structure and strategy.

### Test Structure

#### Unit Tests (`Rdk/Tests/Unit/`)

- **Engine/UComponent/** - component tests
- **Engine/UProperty/** - property tests
- **Engine/UContainer/** - container tests
- **Engine/UStorage/** - storage tests
- **Engine/Logging/** - logging tests
- **Graphics/** - graphics tests
- **Serialize/** - serialization tests

#### Integration Tests (`Rdk/Tests/Integration/`)

- **Application/** - application tests
- **Core/** - core tests

### Running Tests

### See Also

- [Docs/Performance-And-Testing/Testing-Strategy.md](../../Docs/Performance-And-Testing/Testing-Strategy.md) - general testing strategy
- [Reports/33-34](../../Reports/) - test results
