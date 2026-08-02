#include "UWatchSourcePickerWidget.h"

#include "../UComponentsListWidget.h"

#include "rdk.h"
#include <rdk_application.h>
#include "../../Core/Math/UWatchablePropertyTypes.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QSplitter>
#include <typeinfo>

UWatchSourcePickerWidget::UWatchSourcePickerWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);

    m_bindingReadout = new QLabel(tr("No property selected"), this);
    m_bindingReadout->setWordWrap(true);
    root->addWidget(m_bindingReadout);

    m_split = new QSplitter(Qt::Vertical, this);
    root->addWidget(m_split, 1);

    m_matrix = new UWatchMatrixSelector(this);
    m_matrix->setMinimumHeight(100);

    connect(m_matrix, &UWatchMatrixSelector::selectionChanged, this, [this]() {
        updateReadout();
        emit selectionChanged();
    });
}

void UWatchSourcePickerWidget::configureForWatch(RDK::UApplication* app, bool showChannels)
{
    if (m_list)
        return;

    m_list = new UComponentsListWidget(this, app, 0);
    m_list->setUpdateInterval(0);
    m_list->setVerticalOrientation(false);
    m_list->setTreeExpansionPolicy(1);
    m_list->setWatchablePropertiesOnly(true);
    m_list->setChannelsListVisible(showChannels);
    m_list->openTabN(3);
    m_list->UpdateInterface(true);

    m_split->addWidget(m_list);
    m_split->addWidget(m_matrix);
    m_split->setStretchFactor(0, 3);
    m_split->setStretchFactor(1, 2);

    connect(m_list, SIGNAL(componentSelected(QString)), this, SLOT(onListSelectionChanged()));
    connect(m_list, SIGNAL(selectedPropertyValue(QString)), this, SLOT(onListSelectionChanged()));
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
}
