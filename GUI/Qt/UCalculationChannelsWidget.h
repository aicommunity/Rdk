#ifndef UCALCULATIONCHANNELSWIDGET_H
#define UCALCULATIONCHANNELSWIDGET_H

#include <QWidget>
#include "UVisualControllerWidget.h"

namespace Ui {
class UCalculationChannelsWidget;
}

class UCalculationChannelsWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UCalculationChannelsWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UCalculationChannelsWidget();

    void AUpdateInterface();

public slots:
    void channelSelectionChanged();

    //context menu actions
    void actionAddChannel();
    void actionInsertChannel();
    void actionDeleteSelectedChannel();
    void actionStartChannel();
    void actionPauseChannel();
    void actionResetChannel();
    void actionCloneChannel();

signals:
    void updateModel();

private:
    Ui::UCalculationChannelsWidget *ui;

    int channelsCounter;
    int currentChannel;
};

#endif // UCALCULATIONCHANNELSWIDGET_H
