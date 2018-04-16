#ifndef USETTINGSREADERWIDGET_H
#define USETTINGSREADERWIDGET_H

#include "UVisualControllerWidget.h"

/// Предоставляет возможность произволным класссам получать события load/save Parameters из RDK
///
/// Получает на вход объект и две функции, при событиях:
/// RDK::UIVisualControllerStorage::LoadParameters
/// RDK::UIVisualControllerStorage::SaveParameters
/// вызывает переданные ему в конструктор функции
template<class T>
class USettingsReaderWidget : public UVisualControllerWidget
{
public:
  explicit USettingsReaderWidget(QWidget *parent = nullptr, T* obj = NULL,
                                 void (T::*load)() = NULL, void (T::*save)() = NULL)
  {
    object = obj;
    loadFunc = load;
    saveFunc = save;
  }

  /// запись файла настроек
  virtual void ASaveParameters()
  {
    if(object && saveFunc)
      (object->*saveFunc)();
  }

  /// считывание файла настроек
  virtual void ALoadParameters()
  {
    if(object && loadFunc)
      (object->*loadFunc)();
  }

private:
  T* object;
  void (T::*loadFunc)();
  void (T::*saveFunc)();
};

#endif // USETTINGSREADERWIDGET_H
