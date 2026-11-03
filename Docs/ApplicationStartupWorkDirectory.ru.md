# Рабочий каталог приложения при Init

UApplication::SetWorkDirectory задаёт рабочий каталог экземпляра. Init
сохраняет явно выбранное до запуска значение; если каталог пуст, штатный
default остаётся каталогом ApplicationFileName. Core_SetSystemDir всегда
получает фактический каталог программы и не подменяется рабочим каталогом.

Файловый glog и сессия UGlogGuiSink используют GetWorkLogDir, то есть
WorkDirectory/EventsLog. FixedLogPath регулирует primary/read destinations,
но не заменяет базовый рабочий каталог glog. Для изолированного batch run
владелец должен выбрать отдельный writable WorkDirectory до Init.
Это использует существующий API; полей, virtual методов и ABI не добавлено.

При повторном Init сохраняется текущий WorkDirectory. Для нового выбора
default caller явно сбрасывает SetWorkDirectory("") перед Init. Ресурсные
пути при отдельном рабочем каталоге следует задавать явно; native Console
использует application ini с абсолютными путями библиотек/ClDesc/configs.
Семантика компонентов Default/Build/Calculate/Reset не меняется.

Основание: native Console per-job FixedLogPath не предотвращал запись в
runtime/EventsLog, поскольку Init безусловно стирал SetWorkDirectory, а glog
по штатной логике писал в GetWorkLogDir. Проверка должна включать настоящие
native preflight и frozen responses, immutable executable runtime, отказ
для повторного output/INI override и одновременный duplicate launch.
Проверка04.11 выполнена на Elder normal и M40 ASan через jump host. В корневом
Nmsdk опубликованы исходные evidence commits:
normal f6004c6f0d3208005c3a14648929735174526f39,
ASan948b7f2da761c87b8cb2b09e6bef1cddc7ff6d07;
source root b3bfe86e913bc1f7aeb5a340a72698eacb060f00, tested Rdk0e70cf07.
Каждый worker:4 zero-tick preflights;4 frozen imports по4 полных trial с точным
совпадением всех наблюдений и моделей;4 реальные калибровки с совпадением
всей transaction/model;3 отказа до Init; concurrent duplicate0/2.
Old/new runtime inventories неизменны. Отдельный обычный запуск того же binary
без native flags подтвердил прежний default executable-directory/EventsLog.
Actual loaded-files ASan содержит libasan, detect_leaks=0; leak freedom не заявлена.

Совместное изменение Console использует существующий SetWorkDirectory до Init.
Stock physics, компонентные Calculate/Build/Reset, storage и обучающие цели
этим Rdk изменением не менялись. Это scoped startup contract, не полный CTest
Rdk и не завершение полного плана TimeLearner. Первые неудачные попытки root
сохранены: Console-only путь не менял glog; затем упаковщик запрещал штатный
severity alias. Production сборщик теперь проверяет alias/owned PID и хранит
только реальные log files без дублирующих ссылок.
