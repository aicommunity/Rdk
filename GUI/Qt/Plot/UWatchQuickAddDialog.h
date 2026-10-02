#ifndef UWATCH_QUICK_ADD_DIALOG_H
#define UWATCH_QUICK_ADD_DIALOG_H

#include <QDialog>

namespace RDK
{
class UApplication;
}

class UWatchQuickAddDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UWatchQuickAddDialog(RDK::UApplication* app, QWidget* parent = nullptr);

    QString componentLongName() const;
    QString propertyName() const;
    int matrixJx() const;
    int matrixJy() const;
    int channelIndex() const;
    bool selectionComplete() const;

private:
    class UWatchSourcePickerWidget* m_picker = nullptr;
};

#endif // UWATCH_QUICK_ADD_DIALOG_H
