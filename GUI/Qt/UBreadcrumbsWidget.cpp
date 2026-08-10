#include "UBreadcrumbsWidget.h"

#include <QSignalBlocker>
#include <QSizePolicy>
#include "../../Deploy/Include/rdk_init.h"
#include "UGuiTelemetry.h"

namespace {

constexpr char kBreadcrumbButtonObjectName[] = "breadcrumbButton";

QPushButton* createBreadcrumbButton(const QString& text, QWidget* parent)
{
    auto* button = new QPushButton(text, parent);
    button->setObjectName(QLatin1String(kBreadcrumbButtonObjectName));
    button->setFlat(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    button->setMaximumHeight(22);
    button->setStyleSheet(
        QStringLiteral("QPushButton#breadcrumbButton {"
                       "  text-align: left;"
                       "  border: none;"
                       "  border-radius: 0;"
                       "  background: transparent;"
                       "  min-width: 0;"
                       "  padding: 0px 4px;"
                       "}"));
    return button;
}

} // namespace

UBreadcrumbsWidget::UBreadcrumbsWidget(QWidget *parent)
    : QWidget(parent)
{
    breadcrumbsLayout = new QHBoxLayout(this);
    breadcrumbsLayout->setContentsMargins(2, 0, 2, 0);
    breadcrumbsLayout->setSpacing(2);
    breadcrumbsLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    setMinimumHeight(22);
    setMaximumHeight(24);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    updateBreadcrumbs("");
}

UBreadcrumbsWidget::~UBreadcrumbsWidget()
{
}

void UBreadcrumbsWidget::updateBreadcrumbs(const QString &componentPath)
{
    QList<QLayoutItem*> itemsToRemove;
    for (int i = 0; i < breadcrumbsLayout->count(); ++i) {
        QLayoutItem* item = breadcrumbsLayout->itemAt(i);
        if (item) {
            QWidget* widget = item->widget();
            if (widget) {
                itemsToRemove.append(item);
            }
        }
    }

    for (QLayoutItem* item : itemsToRemove) {
        breadcrumbsLayout->removeItem(item);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    breadcrumbButtons.clear();

    QPushButton* modelButton = createBreadcrumbButton(tr("Model"), this);
    connect(modelButton, &QPushButton::clicked, this, [this]() { onBreadcrumbClicked(""); });
    breadcrumbsLayout->addWidget(modelButton);
    breadcrumbButtons.append(modelButton);

    if (componentPath.isEmpty())
        return;

    QStringList pathParts = componentPath.split(".");
    QString currentPath;

    for (int i = 0; i < pathParts.size(); ++i) {
        QLabel* separator = new QLabel(tr(">"), this);
        separator->setStyleSheet(QStringLiteral("QLabel { color: gray; padding: 0px 2px; }"));
        breadcrumbsLayout->addWidget(separator);

        if (currentPath.isEmpty())
            currentPath = pathParts[i];
        else
            currentPath += "." + pathParts[i];

        QPushButton* button = createBreadcrumbButton(pathParts[i], this);
        const QString pathToSelect = currentPath;
        connect(button, &QPushButton::clicked, this, [this, pathToSelect]() {
            onBreadcrumbClicked(pathToSelect);
        });

        breadcrumbsLayout->addWidget(button);
        breadcrumbButtons.append(button);
    }
}

void UBreadcrumbsWidget::onBreadcrumbClicked(const QString &componentPath)
{
    emit componentPathSelected(componentPath);
    updateBreadcrumbs(componentPath);
}
