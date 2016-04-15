#include "../../../Rdk/Deploy/Include/rdk_initdll.h"
#include "PerformanceView.h"

#include <qdebug.h>

PerformanceView::PerformanceView(QWidget *parent)
    : QWidget(parent)
{
 ui.setupUi(this);
 maxCycleValue = 10; // default
 ui.cycles->setValue(maxCycleValue);
 cycleCounter = 0;
 fullStepSum = 0;
 interStepSum = 0;

 connect(ui.cycles, SIGNAL(valueChanged(int)), this, SLOT(SetCycleCounterValue(int)));
}

PerformanceView::~PerformanceView()
{

}

void PerformanceView::UpdateCycleDurationValues()
{
 long long int avgFullStep = fullStepSum/maxCycleValue;
 long long int avgInterStep = interStepSum/maxCycleValue;

 ui.fullStep->setText(QString::number(avgFullStep));
 ui.interStep->setText(QString::number(avgInterStep));
 ui.sum->setText(QString::number(avgFullStep + avgInterStep));
}

void PerformanceView::CalculateDurations()
{
 if(!Model_Check())
  return;
  
 qDebug() << "fullstep" << Model_GetFullStepDuration("") << "interstep" << Model_GetInterstepsInterval("");

 fullStepSum += Model_GetFullStepDuration("");
 interStepSum += Model_GetInterstepsInterval("");

 ++cycleCounter;
 if (cycleCounter > maxCycleValue-1) // maxCycleValue cycles passed
 {
  UpdateCycleDurationValues();
  cycleCounter = 0;
  fullStepSum = 0;
  interStepSum = 0;
 }
}

void PerformanceView::SetCycleCounterValue(int value)
{
 maxCycleValue = value;
}
