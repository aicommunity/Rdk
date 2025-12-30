#include "UGraphControlDialog.h"
#include "ui_UGraphControlDialog.h"
#include "UGraphPaintWidget.h"
#include "UComponentPropertySelectionWidget.h"
#include <QLineEdit>
#include <QListWidgetItem>
#include <QColorDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

UGraphControlDialog::UGraphControlDialog(QWidget *parent, UGraphPaintWidget *painter) :
    QDialog(parent),
    ui(new Ui::UGraphControlDialog),
    graphPainter(painter),
    selectedGraphIndex(-1),
    graphNameChanged(false),
    graphDataSourceChanged(false),
    newChannelIndex(0),
    newJx(0),
    newJy(0),
    deleteRequested(false)
{
    ui->setupUi(this);
    setWindowTitle(tr("Graph Settings"));

    initializeColorList();

    if (graphPainter) {
        refreshGraphsList(graphPainter->getCurrentItem());
    }

    connect(ui->checkBoxAxisX, SIGNAL(toggled(bool)), this, SLOT(onAxisXToggled(bool)));
    if (ui->colorListWidget) {
        connect(ui->colorListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onColorSelectionChanged(int)));
    }

    // Подключаем новые элементы UI
    if (ui->graphsListWidget) {
        connect(ui->graphsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onGraphSelectionChanged(int)));
    }

    if (ui->lineEditGraphName) {
        connect(ui->lineEditGraphName, SIGNAL(textChanged(QString)), this, SLOT(onGraphNameChanged()));
    }

    if (ui->pushButtonChangeDataSource) {
        connect(ui->pushButtonChangeDataSource, SIGNAL(clicked()), this, SLOT(onChangeDataSourceClicked()));
    }

    if (ui->pushButtonDeleteGraph) {
        connect(ui->pushButtonDeleteGraph, SIGNAL(clicked()), this, SLOT(onDeleteGraphClicked()));
    }
}

UGraphControlDialog::~UGraphControlDialog()
{
    delete ui;
}

void UGraphControlDialog::setGraphPainter(UGraphPaintWidget *painter)
{
    graphPainter = painter;
    if (graphPainter) {
        refreshGraphsList(graphPainter->getCurrentItem());
    }
}

double UGraphControlDialog::getDisplayInterval() const
{
    if (ui->doubleSpinBox) {
        return ui->doubleSpinBox->value();
    }
    return 0.0;
}

int UGraphControlDialog::getAxisXChangeData() const
{
    if (ui->checkBoxAxisX && ui->checkBoxAxisX->isChecked()) {
        return 1;
    }
    return -1;
}

int UGraphControlDialog::getAxisMaxYChangeData() const
{
    if (ui->checkBoxAxisMaxY && ui->checkBoxAxisMaxY->isChecked()) {
        return 1;
    }
    return -1;
}

int UGraphControlDialog::getAxisMinYChangeData() const
{
    if (ui->checkBoxAxisMinY && ui->checkBoxAxisMinY->isChecked()) {
        return 1;
    }
    return -1;
}

int UGraphControlDialog::getColorIndex() const
{
    if (ui->colorListWidget) {
        return ui->colorListWidget->currentRow();
    }
    return -1;
}

QColor UGraphControlDialog::getColor() const
{
    return getColorByIndex(getColorIndex());
}

QString UGraphControlDialog::getLabelX() const
{
    if (ui->lineEditLabelX) {
        return ui->lineEditLabelX->text();
    }
    return QString();
}

QString UGraphControlDialog::getLabelY() const
{
    if (ui->lineEditLabelY) {
        return ui->lineEditLabelY->text();
    }
    return QString();
}

int UGraphControlDialog::getSelectedGraphIndex() const
{
    return selectedGraphIndex;
}

QString UGraphControlDialog::getGraphName() const
{
    return newGraphName;
}

bool UGraphControlDialog::isGraphNameChanged() const
{
    return graphNameChanged;
}

bool UGraphControlDialog::isGraphDataSourceChanged() const
{
    return graphDataSourceChanged;
}

void UGraphControlDialog::getGraphDataSource(int &channelIndex, QString &componentName, QString &propertyName, int &jx, int &jy) const
{
    channelIndex = newChannelIndex;
    componentName = newComponentName;
    propertyName = newPropertyName;
    jx = newJx;
    jy = newJy;
}

bool UGraphControlDialog::shouldDeleteGraph() const
{
    return deleteRequested;
}

void UGraphControlDialog::setDeleteRequested(bool requested)
{
    deleteRequested = requested;
}

void UGraphControlDialog::setAxisXChangeData(int axisX)
{
    if (ui->checkBoxAxisX) {
        ui->checkBoxAxisX->setChecked(axisX != -1);
        onAxisXToggled(axisX != -1);
    }
}

void UGraphControlDialog::setAxisMaxYChangeData(int axisY)
{
    if (ui->checkBoxAxisMaxY) {
        ui->checkBoxAxisMaxY->setChecked(axisY != -1);
    }
}

void UGraphControlDialog::setAxisMinYChangeData(int axisY)
{
    if (ui->checkBoxAxisMinY) {
        ui->checkBoxAxisMinY->setChecked(axisY != -1);
    }
}

void UGraphControlDialog::setDecimalPlaces(int i)
{
    if (ui->doubleSpinBox) {
        ui->doubleSpinBox->setDecimals(i);
    }
}

void UGraphControlDialog::setDisplayInterval(double value)
{
    if (ui->doubleSpinBox) {
        ui->doubleSpinBox->setValue(value);
    }
}

void UGraphControlDialog::setLabelX(const QString &labelX)
{
    if (ui->lineEditLabelX) {
        ui->lineEditLabelX->setText(labelX);
    }
}

void UGraphControlDialog::setLabelY(const QString &labelY)
{
    if (ui->lineEditLabelY) {
        ui->lineEditLabelY->setText(labelY);
    }
}

void UGraphControlDialog::refreshGraphsList(int currentGraphIndex)
{
    if (!graphPainter || !ui->graphsListWidget) {
        return;
    }

    ui->graphsListWidget->clear();

    const int graphCount = graphPainter->getSize();
    for (int i = 0; i < graphCount; ++i)
    {
        const auto &graph = graphPainter->getGraph(i);
        QString itemText = QString::fromStdString(graph.t_graphName);
        if (graph.nameComponent.empty()) {
            itemText += tr(" (not configured)");
        } else {
            itemText += QString(" - %1.%2").arg(QString::fromStdString(graph.nameComponent))
                                           .arg(QString::fromStdString(graph.nameProperty));
        }

        QListWidgetItem *item = new QListWidgetItem(itemText, ui->graphsListWidget);
        QPixmap colorPixmap(16, 16);
        colorPixmap.fill(graph.t_graphColor);
        item->setIcon(QIcon(colorPixmap));
        item->setData(Qt::UserRole, i);
    }

    selectedGraphIndex = currentGraphIndex;
    if (selectedGraphIndex < 0 && graphCount > 0) {
        selectedGraphIndex = 0;
    }

    if (selectedGraphIndex >= 0 && selectedGraphIndex < ui->graphsListWidget->count()) {
        ui->graphsListWidget->setCurrentRow(selectedGraphIndex);
    }

    updateGraphInfo(selectedGraphIndex);
}

void UGraphControlDialog::onDeleteGraphClicked()
{
    if (selectedGraphIndex < 0 || !graphPainter) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a graph to delete."));
        return;
    }

    const auto &graph = graphPainter->getGraph(selectedGraphIndex);
    QString graphName = QString::fromStdString(graph.t_graphName);
    if (graphName.isEmpty()) {
        graphName = tr("Graph %1").arg(selectedGraphIndex + 1);
    }

    int ret = QMessageBox::question(this, tr("Delete Graph"),
                                     tr("Are you sure you want to delete graph \"%1\"?").arg(graphName),
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        deleteRequested = true;
        accept();
    }
}

void UGraphControlDialog::onAxisXToggled(bool enabled)
{
    if (ui->doubleSpinBox) {
        ui->doubleSpinBox->setEnabled(enabled);
    }
}

void UGraphControlDialog::onColorSelectionChanged(int index)
{
    // Обновим предпросмотр цвета, если есть соответствующий виджет
    // Пока что просто сохраняем индекс
}

void UGraphControlDialog::onGraphSelectionChanged(int index)
{
    if (index < 0 || !ui->graphsListWidget) {
        return;
    }

    QListWidgetItem *item = ui->graphsListWidget->item(index);
    if (item) {
        selectedGraphIndex = item->data(Qt::UserRole).toInt();
        updateGraphInfo(selectedGraphIndex);
    }
}

void UGraphControlDialog::onGraphNameChanged()
{
    graphNameChanged = true;
    if (ui->lineEditGraphName) {
        newGraphName = ui->lineEditGraphName->text();
    }
}

void UGraphControlDialog::onChangeDataSourceClicked()
{
    if (selectedGraphIndex < 0 || !graphPainter) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a graph first."));
        return;
    }

    // Откроем диалог выбора источника данных
    // Это будет реализовано через UComponentPropertySelectionWidget
    // Пока что заглушка - нужно передать application из UGraphWidget
    QMessageBox::information(this, tr("Change Data Source"),
                             tr("This feature will be implemented to allow changing the data source of the selected graph."));
    graphDataSourceChanged = true;
}

void UGraphControlDialog::updateGraphInfo(int graphIndex)
{
    if (!graphPainter || graphIndex < 0 || graphIndex >= graphPainter->getSize()) {
        // Очистим поля, если график не выбран
        if (ui->lineEditGraphName) {
            ui->lineEditGraphName->clear();
            ui->lineEditGraphName->setEnabled(false);
        }
        if (ui->labelDataSourceInfo) {
            ui->labelDataSourceInfo->setText(tr("No graph selected"));
        }
        if (ui->pushButtonChangeDataSource) {
            ui->pushButtonChangeDataSource->setEnabled(false);
        }
        if (ui->pushButtonDeleteGraph) {
            ui->pushButtonDeleteGraph->setEnabled(false);
        }
        return;
    }

    const auto &graph = graphPainter->getGraph(graphIndex);

    // Обновим имя графика
    if (ui->lineEditGraphName) {
        ui->lineEditGraphName->setText(QString::fromStdString(graph.t_graphName));
        ui->lineEditGraphName->setEnabled(true);
    }

    // Обновим информацию об источнике данных
    if (ui->labelDataSourceInfo) {
        if (graph.nameComponent.empty()) {
            ui->labelDataSourceInfo->setText(tr("Not configured"));
        } else {
            QString info = QString("%1.%2")
                          .arg(QString::fromStdString(graph.nameComponent))
                          .arg(QString::fromStdString(graph.nameProperty));
            if (graph.Jx != 0 || graph.Jy != 0) {
                info += QString(" [%1, %2]").arg(graph.Jx).arg(graph.Jy);
            }
            ui->labelDataSourceInfo->setText(info);
        }
    }

    if (ui->pushButtonChangeDataSource) {
        ui->pushButtonChangeDataSource->setEnabled(true);
    }

    if (ui->pushButtonDeleteGraph) {
        ui->pushButtonDeleteGraph->setEnabled(true);
    }

    // Установим текущий цвет в списке цветов
    if (ui->colorListWidget) {
        int colorIndex = getColorIndex(graph.t_graphColor);
        if (colorIndex >= 0) {
            ui->colorListWidget->setCurrentRow(colorIndex);
        }
    }
}

void UGraphControlDialog::initializeColorList()
{
    if (!ui->colorListWidget) {
        return;
    }

    // Цвета уже должны быть в UI файле, но можем добавить визуальные индикаторы
    for (int i = 0; i < ui->colorListWidget->count(); ++i) {
        QListWidgetItem *item = ui->colorListWidget->item(i);
        if (item) {
            QColor color = getColorByIndex(i);
            QPixmap pixmap(16, 16);
            pixmap.fill(color);
            item->setIcon(QIcon(pixmap));
        }
    }
}

QColor UGraphControlDialog::getColorByIndex(int index) const
{
    static const QColor colors[] = {
        QColor(Qt::color1),
        QColor(Qt::black),
        QColor(Qt::white),
        QColor(Qt::darkGray),
        QColor(Qt::gray),
        QColor(Qt::lightGray),
        QColor(Qt::red),
        QColor(Qt::green),
        QColor(Qt::blue),
        QColor(Qt::cyan),
        QColor(Qt::magenta),
        QColor(Qt::yellow),
        QColor(Qt::darkRed),
        QColor(Qt::darkGreen),
        QColor(Qt::darkBlue),
        QColor(Qt::darkCyan),
        QColor(Qt::darkMagenta),
        QColor(Qt::darkYellow),
        QColor(Qt::transparent)
    };

    if (index >= 0 && index < 18) {
        return colors[index];
    }
    return QColor(Qt::black);
}

int UGraphControlDialog::getColorIndex(const QColor &color) const
{
    for (int i = 0; i < 18; ++i) {
        if (getColorByIndex(i) == color) {
            return i;
        }
    }
    return 1; // По умолчанию black
}
