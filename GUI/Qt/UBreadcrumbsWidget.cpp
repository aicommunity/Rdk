#include "UBreadcrumbsWidget.h"
#include "UStyleManager.h"

#include <QSignalBlocker>
#include <QSizePolicy>
#include <QFontMetrics>
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
    const int chromeH = qMax(18, UStyleManager::densitySpace(5));
    button->setMaximumHeight(chromeH);
    button->setMinimumHeight(chromeH - 2);
    const QColor textColor = UStyleManager::instance()->getTextColor();
    button->setStyleSheet(
        QStringLiteral("QPushButton#breadcrumbButton {"
                       "  text-align: left;"
                       "  border: none;"
                       "  border-radius: 0;"
                       "  background: transparent;"
                       "  min-width: 0;"
                       "  padding: 0px %1px;"
                       "  color: %2;"
                       "}")
            .arg(UStyleManager::densitySpace(1))
            .arg(textColor.name()));
    return button;
}

} // namespace

UBreadcrumbsWidget::UBreadcrumbsWidget(QWidget *parent)
    : QWidget(parent)
{
    breadcrumbsLayout = new QHBoxLayout(this);
    breadcrumbsLayout->setContentsMargins(UStyleManager::densitySpace(1), 0,
                                          UStyleManager::densitySpace(1), 0);
    breadcrumbsLayout->setSpacing(UStyleManager::densitySpace(1));
    breadcrumbsLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    const int chromeH = qMax(20, UStyleManager::densitySpace(5));
    setMinimumHeight(chromeH);
    setMaximumHeight(chromeH + 4);

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
        const QColor sepColor = UStyleManager::instance()->getTextSecondaryColor();
        separator->setStyleSheet(QStringLiteral("QLabel { color: %1; padding: 0px %2px; }")
                                     .arg(sepColor.name())
                                     .arg(UStyleManager::densitySpace(1)));
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
