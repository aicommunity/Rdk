#ifndef TProjectLoadDiagnostics_H
#define TProjectLoadDiagnostics_H

#include <string>
#include <vector>

namespace RDK {

/// Структура для диагностики загрузки конфигурации
struct RDK_LIB_TYPE TProjectLoadDiagnostics
{
/// Список ошибок
std::vector<std::string> errors;

/// Список предупреждений
std::vector<std::string> warnings;

/// Существует ли модель
bool modelExists;

/// Пустая ли модель
bool modelEmpty;

/// Количество компонентов в модели (для каждого канала)
std::vector<int> componentsCount;

/// Отсутствующие файлы
std::vector<std::string> missingFiles;

/// Каналы с ошибками загрузки (индексы)
std::vector<int> failedChannels;

/// Общая валидность конфигурации
bool isValid;

/// Количество успешно загруженных каналов
int channelsLoaded;

/// Общее количество каналов
int channelsTotal;

TProjectLoadDiagnostics()
 : modelExists(false)
 , modelEmpty(true)
 , isValid(false)
 , channelsLoaded(0)
 , channelsTotal(0)
{
}
};

} // namespace RDK

#endif // TProjectLoadDiagnostics_H
