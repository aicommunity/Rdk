# Полная загрузка графа и прежние входные связи

`UNet::LoadComponent` заменяет граф компонента. Перед присваиванием свойств
из XML он отсоединяет прежние связи штатным
`UContainer::DisconnectPropertyDataSources`, как при `UContainer::Copy`.
Порядок присваивания свойств и создания дочерних компонентов сохранён.
`LoadComponentProperties` остаётся наложением свойств существующего графа.

Причина изменения: на Rdk `d6c9df759a3ee20ad42fa0a83841f19385fba60a`
реальные проверки `NativeLoadHistory/ConnectedChildAt2000` и
`ConnectedChildAt20000` на M40 показали сохранение старого connected input
0.75 вместо сохранённого локального 0.25. После Init/Reset и восьми шагов
штатного scheduler Feedback оставался 1.5 вместо 0.5 при gain=2.
Контроли Fresh и DetachedChild прошли на обеих частотах.
Исходный XML не содержит прежней связи.

Вызов до записи свойств необходим: connected input может игнорировать запись
локального значения, а external data source перенаправляет её источнику.
Отключение после записи или только удаление прежних детей этого не исправляет.
Не добавляются Default, вложенный Calculate или специальный сброс нейрона.
Возвращаемые значения lifecycle не меняются.

На M40 SDKf39/Rdkde413423 все шесть native Load-контролей прошли. Во всех
случаях восстановлен local input1×1=0.25 после Load и Init/Reset, восемь
root scheduler ticks дают Feedback0.5, полный physical identity valid/match.
Положительные model archive transfers в свежий процесс при2000/20000Hz
также прошли: actual input/soma arrays совпадают в полных100ms окнах.

Это адресная runtime-проверка generic Load пути на реальной мембране.
Полный lifecycle/storage/physics matrix не закрыт. Packages5 того же SDK
остановлен kernel OOM при4GiB и не получил итогового JUnit; это отдельный
незавершённый gate, не подтверждение корректности всех learner imports.
Elder и дальнейшие проверки остаются обязательными.

Полные первичные данные сохранены в root evidence
219ceb0c439ef91ce410cf564f9a1dc9f2f63364;4074 compiled-source hashes,
реальные outcomes, closure и released leases проверены независимо.
[Отчёт и ограничения](https://github.com/aicommunity/Nmsdk/blob/3fe3fa0105074df4eef0c07ee0e07fb252e92f0c/Docs/StructTrain/TimeLearner/NativeArchiveRepair17Oom-20261110.ru.md).
Документационное уточнение не меняет измеренный Rdkde413423 и frozen runtime.
