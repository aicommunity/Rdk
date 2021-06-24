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

    /// ������ � ������� ��������� � �������
    int SelectedRow,SelectedCol;

    /// ��� ����������
    std::string ComponentMatrixName;

    /// ��� ��������
    std::string PropertyMatrixName;

    /// ��� ��������
    /// 0 - ����������
    /// 1 - MDMatrix<double>
    /// 2 - MDMatrix<int>
    /// 3 - MDVector<double>
    /// 4 - MDVector<int>
    int PropertyType;

    // ���� force_update == true, �� ��������� �����������
    // ��� ����������� �� UpdateInterval
    virtual void AUpdateInterface(void);

    // ������� ���������� � �������� ���������
    virtual void AClearInterface(void);

    /// �������� ������� ��� ����������
    bool SelectMatrix(const std::string &comp_name, const std::string &prop_name);


private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::UMatrixForm *ui;
};

#endif // UMATRIXFORM_H
