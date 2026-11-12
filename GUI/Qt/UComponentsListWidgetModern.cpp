#include "UComponentsListWidgetModern.h"
#include "ui_UComponentsListWidgetModern.h"

#include <QDebug>
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include <QSizePolicy>
#include <QList>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <QSet>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QTabBar>

#include "UGuiTelemetry.h"
#include "UEngineSelectionSync.h"
#include "UComponentGuiService.h"
#include "UComponentFormRegistry.h"

#include "UModernDiagramWidget.h"
#include "UPropertyXMLWidget.h"
#include "Plot/PlotDocument.h"
#include "../../Core/Math/UWatchablePropertyTypes.h"

#include <QApplication>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTreeWidget>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QShortcut>
#include <QLineEdit>
#include <QToolTip>
#include <QCursor>
#include <QAbstractItemView>
#include <QListView>
#include <QHeaderView>

#include "../../Deploy/Include/rdk_error_codes.h"
#include "../../Core/Utilities/USupport.h"
#include "../../Core/Engine/UComponent.h"
#include "../../Core/Engine/UNet.h"
#include "../../Core/Engine/UContainerDescription.h"

namespace {

bool fetchPropertyValueFromModel(int channel,
                                 const QString& componentLongName,
                                 const QString& propertyName,
                                 std::string& outValue)
{
 RDK::UELockPtr<RDK::UContainer> model =
     RDK::GetModelLockTimeout(channel, 250);
 if(!model)
  return false;

 RDK::UEPtr<RDK::UContainer> cont;
 if(componentLongName.isEmpty())
  cont = model.Get();
 else
  cont = model->GetComponentL(componentLongName.toLocal8Bit().constData(), true);

 RDK::UEPtr<RDK::UNet> unet = RDK::dynamic_pointer_cast<RDK::UNet>(cont);
 if(!unet)
  return false;

 unet->GetPropertyValue(propertyName.toStdString(), outValue);
 return true;
}

constexpr int kFavRoleProp = Qt::UserRole;           // property name (unified + favorites)
constexpr int kFavRoleDisplayPath = Qt::UserRole + 1;  // subtitle text
constexpr int kFavRoleComponent = Qt::UserRole + 2;
constexpr int kPropRoleIsGroup = Qt::UserRole + 3;
constexpr int kMinNameColumnWidth = 120;
constexpr int kMinEditorWidth = 120;

QTreeWidgetItem* propertyItemFromIndex(QTreeWidget* tree, const QModelIndex& index)
{
    if(!tree || !index.isValid())
        return nullptr;
    // QTreeWidget::itemFromIndex is protected in Qt5 — use the model index pointer instead.
    if(auto* item = static_cast<QTreeWidgetItem*>(index.internalPointer()))
        return item;
    return tree->currentItem();
}

/// True when a temporary item-delegate editor (QLineEdit) still has focus under the tree.
bool treeHasOpenPropertyEditor(QTreeWidget* tree)
{
    if(!tree)
        return false;
    QWidget* fw = QApplication::focusWidget();
    if(!fw)
        return false;
    if(!qobject_cast<QLineEdit*>(fw))
        return false;
    return tree->viewport()->isAncestorOf(fw) || fw->parentWidget() == tree->viewport();
}

void commitAndClosePropertyEditor(QStyledItemDelegate* delegate, QWidget* editor)
{
    if(!delegate || !editor)
        return;
    Q_EMIT delegate->commitData(editor);
    Q_EMIT delegate->closeEditor(editor, QAbstractItemDelegate::SubmitModelCache);
}

void wirePropertyValueEditor(QLineEdit* edit, const QStyledItemDelegate* delegate)
{
    if(!edit || !delegate)
        return;
    Q_UNUSED(delegate);
    edit->setFrame(false);
    edit->setToolTip(QObject::tr("Enter — apply, Esc — cancel"));
    // Global QLineEdit QSS uses 8px padding — too tall for compact property rows.
    // Favorites rows are taller (subtitle), so the same padding looked fine there.
    edit->setStyleSheet(QStringLiteral(
        "QLineEdit { padding: 1px 4px; border-radius: 2px; min-height: 0px; }"
        "QLineEdit:focus { padding: 1px 4px; border-width: 1px; }"));
}

void updatePropertyEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, QTreeWidget* tree)
{
    if(!editor)
        return;
    QRect r = option.rect;
    if(r.width() < kMinEditorWidth)
        r.setWidth(kMinEditorWidth);

    // Compact property rows are shorter than Favorites (no subtitle). Ensure
    // the line edit is at least one text line tall even if the cell is tight.
    const int minH = qMax(option.fontMetrics.height() + 6, 22);
    if(r.height() < minH)
    {
        const int delta = minH - r.height();
        r.setTop(r.top() - delta / 2);
        r.setHeight(minH);
    }

    if(tree && tree->viewport())
    {
        const int vpRight = tree->viewport()->width() - 2;
        if(r.right() > vpRight)
        {
            r.moveRight(vpRight);
            if(r.width() < kMinEditorWidth)
                r.setLeft(qMax(0, vpRight - kMinEditorWidth + 1));
            if(r.left() < 0)
                r.setLeft(0);
        }
        const QRect vp = tree->viewport()->rect();
        if(r.top() < vp.top())
            r.moveTop(vp.top());
        if(r.bottom() > vp.bottom())
            r.moveBottom(vp.bottom());
    }
    editor->setGeometry(r);
}

bool editMultilineValueDialog(QWidget* parent, QString& value)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(QObject::tr("Edit value"));
    dialog.resize(480, 320);
    auto* layout = new QVBoxLayout(&dialog);
    auto* edit = new QPlainTextEdit(&dialog);
    edit->setPlainText(value);
    layout->addWidget(edit);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    auto* ctrlEnter = new QShortcut(QKeySequence(QStringLiteral("Ctrl+Return")), &dialog);
    QObject::connect(ctrlEnter, &QShortcut::activated, &dialog, &QDialog::accept);
    if(dialog.exec() != QDialog::Accepted)
        return false;
    value = edit->toPlainText();
    return true;
}

class FavoritesPathSubtitleDelegate : public QStyledItemDelegate
{
public:
    explicit FavoritesPathSubtitleDelegate(QTreeWidget* tree, UComponentsListWidgetModern* owner)
        : QStyledItemDelegate(tree)
        , m_tree(tree)
        , m_owner(owner)
    {
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QSize sz = QStyledItemDelegate::sizeHint(option, index);
        const QString path = displayPathFor(index);
        if(path.isEmpty())
            return sz;

        QFont pathFont = option.font;
        pathFont.setPointSizeF(qMax(8.0, pathFont.pointSizeF() * 0.85));
        const int pathH = QFontMetrics(pathFont).height();
        sz.setHeight(qMax(sz.height(), option.fontMetrics.height() + 1) + pathH + 1);
        return sz;
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        const QString path = displayPathFor(index);
        if(path.isEmpty())
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        QStyleOptionViewItem topOpt(option);
        initStyleOption(&topOpt, index);

        const int topH = option.fontMetrics.height() + 1;
        topOpt.rect = QRect(option.rect.left(), option.rect.top(),
                            option.rect.width(), qMin(topH, option.rect.height()));

        // Фон на всю высоту ячейки (включая subtitle)
        {
            QStyleOptionViewItem bgOpt(option);
            initStyleOption(&bgOpt, index);
            bgOpt.text.clear();
            bgOpt.icon = QIcon();
            bgOpt.features &= ~QStyleOptionViewItem::HasDisplay;
            bgOpt.features &= ~QStyleOptionViewItem::HasCheckIndicator;
            bgOpt.checkState = Qt::Unchecked;
            const QWidget* widget = option.widget;
            QStyle* style = widget ? widget->style() : QApplication::style();
            style->drawPrimitive(QStyle::PE_PanelItemViewItem, &bgOpt, painter, widget);
        }

        if(index.column() == 0)
        {
            const QColor textColor = (option.state & QStyle::State_Selected)
                ? option.palette.color(QPalette::HighlightedText)
                : option.palette.color(QPalette::Text);

            painter->save();
            painter->setPen(textColor);
            painter->setFont(option.font);
            const QRect nameRect = topOpt.rect.adjusted(3, 0, -3, 0);
            const QString name = option.fontMetrics.elidedText(
                topOpt.text, Qt::ElideRight, nameRect.width());
            painter->drawText(nameRect, Qt::AlignVCenter | Qt::AlignLeft, name);
            painter->restore();
        }
        else
        {
            // Value / checkbox только в верхней половине (без повторного фона)
            QStyleOptionViewItem valueOpt(topOpt);
            valueOpt.state &= ~QStyle::State_Selected;
            valueOpt.state &= ~QStyle::State_MouseOver;
            valueOpt.state &= ~QStyle::State_HasFocus;
            QStyledItemDelegate::paint(painter, valueOpt, index);
        }

        drawPathSubtitle(painter, option, index, path, topH);
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        Q_UNUSED(option);
        if(index.column() != 1)
            return nullptr;
        if(m_owner && !m_owner->propertyListOptions().allowInlineEdit)
            return nullptr;
        auto* edit = new QLineEdit(parent);
        edit->setText(index.data(Qt::UserRole).toString());
        wirePropertyValueEditor(edit, this);
        return edit;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        if(auto* lineEdit = qobject_cast<QLineEdit*>(editor))
            lineEdit->setText(index.data(Qt::UserRole).toString());
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
    {
        auto* lineEdit = qobject_cast<QLineEdit*>(editor);
        if(!lineEdit || !m_tree || !m_owner)
            return;
        QTreeWidgetItem* item = propertyItemFromIndex(m_tree, index);
        if(!item)
            return;
        if(!m_owner->applyPropertyValueFromEditor(item, lineEdit->text()))
            return;
        if(model)
            model->setData(index, item->text(1), Qt::DisplayRole);
    }

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override
    {
        Q_UNUSED(index);
        updatePropertyEditorGeometry(editor, option, m_tree);
    }

    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if(event->type() == QEvent::KeyPress)
        {
            const int key = static_cast<QKeyEvent*>(event)->key();
            if(key == Qt::Key_Return || key == Qt::Key_Enter)
            {
                if(auto* editor = qobject_cast<QWidget*>(watched))
                {
                    commitAndClosePropertyEditor(this, editor);
                    return true;
                }
            }
        }
        return QStyledItemDelegate::eventFilter(watched, event);
    }

private:
    static QString displayPathFor(const QModelIndex& index)
    {
        const QModelIndex nameIndex = index.sibling(index.row(), 0);
        return nameIndex.data(kFavRoleDisplayPath).toString();
    }

    void drawPathSubtitle(QPainter* painter,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index,
                          const QString& path,
                          int topH) const
    {
        const QColor textColor = (option.state & QStyle::State_Selected)
            ? option.palette.color(QPalette::HighlightedText)
            : option.palette.color(QPalette::Text);

        QFont pathFont = option.font;
        pathFont.setPointSizeF(qMax(8.0, pathFont.pointSizeF() * 0.85));
        QFontMetrics pathFm(pathFont);
        QColor pathColor = textColor;
        pathColor.setAlpha(option.state & QStyle::State_Selected ? 200 : 150);

        int pathLeft = option.rect.left() + 3;
        const QModelIndex nameIndex = index.sibling(index.row(), 0);
        if(m_tree)
        {
            const QRect nameVisual = m_tree->visualRect(nameIndex);
            if(nameVisual.isValid())
                pathLeft = nameVisual.left() + 3;
        }

        int pathRight = option.rect.right() - 3;
        if(m_tree && m_tree->viewport())
            pathRight = m_tree->viewport()->width() - 4;

        const QRect pathRect(pathLeft,
                             option.rect.top() + topH,
                             qMax(0, pathRight - pathLeft),
                             qMax(0, option.rect.height() - topH));
        const QRect clipRect(option.rect.left(),
                             option.rect.top() + topH,
                             option.rect.width(),
                             qMax(0, option.rect.height() - topH));

        painter->save();
        painter->setClipRect(clipRect);
        painter->setPen(pathColor);
        painter->setFont(pathFont);
        const QString elidedPath = pathFm.elidedText(path, Qt::ElideMiddle, pathRect.width());
        painter->drawText(pathRect, Qt::AlignVCenter | Qt::AlignLeft, elidedPath);
        painter->restore();
    }

    QTreeWidget* m_tree = nullptr;
    UComponentsListWidgetModern* m_owner = nullptr;
};

class PropertyValueEditDelegate : public QStyledItemDelegate
{
public:
    PropertyValueEditDelegate(QTreeWidget* tree, UComponentsListWidgetModern* owner)
        : QStyledItemDelegate(tree)
        , m_tree(tree)
        , m_owner(owner)
    {
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        Q_UNUSED(option);
        if(index.column() != 1)
            return nullptr;
        auto* edit = new QLineEdit(parent);
        edit->setText(index.data(Qt::UserRole).toString());
        wirePropertyValueEditor(edit, this);
        return edit;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        if(auto* lineEdit = qobject_cast<QLineEdit*>(editor))
            lineEdit->setText(index.data(Qt::UserRole).toString());
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
    {
        auto* lineEdit = qobject_cast<QLineEdit*>(editor);
        if(!lineEdit || !m_tree || !m_owner)
            return;
        QTreeWidgetItem* item = propertyItemFromIndex(m_tree, index);
        if(!item)
            return;
        if(!m_owner->applyPropertyValueFromEditor(item, lineEdit->text()))
            return;
        if(model)
            model->setData(index, item->text(1), Qt::DisplayRole);
    }

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override
    {
        Q_UNUSED(index);
        updatePropertyEditorGeometry(editor, option, m_tree);
    }

    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if(event->type() == QEvent::KeyPress)
        {
            const int key = static_cast<QKeyEvent*>(event)->key();
            if(key == Qt::Key_Return || key == Qt::Key_Enter)
            {
                if(auto* editor = qobject_cast<QWidget*>(watched))
                {
                    commitAndClosePropertyEditor(this, editor);
                    return true;
                }
            }
        }
        return QStyledItemDelegate::eventFilter(watched, event);
    }

private:
    QTreeWidget* m_tree = nullptr;
    UComponentsListWidgetModern* m_owner = nullptr;
};

} // namespace

UComponentsListWidgetModern::UComponentsListWidgetModern(QWidget *parent, RDK::UApplication *app, int channel_mode) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UComponentsListWidgetModern)
{
    channelMode=channel_mode;
    CheckModelFlag=false;
    ui->setupUi(this);

    // Создание кнопки переключения режимов (одна строка с классом компонента)
    toggleModeButton = new QToolButton(this);
    toggleModeButton->setText(tr("☰"));
    toggleModeButton->setToolTip(tr("Показать/скрыть дерево компонентов"));
    toggleModeButton->setCheckable(true);
    toggleModeButton->setChecked(false); // false = компактный режим
    toggleModeButton->setFixedSize(24, 24);
    toggleModeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->horizontalLayoutTreeWidget->setAlignment(Qt::AlignVCenter);
    // Порядок: [channels?] [☰] [className…]
    ui->horizontalLayoutTreeWidget->insertWidget(1, toggleModeButton);
    connect(toggleModeButton, &QToolButton::clicked, this, &UComponentsListWidgetModern::toggleTreeViewMode);

    ui->labelComponentClassName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->labelComponentClassName->setMaximumHeight(24);
    ui->labelComponentClassName->setMinimumHeight(20);
    ui->labelComponentClassName->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    // Шапка не должна раздуваться за счёт splitter — место уходит вкладкам свойств
    if(QWidget* headerPane = ui->splitter->widget(0))
        headerPane->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);

    // Создание дерева компонентов
    componentsTree = new UComponentListTreeWidgetModern(this);
    QWidget *treeContainer = ui->treeContainer;
    QVBoxLayout *treeLayout = qobject_cast<QVBoxLayout*>(treeContainer->layout());
    if (!treeLayout) {
        treeLayout = new QVBoxLayout(treeContainer);
        treeLayout->setContentsMargins(0,0,0,0);
    }
    filterLineEdit = new QLineEdit(this);
    filterLineEdit->setObjectName(QStringLiteral("componentsFilterLineEdit"));
    filterLineEdit->setPlaceholderText(tr("Фильтр компонентов..."));
    filterLineEdit->setClearButtonEnabled(true);
    connect(filterLineEdit, &QLineEdit::textChanged,
            this, &UComponentsListWidgetModern::handleFilterTextChanged);
    // Виджеты будут перемещаться между контейнерами
    treeLayout->addWidget(filterLineEdit);
    treeLayout->addWidget(componentsTree);

    // Создание popup окна для дерева компонентов
    treePopupDialog = new QDialog(this, Qt::Popup);
    treePopupDialog->setWindowModality(Qt::NonModal);
    treePopupDialog->setAttribute(Qt::WA_DeleteOnClose, false);
    treePopupDialog->setStyleSheet(
        "QDialog { "
        "background-color: palette(window); "
        "border: 1px solid palette(mid); "
        "border-radius: 4px; "
        "}"
    );
    // Закрытие при потере фокуса (клик вне окна)
    connect(treePopupDialog, &QDialog::finished, this, [this]() {
        if (treePopupDialog->isVisible()) {
            hideTreePopup();
        }
    });
    treePopupContainer = new QWidget(treePopupDialog);
    treePopupLayout = new QVBoxLayout(treePopupContainer);
    treePopupLayout->setContentsMargins(4, 4, 4, 4);
    treePopupLayout->setSpacing(4);

    QVBoxLayout *dialogLayout = new QVBoxLayout(treePopupDialog);
    dialogLayout->setContentsMargins(0, 0, 0, 0);
    dialogLayout->addWidget(treePopupContainer);

    // Устанавливаем фильтр событий для обработки Esc
    treePopupDialog->installEventFilter(this);

    connect(componentsTree, SIGNAL(moveComponentUp()), this, SLOT(componentMoveUp()));
    connect(componentsTree, SIGNAL(moveComponentDown()), this, SLOT(componentMoveDown()));

    // Всегда используем компактный режим - дерево скрыто в основном layout
    ui->treeContainer->hide();
    toggleModeButton->setChecked(false);
    toggleModeButton->setText(tr("☰"));
    toggleModeButton->setToolTip(tr("Показать дерево компонентов"));

    currentChannel = Core_GetSelectedChannelIndex();
    ui->listWidgetChannelSelection->hide();
    channelsSelectionVisible = false;

    UpdateInterval = -1;
    setAccessibleName("UComponentsListWidgetModern"); // пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    //readSettings(app, settingsGroup);

    renderedSnapshotVersion = 0;
    lastSnapshot = NMSDK::UGuiModelSnapshot::Instance().CurrentSnapshot();
    componentFilterText.clear();
    connect(&NMSDK::UGuiModelSnapshot::Instance(), &NMSDK::UGuiModelSnapshot::SnapshotUpdated,
            this, &UComponentsListWidgetModern::handleSnapshotUpdated);

    UpdateInterface(true);

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
    connect(componentsTree, SIGNAL(itemSelectionChanged()),
            this, SLOT(componentListItemSelectionChanged()));

    // Обработка одинарного клика для popup режима
    connect(componentsTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(onComponentItemClicked(QTreeWidgetItem*, int)));

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    connect(componentsTree, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(drawSelectedComponent(QModelIndex)));

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ propertys
    connect(ui->treeWidgetParameters, SIGNAL(itemSelectionChanged()),
            this, SLOT(parametersListSelectionChanged()));
    connect(ui->treeWidgetState, SIGNAL(itemSelectionChanged()),
            this, SLOT(stateListSelectionChanged()));
    connect(ui->treeWidgetInputs, SIGNAL(itemSelectionChanged()),
            this, SLOT(inputsListSelectionChanged()));
    connect(ui->treeWidgetOutputs, SIGNAL(itemSelectionChanged()),
            this, SLOT(outputsListSelectionChanged()));
    connect(ui->treeWidgetFavorites, SIGNAL(itemSelectionChanged()),
            this, SLOT(favoritesListSelectionChanged()));

    connect(ui->treeWidgetParameters, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
            this, SLOT(parametersListItemChanged(QTreeWidgetItem *, int )));
    connect(ui->treeWidgetFavorites, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
            this, SLOT(favoritesListItemChanged(QTreeWidgetItem *, int )));
    connect(ui->treeWidgetState, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
            this, SLOT(stateListItemChanged(QTreeWidgetItem *, int )));
    connect(ui->treeWidgetInputs, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
            this, SLOT(inputsListItemChanged(QTreeWidgetItem *, int )));
    connect(ui->treeWidgetOutputs, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
            this, SLOT(outputsListItemChanged(QTreeWidgetItem *, int )));

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
    connect(ui->listWidgetChannelSelection, SIGNAL(itemSelectionChanged()), this, SLOT(channelsListSelectionChanged()));

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    QAction *actionSeparator1 = new QAction(this);
    actionSeparator1->setSeparator(true);
    QAction *actionSeparator2 = new QAction(this);
    actionSeparator2->setSeparator(true);
    QAction *actionSeparator3 = new QAction(this);
    actionSeparator3->setSeparator(true);
    QAction *actionSeparator4 = new QAction(this);
    actionSeparator4->setSeparator(true);
    QAction *actionSeparator5 = new QAction(this);
    actionSeparator5->setSeparator(true);
    componentsTree->addAction(ui->actionComponentMoveUp);
    componentsTree->addAction(ui->actionComponentMoveDown);
    componentsTree->addAction(actionSeparator1);
    componentsTree->addAction(ui->actionComponentRename);
    componentsTree->addAction(ui->actionComponentDelete);
    componentsTree->addAction(actionSeparator2);
    componentsTree->addAction(ui->actionComponentReset);
    componentsTree->addAction(ui->actionComponentCalculate);
    componentsTree->addAction(ui->actionDefaultAllParameters);
    componentsTree->addAction(ui->actionComponentInit);
    componentsTree->addAction(ui->actionComponentUnInit);
    componentsTree->addAction(actionSeparator3);
    componentsTree->addAction(ui->actionCopyNameToClipboard);
    componentsTree->addAction(ui->actionCopyLongNameToClipboard);
    componentsTree->addAction(ui->actionCopyClassNameToClipboard);
    componentsTree->addAction(actionSeparator4);
    componentsTree->addAction(ui->actionComponentGUI);
    componentsTree->addAction(actionSeparator5);
    componentsTree->addAction(ui->actionReloadTree);
    ui->actionComponentGUI->setEnabled(false); // пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ, пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ enable
    connect(ui->actionComponentMoveUp, SIGNAL(triggered()), this, SLOT(componentMoveUp()));
    connect(ui->actionComponentMoveDown, SIGNAL(triggered()), this, SLOT(componentMoveDown()));
    connect(ui->actionComponentRename, SIGNAL(triggered()), this, SLOT(componentRename()));
    connect(ui->actionComponentDelete, SIGNAL(triggered()), this, SLOT(componentDelete()));
    connect(ui->actionComponentReset, SIGNAL(triggered()), this, SLOT(componentReset()));
    connect(ui->actionComponentCalculate, SIGNAL(triggered()), this, SLOT(componentCalculate()));
 //   connect(ui->actionDefaultAllParameters, SIGNAL(triggered()), this, SLOT(on_actionDefaultAllParameters_triggered()));

    connect(ui->actionComponentInit, SIGNAL(triggered()), this, SLOT(componentInit()));
    connect(ui->actionComponentUnInit, SIGNAL(triggered()), this, SLOT(componentUnInit()));
    connect(ui->actionCopyNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyNameToClipboard()));
    connect(ui->actionCopyLongNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyLongNameToClipboard()));
    connect(ui->actionCopyClassNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyClassNameToClipboard()));
    connect(ui->actionComponentGUI, SIGNAL(triggered()), this, SLOT(componentGUI()));

    QAction *actionSeparatorProperty1 = new QAction(this);
    actionSeparatorProperty1->setSeparator(true);

    m_actionEditProperty = new QAction(tr("Edit value…"), this);
    m_actionEditProperty->setShortcut(QKeySequence(Qt::Key_F2));
    m_actionEditProperty->setToolTip(tr("Double-click the Value cell, or press F2"));
    connect(m_actionEditProperty, &QAction::triggered, this, &UComponentsListWidgetModern::propertyEditValue);

    m_actionShowPropertyXml = new QAction(tr("Show XML…"), this);
    connect(m_actionShowPropertyXml, &QAction::triggered, this, &UComponentsListWidgetModern::propertyShowXml);

    for(QTreeWidget* tree : {ui->treeWidgetParameters, ui->treeWidgetState, ui->treeWidgetInputs,
                             ui->treeWidgetOutputs, ui->treeWidgetFavorites})
    {
        tree->addAction(m_actionEditProperty);
        tree->addAction(m_actionShowPropertyXml);
        tree->addAction(ui->actionCopyPropertyNameToClipboard);
        tree->addAction(ui->actionCopyPropertyValueToClipboard);
        tree->addAction(ui->actionPastePropertyValueFromClipboard);
    }
    connect(ui->actionCopyPropertyNameToClipboard, SIGNAL(triggered()), this, SLOT(propertyCopyNameToClipboard()));
    connect(ui->actionCopyPropertyValueToClipboard, SIGNAL(triggered()), this, SLOT(propertyCopyValueToClipboard()));
    connect(ui->actionPastePropertyValueFromClipboard, SIGNAL(triggered()), this, SLOT(propertyPasteValueFromClipboard()));

    m_actionFavoritesShowInAllSections = new QAction(tr("Show in all matching sections"), this);
    m_actionFavoritesShowInAllSections->setCheckable(true);
    m_actionFavoritesShowInAllSections->setChecked(m_favoritesShowInAllSections);
    connect(m_actionFavoritesShowInAllSections, &QAction::toggled,
            this, &UComponentsListWidgetModern::onFavoritesShowInAllSectionsToggled);
    ui->treeWidgetFavorites->addAction(m_actionFavoritesShowInAllSections);

    setupPropertyEditing();
    setupColumnWidthSync();

    // Favorites показываем только при наличии избранных свойств
    updateFavoritesTabVisibility(false, false);
}

UComponentsListWidgetModern::~UComponentsListWidgetModern()
{
    delete ui;
}

void UComponentsListWidgetModern::updateComponentsListFromScheme()
{
    UpdateInterface(true);
    reloadPropertys(true);
}

void UComponentsListWidgetModern::AUpdateInterface()
{
    NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UComponentsListModern"), accessibleName());

    QString oldRootItem = currentDrawComponentName;
    QString oldSelectedItem = selectedComponentLongName;

    int componentsListScrollMaximum = componentsTree->verticalScrollBar()->maximum();
    int componentsListScrollPosition = componentsTree->verticalScrollBar()->value();

    // Сохраняем состояние развернутости всех узлов перед очисткой дерева
    QSet<QString> expandedItems;
    {
        QTreeWidgetItemIterator it(componentsTree);
        while (*it) {
            QTreeWidgetItem *item = *it;
            if (item->isExpanded() && item->childCount() > 0) {
                QString itemName = item->data(0, Qt::UserRole).toString();
                if (!itemName.isEmpty())
                    expandedItems.insert(itemName);
            }
            ++it;
        }
    }

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Блокируем сигналы во время обновления интерфейса,
    // чтобы предотвратить вызов componentListItemSelectionChanged при восстановлении выделения
    // через setCurrentItem() в addComponentSons()
    m_isUpdatingFromScheme = true;
    componentsTree->blockSignals(true);

    if(lastSnapshot && !lastSnapshot->Components.isEmpty())
    {
        rebuildTreeFromSnapshot(lastSnapshot);
        if(expandedItems.isEmpty())
        {
            componentsTree->expandAll();
        }
        else
        {
            QTreeWidgetItemIterator restoreIterator(componentsTree);
            while (*restoreIterator)
            {
                QTreeWidgetItem *item = *restoreIterator;
                const QString itemName = item->data(0, Qt::UserRole).toString();
                if (expandedItems.contains(itemName))
                    item->setExpanded(true);
                ++restoreIterator;
            }
        }
        restoreTreeSelection(oldRootItem, oldSelectedItem);
        m_treeRebuildRetryCount = 0;
    }
    else
    {
        componentsTree->clear();

        QTreeWidgetItem *rootItem = new QTreeWidgetItem(componentsTree);
        rootItem->setText(0, "Model");
        rootItem->setData(0, Qt::UserRole, QString());
        rootItem->setExpanded(true);
        addComponentSons("", rootItem, oldRootItem, oldSelectedItem, expandedItems);

        applyFilter(rootItem);

        // При первой загрузке (expandedItems пустой) разворачиваем всё дерево
        if(expandedItems.isEmpty())
            componentsTree->expandAll();
        m_treeRebuildRetryCount = 0;
    }

    componentsTree->verticalScrollBar()->setMaximum(componentsListScrollMaximum);
    componentsTree->verticalScrollBar()->setValue(componentsListScrollPosition);

    // Разблокируем сигналы после завершения обновления
    componentsTree->blockSignals(false);

    // Сбрасываем флаг после небольшой задержки, чтобы все события успели обработаться
    QTimer::singleShot(0, [this]() {
        m_isUpdatingFromScheme = false;
    });

    if(channelsSelectionVisible)
    {
      redrawChannelsList();
    }
}
void UComponentsListWidgetModern::AClearInterface()
{
 componentsTree->clear();
}

void UComponentsListWidgetModern::ASaveParameters()
{
    if(!application) return;
    const QString projectPath = QString::fromLocal8Bit(application->GetProjectPath().c_str());
    if(projectPath.isEmpty())
        return;

    QSettings settings(projectPath + "settings.qt", QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    settings.setValue("splitterState", ui->splitter->saveState());
    settings.setValue("columnWidthName", ui->treeWidgetParameters->header()->sectionSize(0));
    settings.setValue("columnWidthValue", ui->treeWidgetParameters->header()->sectionSize(1));
    settings.setValue("columnWidthType", ui->treeWidgetInputs->header()->sectionSize(2));
    settings.setValue("favoritesShowInAllSections", m_favoritesShowInAllSections);
    settings.sync();
    settings.endGroup();
}

void UComponentsListWidgetModern::ALoadParameters()
{
    if(!application) return;
    const QString projectPath = QString::fromLocal8Bit(application->GetProjectPath().c_str());
    if(projectPath.isEmpty())
        return;

    QSettings settings(projectPath + "settings.qt", QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    ui->splitter->restoreState(settings.value("splitterState").toByteArray());

    int nameW = settings.value("columnWidthName", -1).toInt();
    int valueW = settings.value("columnWidthValue", -1).toInt();
    int typeW = settings.value("columnWidthType", -1).toInt();

    // Миграция со старых headerState
    if(nameW <= 0 || valueW <= 0)
    {
        const QByteArray oldHeader = settings.value("treeWidgetParameters").toByteArray();
        if(!oldHeader.isEmpty())
        {
            const QSignalBlocker blocker(ui->treeWidgetParameters->header());
            ui->treeWidgetParameters->header()->restoreState(oldHeader);
            if(nameW <= 0)
                nameW = ui->treeWidgetParameters->header()->sectionSize(0);
            if(valueW <= 0)
                valueW = ui->treeWidgetParameters->header()->sectionSize(1);
        }
    }
    if(typeW <= 0)
    {
        const QByteArray oldInputs = settings.value("treeWidgetInputsHeader").toByteArray();
        if(!oldInputs.isEmpty())
        {
            const QSignalBlocker blocker(ui->treeWidgetInputs->header());
            ui->treeWidgetInputs->header()->restoreState(oldInputs);
            typeW = ui->treeWidgetInputs->header()->sectionSize(2);
        }
    }

    if(nameW <= 0)
        nameW = ui->treeWidgetParameters->header()->sectionSize(0);
    if(valueW <= 0)
        valueW = ui->treeWidgetParameters->header()->sectionSize(1);
    if(typeW <= 0)
        typeW = ui->treeWidgetInputs->header()->sectionSize(2);

    applySharedColumnWidths(nameW, valueW, typeW);

    m_favoritesShowInAllSections = settings.value("favoritesShowInAllSections", true).toBool();
    if(m_actionFavoritesShowInAllSections)
        m_actionFavoritesShowInAllSections->setChecked(m_favoritesShowInAllSections);
    settings.endGroup();

    // После restoreState дока/первого show ширины снова пересчитываются — повторить на следующем тике
    QTimer::singleShot(0, this, [this, nameW, valueW, typeW]() {
        applySharedColumnWidths(nameW, valueW, typeW);
    });
}

void UComponentsListWidgetModern::setVerticalOrientation(bool vertical)
{
    if (vertical)
        ui->splitter->setOrientation(Qt::Vertical);
    else
        ui->splitter->setOrientation(Qt::Horizontal);
}

QString UComponentsListWidgetModern::getSelectedComponentLongName()
{
    if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped
       && m_unifiedTree)
    {
        QTreeWidgetItem* item = m_unifiedTree->currentItem();
        if(item && item->childCount() == 0
           && !item->data(0, kPropRoleIsGroup).toBool())
        {
            const QString fromRole = item->data(0, kFavRoleComponent).toString();
            if(!fromRole.isEmpty())
                return fromRole;
        }
        if(componentsTree->currentItem())
            return componentsTree->currentItem()->data(0, Qt::UserRole).toString();
        return QString();
    }

    if(!componentsTree->currentItem())
      return "";
    else
    {
      if(ui->tabWidgetComponentInfo->currentWidget() == ui->tabFavorites)
      {
        QTreeWidgetItem* item = ui->treeWidgetFavorites->currentItem();
        if(item && item->childCount() == 0)
            return item->data(0, kFavRoleComponent).toString();
        return "";
      }
      else
      {
        return componentsTree->currentItem()->data(0, Qt::UserRole).toString();
      }
    }

}

void UComponentsListWidgetModern::openTabN(int n)
{
    QWidget* tab = widgetFromLogicalTabIndex(n);
    if(!tab)
        return;
    if(ui->tabWidgetComponentInfo->indexOf(tab) < 0)
        return;
    ui->tabWidgetComponentInfo->setCurrentWidget(tab);
}

int UComponentsListWidgetModern::currentTabIndex()
{
    return logicalTabIndexFromWidget(ui->tabWidgetComponentInfo->currentWidget());
}

int UComponentsListWidgetModern::currentPropertyXmlMask() const
{
    if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped
       && m_unifiedTree)
    {
        QTreeWidgetItem* item = m_unifiedTree->currentItem();
        if(item)
        {
            QTreeWidgetItem* group = item->data(0, kPropRoleIsGroup).toBool() ? item : item->parent();
            if(group)
            {
                const QString title = group->text(0);
                if(title == tr("State"))
                    return static_cast<int>(ptPubState);
                if(title == tr("Inputs"))
                    return static_cast<int>(ptPubInput);
                if(title == tr("Outputs"))
                    return static_cast<int>(ptPubOutput);
            }
        }
        return static_cast<int>(ptPubParameter);
    }

    switch(logicalTabIndexFromWidget(ui->tabWidgetComponentInfo->currentWidget()))
    {
    case 1: return static_cast<int>(ptPubState);
    case 2: return static_cast<int>(ptPubInput);
    case 3: return static_cast<int>(ptPubOutput);
    case 0:
    case 4:
    default:
        return static_cast<int>(ptPubParameter);
    }
}

QString UComponentsListWidgetModern::getSelectedPropertyName()
{
  if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped
     && m_unifiedTree)
  {
      QTreeWidgetItem* item = m_unifiedTree->currentItem();
      if(!item || item->childCount() > 0 || item->data(0, kPropRoleIsGroup).toBool())
          return QString();
      const QString prop = item->data(0, kFavRoleProp).toString();
      return prop.isEmpty() ? item->data(0, Qt::DisplayRole).toString() : prop;
  }

  QWidget* tab = ui->tabWidgetComponentInfo->currentWidget();
  if(tab == ui->tabParameters)
  {
      if(!ui->treeWidgetParameters->currentItem())
        return "";
      return ui->treeWidgetParameters->currentItem()->data(0, Qt::DisplayRole).toString();
  }
  if(tab == ui->tabState)
  {
      if(!ui->treeWidgetState->currentItem())
        return "";
      return ui->treeWidgetState->currentItem()->data(0, Qt::DisplayRole).toString();
  }
  if(tab == ui->tabInputs)
  {
      if(!ui->treeWidgetInputs->currentItem())
        return "";
      return ui->treeWidgetInputs->currentItem()->data(0, Qt::DisplayRole).toString();
  }
  if(tab == ui->tabOutputs)
  {
      if(!ui->treeWidgetOutputs->currentItem())
        return "";
      return ui->treeWidgetOutputs->currentItem()->data(0, Qt::DisplayRole).toString();
  }
  if(tab == ui->tabFavorites)
  {
      QTreeWidgetItem* item = ui->treeWidgetFavorites->currentItem();
      if(!item || item->childCount() > 0)
        return "";
      return item->data(0, kFavRoleProp).toString();
  }
  return "";
}

int UComponentsListWidgetModern::getSelectedChannelIndex()
{
    return currentChannel;
}

/// пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
/// 0 - пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
/// 1 - пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
void UComponentsListWidgetModern::setChannelMode(int mode)
{
 channelMode=mode;
}

/// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
/// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
int UComponentsListWidgetModern::getWorkChannelIndex()
{
 return (channelMode == 0)?Core_GetSelectedChannelIndex():currentChannel;
}


void UComponentsListWidgetModern::setEnableTabN(int n, bool enable)
{
    QWidget* tab = widgetFromLogicalTabIndex(n);
    if(!tab)
      return;
    const int index = ui->tabWidgetComponentInfo->indexOf(tab);
    if(index >= 0)
      ui->tabWidgetComponentInfo->setTabEnabled(index, enable);
}

void UComponentsListWidgetModern::setChannelsListVisible(bool value)
{
    ui->listWidgetChannelSelection->setVisible(value);
    channelsSelectionVisible = value;
    redrawChannelsList();
}

void UComponentsListWidgetModern::setComponentTreeEmbedded(bool embedded)
{
    if(m_componentTreeEmbedded == embedded)
    {
        // Re-apply UI in case widgets were moved to popup
        if(embedded)
        {
            if(treePopupDialog && treePopupDialog->isVisible())
                hideTreePopup();
            if(ui->treeContainer)
                ui->treeContainer->show();
            if(toggleModeButton)
                toggleModeButton->hide();
        }
        return;
    }

    m_componentTreeEmbedded = embedded;

    if(embedded)
    {
        if(treePopupDialog && treePopupDialog->isVisible())
            hideTreePopup();

        // Ensure filter + tree live in the main treeContainer (not popup layout)
        if(treePopupLayout)
        {
            treePopupLayout->removeWidget(filterLineEdit);
            treePopupLayout->removeWidget(componentsTree);
        }
        QWidget* treeContainer = ui->treeContainer;
        QVBoxLayout* treeLayout = treeContainer
            ? qobject_cast<QVBoxLayout*>(treeContainer->layout()) : nullptr;
        if(treeLayout)
        {
            if(treeLayout->indexOf(filterLineEdit) < 0)
                treeLayout->addWidget(filterLineEdit);
            if(treeLayout->indexOf(componentsTree) < 0)
                treeLayout->addWidget(componentsTree);
        }
        if(treeContainer)
            treeContainer->show();
        if(toggleModeButton)
            toggleModeButton->hide();
    }
    else
    {
        if(ui->treeContainer)
            ui->treeContainer->hide();
        if(toggleModeButton)
        {
            toggleModeButton->show();
            toggleModeButton->setChecked(false);
            toggleModeButton->setText(tr("☰"));
            toggleModeButton->setToolTip(tr("Показать дерево компонентов"));
        }
    }
}

void UComponentsListWidgetModern::setLayoutPreset(ComponentsListLayoutPreset preset)
{
    if(m_layoutPreset == preset &&
       (preset != ComponentsListLayoutPreset::PropertyPicker || m_pickerLayoutApplied))
    {
        applyLayoutPresetUi();
        return;
    }
    m_layoutPreset = preset;
    applyLayoutPresetUi();
}

void UComponentsListWidgetModern::applyLayoutPresetUi()
{
    QWidget* leftPane = ui->splitter ? ui->splitter->widget(0) : nullptr;
    QWidget* rightPane = ui->splitter ? ui->splitter->widget(1) : nullptr;
    QVBoxLayout* leftLayout = leftPane ? qobject_cast<QVBoxLayout*>(leftPane->layout()) : nullptr;
    QVBoxLayout* rightLayout = rightPane ? qobject_cast<QVBoxLayout*>(rightPane->layout()) : nullptr;

    if(m_layoutPreset == ComponentsListLayoutPreset::PropertyPicker)
    {
        setComponentTreeEmbedded(true);
        setVerticalOrientation(false); // left: components, right: properties

        if(toggleModeButton)
            toggleModeButton->hide();

        // Channel strip: compact bar above the component tree (full left width)
        if(ui->listWidgetChannelSelection)
        {
            ui->horizontalLayoutTreeWidget->removeWidget(ui->listWidgetChannelSelection);
            ui->listWidgetChannelSelection->setMinimumWidth(0);
            ui->listWidgetChannelSelection->setMaximumWidth(16777215);
            ui->listWidgetChannelSelection->setMaximumHeight(64);
            ui->listWidgetChannelSelection->setMinimumHeight(0);
            ui->listWidgetChannelSelection->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
            ui->listWidgetChannelSelection->setFlow(QListView::LeftToRight);
            ui->listWidgetChannelSelection->setWrapping(true);
            ui->listWidgetChannelSelection->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            ui->listWidgetChannelSelection->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            if(leftLayout && leftLayout->indexOf(ui->listWidgetChannelSelection) < 0)
                leftLayout->insertWidget(0, ui->listWidgetChannelSelection);
        }

        // Class name sits above properties (right pane), not beside channels
        if(ui->labelComponentClassName && rightLayout)
        {
            ui->horizontalLayoutTreeWidget->removeWidget(ui->labelComponentClassName);
            ui->labelComponentClassName->setMaximumHeight(22);
            ui->labelComponentClassName->setMinimumHeight(18);
            ui->labelComponentClassName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            if(rightLayout->indexOf(ui->labelComponentClassName) < 0)
                rightLayout->insertWidget(0, ui->labelComponentClassName);
        }

        // Hide empty header row on the left if only toggle was there
        if(ui->horizontalLayoutTreeWidget)
        {
            bool anyVisible = false;
            for(int i = 0; i < ui->horizontalLayoutTreeWidget->count(); ++i)
            {
                if(QWidget* w = ui->horizontalLayoutTreeWidget->itemAt(i)->widget())
                {
                    if(w->isVisibleTo(this))
                        anyVisible = true;
                }
            }
            // Keep layout item but collapse left header chrome
            Q_UNUSED(anyVisible);
        }

        if(leftPane)
        {
            leftPane->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
            leftPane->setMinimumWidth(220);
        }
        if(rightPane)
        {
            rightPane->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            rightPane->setMinimumWidth(280);
        }
        if(ui->splitter)
        {
            ui->splitter->setStretchFactor(0, 2);
            ui->splitter->setStretchFactor(1, 3);
            // Give properties more width for Name column (Favorites-like)
            QList<int> sz = ui->splitter->sizes();
            if(sz.size() >= 2 && (sz[0] + sz[1]) > 0)
            {
                const int total = qMax(600, sz[0] + sz[1]);
                ui->splitter->setSizes({ total * 2 / 5, total * 3 / 5 });
            }
            else
            {
                ui->splitter->setSizes({ 280, 420 });
            }
        }

        if(ui->treeContainer)
            ui->treeContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        if(leftLayout && ui->treeContainer)
        {
            const int treeIdx = leftLayout->indexOf(ui->treeContainer);
            if(treeIdx >= 0)
                leftLayout->setStretch(treeIdx, 1);
            // Collapse leftover header row (toggle hidden) so tree gets vertical space
            for(int i = 0; i < leftLayout->count(); ++i)
            {
                QLayoutItem* it = leftLayout->itemAt(i);
                if(it && it->layout() == ui->horizontalLayoutTreeWidget)
                    leftLayout->setStretch(i, 0);
            }
        }

        m_pickerLayoutApplied = true;
        applyPropertyColumnLayout();
        return;
    }

    // Inspector (default main-form layout)
    if(m_pickerLayoutApplied)
    {
        // Restore channel + class into the top header row
        if(ui->listWidgetChannelSelection)
        {
            if(leftLayout)
                leftLayout->removeWidget(ui->listWidgetChannelSelection);
            ui->listWidgetChannelSelection->setFlow(QListView::TopToBottom);
            ui->listWidgetChannelSelection->setWrapping(false);
            ui->listWidgetChannelSelection->setMaximumHeight(16777215);
            ui->listWidgetChannelSelection->setMinimumWidth(150);
            ui->listWidgetChannelSelection->setMaximumWidth(180);
            ui->listWidgetChannelSelection->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            ui->listWidgetChannelSelection->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            if(ui->horizontalLayoutTreeWidget->indexOf(ui->listWidgetChannelSelection) < 0)
                ui->horizontalLayoutTreeWidget->insertWidget(0, ui->listWidgetChannelSelection);
        }
        if(ui->labelComponentClassName)
        {
            if(rightLayout)
                rightLayout->removeWidget(ui->labelComponentClassName);
            ui->labelComponentClassName->setMaximumHeight(24);
            ui->labelComponentClassName->setMinimumHeight(20);
            if(ui->horizontalLayoutTreeWidget->indexOf(ui->labelComponentClassName) < 0)
            {
                // After channels and toggle: [ch][☰][class]
                ui->horizontalLayoutTreeWidget->addWidget(ui->labelComponentClassName);
            }
        }
        if(toggleModeButton && !m_componentTreeEmbedded)
            toggleModeButton->show();

        setVerticalOrientation(true);
        if(leftPane)
            leftPane->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        if(ui->splitter)
        {
            ui->splitter->setStretchFactor(0, 0);
            ui->splitter->setStretchFactor(1, 1);
        }
        if(!m_componentTreeEmbedded && ui->treeContainer)
            ui->treeContainer->hide();

        m_pickerLayoutApplied = false;
    }
    applyPropertyColumnLayout();
}

void UComponentsListWidgetModern::applyPropertyColumnLayout()
{
    const bool picker = (m_layoutPreset == ComponentsListLayoutPreset::PropertyPicker);

    auto applyOne = [&](QTreeWidget* tree) {
        if(!tree || tree->columnCount() < 1)
            return;
        QHeaderView* header = tree->header();
        const QSignalBlocker blocker(header);
        header->setStretchLastSection(false);
        tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        if(picker)
        {
            // Favorites-like: Name dominates; Value is secondary
            header->setSectionResizeMode(0, QHeaderView::Stretch);
            if(tree->columnCount() > 1)
            {
                header->setSectionResizeMode(1, QHeaderView::Interactive);
                header->resizeSection(1, 110);
            }
        }
        else
        {
            header->setSectionResizeMode(0, QHeaderView::Interactive);
            header->resizeSection(0, qMax(header->sectionSize(0), kMinNameColumnWidth));
            if(tree->columnCount() > 1)
                header->setSectionResizeMode(1, QHeaderView::Stretch);
        }
        if(tree->columnCount() > 2)
            header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    };

    m_syncingColumnWidths = true;
    for(QTreeWidget* tree : {ui->treeWidgetParameters, ui->treeWidgetState, ui->treeWidgetInputs,
                             ui->treeWidgetOutputs, ui->treeWidgetFavorites, m_unifiedTree})
        applyOne(tree);
    m_syncingColumnWidths = false;
}

void UComponentsListWidgetModern::componentListItemSelectionChanged()
{
    QTreeWidgetItem * item = componentsTree->currentItem();
    if(!item)
    {
        // DEBUG: Commented out to reduce log flood
        // QString logMsg = QString("[SELECTION_DEBUG] UComponentsListWidgetModern::componentListItemSelectionChanged: no current item, returning");
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        return;
    }

    selectedComponentLongName = item->data(0,Qt::UserRole).toString();
    m_propertyReloadRetryCount = 0;
    m_propertyReloadRetryTarget = selectedComponentLongName;

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Не эмитируем componentSelected если выделение обновляется программно из схемы
    // Это предотвращает бесконечный цикл: selectComponent -> componentSelected -> componentSingleClick -> selectComponent
    if(m_isUpdatingFromScheme)
    {
        // DEBUG: Commented out to reduce log flood
        // QString logMsg = QString("[SELECTION_DEBUG] UComponentsListWidgetModern::componentListItemSelectionChanged: skipping emit (m_isUpdatingFromScheme=true) for '%1'")
        //     .arg(selectedComponentLongName);
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        reloadPropertys();
        return;
    }

    // DEBUG: Commented out to reduce log flood
    // QString logMsg = QString("[SELECTION_DEBUG] UComponentsListWidgetModern::componentListItemSelectionChanged: current item changed, emitting componentSelected('%1')")
    //     .arg(selectedComponentLongName);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

    currentDrawComponentName = selectedComponentLongName;
    // Do not Env_Select on selection (TD-111 reverted): FindComponent stays at Model.
    reloadPropertys();
    emit componentSelected(selectedComponentLongName);
}

void UComponentsListWidgetModern::schedulePropertyReloadRetry()
{
    if(m_propertyReloadRetryTarget != selectedComponentLongName)
    {
        m_propertyReloadRetryCount = 0;
        m_propertyReloadRetryTarget = selectedComponentLongName;
    }
    if(m_propertyReloadRetryCount >= kMaxPropertyReloadRetries)
        return;

    ++m_propertyReloadRetryCount;
    QTimer::singleShot(200, this, [this]() {
        if(m_propertyReloadRetryTarget != selectedComponentLongName)
            return;
        reloadPropertys(true);
    });
}

void UComponentsListWidgetModern::scheduleTreeRebuildRetry()
{
    if(m_treeRebuildRetryCount >= kMaxTreeRebuildRetries)
        return;

    ++m_treeRebuildRetryCount;
    QTimer::singleShot(200, this, [this]() {
        UpdateInterface(true);
    });
}

void UComponentsListWidgetModern::restoreTreeSelection(const QString& oldRootItem,
                                                       const QString& oldSelectedItem)
{
    const QString selectName = !oldSelectedItem.isEmpty() ? oldSelectedItem : oldRootItem;
    if(selectName.isEmpty())
        return;

    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole).toString() == selectName)
        {
            QTreeWidgetItem* item = *iterator;
            while(item)
            {
                item->setExpanded(true);
                item = item->parent();
            }
            componentsTree->setCurrentItem(*iterator);
            componentsTree->scrollToItem(*iterator, QAbstractItemView::EnsureVisible);
            return;
        }
        ++iterator;
    }
}

void UComponentsListWidgetModern::reloadPropertys(bool forceReload)
{
    if(currentDrawPropertyComponentName == selectedComponentLongName && !forceReload)
      return;

    const QString targetComponent = selectedComponentLongName;

    std::map<std::string, std::string> Favorites;
    // описание класса компонента, нужно ниже для проверки алиасов избранных свойств
    RDK::UEPtr<RDK::UContainerDescription> class_desc;

    const QString classNameStr =
        componentClassNameFromModelScope(getWorkChannelIndex(), targetComponent);

    if(!classNameStr.isEmpty())
    {
        class_desc = RDK::GetStorageLock()->GetClassDescription(classNameStr.toLocal8Bit().constData(), true);

        if(class_desc)
            Favorites = class_desc->GetFavorites();

        ui->labelComponentClassName->setText(classNameStr);
        ui->labelComponentClassName->setToolTip(classNameStr);
    }
    else
    {
        ui->labelComponentClassName->clear();
        ui->labelComponentClassName->setToolTip(QString());
    }

    // пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ treeWidget'пїЅпїЅ
    int paramScrollPosition = ui->treeWidgetParameters->verticalScrollBar()->value(),
        stateScrollPosition = ui->treeWidgetState->verticalScrollBar()->value(),
        inputsScrollPosition = ui->treeWidgetInputs->verticalScrollBar()->value(),
        outputsScrollPosition = ui->treeWidgetOutputs->verticalScrollBar()->value(),
        favoritesScrollPosition = ui->treeWidgetFavorites->verticalScrollBar()->value();

    try
    {
     UpdateInterfaceFlag=true;
        // Use timeout to avoid blocking UI for too long during calculation
        RDK::UELockPtr<RDK::UContainer> model =
            RDK::GetModelLockTimeout(getWorkChannelIndex(), kModelLockTimeoutMs);
        if (!model) {
            UpdateInterfaceFlag=false;
            schedulePropertyReloadRetry();
            return;
        }

        RDK::UEPtr<RDK::UContainer> cont;
        if (targetComponent.isEmpty())
            cont = model.Get();
        else
            cont = model->GetComponentL(targetComponent.toLocal8Bit().constData(), true);

        if(!cont)
        {
         UpdateInterfaceFlag=false;
         schedulePropertyReloadRetry();
         return;
        }

    ui->treeWidgetParameters->clear();
    ui->treeWidgetState->clear();
    ui->treeWidgetInputs->clear();
    ui->treeWidgetOutputs->clear();
    ui->treeWidgetFavorites->clear();
    if(m_unifiedTree)
        m_unifiedTree->clear();
        const RDK::UComponent::VariableMapT& varMap = cont->GetPropertiesList();
        const std::vector<RDK::NameT>& propOrder = cont->GetPropertiesOrder();
        std::string buffer;

        bool is_new_outputs(false);
        bool is_new_inputs(false);

        for(size_t oi = 0; oi < propOrder.size(); ++oi)
        {
            RDK::UComponent::VariableMapCIteratorT i = varMap.find(propOrder[oi]);
            if(i == varMap.end())
             continue;
            if (i->second.CheckMask(ptPubInput))
            {
             std::string::size_type k=i->first.find("DataInput");
             if(k != 0 || k == std::string::npos)
              is_new_inputs=true;
            }

            if (i->second.CheckMask(ptPubOutput))
            {
             std::string::size_type k=i->first.find("DataOutput");
             if(k != 0 || k == std::string::npos)
              is_new_outputs=true;
            }

            if(is_new_inputs && is_new_outputs)
             break;
        }

        if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped)
        {
            ensureUnifiedPropertyPage();
            fillUnifiedPropertyTree(cont.Get(), varMap, propOrder, is_new_inputs, is_new_outputs,
                                    Favorites, class_desc.Get());
            currentDrawPropertyComponentName = targetComponent;
            m_propertyReloadRetryCount = 0;
            UpdateInterfaceFlag=false;
            return;
        }

        for(size_t oi = 0; oi < propOrder.size(); ++oi)
        {
            RDK::UComponent::VariableMapCIteratorT i = varMap.find(propOrder[oi]);
            if(i == varMap.end())
             continue;
            if (i->second.CheckMask(ptPubInput) && is_new_inputs && i->first.find("DataInput") == 0)
             continue;
            if (i->second.CheckMask(ptPubOutput) && is_new_outputs && i->first.find("DataOutput") == 0)
             continue;

            if (i->second.CheckMask(ptPubParameter) && ui->tabWidgetComponentInfo->currentWidget() == ui->tabParameters)
            {
                QTreeWidgetItem* parametersItem = new QTreeWidgetItem(ui->treeWidgetParameters);
                QString parameterName = QString::fromLocal8Bit(i->first.c_str());
                parametersItem->setText(0, parameterName);
                cont->GetPropertyValue(i->first, buffer);
                if(RDK::IsMatrixPropertyTypeName(i->second.Property->GetLanguageType().name()))
                    buffer = RDK::NormalizeMatrixPropertyText(buffer);
                const QString rawValue = QString::fromLocal8Bit(buffer.c_str());
                parametersItem->setData(1, Qt::UserRole, rawValue);
                parametersItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                parametersItem->setToolTip(1, rawValue);
                if(parameterName == selectedParameterName)
                    ui->treeWidgetParameters->setCurrentItem(parametersItem);
                if(i->second.Property->GetLanguageType() == typeid(bool))
                {
                 applyBoolCheckFlags(parametersItem, parseBoolPropertyValue(rawValue));
                }
                else
                {
                 parametersItem->setFlags((parametersItem->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable)
                                          & ~Qt::ItemIsUserCheckable);
                }
            }
            if (i->second.CheckMask(ptPubState) && ui->tabWidgetComponentInfo->currentWidget() == ui->tabState)
            {
                QTreeWidgetItem* stateItem = new QTreeWidgetItem(ui->treeWidgetState);
                QString stateName = QString::fromLocal8Bit(i->first.c_str());
                stateItem->setText(0, stateName);
                cont->GetPropertyValue(i->first, buffer);
                if(RDK::IsMatrixPropertyTypeName(i->second.Property->GetLanguageType().name()))
                    buffer = RDK::NormalizeMatrixPropertyText(buffer);
                const QString rawValue = QString::fromLocal8Bit(buffer.c_str());
                stateItem->setData(1, Qt::UserRole, rawValue);
                stateItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                stateItem->setToolTip(1, rawValue);
                if(i->second.Property->GetLanguageType() == typeid(bool))
                    applyBoolCheckFlags(stateItem, parseBoolPropertyValue(rawValue));
                else
                    stateItem->setFlags((stateItem->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable)
                                        & ~Qt::ItemIsUserCheckable);
                if(stateName == selectedStateName)
                    ui->treeWidgetState->setCurrentItem(stateItem);
            }
            if (i->second.CheckMask(ptPubInput) && ui->tabWidgetComponentInfo->currentWidget() == ui->tabInputs)
            {
                QTreeWidgetItem* inputItem = new QTreeWidgetItem(ui->treeWidgetInputs);
                QString inputName = QString::fromLocal8Bit(i->first.c_str());
                inputItem->setText(0, inputName);
                cont->GetPropertyValue(i->first, buffer);
                if(RDK::IsMatrixPropertyTypeName(i->second.Property->GetLanguageType().name()))
                    buffer = RDK::NormalizeMatrixPropertyText(buffer);
                const QString rawValue = QString::fromLocal8Bit(buffer.c_str());
                inputItem->setData(1, Qt::UserRole, rawValue);
                inputItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                inputItem->setToolTip(1, rawValue);
                inputItem->setText(2, QString(i->second.Property->GetLanguageType().name()));
                if(i->second.Property->GetLanguageType() == typeid(bool))
                    applyBoolCheckFlags(inputItem, parseBoolPropertyValue(rawValue));
                else
                    inputItem->setFlags((inputItem->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable)
                                        & ~Qt::ItemIsUserCheckable);
                if(inputName == selectedInputName)
                    ui->treeWidgetInputs->setCurrentItem(inputItem);
            }
            if (i->second.CheckMask(ptPubOutput) && ui->tabWidgetComponentInfo->currentWidget() == ui->tabOutputs)
            {
                QTreeWidgetItem* outputItem = new QTreeWidgetItem(ui->treeWidgetOutputs);
                QString outputName = QString::fromLocal8Bit(i->first.c_str());
                outputItem->setText(0, outputName);
                cont->GetPropertyValue(i->first, buffer);
                if(RDK::IsMatrixPropertyTypeName(i->second.Property->GetLanguageType().name()))
                    buffer = RDK::NormalizeMatrixPropertyText(buffer);
                const QString rawValue = QString::fromLocal8Bit(buffer.c_str());
                outputItem->setData(1, Qt::UserRole, rawValue);
                outputItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                outputItem->setToolTip(1, rawValue);
                outputItem->setText(2, QString(i->second.Property->GetLanguageType().name()));
                if(i->second.Property->GetLanguageType() == typeid(bool))
                    applyBoolCheckFlags(outputItem, parseBoolPropertyValue(rawValue));
                else
                    outputItem->setFlags((outputItem->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable)
                                         & ~Qt::ItemIsUserCheckable);
                if(outputName == selectedOutputName)
                    ui->treeWidgetOutputs->setCurrentItem(outputItem);
            }
        }


        const bool favoritesTabWasPresent =
            ui->tabWidgetComponentInfo->indexOf(ui->tabFavorites) >= 0;
        const bool componentChangedForFavorites =
            (currentDrawPropertyComponentName != targetComponent);

        QTreeWidgetItem* groupParameters = new QTreeWidgetItem(ui->treeWidgetFavorites);
        groupParameters->setText(0, tr("Parameters"));
        QTreeWidgetItem* groupOutputs = new QTreeWidgetItem(ui->treeWidgetFavorites);
        groupOutputs->setText(0, tr("Outputs"));
        QTreeWidgetItem* groupInputs = new QTreeWidgetItem(ui->treeWidgetFavorites);
        groupInputs->setText(0, tr("Inputs"));
        QTreeWidgetItem* groupStates = new QTreeWidgetItem(ui->treeWidgetFavorites);
        groupStates->setText(0, tr("States"));

        for(std::map<std::string, std::string>::iterator i = Favorites.begin(); i != Favorites.end(); ++i)
        {
            QString favoriteName = QString::fromLocal8Bit(i->first.c_str());
            QString favoritePath = QString::fromLocal8Bit(i->second.c_str());
            favoritePath.replace("{CompName}", targetComponent);

            // Проверяем, является ли это алиасом
            bool isAlias = class_desc && class_desc->IsFavoriteAlias(i->first);

            // Parse path - для алиасов путь может быть в формате "ComponentPath.PropertyName"
            QString component_long_name;
            QString prop_name;

            if (isAlias && class_desc)
            {
                // Для алиаса разбираем путь через ParseFavoritePath
                std::string componentPath, propertyName;
                if (class_desc->ParseFavoritePath(i->second, componentPath, propertyName))
                {
                    // Формируем полный путь: текущий компонент + путь к вложенному компоненту
                    component_long_name = targetComponent;
                    if (!componentPath.empty())
                    {
                        component_long_name += "." + QString::fromStdString(componentPath);
                    }
                    prop_name = QString::fromStdString(propertyName);
                }
            }
            else
            {
                // Старый формат: "ComponentName:PropertyName"
                QStringList vals = favoritePath.split(":");
                if(vals.size()==2)
                {
                    component_long_name = vals[0];
                    prop_name = vals[1];
                }
            }

            RDK::UEPtr<RDK::UContainer> child_cont;
            child_cont = model->GetComponentL(component_long_name.toLocal8Bit().constData(), true);

            QList<QTreeWidgetItem*> targetGroups;
            if(child_cont)
            {
                RDK::UComponent::VariableMapT childVars = child_cont->GetPropertiesList();
                RDK::UComponent::VariableMapIteratorT propIt = childVars.find(prop_name.toStdString());
                if(propIt != childVars.end())
                {
                    if(m_favoritesShowInAllSections)
                    {
                        if(propIt->second.CheckMask(ptPubParameter))
                            targetGroups << groupParameters;
                        if(propIt->second.CheckMask(ptPubOutput))
                            targetGroups << groupOutputs;
                        if(propIt->second.CheckMask(ptPubInput))
                            targetGroups << groupInputs;
                        if(propIt->second.CheckMask(ptPubState))
                            targetGroups << groupStates;
                    }
                    else
                    {
                        // Приоритет роли: Parameter > Output > Input > State
                        if(propIt->second.CheckMask(ptPubParameter))
                            targetGroups << groupParameters;
                        else if(propIt->second.CheckMask(ptPubOutput))
                            targetGroups << groupOutputs;
                        else if(propIt->second.CheckMask(ptPubInput))
                            targetGroups << groupInputs;
                        else if(propIt->second.CheckMask(ptPubState))
                            targetGroups << groupStates;
                    }
                }
            }
            if(targetGroups.isEmpty())
                targetGroups << groupParameters;

            QString displayName = isAlias ? (favoriteName + " [Alias]") : favoriteName;
            QString rawValue;
            RDK::UEPtr<RDK::UIProperty> prop;
            if(child_cont)
            {
                child_cont->GetPropertyValue(prop_name.toStdString(), buffer);
                prop = child_cont->FindProperty(prop_name.toStdString());
                if(prop && RDK::IsMatrixPropertyTypeName(prop->GetLanguageType().name()))
                    buffer = RDK::NormalizeMatrixPropertyText(buffer);
                rawValue = QString::fromLocal8Bit(buffer.c_str());
            }

            for(QTreeWidgetItem* groupParent : targetGroups)
            {
                QTreeWidgetItem* favoriteItem = new QTreeWidgetItem(groupParent);
                favoriteItem->setText(0, displayName);
                favoriteItem->setToolTip(0, favoritePath);
                favoriteItem->setData(0, kFavRoleProp, prop_name);
                favoriteItem->setData(0, kFavRoleDisplayPath, favoritePath);
                favoriteItem->setData(0, kFavRoleComponent, component_long_name);

                if(child_cont)
                {
                    std::string displayBuf = buffer;
                    favoriteItem->setData(1, Qt::UserRole, rawValue);
                    favoriteItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(displayBuf)).c_str()));
                    favoriteItem->setToolTip(1, rawValue);

                    if(favoriteItem->text(0) == selectedFavName)
                        ui->treeWidgetFavorites->setCurrentItem(favoriteItem);

                    if(prop && prop->GetLanguageType() == typeid(bool))
                    {
                     applyBoolCheckFlags(favoriteItem, parseBoolPropertyValue(rawValue));
                    }
                    else
                    {
                     favoriteItem->setFlags((favoriteItem->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable)
                                            & ~Qt::ItemIsUserCheckable);
                    }
                }
            }

        }

        // Удаляем пустые группы, оставшиеся разворачиваем
        QList<QTreeWidgetItem*> groups;
        groups << groupParameters << groupOutputs << groupInputs << groupStates;
        for(QTreeWidgetItem* group : groups)
        {
            if(group->childCount() == 0)
                delete group;
            else
                group->setExpanded(true);
        }

        const bool hasFavorites = !Favorites.empty();
        const bool selectFavorites =
            hasFavorites && (componentChangedForFavorites || !favoritesTabWasPresent);

        // пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ treeWidget'пїЅпїЅ
        ui->treeWidgetParameters->verticalScrollBar()->setMaximum(paramScrollPosition);
        ui->treeWidgetParameters->verticalScrollBar()->setValue(paramScrollPosition);
        ui->treeWidgetState->verticalScrollBar()->setMaximum(stateScrollPosition);
        ui->treeWidgetState->verticalScrollBar()->setValue(stateScrollPosition);
        ui->treeWidgetInputs->verticalScrollBar()->setMaximum(inputsScrollPosition);
        ui->treeWidgetInputs->verticalScrollBar()->setValue(inputsScrollPosition);
        ui->treeWidgetOutputs->verticalScrollBar()->setMaximum(outputsScrollPosition);
        ui->treeWidgetOutputs->verticalScrollBar()->setValue(outputsScrollPosition);
        ui->treeWidgetFavorites->verticalScrollBar()->setMaximum(favoritesScrollPosition);
        ui->treeWidgetFavorites->verticalScrollBar()->setValue(favoritesScrollPosition);
        currentDrawPropertyComponentName = targetComponent;
        m_propertyReloadRetryCount = 0;
        UpdateInterfaceFlag=false;

        updateFavoritesTabVisibility(hasFavorites, selectFavorites);
    }
    catch (RDK::UException &exception)
    {
     UpdateInterfaceFlag=false;
        schedulePropertyReloadRetry();
        RDK::Logging::SystemLog(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
     UpdateInterfaceFlag=false;
        schedulePropertyReloadRetry();
        RDK::Logging::SystemLog(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
}

void UComponentsListWidgetModern::parametersListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetParameters->currentItem();
    if(!item)
      return;

    QString changedParameterName = item->data(0, Qt::DisplayRole).toString();
    if(selectedParameterName == changedParameterName)
      return;

    selectedParameterName = changedParameterName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::parametersListItemChanged(QTreeWidgetItem *item, int column)
{
 Q_UNUSED(column);
 try
 {
  if(UpdateInterfaceFlag || !item)
   return;
  commitBoolPropertyFromItem(item, currentDrawPropertyComponentName, item->text(0));
 }
catch (RDK::UException &exception)
{
    RDK::Logging::SystemLog(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
}
catch (std::exception &exception)
{
    RDK::Logging::SystemLog(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
}
}

void UComponentsListWidgetModern::stateListItemChanged(QTreeWidgetItem *item, int column)
{
    parametersListItemChanged(item, column);
}

void UComponentsListWidgetModern::inputsListItemChanged(QTreeWidgetItem *item, int column)
{
    parametersListItemChanged(item, column);
}

void UComponentsListWidgetModern::outputsListItemChanged(QTreeWidgetItem *item, int column)
{
    parametersListItemChanged(item, column);
}

void UComponentsListWidgetModern::stateListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetState->currentItem();
    if(!item)
      return;

    QString changedStateName = item->data(0, Qt::DisplayRole).toString();
    if(selectedStateName == changedStateName)
      return;

    selectedStateName = changedStateName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::inputsListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetInputs->currentItem();
    if(!item)
      return;

    QString changedInputName = item->data(0, Qt::DisplayRole).toString();
    if(selectedInputName == changedInputName)
      return;

    selectedInputName = changedInputName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::outputsListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetOutputs->currentItem();
    if(!item)
      return;

    QString changedOutputName = item->data(0, Qt::DisplayRole).toString();
    if(selectedOutputName == changedOutputName)
      return;

    selectedOutputName = changedOutputName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::favoritesListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetFavorites->currentItem();
    if(!item || item->childCount() > 0)
      return;

    if(item->data(0, kFavRoleProp).toString().isEmpty())
      return;

    QString changedFavName = item->data(0, Qt::DisplayRole).toString();
    if(selectedFavName == changedFavName)
      return;

    selectedFavName = changedFavName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::favoritesListItemChanged(QTreeWidgetItem *item, int column)
{
 Q_UNUSED(column);
try
{
    if(UpdateInterfaceFlag)
        return;

     if(!item || item->childCount() > 0)
        return;

     QString prop_name = item->data(0, kFavRoleProp).toString();
     QString component_long_name = item->data(0, kFavRoleComponent).toString();
     if(prop_name.isEmpty() || component_long_name.isEmpty())
        return;

     commitBoolPropertyFromItem(item, component_long_name, prop_name);
}

catch (RDK::UException &exception)
{
    RDK::Logging::SystemLog(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
}
catch (std::exception &exception)
{
    RDK::Logging::SystemLog(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
}

}

void UComponentsListWidgetModern::unifiedListItemChanged(QTreeWidgetItem *item, int column)
{
    favoritesListItemChanged(item, column);
}

void UComponentsListWidgetModern::applyBoolCheckFlags(QTreeWidgetItem* item, bool checked)
{
    if(!item)
        return;
    item->setFlags((item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable)
                   & ~Qt::ItemIsEditable);
    item->setCheckState(1, checked ? Qt::Checked : Qt::Unchecked);
    const QString text = checked ? QStringLiteral("true") : QStringLiteral("false");
    item->setText(1, text);
    item->setData(1, Qt::UserRole, text);
    item->setToolTip(1, text);
}

bool UComponentsListWidgetModern::parseBoolPropertyValue(const QString& raw) const
{
    const QString s = raw.trimmed().toLower();
    if(s.isEmpty())
        return false;
    if(s == QStringLiteral("true") || s == QStringLiteral("1") || s == QStringLiteral("yes")
       || s == QStringLiteral("on"))
        return true;
    if(s == QStringLiteral("false") || s == QStringLiteral("0") || s == QStringLiteral("no")
       || s == QStringLiteral("off"))
        return false;
    // Numeric / stream leftovers ("1\n", etc.)
    bool ok = false;
    const int n = s.toInt(&ok);
    if(ok)
        return n != 0;
    return s.startsWith(QLatin1Char('t')) || s.startsWith(QLatin1Char('y'));
}

bool UComponentsListWidgetModern::commitBoolPropertyFromItem(QTreeWidgetItem* item,
                                                             const QString& componentLongName,
                                                             const QString& propertyName)
{
    if(!item || propertyName.isEmpty())
        return false;

    RDK::UELockPtr<RDK::UContainer> model =
        RDK::GetModelLockTimeout(getWorkChannelIndex(), kModelLockTimeoutMs);
    if(!model)
    {
        schedulePropertyReloadRetry();
        return false;
    }

    RDK::UEPtr<RDK::UContainer> cont;
    if(componentLongName.isEmpty())
        cont = model.Get();
    else
        cont = model->GetComponentL(componentLongName.toLocal8Bit().constData(), true);
    if(!cont)
        return false;

    RDK::UEPtr<RDK::UIProperty> property =
        cont->FindProperty(propertyName.toLocal8Bit().constData());
    if(!property || property->GetLanguageType() != typeid(bool))
        return false;

    bool writable = (item->checkState(1) == Qt::Checked);
    if(!property->ReadFromMemory(&writable))
        return false;

    // Keep label in sync with checkbox (same string Show XML uses).
    const QString text = writable ? QStringLiteral("true") : QStringLiteral("false");
    item->setText(1, text);
    item->setData(1, Qt::UserRole, text);
    item->setToolTip(1, text);
    emit selectedPropertyValue(text);
    return true;
}

void UComponentsListWidgetModern::handleSnapshotUpdated(NMSDK::UGuiSnapshotPtr snapshot,
                                                  const QStringList &added,
                                                  const QStringList &removed,
                                                  const QStringList &changed)
{
    lastSnapshot = snapshot;
    if (!snapshot)
        return;

    // Structural rebuild only when components added/removed/renamed (fingerprint).
    // Property-value ticks must not clear()/rebuild the tree (expand flicker).
    const bool treeEmpty = !componentsTree || componentsTree->topLevelItemCount() == 0;
    const bool structureChanged = !added.isEmpty() || !removed.isEmpty() || !changed.isEmpty();
    if (!treeEmpty && !structureChanged)
        return;

    if (UpdateInterfaceFlag) {
        QMetaObject::invokeMethod(this, [this]() { UpdateInterface(true); }, Qt::QueuedConnection);
    } else {
        UpdateInterface(true);
    }
}

void UComponentsListWidgetModern::handleFilterTextChanged(const QString &text)
{
    if(!componentsTree)
        return;

    componentFilterText = text.trimmed();
    applyFilter(componentsTree->invisibleRootItem());

    // Обновляем виджет для отображения изменений
    componentsTree->update();
}

void UComponentsListWidgetModern::rebuildTreeFromSnapshot(const NMSDK::UGuiSnapshotPtr &snapshot)
{
    if (!snapshot)
        return;

    const int scrollMax = componentsTree->verticalScrollBar()->maximum();
    const int scrollPos = componentsTree->verticalScrollBar()->value();

    QSignalBlocker blocker(componentsTree);
    componentsTree->clear();

    auto *rootItem = new QTreeWidgetItem(componentsTree);
    rootItem->setText(0, tr("Model"));
    rootItem->setData(0, Qt::UserRole, QString());
    rootItem->setExpanded(true);

    QHash<QString, QTreeWidgetItem*> items;
    items.insert(QString(), rootItem);

    // Pass 1: create items detached; order from ComponentOrder (model DFS), not QHash::keys().
    const auto& componentNames = snapshot->ComponentOrder;
    for (const QString &name : componentNames) {
        const auto summary = snapshot->Components.value(name);
        auto *item = new QTreeWidgetItem();
        item->setText(0, summary.ShortName);
        item->setToolTip(0, summary.LongName + QStringLiteral("\n") + summary.ClassName);
        item->setData(0, Qt::UserRole, summary.LongName);
        items.insert(summary.LongName, item);
    }

    // Pass 2: attach under parent (or Model root).
    for (const QString &name : componentNames) {
        const auto summary = snapshot->Components.value(name);
        QTreeWidgetItem *item = items.value(summary.LongName);
        if (!item || item == rootItem)
            continue;
        QTreeWidgetItem *parent = items.value(summary.ParentName, rootItem);
        if (!parent)
            parent = rootItem;
        parent->addChild(item);
    }

    applyFilter(rootItem);
    componentsTree->verticalScrollBar()->setMaximum(scrollMax);
    componentsTree->verticalScrollBar()->setValue(scrollPos);

    if(channelsSelectionVisible)
    {
      redrawChannelsList();
    }
}

bool UComponentsListWidgetModern::applyFilter(QTreeWidgetItem *item)
{
    if (!item)
        return false;

    bool matches = componentFilterText.isEmpty()
            || item->text(0).contains(componentFilterText, Qt::CaseInsensitive)
            || item->data(0, Qt::UserRole).toString().contains(componentFilterText, Qt::CaseInsensitive);

    bool childMatches = false;
    for (int i = 0; i < item->childCount(); ++i) {
        childMatches |= applyFilter(item->child(i));
    }

    const bool isRoot = item->data(0, Qt::UserRole).toString().isEmpty();
    const bool visible = matches || childMatches || isRoot;
    item->setHidden(!visible);
    // Разворачиваем узлы только при активном фильтре; при пустом фильтре не меняем развёрнутость
    if (visible && matches && !isRoot && !componentFilterText.isEmpty()) {
        item->setExpanded(true);
    }
    return visible;
}

void UComponentsListWidgetModern::componentSelectedFromScheme(QString name)
{
    const QString resolvedName =
        resolveComponentLongNameFromModelRoot(getWorkChannelIndex(), name);

    m_propertyReloadRetryCount = 0;
    m_propertyReloadRetryTarget = resolvedName;
    selectedComponentLongName = resolvedName;
    currentDrawComponentName = resolvedName;

    m_isUpdatingFromScheme = true;
    componentsTree->blockSignals(true);

    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole).toString() == resolvedName)
        {
            QTreeWidgetItem *item = *iterator;
            while(item)
            {
                item->setExpanded(true);
                item = item->parent();
            }

            componentsTree->setCurrentItem(*iterator);
            componentsTree->scrollToItem(*iterator, QAbstractItemView::EnsureVisible);
            break;
        }
        ++iterator;
    }

    componentsTree->blockSignals(false);
    reloadPropertys(true);

    QTimer::singleShot(0, [this]() {
        m_isUpdatingFromScheme = false;
    });
}

void UComponentsListWidgetModern::componentDoubleClickFromScheme(QString name)
{
    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole) == name)
        {
            componentsTree->setCurrentItem(*iterator);
            (*iterator)->setExpanded(true);
            currentDrawComponentName = (*iterator)->data(0, Qt::UserRole).toString();
            return;
        }
        ++iterator;
    }
}

void UComponentsListWidgetModern::componentStapBackFromScheme()
{
    QStringList list = currentDrawComponentName.split(".");
    if (list.isEmpty()) return;
    list.pop_back();
    currentDrawComponentName = list.join(".");
    componentSelectedFromScheme(currentDrawComponentName);
    emit componentSelected(currentDrawComponentName);
}

void UComponentsListWidgetModern::channelsListSelectionChanged()
{
  if(ui->listWidgetChannelSelection->currentItem())
  {
    currentChannel = ui->listWidgetChannelSelection->currentItem()->data(Qt::UserRole).toInt();
  }
}

void UComponentsListWidgetModern::onComponentItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if(!item) return;

    // Раскрываем путь к компоненту (все родительские элементы)
    QTreeWidgetItem *currentItem = item;
    while (currentItem) {
        currentItem->setExpanded(true);
        currentItem = currentItem->parent();
    }

    // Устанавливаем текущий элемент (если еще не установлен)
    if(componentsTree->currentItem() != item) {
        componentsTree->setCurrentItem(item);
    }

    // Прокручиваем к выбранному элементу
    componentsTree->scrollToItem(item, QAbstractItemView::EnsureVisible);
}

void UComponentsListWidgetModern::drawSelectedComponent(QModelIndex index)
{
    // Получаем данные компонента из index
    QString componentName = index.data(Qt::UserRole).toString();
    if(componentName.isEmpty()) return;

    // Находим элемент в дереве по данным
    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole) == componentName)
        {
            // Устанавливаем текущий элемент
            componentsTree->setCurrentItem(*iterator);

            // Вызываем обработчик выбора компонента (как при одинарном клике)
            componentListItemSelectionChanged();

            // Закрываем popup после выбора компонента
            if (treePopupDialog->isVisible()) {
                hideTreePopup();
            }
            return;
        }
        ++iterator;
    }
}


void UComponentsListWidgetModern::componentMoveUp()
{
    if(componentsTree->currentItem())
    {
        MModel_ChangeComponentPosition(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit(),-1);
        UpdateInterface(true);
    }
}

void UComponentsListWidgetModern::componentMoveDown()
{
    if(componentsTree->currentItem())
    {
        MModel_ChangeComponentPosition(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit(), 1);
        UpdateInterface(true);
    }
}

void UComponentsListWidgetModern::componentRename()
{
    if(componentsTree->currentItem())
    {
        bool ok;
        QString restoreName = componentsTree->currentItem()->data(0, Qt::DisplayRole).toString();
        QString text = QInputDialog::getText(this, tr("Rename component"),
                                             tr("Enter new component name: "), QLineEdit::Normal,
                                             restoreName, &ok);
        if (ok && !text.isEmpty())
        {
            std::string new_name(text.toLocal8Bit());
            MModel_SetComponentPropertyData(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit(),"Name", &new_name);

            QStringList nameSeparator = selectedComponentLongName.split(".");
            nameSeparator.pop_back();
            nameSeparator.push_back(text);
            selectedComponentLongName = nameSeparator.join(".");
            emit updateScheme(true);
            UpdateInterface(true);
        }
    }
}

void UComponentsListWidgetModern::componentDelete()
{
    if(componentsTree->currentItem())
    {
        if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Are you sure you want to delete component "+selectedComponentLongName+"?", QMessageBox::Yes|QMessageBox::Cancel);
            if (reply != QMessageBox::Yes) return;
        }

        const int rc = MModel_DelComponent(getWorkChannelIndex(), "", selectedComponentLongName.toLocal8Bit());
        if(rc != RDK_SUCCESS)
        {
            QMessageBox::warning(this, "Warning",
                QString("Failed to delete component %1 (code %2).")
                    .arg(selectedComponentLongName).arg(rc));
            return;
        }
        RDK::UIVisualControllerStorage::UpdateInterface(true);
        emit updateScheme(true);
    }
}

void UComponentsListWidgetModern::componentCopyNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QStringList list = selectedComponentLongName.split(".");
    QString name = list.last();
    clipboard->setText(name);
}

void UComponentsListWidgetModern::componentCopyLongNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(selectedComponentLongName);
}

void UComponentsListWidgetModern::componentCopyClassNameToClipboard()
{
    const QString className =
        componentClassNameFromModelScope(getWorkChannelIndex(), selectedComponentLongName);
    if(!className.isEmpty())
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(className);
    }
}

void UComponentsListWidgetModern::componentReset()
{
    if(componentsTree->currentItem())
    {
        Env_Reset(selectedComponentLongName.toLocal8Bit());
        UpdateInterface(true);
        refreshAfterComponentMutation(selectedComponentLongName);
    }
}

void UComponentsListWidgetModern::componentCalculate()
{
    if(componentsTree->currentItem())
    {
        Env_Calculate(selectedComponentLongName.toLocal8Bit());
        refreshAfterComponentMutation(selectedComponentLongName);
    }
}

void UComponentsListWidgetModern::componentInit()
{
 if(componentsTree->currentItem())
 {
     Env_ModelInit(selectedComponentLongName.toLocal8Bit());
     refreshAfterComponentMutation(selectedComponentLongName);
 }
}

void UComponentsListWidgetModern::componentUnInit()
{
 if(componentsTree->currentItem())
 {
     Env_ModelUnInit(selectedComponentLongName.toLocal8Bit());
     refreshAfterComponentMutation(selectedComponentLongName);
 }
}

void UComponentsListWidgetModern::componentGUI()
{
    if(!componentsTree->currentItem() || selectedComponentLongName.isEmpty())
        return;

    const QString componentClassName =
        componentClassNameFromModelScope(getWorkChannelIndex(), selectedComponentLongName);

    UComponentGuiContext context;
    context.componentLongName = selectedComponentLongName;
    context.componentClassName = componentClassName;
    context.channelIndex = Core_GetSelectedChannelIndex();

    emit openComponentGuiRequested(context);

    if(receivers(SIGNAL(openComponentGuiRequested(UComponentGuiContext))) == 0)
    {
        if(!UComponentFormRegistry::instance().canOpen(context))
        {
            QMessageBox::information(this, "Component GUI", "Component GUI form is not registered for this class.");
            return;
        }
        static UComponentGuiService fallbackService;
        fallbackService.setApplication(application);
        if(!fallbackService.createOrActivate(this, context))
            QMessageBox::information(this, "Component GUI", "Failed to open component GUI form.");
    }
}

void UComponentsListWidgetModern::setUpdateInterval(long value)
{
  UpdateInterval = value;
}

void UComponentsListWidgetModern::addComponentSons(QString componentName, QTreeWidgetItem *treeWidgetFather, QString oldRootItem, QString oldSelectedItem, const QSet<QString> &expandedItems)
{
 // Use timeout to avoid blocking UI during calculation
 RDK::UELockPtr<RDK::UEngine> engine=
     RDK::GetEngineLockTimeout<RDK::UEngine>(getWorkChannelIndex(), kModelLockTimeoutMs);
 if (!engine)
 {
  scheduleTreeRebuildRetry();
  return;
 }
    const char * stringBuff = MModel_GetComponentsNameList(getWorkChannelIndex(), componentName.toLocal8Bit());
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    QString str;
    if(!componentNames.empty()&&componentNames[0]!="")
    {
        QString father;
        if(!componentName.isEmpty()) father = componentName + ".";
        foreach(str, componentNames)
        {
            QString fullName = father + str;
            QTreeWidgetItem* childItem = new QTreeWidgetItem(treeWidgetFather);
            childItem->setText(0, str);
            childItem->setData(0, Qt::UserRole, fullName);

            if (!expandedItems.isEmpty() && expandedItems.contains(fullName))
                childItem->setExpanded(true);

            if(oldRootItem == fullName)
            {
                componentsTree->setCurrentItem(childItem);
                childItem->setExpanded(true);
            }
            if(oldSelectedItem == fullName)
            {
                componentsTree->setCurrentItem(childItem);
                childItem->setExpanded(true);
            }

            addComponentSons(father+str, childItem, oldRootItem, oldSelectedItem, expandedItems);
        }
    }
}

void UComponentsListWidgetModern::redrawChannelsList()
{
  ui->listWidgetChannelSelection->clear();
  int channelsCounter = Core_GetNumChannels();
  for(int i = 0; i < channelsCounter; i++)
  {
    QListWidgetItem *item = new QListWidgetItem(ui->listWidgetChannelSelection);
    item->setText(QString::number(i) + " ch.");
    item->setData(Qt::UserRole, i);
    if(i == currentChannel)
      ui->listWidgetChannelSelection->setCurrentItem(item);
  }
}

/// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
std::string& UComponentsListWidgetModern::EraseLeadEndls(std::string &value)
{
 std::string::size_type data_i=value.find_first_of("\n");
 if(data_i != std::string::npos)
 {
  value.erase(value.begin(),value.begin()+data_i+1);
 }
 return value;
}

/// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
std::string& UComponentsListWidgetModern::EraseRangeEndls(std::string &value)
{
 std::string::size_type data_i=value.find_first_of("\n");
 if(data_i != std::string::npos && data_i<2)
 {
  value.erase(value.begin(),value.begin()+data_i+1);
 }

 data_i=value.find_last_of("\r\n");
 if(data_i != std::string::npos && data_i>=value.size()-1)
 {
  value.erase(value.begin()+data_i,value.end());
 }
 return value;
}

/// пїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ, пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
/// пїЅпїЅ "[SEE BELOW]"
std::string& UComponentsListWidgetModern::PreparePropertyValueToListView(std::string &value)
{
 EraseRangeEndls(value);
 if(value.empty())
 {
  value="[EMPTY]";
 }
 else
 {
  if(value.find_first_of("\n") != std::string::npos)
   value="[SEE BELOW]";
 }
 return value;
}


void UComponentsListWidgetModern::propertyCopyNameToClipboard()
{
 QClipboard *clipboard = QApplication::clipboard();

 QString value;
 QWidget* tab = ui->tabWidgetComponentInfo->currentWidget();
 if(tab == ui->tabParameters)
 {
     if(ui->treeWidgetParameters->currentItem())
       value=ui->treeWidgetParameters->currentItem()->data(0, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabState)
 {
     if(ui->treeWidgetState->currentItem())
       value=ui->treeWidgetState->currentItem()->data(0, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabInputs)
 {
     if(ui->treeWidgetInputs->currentItem())
       value=ui->treeWidgetInputs->currentItem()->data(0, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabOutputs)
 {
     if(ui->treeWidgetOutputs->currentItem())
       value=ui->treeWidgetOutputs->currentItem()->data(0, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabFavorites)
 {
     QTreeWidgetItem* item = ui->treeWidgetFavorites->currentItem();
     if(item && item->childCount() == 0)
       value=item->data(0, kFavRoleProp).toString();
 }

 clipboard->setText(value);
}

void UComponentsListWidgetModern::propertyCopyValueToClipboard()
{
 QClipboard *clipboard = QApplication::clipboard();

 QTreeWidgetItem* item = currentPropertyItem();
 if(!item)
 {
   clipboard->setText(QString());
   return;
 }
 clipboard->setText(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::propertyPasteValueFromClipboard()
{
 QClipboard *clipboard = QApplication::clipboard();
 QString value = clipboard->text();
 QTreeWidgetItem* item = currentPropertyItem();
 if(!item)
   return;
 applyPropertyValueFromEditor(item, value);
}

void UComponentsListWidgetModern::propertyEditValue()
{
  beginPropertyValueEdit(currentPropertyItem());
}

void UComponentsListWidgetModern::propertyShowXml()
{
  const QString longName = getSelectedComponentLongName();
  if(longName.isEmpty())
    return;
  if(!m_propertyXmlDialog)
  {
    m_propertyXmlDialog = new UPropertyXMLWidget(this);
    connect(m_propertyXmlDialog, &UPropertyXMLWidget::propertiesApplied, this, [this]() {
      reloadPropertys(true);
    });
  }
  m_propertyXmlDialog->initWidget(longName, currentPropertyXmlMask());
  m_propertyXmlDialog->show();
  m_propertyXmlDialog->raise();
  m_propertyXmlDialog->activateWindow();
}


void UComponentsListWidgetModern::on_actionReloadTree_triggered()
{
    UpdateInterface(true);
}


void UComponentsListWidgetModern::on_tabWidgetComponentInfo_currentChanged(int index)
{
    reloadPropertys(true);
}


void UComponentsListWidgetModern::on_actionDefaultAllParameters_triggered()
{
    if(componentsTree->currentItem())
    {
        if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Are you sure you want to reset all parameters for component "+selectedComponentLongName+" to default values?", QMessageBox::Yes|QMessageBox::Cancel);
            if (reply == QMessageBox::Cancel) return;
        }

        RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
        std::string stringid = selectedComponentLongName.toLocal8Bit().constData();
        RDK::UEPtr<RDK::UNet> object;
        if(stringid.empty())
         object=RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetModel());
        else
         object=RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetEngine()->FindComponent(stringid.c_str()));

        RDK::UEPtr<RDK::UNet> owner = RDK::dynamic_pointer_cast<RDK::UNet>(object->GetOwner());
        RDK::UStringLinksList links_list;

        if(owner)
         object->GetLinks(links_list, owner, true, object);
        storage->DefaultObject(object);
        if(owner)
         object->CreateLinks(links_list, owner);
        refreshAfterComponentMutation(selectedComponentLongName);
    }
}

void UComponentsListWidgetModern::toggleTreeViewMode()
{
    if(m_componentTreeEmbedded)
        return; // дерево всегда на панели — popup не используется

    if (treePopupDialog->isVisible()) {
        // Если popup открыт, закрываем его
        hideTreePopup();
    } else {
        // Если popup закрыт, открываем его
        showTreePopup();
    }
}


void UComponentsListWidgetModern::showTreePopup()
{
    // Перемещаем виджеты из основного контейнера в popup
    QWidget *treeContainer = ui->treeContainer;
    QVBoxLayout *treeLayout = qobject_cast<QVBoxLayout*>(treeContainer->layout());

    if (treeLayout) {
        // Удаляем виджеты из основного layout
        treeLayout->removeWidget(filterLineEdit);
        treeLayout->removeWidget(componentsTree);
    }

    // Добавляем виджеты в popup layout (если их еще нет)
    if (treePopupLayout->indexOf(filterLineEdit) == -1) {
        treePopupLayout->addWidget(filterLineEdit);
    }
    if (treePopupLayout->indexOf(componentsTree) == -1) {
        treePopupLayout->addWidget(componentsTree);
    }

    // Вычисляем размер и позицию popup
    QPoint globalPos = toggleModeButton->mapToGlobal(QPoint(0, toggleModeButton->height()));

    // Современный способ получения геометрии экрана без использования устаревшего QDesktopWidget
    QScreen *screen = nullptr;
    if (window()) {
        screen = window()->screen();
    }
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }

    QRect screenGeometry;
    if (screen) {
        // Используем перегрузку без аргументов (совместимо с Qt 5.15.2)
        screenGeometry = screen->availableGeometry();
    } else {
        // Резервный вариант на случай отсутствия данных об экране
        screenGeometry = QRect(globalPos.x(), globalPos.y(), 800, 600);
    }

    // Ширина popup = минимальная ширина
    int popupWidth = 300;
    // Высота = доступная высота экрана минус позиция минус отступ
    int popupHeight = qMin(screenGeometry.height() - globalPos.y() - 10, 600);
    popupHeight = qMax(popupHeight, 200); // Минимальная высота

    // Проверяем, не выходит ли popup за границы экрана
    if (globalPos.x() + popupWidth > screenGeometry.right()) {
        globalPos.setX(screenGeometry.right() - popupWidth);
    }
    if (globalPos.x() < screenGeometry.left()) {
        globalPos.setX(screenGeometry.left());
    }

    treePopupDialog->setGeometry(globalPos.x(), globalPos.y(), popupWidth, popupHeight);
    treePopupDialog->show();
    treePopupDialog->raise();
    treePopupDialog->activateWindow();

    // Keep current expand state (do not expandAll — resets user navigation).

    // Устанавливаем фокус на поле фильтра
    filterLineEdit->setFocus();

    toggleModeButton->setChecked(true);
    toggleModeButton->setText(tr("☷"));
    toggleModeButton->setToolTip(tr("Скрыть дерево компонентов"));
}

void UComponentsListWidgetModern::hideTreePopup()
{
    if (!treePopupDialog->isVisible()) {
        return;
    }

    // Перемещаем виджеты обратно в основной контейнер
    treePopupLayout->removeWidget(filterLineEdit);
    treePopupLayout->removeWidget(componentsTree);

    QWidget *treeContainer = ui->treeContainer;
    QVBoxLayout *treeLayout = qobject_cast<QVBoxLayout*>(treeContainer->layout());
    if (treeLayout) {
        treeLayout->addWidget(filterLineEdit);
        treeLayout->addWidget(componentsTree);
    }

    treePopupDialog->hide();

    toggleModeButton->setChecked(false);
    toggleModeButton->setText(tr("☰"));
    toggleModeButton->setToolTip(tr("Показать дерево компонентов"));
}

void UComponentsListWidgetModern::updateFavoritesTabVisibility(bool hasFavorites, bool selectFavorites)
{
    QTabWidget* tabs = ui->tabWidgetComponentInfo;
    const int favIndex = tabs->indexOf(ui->tabFavorites);

    if(!hasFavorites)
    {
        if(favIndex >= 0)
            tabs->removeTab(favIndex);
        return;
    }

    {
        const QSignalBlocker blocker(tabs);
        if(favIndex < 0)
            tabs->insertTab(0, ui->tabFavorites, tr("Favorites"));
        else if(favIndex != 0)
            tabs->tabBar()->moveTab(favIndex, 0);

        if(selectFavorites)
            tabs->setCurrentWidget(ui->tabFavorites);
    }
}

int UComponentsListWidgetModern::logicalTabIndexFromWidget(QWidget* tab) const
{
    if(tab == ui->tabParameters)
        return 0;
    if(tab == ui->tabState)
        return 1;
    if(tab == ui->tabInputs)
        return 2;
    if(tab == ui->tabOutputs)
        return 3;
    if(tab == ui->tabFavorites)
        return 4;
    return -1;
}

QWidget* UComponentsListWidgetModern::widgetFromLogicalTabIndex(int logicalIndex) const
{
    switch(logicalIndex)
    {
    case 0: return ui->tabParameters;
    case 1: return ui->tabState;
    case 2: return ui->tabInputs;
    case 3: return ui->tabOutputs;
    case 4: return ui->tabFavorites;
    default: return nullptr;
    }
}


void UComponentsListWidgetModern::setPropertyListOptions(const PropertyListOptions& opt)
{
    m_propertyListOptions = opt;
    applyPropertyListPresentationUi();
    reloadPropertys(true);
}

void UComponentsListWidgetModern::setPropertyListPresentation(PropertyListPresentation mode)
{
    if(m_propertyListOptions.presentation == mode)
        return;
    m_propertyListOptions.presentation = mode;
    applyPropertyListPresentationUi();
    reloadPropertys(true);
}

void UComponentsListWidgetModern::setVisiblePropertyGroups(PropertyGroupFlags flags)
{
    m_propertyListOptions.visibleGroups = flags;
    if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped)
        reloadPropertys(true);
}

void UComponentsListWidgetModern::setWatchablePropertiesOnly(bool on)
{
    if(m_propertyListOptions.watchableOnly == on)
        return;
    m_propertyListOptions.watchableOnly = on;
    reloadPropertys(true);
}

void UComponentsListWidgetModern::setPropertySubtitleMode(PropertySubtitleMode mode)
{
    m_propertyListOptions.subtitleMode = mode;
    if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped)
        reloadPropertys(true);
}

void UComponentsListWidgetModern::setPropertyInlineEditEnabled(bool on)
{
    m_propertyListOptions.allowInlineEdit = on;
}

void UComponentsListWidgetModern::setPropertyValueColumnVisible(bool on)
{
    m_propertyListOptions.showValueColumn = on;
    if(m_unifiedTree)
        m_unifiedTree->setColumnHidden(1, !on);
}

void UComponentsListWidgetModern::setExpandPropertyGroups(bool on)
{
    m_propertyListOptions.expandGroups = on;
}

void UComponentsListWidgetModern::ensureUnifiedPropertyPage()
{
    if(m_unifiedPage && m_unifiedTree)
        return;

    m_unifiedPage = new QWidget(ui->tabWidgetComponentInfo);
    auto* layout = new QVBoxLayout(m_unifiedPage);
    layout->setContentsMargins(0, 0, 0, 0);
    m_unifiedTree = new QTreeWidget(m_unifiedPage);
    m_unifiedTree->setColumnCount(2);
    m_unifiedTree->setHeaderLabels(QStringList() << tr("Name") << tr("Value"));
    m_unifiedTree->setRootIsDecorated(true);
    m_unifiedTree->setAlternatingRowColors(true);
    layout->addWidget(m_unifiedTree);

    m_unifiedTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_unifiedTree->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_unifiedTree->setItemDelegate(new FavoritesPathSubtitleDelegate(m_unifiedTree, this));
    connect(m_unifiedTree, &QTreeWidget::itemDoubleClicked,
            this, &UComponentsListWidgetModern::onPropertyItemDoubleClicked);
    connect(m_unifiedTree, &QTreeWidget::itemSelectionChanged,
            this, &UComponentsListWidgetModern::unifiedListSelectionChanged);
    connect(m_unifiedTree, &QTreeWidget::itemChanged,
            this, &UComponentsListWidgetModern::unifiedListItemChanged);
    m_unifiedTree->addAction(m_actionEditProperty);
    if(m_actionShowPropertyXml)
        m_unifiedTree->addAction(m_actionShowPropertyXml);
    m_unifiedTree->viewport()->installEventFilter(this);
    m_unifiedTree->installEventFilter(this);
}

void UComponentsListWidgetModern::applyPropertyListPresentationUi()
{
    ensureUnifiedPropertyPage();
    QTabWidget* tabs = ui->tabWidgetComponentInfo;
    if(!tabs)
        return;

    if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped)
    {
        if(m_savedPropertyTabs.isEmpty())
        {
            for(int i = 0; i < tabs->count(); ++i)
            {
                m_savedPropertyTabs.append(tabs->widget(i));
                m_savedPropertyTabLabels.append(tabs->tabText(i));
            }
        }
        while(tabs->count() > 0)
            tabs->removeTab(0);
        m_unifiedTabIndex = tabs->addTab(m_unifiedPage, tr("Properties"));
        if(m_propertyListOptions.hideTabBarInUnified && tabs->tabBar())
            tabs->tabBar()->hide();
        m_unifiedTree->setColumnHidden(1, !m_propertyListOptions.showValueColumn);
        applyPropertyColumnLayout();
    }
    else
    {
        if(tabs->tabBar())
            tabs->tabBar()->show();
        // Restore saved tabs if we previously stripped them
        if(!m_savedPropertyTabs.isEmpty())
        {
            while(tabs->count() > 0)
                tabs->removeTab(0);
            for(int i = 0; i < m_savedPropertyTabs.size(); ++i)
                tabs->addTab(m_savedPropertyTabs[i], m_savedPropertyTabLabels.value(i));
            m_savedPropertyTabs.clear();
            m_savedPropertyTabLabels.clear();
            m_unifiedTabIndex = -1;
        }
        else if(m_unifiedTabIndex >= 0)
        {
            const int idx = tabs->indexOf(m_unifiedPage);
            if(idx >= 0)
                tabs->removeTab(idx);
            m_unifiedTabIndex = -1;
        }
    }
}

QString UComponentsListWidgetModern::buildPropertySubtitle(const QString& propName,
                                                          const QString& typeName,
                                                          const QString& componentLongName) const
{
    switch(m_propertyListOptions.subtitleMode)
    {
    case PropertySubtitleMode::None:
        return QString();
    case PropertySubtitleMode::Type:
        return typeName;
    case PropertySubtitleMode::Path:
        return componentLongName.isEmpty() ? propName : (componentLongName + ":" + propName);
    case PropertySubtitleMode::TypeAndPath:
    {
        const QString path = componentLongName.isEmpty() ? propName : (componentLongName + ":" + propName);
        if(typeName.isEmpty())
            return path;
        return typeName + QStringLiteral(" — ") + path;
    }
    }
    return typeName;
}

void UComponentsListWidgetModern::unifiedListSelectionChanged()
{
    if(!m_unifiedTree)
        return;
    QTreeWidgetItem* item = m_unifiedTree->currentItem();
    if(!item || item->childCount() > 0 || item->data(0, kPropRoleIsGroup).toBool())
        return;
    if(item->data(0, kFavRoleProp).toString().isEmpty())
        return;
    const QString changed = item->data(0, Qt::DisplayRole).toString();
    if(selectedUnifiedName == changed)
        return;
    selectedUnifiedName = changed;
    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::fillUnifiedPropertyTree(RDK::UContainer* cont,
                                                         const RDK::UComponent::VariableMapT& varMap,
                                                         const std::vector<RDK::NameT>& propOrder,
                                                         bool is_new_inputs,
                                                         bool is_new_outputs,
                                                         const std::map<std::string, std::string>& favorites,
                                                         RDK::UContainerDescription* class_desc)
{
    if(!m_unifiedTree || !cont)
        return;

    const QString targetComponent = selectedComponentLongName;
    const PropertyGroupFlags groups = m_propertyListOptions.visibleGroups;
    m_unifiedTree->clear();

    auto makeGroup = [&](const QString& title) -> QTreeWidgetItem* {
        auto* g = new QTreeWidgetItem(m_unifiedTree);
        g->setText(0, title);
        g->setData(0, kPropRoleIsGroup, true);
        Qt::ItemFlags f = Qt::ItemIsEnabled;
        if(!m_propertyListOptions.selectLeavesOnly)
            f |= Qt::ItemIsSelectable;
        g->setFlags(f);
        QFont font = g->font(0);
        font.setBold(true);
        g->setFont(0, font);
        return g;
    };

    QTreeWidgetItem* groupParameters = (groups & GroupParameters) ? makeGroup(tr("Parameters")) : nullptr;
    QTreeWidgetItem* groupState = (groups & GroupState) ? makeGroup(tr("State")) : nullptr;
    QTreeWidgetItem* groupInputs = (groups & GroupInputs) ? makeGroup(tr("Inputs")) : nullptr;
    QTreeWidgetItem* groupOutputs = (groups & GroupOutputs) ? makeGroup(tr("Outputs")) : nullptr;
    QTreeWidgetItem* groupFavorites = (groups & GroupFavorites) ? makeGroup(tr("Favorites")) : nullptr;

    std::string buffer;

    for(size_t oi = 0; oi < propOrder.size(); ++oi)
    {
        RDK::UComponent::VariableMapCIteratorT i = varMap.find(propOrder[oi]);
        if(i == varMap.end() || !i->second.Property)
            continue;
        if(i->second.CheckMask(ptPubInput) && is_new_inputs && i->first.find("DataInput") == 0)
            continue;
        if(i->second.CheckMask(ptPubOutput) && is_new_outputs && i->first.find("DataOutput") == 0)
            continue;
        if(m_propertyListOptions.watchableOnly
           && !NMSDK::Plot::isWatchableLanguageType(i->second.Property->GetLanguageType()))
            continue;

        QTreeWidgetItem* parent = nullptr;
        // Single group: Parameter > Output > Input > State
        if(i->second.CheckMask(ptPubParameter) && groupParameters)
            parent = groupParameters;
        else if(i->second.CheckMask(ptPubOutput) && groupOutputs)
            parent = groupOutputs;
        else if(i->second.CheckMask(ptPubInput) && groupInputs)
            parent = groupInputs;
        else if(i->second.CheckMask(ptPubState) && groupState)
            parent = groupState;
        if(!parent)
            continue;

        const QString propName = QString::fromLocal8Bit(i->first.c_str());
        cont->GetPropertyValue(i->first, buffer);
        if(RDK::IsMatrixPropertyTypeName(i->second.Property->GetLanguageType().name()))
            buffer = RDK::NormalizeMatrixPropertyText(buffer);
        const QString rawValue = QString::fromLocal8Bit(buffer.c_str());
        QString typeName = QString::fromLocal8Bit(i->second.Property->GetLanguageType().name());
        if(RDK::IsMatrixPropertyTypeName(i->second.Property->GetLanguageType().name()))
            typeName = QStringLiteral("Matrix");

        auto* item = new QTreeWidgetItem(parent);
        item->setText(0, propName);
        item->setData(0, kFavRoleProp, propName);
        item->setData(0, kFavRoleComponent, targetComponent);
        item->setData(0, kFavRoleDisplayPath,
                      buildPropertySubtitle(propName, typeName, targetComponent));
        item->setData(0, kPropRoleIsGroup, false);
        std::string displayBuf = buffer;
        item->setData(1, Qt::UserRole, rawValue);
        item->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(displayBuf).c_str()));
        item->setToolTip(1, rawValue);

        if(m_propertyListOptions.allowInlineEdit)
        {
            if(i->second.Property->GetLanguageType() == typeid(bool))
            {
                applyBoolCheckFlags(item, parseBoolPropertyValue(rawValue));
            }
            else
            {
                item->setFlags((item->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable)
                               & ~Qt::ItemIsUserCheckable);
            }
        }
        else
        {
            item->setFlags((item->flags() | Qt::ItemIsSelectable)
                           & ~(Qt::ItemIsEditable | Qt::ItemIsUserCheckable));
        }

        if(propName == selectedUnifiedName)
            m_unifiedTree->setCurrentItem(item);
    }

    // Optional Favorites group (ClDesc) — single placement, no multi-section duplicates
    if(groupFavorites && !favorites.empty())
    {
        RDK::UELockPtr<RDK::UContainer> model =
            RDK::GetModelLockTimeout(getWorkChannelIndex(), kModelLockTimeoutMs);
        for(std::map<std::string, std::string>::const_iterator it = favorites.begin();
            it != favorites.end(); ++it)
        {
            QString favoriteName = QString::fromLocal8Bit(it->first.c_str());
            QString favoritePath = QString::fromLocal8Bit(it->second.c_str());
            favoritePath.replace("{CompName}", targetComponent);
            bool isAlias = class_desc && class_desc->IsFavoriteAlias(it->first);
            QString component_long_name;
            QString prop_name;
            if(isAlias && class_desc)
            {
                std::string componentPath, propertyName;
                if(class_desc->ParseFavoritePath(it->second, componentPath, propertyName))
                {
                    component_long_name = targetComponent;
                    if(!componentPath.empty())
                        component_long_name += "." + QString::fromStdString(componentPath);
                    prop_name = QString::fromStdString(propertyName);
                }
            }
            else
            {
                QStringList vals = favoritePath.split(":");
                if(vals.size() == 2)
                {
                    component_long_name = vals[0];
                    prop_name = vals[1];
                }
            }
            if(prop_name.isEmpty())
                continue;

            RDK::UEPtr<RDK::UContainer> child_cont;
            if(model)
                child_cont = model->GetComponentL(component_long_name.toLocal8Bit().constData(), true);
            if(m_propertyListOptions.watchableOnly && child_cont)
            {
                RDK::UEPtr<RDK::UIProperty> p = child_cont->FindProperty(prop_name.toStdString());
                if(!p || !NMSDK::Plot::isWatchableLanguageType(p->GetLanguageType()))
                    continue;
            }

            QString typeName;
            QString rawValue;
            RDK::UEPtr<RDK::UIProperty> favProp;
            if(child_cont)
            {
                child_cont->GetPropertyValue(prop_name.toStdString(), buffer);
                favProp = child_cont->FindProperty(prop_name.toStdString());
                if(favProp)
                {
                    typeName = QString::fromLocal8Bit(favProp->GetLanguageType().name());
                    if(RDK::IsMatrixPropertyTypeName(favProp->GetLanguageType().name()))
                    {
                        buffer = RDK::NormalizeMatrixPropertyText(buffer);
                        typeName = QStringLiteral("Matrix");
                    }
                }
                rawValue = QString::fromLocal8Bit(buffer.c_str());
            }

            auto* favoriteItem = new QTreeWidgetItem(groupFavorites);
            favoriteItem->setText(0, isAlias ? (favoriteName + " [Alias]") : favoriteName);
            favoriteItem->setToolTip(0, favoritePath);
            favoriteItem->setData(0, kFavRoleProp, prop_name);
            favoriteItem->setData(0, kFavRoleComponent, component_long_name);
            favoriteItem->setData(0, kFavRoleDisplayPath,
                                  buildPropertySubtitle(prop_name, typeName, component_long_name));
            favoriteItem->setData(0, kPropRoleIsGroup, false);
            if(child_cont)
            {
                std::string displayBuf = buffer;
                favoriteItem->setData(1, Qt::UserRole, rawValue);
                favoriteItem->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(displayBuf).c_str()));
                favoriteItem->setToolTip(1, rawValue);
            }
            if(m_propertyListOptions.allowInlineEdit && favProp
               && favProp->GetLanguageType() == typeid(bool))
            {
                applyBoolCheckFlags(favoriteItem, parseBoolPropertyValue(rawValue));
            }
            else if(m_propertyListOptions.allowInlineEdit)
            {
                favoriteItem->setFlags((favoriteItem->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable)
                                       & ~Qt::ItemIsUserCheckable);
            }
            else
            {
                favoriteItem->setFlags((favoriteItem->flags() | Qt::ItemIsSelectable)
                                       & ~(Qt::ItemIsEditable | Qt::ItemIsUserCheckable));
            }
        }
    }

    QList<QTreeWidgetItem*> allGroups;
    allGroups << groupParameters << groupState << groupInputs << groupOutputs << groupFavorites;
    for(QTreeWidgetItem* group : allGroups)
    {
        if(!group)
            continue;
        if(group->childCount() == 0)
        {
            delete group;
            continue;
        }
        if(m_propertyListOptions.sortWithinGroup == PropertySortMode::Alphabetical)
            group->sortChildren(0, Qt::AscendingOrder);
        if(m_propertyListOptions.expandGroups)
            group->setExpanded(true);
    }
    applyPropertyColumnLayout();
}

void UComponentsListWidgetModern::setupPropertyEditing()
{
    auto setupTree = [this](QTreeWidget* tree) {
        tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tree->setContextMenuPolicy(Qt::ActionsContextMenu);
        tree->setItemDelegate(new PropertyValueEditDelegate(tree, this));
        connect(tree, &QTreeWidget::itemDoubleClicked,
                this, &UComponentsListWidgetModern::onPropertyItemDoubleClicked);
        tree->viewport()->installEventFilter(this);
        tree->installEventFilter(this);
    };

    setupTree(ui->treeWidgetParameters);
    setupTree(ui->treeWidgetState);
    setupTree(ui->treeWidgetInputs);
    setupTree(ui->treeWidgetOutputs);

    ui->treeWidgetFavorites->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidgetFavorites->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeWidgetFavorites->setItemDelegate(
        new FavoritesPathSubtitleDelegate(ui->treeWidgetFavorites, this));
    connect(ui->treeWidgetFavorites, &QTreeWidget::itemDoubleClicked,
            this, &UComponentsListWidgetModern::onPropertyItemDoubleClicked);
    ui->treeWidgetFavorites->viewport()->installEventFilter(this);
    ui->treeWidgetFavorites->installEventFilter(this);
}

void UComponentsListWidgetModern::setupColumnWidthSync()
{
    for(QTreeWidget* tree : {ui->treeWidgetParameters, ui->treeWidgetState, ui->treeWidgetInputs,
                             ui->treeWidgetOutputs, ui->treeWidgetFavorites})
    {
        QHeaderView* header = tree->header();
        // Иначе last section всегда «съедает» сохранённую ширину Value/Type при layout
        header->setStretchLastSection(false);
        tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        // Name — Interactive (пользователь меняет ширину); Value — Stretch (остаток)
        if(tree->columnCount() > 0)
        {
            header->setSectionResizeMode(0, QHeaderView::Interactive);
            if(header->sectionSize(0) < kMinNameColumnWidth)
                header->resizeSection(0, kMinNameColumnWidth);
        }
        if(tree->columnCount() > 1)
            header->setSectionResizeMode(1, QHeaderView::Stretch);
        if(tree->columnCount() > 2)
            header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        connect(header, &QHeaderView::sectionResized,
                this, &UComponentsListWidgetModern::onPropertyHeaderSectionResized);
    }
}

void UComponentsListWidgetModern::onPropertyHeaderSectionResized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(oldSize);
    auto* header = qobject_cast<QHeaderView*>(sender());
    if(!header)
        return;
    auto* tree = qobject_cast<QTreeWidget*>(header->parentWidget());
    if(!tree)
        tree = qobject_cast<QTreeWidget*>(header->parent());
    // QHeaderView parent is the tree viewport's sibling — use header()->parent()
    if(!tree)
    {
        for(QTreeWidget* candidate : {ui->treeWidgetParameters, ui->treeWidgetState, ui->treeWidgetInputs,
                                      ui->treeWidgetOutputs, ui->treeWidgetFavorites})
        {
            if(candidate->header() == header)
            {
                tree = candidate;
                break;
            }
        }
    }
    if(!tree)
        return;
    syncPropertyColumnWidths(tree, logicalIndex, newSize);
}

void UComponentsListWidgetModern::syncPropertyColumnWidths(QTreeWidget* sourceTree, int logicalIndex, int newSize)
{
    if(m_syncingColumnWidths || !sourceTree)
        return;

    // Value (1) — Stretch: абсолютную ширину не синхронизируем
    QList<QTreeWidget*> targets;
    if(logicalIndex == 0)
    {
        targets << ui->treeWidgetParameters << ui->treeWidgetState
                << ui->treeWidgetInputs << ui->treeWidgetOutputs << ui->treeWidgetFavorites;
    }
    else if(logicalIndex == 2)
    {
        targets << ui->treeWidgetInputs << ui->treeWidgetOutputs;
    }
    else
    {
        return;
    }

    m_syncingColumnWidths = true;
    for(QTreeWidget* tree : targets)
    {
        if(tree == sourceTree)
            continue;
        if(logicalIndex >= tree->columnCount())
            continue;
        QHeaderView* header = tree->header();
        const QSignalBlocker blocker(header);
        header->resizeSection(logicalIndex, newSize);
    }
    m_syncingColumnWidths = false;

    // Пишем сразу — не ждём Save Project (раньше writeSettings почти не вызывался)
    ASaveParameters();
}

void UComponentsListWidgetModern::applySharedColumnWidths(int nameWidth, int valueWidth, int typeWidth)
{
    if(m_layoutPreset == ComponentsListLayoutPreset::PropertyPicker)
    {
        applyPropertyColumnLayout();
        return;
    }

    Q_UNUSED(valueWidth);
    m_syncingColumnWidths = true;
    for(QTreeWidget* tree : {ui->treeWidgetParameters, ui->treeWidgetState, ui->treeWidgetInputs,
                             ui->treeWidgetOutputs, ui->treeWidgetFavorites, m_unifiedTree})
    {
        if(!tree)
            continue;
        QHeaderView* header = tree->header();
        const QSignalBlocker blocker(header);
        header->setStretchLastSection(false);
        tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        if(tree->columnCount() > 0)
        {
            header->setSectionResizeMode(0, QHeaderView::Interactive);
            const int nw = nameWidth > 0 ? qMax(nameWidth, kMinNameColumnWidth) : kMinNameColumnWidth;
            header->resizeSection(0, nw);
        }
        if(tree->columnCount() > 1)
            header->setSectionResizeMode(1, QHeaderView::Stretch);
        if(tree->columnCount() > 2)
        {
            header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
            if(typeWidth > 0)
                header->resizeSection(2, typeWidth);
        }
    }
    m_syncingColumnWidths = false;
}

void UComponentsListWidgetModern::onPropertyItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if(!item)
        return;
    QTreeWidget* tree = item->treeWidget();
    if(tree)
        tree->setCurrentItem(item);

    // Bool: toggle is handled in viewport eventFilter on MouseButtonDblClick.
    // Fallback if the filter did not consume the event.
    if(item->data(1, Qt::CheckStateRole).isValid())
    {
        if(column == 1)
            toggleBoolPropertyItem(item);
        return;
    }

    beginPropertyValueEdit(item);
}

void UComponentsListWidgetModern::refreshAfterComponentMutation(const QString& longName)
{
    UModernDiagramWidget::invalidatePortsCacheEverywhere(longName);
    reloadPropertys(true);
    emit updateScheme(true);
}

bool UComponentsListWidgetModern::toggleBoolPropertyItem(QTreeWidgetItem* item)
{
    if(!item || !item->data(1, Qt::CheckStateRole).isValid())
        return false;
    if(!m_propertyListOptions.allowInlineEdit)
        return false;

    const bool next = item->checkState(1) != Qt::Checked;
    if(QTreeWidget* tree = item->treeWidget())
    {
        const QSignalBlocker blocker(tree);
        applyBoolCheckFlags(item, next);
    }
    else
    {
        applyBoolCheckFlags(item, next);
    }
    return commitBoolPropertyFromItem(item,
                                      propertyComponentForItem(item),
                                      propertyNameForItem(item));
}

QTreeWidget* UComponentsListWidgetModern::propertyTreeFromFilterObject(QObject* obj) const
{
    if(obj == ui->treeWidgetParameters || obj == ui->treeWidgetParameters->viewport())
        return ui->treeWidgetParameters;
    if(obj == ui->treeWidgetState || obj == ui->treeWidgetState->viewport())
        return ui->treeWidgetState;
    if(obj == ui->treeWidgetInputs || obj == ui->treeWidgetInputs->viewport())
        return ui->treeWidgetInputs;
    if(obj == ui->treeWidgetOutputs || obj == ui->treeWidgetOutputs->viewport())
        return ui->treeWidgetOutputs;
    if(obj == ui->treeWidgetFavorites || obj == ui->treeWidgetFavorites->viewport())
        return ui->treeWidgetFavorites;
    if(m_unifiedTree && (obj == m_unifiedTree || obj == m_unifiedTree->viewport()))
        return m_unifiedTree;
    return nullptr;
}

bool UComponentsListWidgetModern::handleBoolValueMouseEvent(QTreeWidget* tree, QMouseEvent* mouseEvent)
{
    if(!tree || !mouseEvent || !m_propertyListOptions.allowInlineEdit)
        return false;

    QTreeWidgetItem* item = tree->itemAt(mouseEvent->pos());
    if(!item)
        return false;

    const QModelIndex index = tree->indexAt(mouseEvent->pos());
    if(!index.isValid() || index.column() != 1)
        return false;
    if(!item->data(1, Qt::CheckStateRole).isValid())
        return false;

    if(mouseEvent->type() == QEvent::MouseButtonPress
       && mouseEvent->button() == Qt::LeftButton)
    {
        tree->setCurrentItem(item);
        return true; // select only — prevent Qt auto-toggle
    }

    if(mouseEvent->type() == QEvent::MouseButtonDblClick
       && mouseEvent->button() == Qt::LeftButton)
    {
        tree->setCurrentItem(item);
        toggleBoolPropertyItem(item);
        return true;
    }

    return false;
}

void UComponentsListWidgetModern::onFavoritesShowInAllSectionsToggled(bool checked)
{
    m_favoritesShowInAllSections = checked;
    reloadPropertys(true);
}

bool UComponentsListWidgetModern::isMultilinePropertyValue(const QString& rawValue, const QString& displayValue) const
{
    return rawValue.contains(QLatin1Char('\n')) || displayValue == QLatin1String("[SEE BELOW]");
}

QTreeWidget* UComponentsListWidgetModern::currentPropertyTree() const
{
    if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped)
        return m_unifiedTree;
    QWidget* tab = ui->tabWidgetComponentInfo->currentWidget();
    if(tab == ui->tabParameters) return ui->treeWidgetParameters;
    if(tab == ui->tabState) return ui->treeWidgetState;
    if(tab == ui->tabInputs) return ui->treeWidgetInputs;
    if(tab == ui->tabOutputs) return ui->treeWidgetOutputs;
    if(tab == ui->tabFavorites) return ui->treeWidgetFavorites;
    return nullptr;
}

QTreeWidgetItem* UComponentsListWidgetModern::currentPropertyItem() const
{
    QTreeWidget* tree = currentPropertyTree();
    if(!tree)
        return nullptr;
    QTreeWidgetItem* item = tree->currentItem();
    if(!item)
        return nullptr;
    if((tree == ui->treeWidgetFavorites || tree == m_unifiedTree) &&
       (item->childCount() > 0 || item->data(0, kPropRoleIsGroup).toBool()))
        return nullptr;
    return item;
}

QString UComponentsListWidgetModern::propertyComponentForItem(QTreeWidgetItem* item) const
{
    if(!item)
        return QString();
    if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped
       || ui->tabWidgetComponentInfo->currentWidget() == ui->tabFavorites)
    {
        const QString fromRole = item->data(0, kFavRoleComponent).toString();
        if(!fromRole.isEmpty())
            return fromRole;
    }
    return currentDrawPropertyComponentName;
}

QString UComponentsListWidgetModern::propertyNameForItem(QTreeWidgetItem* item) const
{
    if(!item)
        return QString();
    if(m_propertyListOptions.presentation == PropertyListPresentation::UnifiedGrouped
       || ui->tabWidgetComponentInfo->currentWidget() == ui->tabFavorites)
    {
        const QString prop = item->data(0, kFavRoleProp).toString();
        if(!prop.isEmpty())
            return prop;
    }
    return item->data(0, Qt::DisplayRole).toString();
}

void UComponentsListWidgetModern::updatePropertyItemDisplay(QTreeWidgetItem* item, const QString& rawValue)
{
    if(!item)
        return;
    item->setData(1, Qt::UserRole, rawValue);
    item->setToolTip(1, rawValue);
    std::string temp = rawValue.toLocal8Bit().constData();
    item->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
}

bool UComponentsListWidgetModern::commitPropertyValue(const QString& componentLongName,
                                                     const QString& propertyName,
                                                     const QString& value)
{
    if(propertyName.isEmpty())
        return false;
    const int rc = MModel_SetComponentPropertyValue(
        getWorkChannelIndex(),
        componentLongName.toLocal8Bit().constData(),
        propertyName.toLocal8Bit().constData(),
        value.toLocal8Bit().constData());
    if(rc != RDK_SUCCESS)
        return false;
    emit selectedPropertyValue(value);
    return true;
}

bool UComponentsListWidgetModern::applyPropertyValueFromEditor(QTreeWidgetItem* item, const QString& value)
{
    if(!item || UpdateInterfaceFlag)
        return false;
    if(item->data(1, Qt::CheckStateRole).isValid())
        return false;

    const QString comp = propertyComponentForItem(item);
    const QString prop = propertyNameForItem(item);
    if(prop.isEmpty())
        return false;

    QString commitValue = value;
    {
        RDK::UELockPtr<RDK::UContainer> model =
            RDK::GetModelLockTimeout(getWorkChannelIndex(), kModelLockTimeoutMs);
        if(model)
        {
            RDK::UEPtr<RDK::UContainer> cont;
            if(comp.isEmpty())
                cont = model.Get();
            else
                cont = model->GetComponentL(comp.toLocal8Bit().constData(), true);
            RDK::UEPtr<RDK::UNet> unet = RDK::dynamic_pointer_cast<RDK::UNet>(cont);
            if(unet)
            {
                RDK::UEPtr<RDK::UIProperty> property = unet->FindProperty(prop.toStdString());
                if(property && RDK::IsMatrixPropertyTypeName(property->GetLanguageType().name()))
                {
                    std::string normalized = RDK::NormalizeMatrixPropertyText(
                        value.toLocal8Bit().constData());
                    commitValue = QString::fromLocal8Bit(normalized.c_str());
                }
            }
        }
    }

    if(!commitPropertyValue(comp, prop, commitValue))
    {
        QToolTip::showText(QCursor::pos(),
                           tr("Failed to apply property value"),
                           this);
        return false;
    }

    QString displayValue = commitValue;
    std::string canonical;
    if(fetchPropertyValueFromModel(getWorkChannelIndex(), comp, prop, canonical))
        displayValue = QString::fromLocal8Bit(canonical.c_str());

    UpdateInterfaceFlag = true;
    updatePropertyItemDisplay(item, displayValue);
    UpdateInterfaceFlag = false;
    return true;
}

bool UComponentsListWidgetModern::beginPropertyValueEdit(QTreeWidgetItem* item)
{
    if(!item || (item->childCount() > 0 && ui->tabWidgetComponentInfo->currentWidget() == ui->tabFavorites))
        return false;
    // У QTreeWidgetItem ItemIsUserCheckable включён по умолчанию — смотрим реальный checkbox
    if(item->data(1, Qt::CheckStateRole).isValid())
        return false;

    const QString prop = propertyNameForItem(item);
    if(prop.isEmpty())
        return false;

    const QString raw = item->data(1, Qt::UserRole).toString();
    const QString display = item->text(1);
    QTreeWidget* tree = item->treeWidget();
    if(!tree)
        return false;

    if(isMultilinePropertyValue(raw, display))
    {
        QString edited = raw;
        {
            std::string normalized = edited.toLocal8Bit().constData();
            if(normalized.find('\t') != std::string::npos
               || (!normalized.empty() && (normalized.front() == '\n' || normalized.front() == '\r')))
            {
                normalized = RDK::NormalizeMatrixPropertyText(normalized);
                edited = QString::fromLocal8Bit(normalized.c_str());
            }
        }
        if(!editMultilineValueDialog(this, edited))
            return false;
        return applyPropertyValueFromEditor(item, edited);
    }

    // Гарантируем editable на Value-колонке перед открытием редактора
    item->setFlags((item->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable) & ~Qt::ItemIsUserCheckable);
    tree->setCurrentItem(item);
    tree->editItem(item, 1);
    return true;
}

bool UComponentsListWidgetModern::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == treePopupDialog && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            hideTreePopup();
            return true;
        }
    }

    if(event->type() == QEvent::MouseButtonPress
       || event->type() == QEvent::MouseButtonDblClick)
    {
        if(QTreeWidget* tree = propertyTreeFromFilterObject(obj))
        {
            if(handleBoolValueMouseEvent(tree, static_cast<QMouseEvent*>(event)))
                return true;
        }
    }

    if(event->type() == QEvent::KeyPress)
    {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        const bool isPropertyTree =
            obj == ui->treeWidgetParameters || obj == ui->treeWidgetState ||
            obj == ui->treeWidgetInputs || obj == ui->treeWidgetOutputs ||
            obj == ui->treeWidgetFavorites ||
            obj == ui->treeWidgetParameters->viewport() ||
            obj == ui->treeWidgetState->viewport() ||
            obj == ui->treeWidgetInputs->viewport() ||
            obj == ui->treeWidgetOutputs->viewport() ||
            obj == ui->treeWidgetFavorites->viewport() ||
            (m_unifiedTree && (obj == m_unifiedTree || obj == m_unifiedTree->viewport()));

        if(isPropertyTree &&
           (keyEvent->key() == Qt::Key_F2 || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter))
        {
            QTreeWidget* tree = propertyTreeFromFilterObject(obj);

            // Не переоткрывать редактор, пока уже идёт edit (Enter = commit).
            // QAbstractItemView::state() is protected in Qt5 — detect via focused QLineEdit.
            // Consume the key so the tree does not reopen edit after commit.
            if(treeHasOpenPropertyEditor(tree))
                return true;

            if(beginPropertyValueEdit(currentPropertyItem()))
                return true;
        }
    }

    return UVisualControllerWidget::eventFilter(obj, event);
}
