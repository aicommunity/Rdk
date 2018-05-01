#ifndef USETTINGSREADERWIDGET_H
#define USETTINGSREADERWIDGET_H

#include "UVisualControllerWidget.h"

/// Предоставляет возможность произволным класссам от QObject получать события load/save Parameters из RDK
///
/// Получает на вход объект и две функции, при событиях:
/// RDK::UIVisualControllerStorage::LoadParameters
/// RDK::UIVisualControllerStorage::SaveParameters
/// вызывает переданные ему в конструктор функции

class USettingsReaderWidget : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit USettingsReaderWidget(QWidget *parent = nullptr) :
    UVisualControllerWidget(parent)
  {

  }

  /// запись файла настроек
  virtual void ASaveParameters()
  {
    emit writeSetting();
  }

  /// считывание файла настроек
  virtual void ALoadParameters()
  {
    emit readSetting();
  }

signals:
  void readSetting();
  void writeSetting();
};

#endif // USETTINGSREADERWIDGET_H
