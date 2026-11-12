#ifndef UWATCH_MATRIX_SELECTOR_H
#define UWATCH_MATRIX_SELECTOR_H

#include <QWidget>
#include <QVector>
#include <QString>

class QTableWidget;
class QLabel;
class QTimer;

enum class MatrixPickMode
{
    SingleCell = 0,
    MultiCells = 1,
    WholeRow = 2,
    WholeColumn = 3
};

/// Inline matrix/vector cell picker for Watch series wizard (non-dialog).
class UWatchMatrixSelector : public QWidget
{
    Q_OBJECT
public:
    struct CellRef
    {
        int jx = 0;
        int jy = 0;
    };

    explicit UWatchMatrixSelector(QWidget* parent = nullptr);

    void setPickMode(MatrixPickMode mode);
    MatrixPickMode pickMode() const { return m_mode; }

    /// Bind to matrix/vector property. Returns false if not a matrix/vector type.
    bool bind(const QString& component, const QString& property);
    void clearBinding();

    bool isScalarProperty() const { return m_scalar; }
    bool isBound() const { return m_propertyType >= 1 && m_propertyType <= 4; }
    bool isReady() const;
    QVector<CellRef> selectedCells() const;

signals:
    void selectionChanged();

private slots:
    void refreshValues();
    void onSelectionChanged();

private:
    void applySelectionPolicy();
    void expandSelectionToMode();

    QLabel* m_label = nullptr;
    QTableWidget* m_table = nullptr;
    QTimer* m_timer = nullptr;

    QString m_component;
    QString m_property;
    int m_propertyType = 0; // 0 unknown, 1..4 as UMatrixFormDialog
    bool m_scalar = true;
    MatrixPickMode m_mode = MatrixPickMode::SingleCell;
};

#endif // UWATCH_MATRIX_SELECTOR_H
