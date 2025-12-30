#include "UQuickLinkDialog.h"
#include <QHeaderView>
#include <QMessageBox>

UQuickLinkDialog::UQuickLinkDialog(QWidget *parent, const QString& componentName, RDK::UApplication *app)
    : QDialog(parent)
    , m_componentName(componentName)
    , m_application(app)
{
    setWindowTitle(tr("Quick Link - Create Connection"));
    setMinimumSize(800, 500);

    // Создаем основной layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Заголовок
    QLabel* headerLabel = new QLabel(tr("Select output (source) and input (target) to create a link:"));
    mainLayout->addWidget(headerLabel);

    // Splitter для двух деревьев
    QSplitter* splitter = new QSplitter(Qt::Horizontal);

    // Левая панель - выходы (источники)
    QWidget* outputPanel = new QWidget();
    QVBoxLayout* outputLayout = new QVBoxLayout(outputPanel);
    outputLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* outputLabel = new QLabel(tr("Outputs (Sources):"));
    outputLayout->addWidget(outputLabel);

    m_outputFilter = new QLineEdit();
    m_outputFilter->setPlaceholderText(tr("Filter outputs..."));
    outputLayout->addWidget(m_outputFilter);

    m_outputTree = new QTreeWidget();
    m_outputTree->setHeaderLabels({tr("Component"), tr("Property"), tr("Alias"), tr("Type")});
    m_outputTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_outputTree->setSelectionMode(QAbstractItemView::SingleSelection);
    outputLayout->addWidget(m_outputTree);

    splitter->addWidget(outputPanel);

    // Правая панель - входы (приемники)
    QWidget* inputPanel = new QWidget();
    QVBoxLayout* inputLayout = new QVBoxLayout(inputPanel);
    inputLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* inputLabel = new QLabel(tr("Inputs (Targets):"));
    inputLayout->addWidget(inputLabel);

    m_inputFilter = new QLineEdit();
    m_inputFilter->setPlaceholderText(tr("Filter inputs..."));
    inputLayout->addWidget(m_inputFilter);

    m_inputTree = new QTreeWidget();
    m_inputTree->setHeaderLabels({tr("Component"), tr("Property"), tr("Alias"), tr("Connected")});
    m_inputTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_inputTree->setSelectionMode(QAbstractItemView::SingleSelection);
    inputLayout->addWidget(m_inputTree);

    splitter->addWidget(inputPanel);

    mainLayout->addWidget(splitter);

    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_createButton = new QPushButton(tr("Create Link"));
    m_createButton->setEnabled(false);
    buttonLayout->addWidget(m_createButton);

    m_cancelButton = new QPushButton(tr("Cancel"));
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Подключаем сигналы
    connect(m_outputFilter, &QLineEdit::textChanged, this, &UQuickLinkDialog::onFilterChanged);
    connect(m_inputFilter, &QLineEdit::textChanged, this, &UQuickLinkDialog::onFilterChanged);
    connect(m_outputTree, &QTreeWidget::itemClicked, this, &UQuickLinkDialog::onOutputSelected);
    connect(m_inputTree, &QTreeWidget::itemClicked, this, &UQuickLinkDialog::onInputSelected);
    connect(m_createButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Заполняем списки
    populateOutputs();
    populateInputs();
}

UQuickLinkDialog::~UQuickLinkDialog()
{
}

QString UQuickLinkDialog::getSelectedOutput() const
{
    if(m_selectedSourceComponent.isEmpty())
        return m_selectedSourceProperty;
    return m_selectedSourceComponent + "." + m_selectedSourceProperty;
}

QString UQuickLinkDialog::getSourceComponent() const
{
    return m_selectedSourceComponent;
}

QString UQuickLinkDialog::getSourceProperty() const
{
    return m_selectedSourceProperty;
}

QString UQuickLinkDialog::getSelectedInput() const
{
    if(m_selectedTargetComponent.isEmpty())
        return m_selectedTargetProperty;
    return m_selectedTargetComponent + "." + m_selectedTargetProperty;
}

QString UQuickLinkDialog::getTargetComponent() const
{
    return m_selectedTargetComponent;
}

QString UQuickLinkDialog::getTargetProperty() const
{
    return m_selectedTargetProperty;
}

void UQuickLinkDialog::onFilterChanged(const QString& text)
{
    QLineEdit* sender = qobject_cast<QLineEdit*>(QObject::sender());
    if(sender == m_outputFilter)
    {
        filterTree(m_outputTree, text);
    }
    else if(sender == m_inputFilter)
    {
        filterTree(m_inputTree, text);
    }
}

void UQuickLinkDialog::onOutputSelected(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);

    if(!item || item->childCount() > 0)
    {
        // Это группа, а не конечный элемент
        m_selectedSourceComponent.clear();
        m_selectedSourceProperty.clear();
    }
    else
    {
        m_selectedSourceComponent = item->data(0, Qt::UserRole).toString();
        m_selectedSourceProperty = item->data(1, Qt::UserRole).toString();
    }

    validateSelection();
}

void UQuickLinkDialog::onInputSelected(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);

    if(!item || item->childCount() > 0)
    {
        // Это группа, а не конечный элемент
        m_selectedTargetComponent.clear();
        m_selectedTargetProperty.clear();
    }
    else
    {
        m_selectedTargetComponent = item->data(0, Qt::UserRole).toString();
        m_selectedTargetProperty = item->data(1, Qt::UserRole).toString();
    }

    validateSelection();
}

void UQuickLinkDialog::validateSelection()
{
    bool canCreate = !m_selectedSourceProperty.isEmpty() && !m_selectedTargetProperty.isEmpty();
    m_createButton->setEnabled(canCreate);
}

void UQuickLinkDialog::populateOutputs()
{
    m_outputTree->clear();

    if(!m_application)
        return;

    // Получаем список компонентов
    std::string componentList = Model_GetComponentsNameList(m_componentName.toStdString().c_str());
    QStringList components = QString::fromStdString(componentList).split(",", Qt::SkipEmptyParts);

    // Добавляем компоненты и их выходы
    for(const QString& compName : components)
    {
        QString fullCompName = m_componentName.isEmpty() ? compName : m_componentName + "." + compName;

        // Получаем свойства компонента (выходы)
        std::string propsXml = Model_GetComponentProperties(fullCompName.toStdString().c_str(), ptPubOutput | ptOutput);

        // Парсим XML и добавляем свойства
        // Упрощенный вариант - создаем группу для компонента
        QTreeWidgetItem* compItem = new QTreeWidgetItem(m_outputTree);
        compItem->setText(0, compName);
        compItem->setExpanded(true);

        // Получаем список свойств
        std::string propsList = Model_GetComponentPropertiesLookupList(fullCompName.toStdString().c_str(), ptPubOutput | ptOutput);
        QStringList props = QString::fromStdString(propsList).split(",", Qt::SkipEmptyParts);

        for(const QString& prop : props)
        {
            QStringList parts = prop.split(":");
            if(parts.size() >= 1)
            {
                QString propName = parts[0].trimmed();

                QTreeWidgetItem* propItem = new QTreeWidgetItem(compItem);
                propItem->setText(0, compName);
                propItem->setText(1, propName);
                propItem->setText(2, ""); // Alias
                propItem->setText(3, "Output");

                // Сохраняем данные для использования при создании связи
                propItem->setData(0, Qt::UserRole, compName);
                propItem->setData(1, Qt::UserRole, propName);
            }
        }

        // Если у компонента нет выходов, удаляем его из дерева
        if(compItem->childCount() == 0)
        {
            delete compItem;
        }
    }
}

void UQuickLinkDialog::populateInputs()
{
    m_inputTree->clear();

    if(!m_application)
        return;

    // Получаем список компонентов
    std::string componentList = Model_GetComponentsNameList(m_componentName.toStdString().c_str());
    QStringList components = QString::fromStdString(componentList).split(",", Qt::SkipEmptyParts);

    // Добавляем компоненты и их входы
    for(const QString& compName : components)
    {
        QString fullCompName = m_componentName.isEmpty() ? compName : m_componentName + "." + compName;

        // Получаем свойства компонента (входы)
        QTreeWidgetItem* compItem = new QTreeWidgetItem(m_inputTree);
        compItem->setText(0, compName);
        compItem->setExpanded(true);

        // Получаем список свойств
        std::string propsList = Model_GetComponentPropertiesLookupList(fullCompName.toStdString().c_str(), ptPubInput | ptInput);
        QStringList props = QString::fromStdString(propsList).split(",", Qt::SkipEmptyParts);

        for(const QString& prop : props)
        {
            QStringList parts = prop.split(":");
            if(parts.size() >= 1)
            {
                QString propName = parts[0].trimmed();

                QTreeWidgetItem* propItem = new QTreeWidgetItem(compItem);
                propItem->setText(0, compName);
                propItem->setText(1, propName);
                propItem->setText(2, ""); // Alias
                propItem->setText(3, ""); // Connected status

                // Сохраняем данные для использования при создании связи
                propItem->setData(0, Qt::UserRole, compName);
                propItem->setData(1, Qt::UserRole, propName);
            }
        }

        // Если у компонента нет входов, удаляем его из дерева
        if(compItem->childCount() == 0)
        {
            delete compItem;
        }
    }
}

void UQuickLinkDialog::filterTree(QTreeWidget* tree, const QString& filter)
{
    for(int i = 0; i < tree->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* topItem = tree->topLevelItem(i);
        bool hasVisibleChildren = false;

        for(int j = 0; j < topItem->childCount(); ++j)
        {
            QTreeWidgetItem* childItem = topItem->child(j);
            bool matches = filter.isEmpty() ||
                           childItem->text(0).contains(filter, Qt::CaseInsensitive) ||
                           childItem->text(1).contains(filter, Qt::CaseInsensitive) ||
                           childItem->text(2).contains(filter, Qt::CaseInsensitive);

            childItem->setHidden(!matches);
            if(matches)
                hasVisibleChildren = true;
        }

        topItem->setHidden(!hasVisibleChildren && !filter.isEmpty());
    }
}

QTreeWidgetItem* UQuickLinkDialog::createPropertyItem(const QString& componentName,
                                                       const QString& propertyName,
                                                       const QString& aliasName,
                                                       unsigned int propertyType,
                                                       bool isConnected)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, componentName);
    item->setText(1, propertyName);
    item->setText(2, aliasName);

    if(propertyType & ptOutput)
        item->setText(3, "Output");
    else if(propertyType & ptInput)
        item->setText(3, isConnected ? "Connected" : "");

    item->setData(0, Qt::UserRole, componentName);
    item->setData(1, Qt::UserRole, propertyName);
    item->setData(2, Qt::UserRole, propertyType);

    return item;
}

bool UQuickLinkDialog::areTypesCompatible(unsigned int outputType, unsigned int inputType)
{
    // Базовая проверка совместимости - выход должен быть Output, вход - Input
    // В будущем можно добавить проверку типов данных
    return (outputType & ptOutput) && (inputType & ptInput);
}

