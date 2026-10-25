# Read-only источники данных свойств

`UIPropertyInput` предоставляет диагностический интерфейс:

```cpp
bool CanInspectDataSources() const;
std::vector<const UIProperty*> GetDataSources() const;
bool UsesExternalDataSource() const;
```

Это чтение существующих bindings, без `GetData`, изменения lazy cache,
Build/Reset/Calculate или создания соединений. Вызывать при остановленном
изменении графа; возвращённые указатели не владеют объектами. Параллельное
редактирование связей или удаление объектов не поддерживается этим API.

`UVProperty` возвращает эффективные источники в порядке входных slots:
`ExternalDataSource`, если он задан, иначе `ConnectedOutputs` при
`IsConnectedFlag=true`. Это тот же приоритет, что у native GetData. При внешнем
источнике затенённые pointer bindings не выдаются как действующие. Пустые slots
сохраняются как null: диагностический потребитель должен обнаруживать их,
а не сдвигать индексы. При отсутствии подключения возвращается пустой массив.

Собственные реализации UIPropertyInput по умолчанию возвращают
`CanInspectDataSources=false`. Для полной диагностики такие реализации должны
переопределить API в соответствии со своим GetData. Connected=true с отсутствующей
или неподдерживаемой диагностикой не является доказательством отсутствия связей.

`UIPropertyOutput::GetNumConnectors` перечисляет отдельный реестр UItem connectors.
Прямой `SetPointer` не обязан добавлять запись в него. Нельзя использовать нулевой
размер этого реестра как признак изолированного свойства или отсутствия wiring.
GetDataSources описывает перенос данных между properties, а не имена/идентичность
UItem/ULink объектов, и не заменяет штатные операции CreateLink/BreakLink/Copy.

Добавлены виртуальные методы: Rdk и зависимые библиотеки/плагины требуют
согласованной пересборки. Семантика GetData/SetPointer и lifecycle не изменена.
Native проверки потребителя находятся в корневом репозитории Nmsdk:
`Tests/Integration/Libraries/Test_PulseLib_ModelInspection.cpp`.
