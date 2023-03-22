#ifndef UABOUTDIALOG_H
#define UABOUTDIALOG_H

#include <QDialog>
#include <rdk_application.h>

namespace Ui {
class AboutDialog;
}

class UAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UAboutDialog(QWidget *parent, RDK::UApplication *app);
    ~UAboutDialog();

private:
    Ui::AboutDialog *ui;
};

#endif // UABOUTDIALOG_H
