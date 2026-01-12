#include "UBreadcrumbsWidget.h"

#include <QSignalBlocker>
#include "../../Deploy/Include/rdk_init.h"
#include "UGuiTelemetry.h"

UBreadcrumbsWidget::UBreadcrumbsWidget(QWidget *parent)
    : QWidget(parent)
{
    // Настройка layout - выравнивание по левому краю
    breadcrumbsLayout = new QHBoxLayout(this);
    breadcrumbsLayout->setContentsMargins(4, 2, 4, 2);
    breadcrumbsLayout->setSpacing(4);
    breadcrumbsLayout->setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

    // Устанавливаем минимальную высоту
    setMinimumHeight(30);
    setMaximumHeight(35);

    // Инициализация breadcrumbs (пустой путь = Model)
    updateBreadcrumbs("");
}

UBreadcrumbsWidget::~UBreadcrumbsWidget()
{
}

void UBreadcrumbsWidget::updateBreadcrumbs(const QString &componentPath)
{
    QString logMsg = QString("[SELECTION_DEBUG] UBreadcrumbsWidget::updateBreadcrumbs: called with componentPath='%1'")
        .arg(componentPath);
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

    // Очищаем все кнопки breadcrumbs и разделители
    QList<QLayoutItem*> itemsToRemove;
    for (int i = 0; i < breadcrumbsLayout->count(); ++i) {
        QLayoutItem* item = breadcrumbsLayout->itemAt(i);
        if (item) {
            QWidget* widget = item->widget();
            // Удаляем все виджеты (кнопки и разделители)
            if (widget) {
                itemsToRemove.append(item);
            }
        }
    }

    // Удаляем найденные виджеты
    for (QLayoutItem* item : itemsToRemove) {
        breadcrumbsLayout->removeItem(item);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    breadcrumbButtons.clear();

    // Если путь пустой, показываем только "Model"
    if (componentPath.isEmpty()) {
        QPushButton* modelButton = new QPushButton(tr("Model"), this);
        modelButton->setFlat(true);
        modelButton->setStyleSheet("QPushButton { text-align: left; border: none; padding: 2px; }");
        connect(modelButton, &QPushButton::clicked, this, [this]() { onBreadcrumbClicked(""); });
        breadcrumbsLayout->addWidget(modelButton);
        breadcrumbButtons.append(modelButton);
    } else {
        // Добавляем кнопку "Model"
        QPushButton* modelButton = new QPushButton(tr("Model"), this);
        modelButton->setFlat(true);
        modelButton->setStyleSheet("QPushButton { text-align: left; border: none; padding: 2px; }");
        connect(modelButton, &QPushButton::clicked, this, [this]() { onBreadcrumbClicked(""); });
        breadcrumbsLayout->addWidget(modelButton);
        breadcrumbButtons.append(modelButton);

        // Разбиваем путь на части и создаем кнопки для каждого уровня
        QStringList pathParts = componentPath.split(".");
        QString currentPath = "";

        for (int i = 0; i < pathParts.size(); ++i) {
            // Добавляем разделитель
            QLabel* separator = new QLabel(tr(">"), this);
            separator->setStyleSheet("QLabel { color: gray; padding: 2px; }");
            breadcrumbsLayout->addWidget(separator);

            // Формируем путь до текущего уровня
            if (currentPath.isEmpty()) {
                currentPath = pathParts[i];
            } else {
                currentPath += "." + pathParts[i];
            }

            // Создаем кнопку для текущего уровня
            QPushButton* button = new QPushButton(pathParts[i], this);
            button->setFlat(true);
            button->setStyleSheet("QPushButton { text-align: left; border: none; padding: 2px; }");

            QString pathToSelect = currentPath;
            connect(button, &QPushButton::clicked, this, [this, pathToSelect]() { onBreadcrumbClicked(pathToSelect); });

            breadcrumbsLayout->addWidget(button);
            breadcrumbButtons.append(button);
        }
    }
}

void UBreadcrumbsWidget::onBreadcrumbClicked(const QString &componentPath)
{
    QString logMsg = QString("[SELECTION_DEBUG] UBreadcrumbsWidget::onBreadcrumbClicked: clicked on path='%1', emitting componentPathSelected")
        .arg(componentPath);
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

    // Эмитируем сигнал для внешних обработчиков
    emit componentPathSelected(componentPath);
}

