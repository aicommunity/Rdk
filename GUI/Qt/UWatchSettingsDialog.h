#ifndef UWATCHSETTINGSDIALOG_H
#define UWATCHSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class UWatchSettingsDialog;
}

class UWatchSettingsDialog : public QDialog
{
 Q_OBJECT

public:
 explicit UWatchSettingsDialog(QWidget *parent = nullptr);
 ~UWatchSettingsDialog();

private:
 Ui::UWatchSettingsDialog *ui;
};

#endif // UWATCHSETTINGSDIALOG_H
