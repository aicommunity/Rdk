#ifndef UMATRIXFORM_H
#define UMATRIXFORM_H

#include <QWidget>
#include "UVisualControllerWidget.h"
#include "rdk.h"

namespace Ui {
class UMatrixForm;
}

class UMatrixForm : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UMatrixForm(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
    ~UMatrixForm();

    /// Строка и столбец выбранные в матрице
    int SelectedRow,SelectedCol;

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

    // Если force_update == true, то интерфейс обновляется
    // вне зависимости от UpdateInterval
    virtual void AUpdateInterface(void);

    // Возврат интерфейса в исходное состояние
    virtual void AClearInterface(void);

    /// Выбирает матрицу для наблюдения
    bool SelectMatrix(const std::string &comp_name, const std::string &prop_name);


private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::UMatrixForm *ui;
};

#endif // UMATRIXFORM_H
