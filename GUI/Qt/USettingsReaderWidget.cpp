#include "USettingsReaderWidget.h"

// Implementation for USettingsReaderWidget
// This provides the vtable for the class

USettingsReaderWidget::~USettingsReaderWidget()
{
    // Virtual destructor implementation
}

void USettingsReaderWidget::ASaveParameters()
{
    emit writeSetting();
}

void USettingsReaderWidget::ALoadParameters()
{
    emit readSetting();
}

#include "USettingsReaderWidget.moc"
