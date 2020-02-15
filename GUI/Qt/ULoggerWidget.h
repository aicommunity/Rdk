#ifndef ULOGGERWIDGET_H
#define ULOGGERWIDGET_H

#include "UVisualControllerWidget.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>

class ULoggerWidget : public UVisualControllerWidget
{
public:
    ULoggerWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~ULoggerWidget();

    void AUpdateInterface();

private:
    void AddString(int log_level, const QString &string);
    QVBoxLayout *layout;
    QPlainTextEdit *textEdit;
};

#endif // ULOGGERWIDGET_H
