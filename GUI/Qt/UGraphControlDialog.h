#ifndef UGRAPHCONTROLDIALOG_H
#define UGRAPHCONTROLDIALOG_H

#include <QDialog>

namespace Ui {
class UGraphControlDialog;
}

class UGraphControlDialog : public QDialog
{
    Q_OBJECT

public:
    int color;
    bool click;
    explicit UGraphControlDialog(QWidget *parent = 0, int axisX = 0, int axisY = 0);
    //explicit UGraphControlDialog(QWidget *parent = 0);
    ~UGraphControlDialog();

    int getDoubleSpinBoxData() const;
    int getAxisXChangeData(void) const;
    int getAxisYChangeData(void) const;
    int getColor (void) const;
    QString getLableX(void) const;
    QString getLableY(void) const;
    bool getDelInf(void) const;


    void setAxisXChangeData(int axisX) const;
    void setAxisYChangeData(int axisY) const;
    void setDecimalPlaces (int i) const;
    void setDisplayInterval (int i) const;
    void setLableX (QString lableX) const;
    void setLableY (QString lableY) const;



private slots:

    void on_pushButton_clicked();

public slots:
    void isActiveDoubleSpinBox(bool b);

    void on_colorListWidget_currentRowChanged(int currentRow);

private:
    Ui::UGraphControlDialog *ui;
};

#endif // UGRAPHCONTROLDIALOG_H
