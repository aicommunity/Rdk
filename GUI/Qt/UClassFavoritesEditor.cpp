#include "UClassFavoritesEditor.h"
#include "ui_UClassFavoritesEditor.h"
#include "../../Core/Engine/UStorage.h"
#include "../../Core/Engine/UContainerDescription.h"
#include "../../Core/Engine/UNet.h"
#include <QMessageBox>
#include <QLabel>


UClassFavoritesEditor::UClassFavoritesEditor(std::string class_name, QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent, app),
    ClassName(class_name),
    ui(new Ui::UClassFavoritesEditor)
{
    ui->setupUi(this);

    // Отображения дерева компонентов одного класса со свойствами
    componentList = new USingleClassListWidget(ClassName, this, application);

    connect(componentList, &USingleClassListWidget::parameterChanged, ui->lineEditFullPath, &QLineEdit::setText);

    ui->labelClassName->setText(QString::fromStdString(ClassName));

    ui->verticalLayoutClassProp->addWidget(componentList);

    connect(ui->pushButtonClose, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->pushButtonCreate,   &QPushButton::clicked, this, &UClassFavoritesEditor::CreateFavorite);
    
    // Добавляем валидацию и предпросмотр пути
    connect(ui->lineEditFullPath, &QLineEdit::textChanged, this, &UClassFavoritesEditor::ValidatePath);
    connect(ui->lineEditFullPath, &QLineEdit::textChanged, this, &UClassFavoritesEditor::ShowPathPreview);
}

UClassFavoritesEditor::~UClassFavoritesEditor()
{
    delete ui;
}

void UClassFavoritesEditor::ChangeClass(std::string class_name)
{
    ClassName = class_name;
    ui->labelClassName->setText(QString::fromStdString(ClassName));

    ui->lineEditFavName->clear();
    ui->lineEditFullPath->clear();

    componentList->ChangeClass(class_name);
}

void UClassFavoritesEditor::CreateFavorite()
{
    // Создание нового Favorite, если строковые данные не пусты
    if(ui->lineEditFullPath->text().isEmpty() || ui->lineEditFavName->text().isEmpty())
    {
       return;
    }
    
    // Валидация пути
    QString errorMessage;
    if(!ValidatePropertyPath(ui->lineEditFullPath->text(), errorMessage))
    {
        QMessageBox::warning(this, "Invalid Path", 
            QString("The path is invalid: %1").arg(errorMessage));
        return;
    }
    
    emit CreateNewFavorite(ui->lineEditFavName->text(), ui->lineEditFullPath->text());
}

void UClassFavoritesEditor::ValidatePath()
{
    QString path = ui->lineEditFullPath->text();
    if(path.isEmpty())
        return;
    
    QString errorMessage;
    bool isValid = ValidatePropertyPath(path, errorMessage);
    
    // Можно добавить визуальную индикацию валидности (например, цвет фона)
    QPalette palette = ui->lineEditFullPath->palette();
    if(!isValid && !path.isEmpty())
    {
        palette.setColor(QPalette::Base, QColor(255, 200, 200)); // Светло-красный
    }
    else
    {
        palette.setColor(QPalette::Base, QColor(255, 255, 255)); // Белый
    }
    ui->lineEditFullPath->setPalette(palette);
}

void UClassFavoritesEditor::ShowPathPreview()
{
    QString path = ui->lineEditFullPath->text();
    if(path.isEmpty())
        return;
    
    QString preview = PreviewResolvedPath(path);
    // Можно добавить label для отображения предпросмотра
    // ui->labelPathPreview->setText(preview);
}

bool UClassFavoritesEditor::ValidatePropertyPath(const QString& path, QString& errorMessage)
{
    if(path.isEmpty())
    {
        errorMessage = "Path is empty";
        return false;
    }
    
    // Проверяем базовый формат пути (должен содержать точку для вложенных свойств)
    if(!path.contains("."))
    {
        // Это может быть свойство верхнего уровня - допустимо
        return true;
    }
    
    // Для вложенных путей проверяем структуру: ComponentPath.PropertyName
    QStringList parts = path.split(".");
    if(parts.size() < 2)
    {
        errorMessage = "Invalid path format. Expected: ComponentPath.PropertyName";
        return false;
    }
    
    // Проверяем, что компонент существует (если возможно)
    try
    {
        RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
        if(storage)
        {
            // TakeObject возвращает UEPtr<UComponent>, поэтому сначала берем базовый компонент,
            // а затем приводим его к нужным типам
            RDK::UEPtr<RDK::UComponent> baseComponent = storage->TakeObject(ClassName);
            if(baseComponent)
            {
                RDK::UEPtr<RDK::UContainer> component = RDK::dynamic_pointer_cast<RDK::UContainer>(baseComponent);
                RDK::UEPtr<RDK::UNet> net = RDK::dynamic_pointer_cast<RDK::UNet>(baseComponent);
                if(net)
                {
                    // Проверяем, существует ли путь к компоненту
                    QString componentPath = parts.mid(0, parts.size() - 1).join(".");
                    QString propertyName = parts.last();
                    
                    RDK::UEPtr<RDK::UContainer> nestedComponent = net->GetComponentL(componentPath.toStdString(), true);
                    if(nestedComponent)
                    {
                        // Проверяем, существует ли свойство
                        RDK::UEPtr<RDK::UIProperty> prop = nestedComponent->FindProperty(propertyName.toStdString());
                        if(!prop)
                        {
                            errorMessage = QString("Property '%1' not found in component '%2'")
                                .arg(propertyName, componentPath);
                            storage->ReturnObject(baseComponent);
                            return false;
                        }
                    }
                    else
                    {
                        errorMessage = QString("Component '%1' not found").arg(componentPath);
                        storage->ReturnObject(baseComponent);
                        return false;
                    }
                }
                storage->ReturnObject(baseComponent);
            }
        }
    }
    catch(...)
    {
        // Игнорируем ошибки при валидации - путь может быть валидным, но компонент еще не создан
    }
    
    return true;
}

QString UClassFavoritesEditor::PreviewResolvedPath(const QString& path)
{
    // Предпросмотр разрешенного пути
    // Для алиасов это будет полный путь к свойству
    if(path.isEmpty())
        return "";
    
    // Если путь уже в формате ComponentPath.PropertyName, возвращаем как есть
    if(path.contains("."))
        return path;
    
    // Иначе это может быть алиас - пытаемся разрешить
    try
    {
        RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
        if(storage)
        {
            RDK::UEPtr<RDK::UContainerDescription> descr = storage->GetClassDescription(ClassName, true);
            if(descr)
            {
                std::string pathStd = path.toStdString();
                std::string componentPath, propertyName;
                if(descr->ParseFavoritePath(pathStd, componentPath, propertyName))
                {
                    return QString::fromStdString(componentPath + "." + propertyName);
                }
            }
        }
    }
    catch(...)
    {
        // Игнорируем ошибки
    }
    
    return path;
}

