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

    /// ������� ��������� �����
    /// ���� ���� ��������� � ����������, �� ������ � �������
    /// ���� ���, �� -1
    int color;

    /// ���������� �� ������ �������� �������
    /// ���� ��� false
    /// ���� �� true
    bool click;

    ///����������� ���� ���������
    explicit UGraphControlDialog(QWidget *parent = 0);

    ///���������� ���� ���������
    ~UGraphControlDialog();



    //**********************************************************������ ���������� ����������
    /// ������ �������� ��� �, �� ������� ����� ����������� ��������
    int getDoubleSpinBoxData() const;

    /// ������ ������� �� ���������� ������� �� ��� �
    int getAxisXChangeData(void) const;

    /// ������ ������� �� ���������� ������� �� ��� � �����
    int getAxisMaxYChangeData(void) const;

    /// ������ ������� �� ���������� ������� �� ��� � ����
    int getAxisMinYChangeData(void) const;

    /// ������ ��������� ����
    int getColor (void) const;

    /// ������ ������� �� ��� �
    QString getLableX(void) const;

    /// ������ ������� �� ��� �
    QString getLableY(void) const;

    /// ������ ������� �� �������� �������� �������
    bool getDelInf(void) const;
    //**********************************************************


    //**********************************************************��������� �������� ���������� ����
    /// ���������� ������� ���������� ������ �� ��� �
    void setAxisXChangeData(int axisX) const;

    ///���������� ������� ���������� ������ �� ��� � �����
    void setAxisMaxYChangeData(int axisY) const;

    ///���������� ������� ���������� ������ �� ��� � ����
    void setAxisMinYChangeData(int axisY) const;

    /// ���������� ���������� ������ ����� �������
    /// � ��������� ��� �, �� ������� ���������� ��������
    void setDecimalPlaces (int i) const;

    ///���������� �������� ��� �, �� ������� ���������� ��������
    void setDisplayInterval (int i) const;

    /// ���������� ������� �� ��� �
    void setLableX (QString lableX) const;

    /// ���������� ������� �� ��� �
    void setLableY (QString lableY) const;
    //**********************************************************

private slots:

    /// ���� ������ �� ������ �������� �������� �������
    void on_pushButton_clicked();

public slots:

    /// ������ �� ���� �������� ���� � ���������� �������� �� ��� �
    void isActiveDoubleSpinBox(bool b);

    /// ���� ������ �������� ���� - ����� �������� ����
    void on_colorListWidget_currentRowChanged(int currentRow);

private:
    Ui::UGraphControlDialog *ui;
};

#endif // UGRAPHCONTROLDIALOG_H
