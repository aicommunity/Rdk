# Точность native XML для MDMatrix

Общий operator<< в Core/Math/MUXMLSerialize.h создавал stringstream без
настройки precision. Для floating MDMatrix это означало6 значащих цифр,
в отличие от scalar double и std::vector<double>, сохраняющих полную мантиссу.
Проблема относится и к публичным параметрам, а не только к временным output.

В реальном ModelArchive roundtrip на Elder четырежды воспроизведён отказ
`archive_model_mismatch` на TrainingPattern.data.value[0][0]: две частоты и
наличие/отсутствие внешнего stimulus port. Остальные12 scalar/storage и
archive checks прошли. Root baseline990290155c9e1f23afe9a904f7052d20b32939ba,
Rdk78e51a9b8760711f44988e6a0131643162c5b55a. Сверка идентичности корректно
отвергает повреждённый roundtrip и не должна ослабляться ради экспорта.

Writer устанавливает numeric_limits<T>::max_digits10 для floating типов;
структура XML, Rows/Cols, разделители, существующий parser и остальные типы
сохраняются. Старые короткие десятичные XML продолжают читаться; уже потерянные
цифры из старого файла этим изменением не восстанавливаются. Scalar/vector
serializer, lifecycle, Build/Reset/Calculate и их bool не меняются.

Test_MatrixNodeTextFormat дополнен реальным XML Save/Load для float/double/
long double с nextafter, малыми физическими величинами, отрицательными
значениями и конечными normal min/max. Проверяется точное равенство, без
амплитудного tolerance и без копирования тела production-функции в тест.
Runtime-проверка исправления ещё требуется. Это не утверждение о полноте
всех сериализаторов матриц/векторов или о возобновлении динамики посреди пробы.

## Адресная runtime-приёмка, 01.11.2026

На Elder и M40 Test_MatrixNodeTextFormat прошёл10/10, включая три floating
roundtrips. Native ModelArchive прошёл16/16 на каждом worker, в том числе
все4 прежде падавших matrix cases. Дополнительно hot XML32/32 на каждом
worker и normal native lifecycle/roundtrip/executor regression21/21 без skips.
Аудит реальных данных прошёл23 archive +16 warm +16 loaded +18 XML checks
на каждом worker; все24 transfer файла имеют одинаковые числовые массивы и
semantic model/image identities между workers, все64 XML CSV побайтно равны.

Numeric implementation —2732a08a833596b2283c17330f33b8b692664612.
Normal primary2d1827febab8ec360d205163a3469f0077eba989 содержит441 файл,
ASan primaryfe8e700ef0695a9a6847c4809260f4abd5819278 —398 файлов; в корневом
Reports/StructTrain/TimeLearner сохранены сжатые models/data/process/runtime
receipts и хэши. После Git retrieval каждый член проверен. Temporal cache
содержит262 PNG;28 выбранных оригиналов1920×960 непосредственно просмотрены,
включая shared/detail companions для входного фронта и пика. В полном объёме
кэш проверен автоматически; ручной просмотр всех262 не заявляется.

Один очень короткий normal a4 unit-процесс завершился до чтения /proc maps:
его incomplete_loaded_files сохранён. Normal a3 содержит complete mappings
того же unit executable/prelaunch runtime; фактические maps a4 не выдуманы.
Оба ASan unit runs, основные native archive/XML и normal regression имеют
loaded_files_captured. Численные результаты и статус provenance разделены.

Это приёмка floating MDMatrix writer и описанных native archive условий,
а не всей матрицы физических моделей, growth, всех serializers или TimeLearner
рефакторинга в целом. Code/Tests tree не менялся после numeric implementation;
этот раздел добавляет только доказательства и ограничения.
