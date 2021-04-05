#ifndef RDKQTCPCLIENTWIDGET_H
#define RDKQTCPCLIENTWIDGET_H

#include <QWidget>

#include "RdkQTcpClientLib.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RdkQTcpClientWidget; }
QT_END_NAMESPACE

class RdkQTcpClientWidget : public QWidget
{
    Q_OBJECT

public:
    RdkQTcpClientWidget(QWidget *parent = nullptr);
    ~RdkQTcpClientWidget();

private:
    QString GetCurrentTime();
    bool WriteLogError(int err_code);
    void WriteLog(const QString& s);

    void InitLibrary();

private slots:
    void onPushButtonPingClick();
    void onPushButtonConnectClick();
    void onPushButtonDisconnectClick();

    void onPushButtonStartChannelClick();
    void onPushButtonStopChannelClick();
    void onPushButtonResetChannelClick();
    void onPushButtonGetNumChannelsClick();
    void onPushButtonSaveServerClick();

    void onPushButtonGetChannelNameClick();
    void onPushButtonSetChannelNameClick();

    void onPushButtonLoadConfigClick();

    void on_pushButtonStartTraining_clicked();

private:
    Ui::RdkQTcpClientWidget *ui;
    std::map<int, QString> Errors;
};
#endif // RDKQTCPCLIENTWIDGET_H
