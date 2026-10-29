# UTF-8 BOM в USerStorageXML::Load

29.10.2026, рабочий подэтап `stage04/xml-utf8-bom`. Исправление source
`8ebf64dd39571cb6f95f23cfc2b55f30972c591c` проверено на M40 ASan, detect_leaks=0:
четыре Rdk CTest groups, включая обе новые encoding regressions, и native readback
четырёх adapter-produced файлов / одиннадцати значений. Baseline Rdk
`1812624ff75c60f03ad57c10b379d3e3848590bc`
вызван production-path fixture из root `7db4d9737`: три адресно изменённых
Project.ini, включая numeric tag `<00>`, читаются; synthetic UTF-8 BOM + XML
declaration приводит к false из `Load`. Удаление только BOM либо только declaration
сохраняет числовые и UTF-8 значения и устраняет отказ на том же binary.

XMLNode::parseString не обрабатывает это сочетание как ожидаемый корневой узел.
USerStorageXML теперь удаляет только начальную трёхбайтовую UTF-8 signature перед
вызовом native parser. Исходный string не меняется; для обычного входа копия не
создаётся. Обе compile branches, narrow/wide, используют одинаковый нормализованный
вход; отдельный wide-platform runtime этим пока не проверен.

Регрессии: четыре сочетания BOM/declaration с numeric child и escaped text;
отсутствующий root, пустой BOM-only документ и очистка прежнего parsed state.
Повтор реального native readback успешен; исходная неудачная попытка сохранена.
Root evidence commit `6210f02353cf2b3956a8a4ee93201607d870541a`,
Reports/StructTrain/TimeLearner/s09-config-m40: 92 stored / 90 raw hashes сверены.
Попытки A2 (launcher syntax) и A3 (disk exhaustion) не являются parser failures.
Более широкий full regression прежнего Rdk181 не подменяет scope новой проверки;
normal-проверка нового source завершена ниже; wide-platform приёмка
по-прежнему не заявлена.

Это исправление serialization owner, не learner workaround и не изменение
Default/Build/Calculate/Reset, scheduler или физической динамики. Их bool-контракт
не меняется. Методы файлового ввода и полнота typed component preflight этим
подэтапом не объявляются проверенными.

## Дополнительный normal gate на Elder-1

Root 796a73075b930d634c905355541e47187aed6f5f / Rdk abc21b65
(Core 8ebf64dd): четыре выбранные CTest groups прошли, включая обе encoding
регрессии. Дополнительный прямой XmlTextEncoding filter: 2/2 cases, отдельный
XML. RDK_UNICODE_RUN=OFF. Runtime создан после завершения предыдущего задания,
без перезаписи прежнего runtime. Evidence root commit
fa39375f17f3cb694a9a42c3d37c3d7c11c5a58b,
Reports/StructTrain/TimeLearner/s04-bom-elder1. Это normal narrow проверка;
исходный M40 adapter readback остаётся отдельным evidence.
