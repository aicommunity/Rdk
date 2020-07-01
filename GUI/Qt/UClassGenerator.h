#ifndef UCLASSGENERATOR_H
#define UCLASSGENERATOR_H

#include <QWidget>

namespace Ui {
class UClassGenerator;
}

class UClassGenerator : public QWidget
{
    Q_OBJECT

public:
    explicit UClassGenerator(QWidget *parent = nullptr);
    ~UClassGenerator();

private:
    Ui::UClassGenerator *ui;

private slots:
    void generate();
    void browsePath();
};

#endif // UCLASSGENERATOR_H
