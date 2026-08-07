#ifndef UWATCH_SOURCE_PICKER_WIDGET_H
#define UWATCH_SOURCE_PICKER_WIDGET_H

#include "UWatchMatrixSelector.h"

#include <QWidget>
#include <QVector>
#include <QString>

namespace RDK
{
class UApplication;
}

class UComponentsListWidgetModern;
class QLabel;
class QSplitter;

class UWatchSourcePickerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UWatchSourcePickerWidget(QWidget* parent = nullptr);

    void configureForWatch(RDK::UApplication* app, bool showChannels);

    int channelIndex() const;
    QString componentLongName() const;
    QString propertyName() const;
    QVector<UWatchMatrixSelector::CellRef> cells() const;
    bool isComplete() const;

    void setMatrixPickMode(MatrixPickMode mode);
    UWatchMatrixSelector* matrixSelector() const { return m_matrix; }

signals:
    void selectionChanged();

private slots:
    void onListSelectionChanged();

private:
    void updateBindingFromList();
    void updateReadout();

    UComponentsListWidgetModern* m_list = nullptr;
    UWatchMatrixSelector* m_matrix = nullptr;
    QLabel* m_bindingReadout = nullptr;
    QSplitter* m_split = nullptr;
};

#endif // UWATCH_SOURCE_PICKER_WIDGET_H
