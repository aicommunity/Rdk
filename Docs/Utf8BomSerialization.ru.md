# UTF-8 BOM в USerStorageXML::Load

29.10.2026, рабочий подэтап `stage04/xml-utf8-bom`; серверная проверка исправления
ещё ожидается. Baseline Rdk `1812624ff75c60f03ad57c10b379d3e3848590bc`
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
Дополнительно требуется повтор реального native readback файлов, созданных
lossless config adapter, с сохранением исходной неудачной попытки.

Это исправление serialization owner, не learner workaround и не изменение
Default/Build/Calculate/Reset, scheduler или физической динамики. Их bool-контракт
не меняется. Методы файлового ввода и полнота typed component preflight этим
подэтапом не объявляются проверенными.
