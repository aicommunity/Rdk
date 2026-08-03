#ifndef UWATCHLAYOUTDIALOG_H
#define UWATCHLAYOUTDIALOG_H

#include <QDialog>
#include <QPair>

class UWatchTab;
class QComboBox;
class QSpinBox;
class QFrame;
class QGridLayout;

/// Compact modal dialog for tab-scoped layout (grid + update interval).
class UWatchLayoutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UWatchLayoutDialog(UWatchTab* tab, QWidget* parent = nullptr);

    static bool execForTab(UWatchTab* tab, QWidget* parent = nullptr);

private slots:
    void onPresetChanged(int index);
    void onCustomSpinChanged();
    void acceptAndApply();

private:
    void buildUi();
    void loadFromTab();
    void updatePreview(int rows, int cols);
    void syncCustomSpinsEnabled();
    QPair<int, int> currentGrid() const;

    UWatchTab* m_tab = nullptr;
    QComboBox* m_presetCombo = nullptr;
    QSpinBox* m_rowsSpin = nullptr;
    QSpinBox* m_colsSpin = nullptr;
    QSpinBox* m_intervalSpin = nullptr;
    QFrame* m_previewFrame = nullptr;
    QGridLayout* m_previewGrid = nullptr;
    bool m_updatingPreset = false;
};

#endif // UWATCHLAYOUTDIALOG_H
