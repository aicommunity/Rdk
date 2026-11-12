#ifndef UBREADCRUMBSWIDGET_H
#define UBREADCRUMBSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

/// UBreadcrumbsWidget - виджет для отображения breadcrumbs навигации по компонентам модели
///
/// Отображает путь к текущему компоненту в формате: Model > Component1 > Component2
/// Каждый элемент breadcrumbs кликабелен и позволяет перейти на соответствующий уровень.
class UBreadcrumbsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UBreadcrumbsWidget(QWidget *parent = nullptr);
    virtual ~UBreadcrumbsWidget();

signals:
    /// Сигнал при выборе пути в breadcrumbs
    void componentPathSelected(QString path);

public slots:
    /// Слот для обновления breadcrumbs извне
    void updateBreadcrumbs(const QString &componentPath);

private slots:
    /// Обработчик клика на элемент breadcrumbs
    void onBreadcrumbClicked(const QString &componentPath);

private:
    QHBoxLayout *breadcrumbsLayout;
    QList<QPushButton*> breadcrumbButtons;
};

#endif // UBREADCRUMBSWIDGET_H

