#include "UWatchSourcePickerWidget.h"

#include "../UComponentsListWidgetModern.h"
#include "../UPropertyListOptions.h"

#include "rdk.h"
#include <rdk_application.h>
#include "../../Core/Math/UWatchablePropertyTypes.h"

#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QSplitter>
#include <typeinfo>

UWatchSourcePickerWidget::UWatchSourcePickerWidget(QWidget* parent)
    : QWidget(parent)
{
    m_rootLayout = new QVBoxLayout(this);
    m_rootLayout->setContentsMargins(0, 0, 0, 0);
    m_rootLayout->setSpacing(4);

    m_split = new QSplitter(Qt::Vertical, this);
    m_rootLayout->addWidget(m_split, 1);

    m_matrix = new UWatchMatrixSelector(this);
    m_matrix->setMinimumHeight(48);
    m_matrix->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    m_bindingReadout = new QLabel(tr("No property selected"), this);
    m_bindingReadout->setWordWrap(true);
    m_bindingReadout->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    m_bindingReadout->setStyleSheet(QStringLiteral("color: palette(mid);"));
    m_rootLayout->addWidget(m_bindingReadout, 0);

    connect(m_matrix, &UWatchMatrixSelector::selectionChanged, this, [this]() {
        updateReadout();
        emit selectionChanged();
    });
}

void UWatchSourcePickerWidget::configureForWatch(RDK::UApplication* app, bool showChannels)
{
    if (m_list)
        return;

    m_list = new UComponentsListWidgetModern(this, app, 0);
    m_list->setUpdateInterval(0);
    m_list->setVerticalOrientation(false);
    m_list->setComponentTreeEmbedded(true);
    m_list->setLayoutPreset(ComponentsListLayoutPreset::PropertyPicker);

    PropertyListOptions opt;
    opt.presentation = PropertyListPresentation::UnifiedGrouped;
    opt.visibleGroups = GroupAllIO;
    opt.watchableOnly = true;
    opt.subtitleMode = PropertySubtitleMode::Type;
    opt.allowInlineEdit = false;
    opt.showValueColumn = true;
    opt.expandGroups = true;
    opt.hideTabBarInUnified = true;
    opt.selectLeavesOnly = true;
    m_list->setPropertyListOptions(opt);
    m_list->setChannelsListVisible(showChannels);
    m_list->UpdateInterface(true);
    // Re-apply after UpdateInterface / channel visibility so geometry sticks
    m_list->setLayoutPreset(ComponentsListLayoutPreset::PropertyPicker);

    m_split->addWidget(m_list);
    m_split->addWidget(m_matrix);
    applyLayoutModes();

    connect(m_list, SIGNAL(componentSelected(QString)), this, SLOT(onListSelectionChanged()));
    connect(m_list, SIGNAL(selectedPropertyValue(QString)), this, SLOT(onListSelectionChanged()));
}

void UWatchSourcePickerWidget::setComponentOnlyMode(bool enabled)
{
    m_componentOnly = enabled;
    applyLayoutModes();
}

void UWatchSourcePickerWidget::setTimeSeriesCompactMode(bool enabled)
{
    m_timeSeriesCompact = enabled;
    applyLayoutModes();
}

void UWatchSourcePickerWidget::applyLayoutModes()
{
    if (!m_split)
        return;

    if (m_componentOnly)
    {
        if (m_bindingReadout)
            m_bindingReadout->setVisible(false);
        if (m_matrix)
            m_matrix->setVisible(false);
        m_split->setStretchFactor(0, 1);
        m_split->setStretchFactor(1, 0);
        QList<int> sizes = m_split->sizes();
        if (sizes.size() >= 2)
        {
            const int total = qMax(1, sizes[0] + sizes[1]);
            m_split->setSizes({total, 0});
        }
        return;
    }

    if (m_bindingReadout)
        m_bindingReadout->setVisible(true);

    const bool scalar = m_matrix && m_matrix->isScalarProperty()
                        && !propertyName().isEmpty();
    const bool hideMatrix = m_timeSeriesCompact && scalar;

    if (m_matrix)
    {
        m_matrix->setVisible(!hideMatrix);
        m_matrix->setMinimumHeight(m_timeSeriesCompact ? 40 : 48);
    }

    if (m_timeSeriesCompact)
    {
        m_split->setStretchFactor(0, 8);
        m_split->setStretchFactor(1, hideMatrix ? 0 : 1);
        if (hideMatrix)
        {
            QList<int> sizes = m_split->sizes();
            if (sizes.size() >= 2)
            {
                const int total = qMax(1, sizes[0] + sizes[1]);
                m_split->setSizes({total, 0});
            }
        }
    }
    else
    {
        m_split->setStretchFactor(0, 5);
        m_split->setStretchFactor(1, 1);
    }
}

void UWatchSourcePickerWidget::onListSelectionChanged()
{
    updateBindingFromList();
}

void UWatchSourcePickerWidget::updateBindingFromList()
{
    if (!m_list || !m_matrix)
        return;

    const QString comp = m_list->getSelectedComponentLongName();
    const QString prop = m_list->getSelectedPropertyName();
    if (comp.isEmpty() || prop.isEmpty())
    {
        m_matrix->clearBinding();
        applyLayoutModes();
        updateReadout();
        emit selectionChanged();
        return;
    }

    bool watchable = false;
    {
        RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>();
        if (model)
        {
            RDK::UContainer* cont = model->GetComponentL(comp.toStdString());
            if (cont)
            {
                RDK::UEPtr<RDK::UIProperty> p = cont->FindProperty(prop.toStdString());
                if (p)
                    watchable = RDK::isWatchableLanguageType(p->GetLanguageType());
            }
        }
    }

    if (!watchable)
        m_matrix->clearBinding();
    else if (!m_matrix->bind(comp, prop))
        m_matrix->clearBinding();

    applyLayoutModes();
    updateReadout();
    emit selectionChanged();
}

void UWatchSourcePickerWidget::updateReadout()
{
    if (!m_bindingReadout)
        return;
    const QString comp = componentLongName();
    const QString prop = propertyName();
    if (comp.isEmpty() || prop.isEmpty())
    {
        m_bindingReadout->setText(tr("No property selected"));
        return;
    }
    const auto cellList = cells();
    if (m_matrix && m_matrix->isScalarProperty())
    {
        m_bindingReadout->setText(tr("ch %1: %2.%3").arg(channelIndex()).arg(comp, prop));
        return;
    }
    if (cellList.size() == 1)
    {
        m_bindingReadout->setText(tr("ch %1: %2.%3[%4,%5]")
                                      .arg(channelIndex())
                                      .arg(comp, prop)
                                      .arg(cellList[0].jx)
                                      .arg(cellList[0].jy));
    }
    else
    {
        m_bindingReadout->setText(tr("ch %1: %2.%3 (%4 cells)")
                                      .arg(channelIndex())
                                      .arg(comp, prop)
                                      .arg(cellList.size()));
    }
}

int UWatchSourcePickerWidget::channelIndex() const
{
    return m_list ? m_list->getSelectedChannelIndex() : 0;
}

QString UWatchSourcePickerWidget::componentLongName() const
{
    return m_list ? m_list->getSelectedComponentLongName() : QString();
}

QString UWatchSourcePickerWidget::propertyName() const
{
    return m_list ? m_list->getSelectedPropertyName() : QString();
}

QVector<UWatchMatrixSelector::CellRef> UWatchSourcePickerWidget::cells() const
{
    if (!m_matrix)
        return {};
    auto c = m_matrix->selectedCells();
    if (c.isEmpty() && m_matrix->isScalarProperty() && !propertyName().isEmpty())
        return {UWatchMatrixSelector::CellRef{0, 0}};
    return c;
}

bool UWatchSourcePickerWidget::isComplete() const
{
    if (propertyName().isEmpty() || componentLongName().isEmpty())
        return false;
    if (!m_matrix)
        return false;
    if (m_matrix->isScalarProperty())
        return true;
    return m_matrix->isReady();
}

void UWatchSourcePickerWidget::setMatrixPickMode(MatrixPickMode mode)
{
    if (m_matrix)
        m_matrix->setPickMode(mode);
    applyLayoutModes();
}
