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
До normal/ASan runtime-проверки изменение остаётся feature/WIP.
