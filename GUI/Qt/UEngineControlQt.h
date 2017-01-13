#ifndef UENGINECONTROLQT_H
#define UENGINECONTROLQT_H

#include <rdk_application.h>
#include <QObject>
#include <QBasicTimer>

class UEngineControlQt : public QObject, public RDK::UEngineControl
{
    Q_OBJECT
public:
    UEngineControlQt(QObject *parent = 0);
    virtual ~UEngineControlQt();

    /// ��������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
    virtual void StartChannel(int channel_index);

    /// ������������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
    virtual void PauseChannel(int channel_index);

public slots:
    void timerEvent(QTimerEvent*);

private:
    QBasicTimer *timer;

};

#endif // UENGINECONTROLQT_H
