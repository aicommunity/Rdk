#ifndef PERFORMANCEVIEW_H
#define PERFORMANCEVIEW_H

#include <QWidget>
#include "ui_PerformanceView.h"

class PerformanceView : public QWidget
{
 Q_OBJECT

public:
 PerformanceView(QWidget *parent = 0);
 ~PerformanceView();

 void CalculateDurations();

private:
 Ui::performanceView ui;

 int cycleCounter;
 int maxCycleValue;
 long long int fullStepSum;
 long long int interStepSum;

public slots:
 void UpdateCycleDurationValues();

private slots:
 void SetCycleCounterValue(int value);
};

#endif // PERFORMANCEVIEW_H
