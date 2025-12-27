#ifndef ULOGGERWIDGET_H
#define ULOGGERWIDGET_H

#include "UVisualControllerWidget.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QTimer>

class ULoggerWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    ULoggerWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~ULoggerWidget();

    void AUpdateInterface();

private slots:
    void onUpdateTimer();

private:
    void AddString(int log_level, const QString &string);
    QVBoxLayout *layout;
    QPlainTextEdit *textEdit;
    QTimer *updateTimer;
};

#endif // ULOGGERWIDGET_H
