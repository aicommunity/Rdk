#ifndef USETTINGSREADERWIDGET_H
#define USETTINGSREADERWIDGET_H

#include "UVisualControllerWidget.h"

/// ������������� ����������� ����������� �������� �������� ������� load/save Parameters �� RDK
///
/// �������� �� ���� ������ � ��� �������, ��� ��������:
/// RDK::UIVisualControllerStorage::LoadParameters
/// RDK::UIVisualControllerStorage::SaveParameters
/// �������� ���������� ��� � ����������� �������
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

  virtual ~USettingsReaderWidget() { clear(); }

  /// ������ ����� ��������
  virtual void ASaveParameters()
  {
    if(object && saveFunc)
      (object->*saveFunc)();
  }

  /// ���������� ����� ��������
  virtual void ALoadParameters()
  {
    if(object && loadFunc)
      (object->*loadFunc)();
  }

  void clear()
  {
    object   = NULL;
    loadFunc = NULL;
    saveFunc = NULL;
  }

private:
  T* object;
  void (T::*loadFunc)();
  void (T::*saveFunc)();
};

#endif // USETTINGSREADERWIDGET_H
