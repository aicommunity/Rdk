#ifndef UMATRIXFORMDIALOG_H
#define UMATRIXFORMDIALOG_H

#include <QDialog>
#include <QTimer>
#include "rdk.h"
#include <rdk_application.h>

namespace Ui {
class UMatrixFormDialog;
}

class UMatrixFormDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UMatrixFormDialog(QWidget *parent = nullptr);
    ~UMatrixFormDialog();

    /// Строка и столбец выбранные в матрице
    std::vector<int> SelectedRows, SelectedCols;

    /// Имя компонента
    std::string ComponentMatrixName;

    /// Имя свойства
    std::string PropertyMatrixName;

    /// Тип свойства
    /// 0 - неизвестно
    /// 1 - MDMatrix<double>
    /// 2 - MDMatrix<int>
    /// 3 - MDVector<double>
    /// 4 - MDVector<int>
    int PropertyType;


     QTimer updateMatrixDataTimer;

    /// Выбирает матрицу для наблюдения
    bool SelectMatrix(const std::string &comp_name, const std::string &prop_name);

private slots:
    void on_buttonBox_accepted();

    // функция обновления отображения матрицы
    void UpdateMatrixData(void);


private:
    Ui::UMatrixFormDialog *ui;
};

#endif // UMATRIXFORMDIALOG_H
