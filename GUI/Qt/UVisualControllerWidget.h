#ifndef UVISUALCONTROLLERWIDGET_H
#define UVISUALCONTROLLERWIDGET_H

//#include <rdk_cpp_initlib.h>
#include <rdk_application.h>
#include "../../Core/Application/UIVisualController.h"
#include <QWidget>
#include <QSettings>
#include <QDebug>
#undef GetClassName //заглушка на системную функцию

/// The UVisualControllerWidget class - Базовый класс виджета
///
/// Каждый виджет, наследуемый от этого класса записывается в ядро RDK

class UVisualControllerWidget:public QWidget, public RDK::UIVisualController
{
    Q_OBJECT

public:
    UVisualControllerWidget(QWidget *parent = 0, RDK::UApplication* app = NULL);
    virtual ~UVisualControllerWidget();

protected:
    /// Экзепляр класса приложения
    RDK::UApplication *application;

    /// true - во время выполнения AUpdateInterface, все остальное время - false
    ///
    /// Устраянет ошибку рекурсивного обновления интерфейса,
    /// появляющуюся в случае попытки одновременного обновления по требованию интерфейса и по тику ядра.
    bool UpdateInterfaceFlag;

    ///Имя объекта и имя класса, определяются в классах наследниках
    std::string Name, ClassName;

    /// Флаг, разрешающий обновление интерфейса, даже если объект не видим
    bool AlwaysUpdateFlag;

    /// Интервал обновления интерфейса в миллисекундах при счете
    /// \details (если =0) то интерфейс не обновляется при счете
    RDK::UELockVar<long> UpdateInterval;

    /// Время последнего обновления
    unsigned long long LastUpdateTime;

    /// Выставляется если форма была перерисованна (обновлена)
    bool CalculationStepUpdatedFlag;

    /// Флаг, разрешающий проверку на существование модели перед обновлением интерфейса
    bool CheckModelFlag;

    /// Время, потраченное на обновление интерфейса
    unsigned long long UpdateTime;

    public:
    /// Флаг, сообщающий что идет расчет
    static RDK::UELockVar<bool> CalculationModeFlag;

    public:
    // -----------------------------
    // Методы управления визуальным интерфейсом
    // -----------------------------
    /// Метод, вызываемый после загрузки проекта
    /// \details Вызывает метод AAfterLoadProject() в блоке обработки исключений
    virtual void AfterLoadProject(void);
    virtual void AAfterLoadProject(void);

    /// Метод, вызываемый перед закрытием проекта
    /// \details Вызывает метод ABeforeCloseProject() в блоке обработки исключений
    virtual void BeforeCloseProject(void);
    virtual void ABeforeCloseProject(void);

    /// Метод, вызываемый перед сбросом модели
    /// \details Вызывает метод ABeforeReset() в блоке обработки исключений
    virtual void BeforeReset(void);
    virtual void ABeforeReset(void);

    /// Метод, вызываемый после сброса модели
    /// \details Вызывает метод AAfterReset() в блоке обработки исключений
    virtual void AfterReset(void);
    virtual void AAfterReset(void);

    /// Метод, вызываемый перед шагом расчета
    /// \details Вызывает метод ABeforeCalculate() в блоке обработки исключений
    virtual void BeforeCalculate(void);
    virtual void ABeforeCalculate(void);

    /// Метод, вызываемый после шага расчета
    /// \details Вызывает метод AAfterCalculate() в блоке обработки исключений
    virtual void AfterCalculate(void);
    virtual void AAfterCalculate(void);

    /// Обновление интерфейса
    /*! Вызывает метод AUpdateInterface() в блоке обработки исключений.
     Если force_update == true, то интерфейс обновляется
     вне зависимости от UpdateInterval*/
    virtual void UpdateInterface(bool force_update=false);
    virtual void AUpdateInterface(void);

    /// Возврат интерфейса в исходное состояние
    /// \details Вызывает метод AClearInterface() в блоке обработки исключений
    virtual void ClearInterface(void);
    virtual void AClearInterface(void);

    /// Возвращает уникальное имя интерфейса
    virtual std::string GetName(void);

    /// Возвращает имя класса интерфейса
    virtual std::string GetClassName(void);

    virtual std::string CalcFullName(void);

    /// Возвращает интервал обновления интерфейса
    virtual long GetUpdateInterval(void);

    /// Задает интервал обновления интерфейса
    virtual bool SetUpdateInterval(long value);

    /// Возвращает флаг разрешения обновления интерфейса даже если он не виден
    virtual bool GetAlwaysUpdateFlag(void);

    /// Сохраняет параметры интерфейса в xml
    /// \details Вызывает метод ASaveParameters() в блоке обработки исключений
    virtual void SaveParameters(RDK::USerStorageXML &);
    virtual void ASaveParameters();

    /// Загружает параметры интерфейса из xml
    /// \details Вызывает метод ALoadParameters() в блоке обработки исключений
    virtual void LoadParameters(RDK::USerStorageXML &);
    virtual void ALoadParameters();

    // Служебные методы управления интерфейсом
    /// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
    virtual void ResetCalculationStepUpdatedFlag(void);

    /// Выставляет флаг прошедшей перерисовки в этой итерации счета
    virtual void SetCalculationStepUpdatedFlag(void);

    /// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
    virtual bool GetCalculationStepUpdatedFlag(void);

    /// Возвращает время обновления интерфейса (мс)
    virtual unsigned long long GetUpdateTime(void);

public slots:
    virtual void unInit();

signals:
    void closeWindow();
};

#endif // UVISUALCONTROLLERWIDGET_H
