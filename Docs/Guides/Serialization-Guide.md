# Руководство по сериализации / Serialization guide

## RU

Для полного графа `UNet` используйте штатные `SaveComponent/LoadComponent` и
`USerStorageXML`. Отдельные значения не заменяют классы детей, связи и структуру.

### Сохранение

`source` — существующий `UEPtr<UNet>`. Расчёт и редактирование графа остановлены.
Ошибки IO/сериализации обрабатывает вызывающий код.

```cpp
RDK::USerStorageXML xml;
if(!xml.Create("GraphExport")) throw std::runtime_error("create failed");
const unsigned int mask = ptPubParameter | ptPubState | pgSystem;
if(!source->SaveComponent(&xml, true, mask))
    throw std::runtime_error("save component failed");
if(!xml.SaveToFile("graph.xml")) throw std::runtime_error("write failed");
```

Маска включает **тип и группу**. `UVariable::CheckMask` требует пересечения
обоих полей. `ptParameter | ptState` без группы даёт пустые Parameters/State,
хотя SaveComponent может вернуть true и сохранить дерево и связи. Для public
параметров применяют `ptPubParameter`; для всех public типов — `ptAnyPub`.
`pgSystem` добавляйте, когда требуются системные свойства соответствующих типов.
Фактическую полноту проверяют по необходимым полям и effective readback.

### Загрузка

Используемые классы/образы должны быть зарегистрированы в storage. Для примера
известны `sourceClass` и `sourceName`; versioned export обязан хранить их и
проверять соответствие профилей. `storage` — штатное хранилище компонентов.

```cpp
RDK::USerStorageXML xml;
if(!xml.LoadFromFile("graph.xml", "GraphExport") || !xml.SelectNode(sourceName))
    throw std::runtime_error("invalid graph document");
auto target = RDK::dynamic_pointer_cast<RDK::UNet>(storage->TakeObject(sourceClass));
if(!target) throw std::runtime_error("class unavailable");
target->Name = sourceName;
if(!target->LoadComponent(&xml, true)) throw std::runtime_error("load failed");
target->Init(); // Native initialization includes Reset/Build as required.
```

В рабочем importer передают diagnostics в LoadComponent и независимо проверяют
полный effective graph: классы, named children, внутренние ссылки, property
bindings с индексами, порядок расчёта, частоту и индивидуальную физику. Успешное
завершение Load не доказывает полноту переноса. Не вызывайте Default после Take
проверенного образа или загрузки: он может стереть перенесённую настройку.

Init/Reset восстанавливают штатную динамику модели. Такой roundtrip не является
checkpoint продолжения посреди незавершённой пробы. Аналогично `Alloc/Copy` и
флаг `copystate` не гарантируют рекурсивного checkpoint всей динамики.
При замене графа UContainer::Copy вызывает native UnInit получателя и завершает
копирование с InitFlag=false/Ready=false. Получатель проходит native Init/Build
перед расчётом нового графа. Сохранение прежнего InitFlag после замены детей
давало устаревшие ссылки и пропущенную инициализацию при pool reuse.

Project.ini может использовать диалект Rdk с числовыми именами узлов (`<00>`).
Для него нужен штатный parser или проверенный lossless adapter, а не допущение
о совместимости с обычным XML parser.

## EN

Use native `UNet::SaveComponent/LoadComponent` with `USerStorageXML` for complete
component graphs. The examples use existing storage images and native Init.
A property mask must contain both a type and a group: `ptParameter | ptState`
alone serializes empty parameter/state sections. The example selects public
and system parameters/states.

Load requires the referenced storage classes. Validate diagnostics and the actual
graph, indexed bindings, calculation order, frequency and individual physics.
A successful serializer call does not establish semantic completeness. Do not
Default a loaded graph. Native Init/Reset restores physical dynamics; this is
not a checkpoint of an unfinished trial. Rdk numeric XML node names require
the native parser or a verified lossless adapter.

## См. также / See also

- [Serialize-Detailed.md](../Serialize-Detailed.md)
- [Property-Data-Sources.md](../Property-Data-Sources.md)
- [Component-Order-Contract.md](../Component-Order-Contract.md)
