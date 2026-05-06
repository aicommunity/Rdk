#ifndef UCOMPONENTGUIGRIDCONTAINERWIDGET_H
#define UCOMPONENTGUIGRIDCONTAINERWIDGET_H

#include <QVector>
#include <QPointer>
#include <QSplitter>
#include <QWidget>
#include <QString>

#include "UVisualControllerWidget.h"
#include "UComponentGuiContext.h"

class UComponentGuiService;
class QVBoxLayout;

class UComponentGuiGridContainerWidget : public UVisualControllerWidget
{
    Q_OBJECT
public:
    explicit UComponentGuiGridContainerWidget(const QString& gridId,
                                              UComponentGuiService* service,
                                              QWidget *parent = nullptr,
                                              RDK::UApplication *app = nullptr);

    QString gridId() const;
    void setGridSize(int rows, int cols);
    int rowCount() const;
    int colCount() const;
    bool assignCell(int row, int col, const UComponentGuiContext& context);
    bool clearCell(int row, int col);
    bool swapCells(int r1, int c1, int r2, int c2);
    bool hasContext(int row, int col) const;
    UComponentGuiContext contextAt(int row, int col) const;
    QByteArray saveGridState() const;
    void restoreGridState(const QByteArray& state);
    QByteArray saveRowState(int row) const;
    void restoreRowState(int row, const QByteArray& state);
    QWidget* cellHostWidget(int row, int col) const;

private:
    struct UGridCell
    {
        QPointer<QWidget> host;
        bool hasContext = false;
        UComponentGuiContext context;
    };

    bool isValidCell(int row, int col) const;
    void rebuildGrid();
    void clearGridWidgets();

private:
    QString m_gridId;
    UComponentGuiService* m_service = nullptr;
    QVBoxLayout* m_rootLayout = nullptr;
    QSplitter* m_rootSplitter = nullptr;
    int m_rows = 1;
    int m_cols = 1;
    QVector<QSplitter*> m_rowSplitters;
    QVector<QVector<UGridCell>> m_cells;
};

#endif // UCOMPONENTGUIGRIDCONTAINERWIDGET_H
