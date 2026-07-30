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
  explicit USettingsReaderWidget(QWidget *parent = 0) :
    UVisualControllerWidget(parent)
  {

  }

  /// запись файла настроек (вызывается из SaveProject через ASaveParameters(xml))
  virtual void ASaveParameters()
  {
    emit writeSetting();
  }

  virtual void ASaveParameters(RDK::USerStorageXML &xml) override
  {
    Q_UNUSED(xml);
    emit writeSetting();
  }

  /// считывание файла настроек (вызывается из OpenProject через ALoadParameters(xml))
  virtual void ALoadParameters()
  {
    emit readSetting();
  }

  virtual void ALoadParameters(RDK::USerStorageXML &xml) override
  {
    Q_UNUSED(xml);
    emit readSetting();
  }

signals:
  void readSetting();
  void writeSetting();
};

#endif // USETTINGSREADERWIDGET_H
