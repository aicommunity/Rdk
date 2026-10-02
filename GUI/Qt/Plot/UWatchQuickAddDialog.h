#ifndef UWATCH_QUICK_ADD_DIALOG_H
#define UWATCH_QUICK_ADD_DIALOG_H

#include <QDialog>
#include <QVector>
#include <QString>

namespace RDK
{
class UApplication;
}

class QComboBox;
class UWatchTab;

class UWatchQuickAddDialog : public QDialog
{
    Q_OBJECT
public:
    struct SignalRef
    {
        QString component;
        QString property;
        int jx = 0;
        int jy = 0;
        int channel = 0;
    };

    explicit UWatchQuickAddDialog(RDK::UApplication* app,
                                  UWatchTab* tab,
                                  int initialChartIndex,
                                  QWidget* parent = nullptr);

    QString componentLongName() const;
    QString propertyName() const;
    int matrixJx() const;
    int matrixJy() const;
    int channelIndex() const;
    int targetChartIndex() const;
    QVector<SignalRef> selectedSignals() const;
    bool selectionComplete() const;

private:
    void refreshQuickPicks();
    void refreshQueue();
    void updateControls();
    void queueCurrentSelection();
    void queueSignal(const SignalRef& signal);
    void addCurrentSelectionToFavorites();
    void removeSelectedFavorite();
    void rememberRecentSignals(const QVector<SignalRef>& entries);

    class UWatchSourcePickerWidget* m_picker = nullptr;
    UWatchTab* m_tab = nullptr;
    QComboBox* m_targetChart = nullptr;
    class QListWidget* m_quickPicks = nullptr;
    class QListWidget* m_queuedSignals = nullptr;
    class QComboBox* m_matrixPickMode = nullptr;
    class QPushButton* m_favoriteCurrentBtn = nullptr;
    class QPushButton* m_queueCurrentBtn = nullptr;
    class QPushButton* m_addQuickPickBtn = nullptr;
    class QPushButton* m_removeFavoriteBtn = nullptr;
    class QPushButton* m_removeQueuedBtn = nullptr;
    class QPushButton* m_clearQueueBtn = nullptr;
    class QPushButton* m_acceptButton = nullptr;
    QVector<SignalRef> m_favorites;
    QVector<SignalRef> m_recent;
    QVector<SignalRef> m_pending;
};

#endif // UWATCH_QUICK_ADD_DIALOG_H
