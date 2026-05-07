#ifndef UCOMPONENTGUIGRIDDIALOG_H
#define UCOMPONENTGUIGRIDDIALOG_H

#include <QDialog>
#include <QStringList>

class QComboBox;
class QSpinBox;
class QDialogButtonBox;
class QLabel;

class UComponentGuiGridDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UComponentGuiGridDialog(QWidget* parent = nullptr);

    void setExistingGridIds(const QStringList& gridIds);
    void setInitialGridId(const QString& gridId);
    void setMoveModeEnabled(bool enabled);
    void setRowColLimits(int maxRow, int maxCol);
    void setDefaultGridSize(int rows, int cols);

    QString selectedGridId() const;
    bool createNewGrid() const;
    int rows() const;
    int cols() const;
    int row() const;
    int col() const;

private slots:
    void updateUiState();
    void accept() override;

private:
    QComboBox* m_gridIdCombo = nullptr;
    QSpinBox* m_rowsSpin = nullptr;
    QSpinBox* m_colsSpin = nullptr;
    QSpinBox* m_rowSpin = nullptr;
    QSpinBox* m_colSpin = nullptr;
    QLabel* m_rowLabel = nullptr;
    QLabel* m_colLabel = nullptr;
    QDialogButtonBox* m_buttons = nullptr;
    QStringList m_existingGridIds;
    bool m_moveModeEnabled = false;
};

#endif // UCOMPONENTGUIGRIDDIALOG_H
