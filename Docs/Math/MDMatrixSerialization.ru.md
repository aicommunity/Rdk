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
