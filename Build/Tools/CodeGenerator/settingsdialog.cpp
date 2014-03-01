#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    :QDialog(parent)
{

    setTemplateNameLabel = new QLabel("Template's name:");
    setTemplatePathLabel = new QLabel("Template's path:");

    setTemplateNameLineEdit1 = new QLineEdit;
    setTemplateNameLineEdit2 = new QLineEdit;
    setTemplateNameLineEdit3 = new QLineEdit;

    setTemplatePathLineEdit1 = new QLineEdit;
    setTemplatePathLineEdit2 = new QLineEdit;
    setTemplatePathLineEdit3 = new QLineEdit;

    setNamespaceLabel = new QLabel("Namespaces:");

    setNamespaceLineEdit1 = new QLineEdit;
    setNamespaceLineEdit2 = new QLineEdit;
    setNamespaceLineEdit3 = new QLineEdit;
    setNamespaceLineEdit4 = new QLineEdit;
    setNamespaceLineEdit5 = new QLineEdit;

    buttons = new QDialogButtonBox;
    buttons->addButton("OK", QDialogButtonBox::AcceptRole);
    buttons->addButton("Cancel", QDialogButtonBox::RejectRole);

    setRootPathLabel = new QLabel("Root path:");
    setRootPathLineEdit = new QLineEdit;
    setLibsLabel = new QLabel("Libs:");
    setLibsLineEdit1 = new QLineEdit;
    setLibsLineEdit2 = new QLineEdit;
    setLibsLineEdit3 = new QLineEdit;

    readSettings();

    QPushButton *setRootPathBrowse = new QPushButton("Browse");
    QPushButton *setTemplatePathBrowse1 = new QPushButton("Browse");
    QPushButton *setTemplatePathBrowse2 = new QPushButton("Browse");
    QPushButton *setTemplatePathBrowse3 = new QPushButton("Browse");

    QGridLayout *settingsLayout = new QGridLayout;
    settingsLayout->addWidget(setTemplateNameLabel,0,0);
    settingsLayout->addWidget(setTemplatePathLabel,0,1);

    settingsLayout->addWidget(setTemplateNameLineEdit1,1,0);
    settingsLayout->addWidget(setTemplatePathLineEdit1,1,1);
    settingsLayout->addWidget(setTemplatePathBrowse1,1,2);

    settingsLayout->addWidget(setTemplateNameLineEdit2,2,0);
    settingsLayout->addWidget(setTemplatePathLineEdit2,2,1);
    settingsLayout->addWidget(setTemplatePathBrowse2,2,2);

    settingsLayout->addWidget(setTemplateNameLineEdit3,3,0);
    settingsLayout->addWidget(setTemplatePathLineEdit3,3,1);
    settingsLayout->addWidget(setTemplatePathBrowse3,3,2);

    settingsLayout->addWidget(setNamespaceLabel,4,0);

    settingsLayout->addWidget(setNamespaceLineEdit1,5,0);
    settingsLayout->addWidget(setNamespaceLineEdit2,6,0);
    settingsLayout->addWidget(setNamespaceLineEdit3,7,0);
    settingsLayout->addWidget(setNamespaceLineEdit4,8,0);
    settingsLayout->addWidget(setNamespaceLineEdit5,9,0);

    settingsLayout->addWidget(buttons,10,4,1,2);

    settingsLayout->addWidget(setRootPathLabel,0,3);
    settingsLayout->addWidget(setRootPathLineEdit,0,4);
    settingsLayout->addWidget(setRootPathBrowse,0,5);
    settingsLayout->addWidget(setLibsLabel,1,3);
    settingsLayout->addWidget(setLibsLineEdit1,1,4);
    settingsLayout->addWidget(setLibsLineEdit2,2,4);
    settingsLayout->addWidget(setLibsLineEdit3,3,4);

    setLayout(settingsLayout);


    connect(setRootPathBrowse, SIGNAL(clicked()), SLOT(slotRootBrowse()));
    connect(setTemplatePathBrowse1, SIGNAL(clicked()), SLOT(slotTemplateBrowse1()));
    connect(setTemplatePathBrowse2, SIGNAL(clicked()), SLOT(slotTemplateBrowse2()));
    connect(setTemplatePathBrowse3, SIGNAL(clicked()), SLOT(slotTemplateBrowse3()));

    connect(setTemplatePathLineEdit1, SIGNAL(textChanged(QString)), SLOT(slotTempPathChanged1()));
    connect(setTemplatePathLineEdit2, SIGNAL(textChanged(QString)), SLOT(slotTempPathChanged2()));
    connect(setTemplatePathLineEdit3, SIGNAL(textChanged(QString)), SLOT(slotTempPathChanged3()));

}

/*virtual*/ SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::slotRootBrowse()
{
    setRootPathLineEdit->home(true);
    setRootPathLineEdit->insert(QFileDialog::getExistingDirectory());
}

//test
void SettingsDialog::readSettings()
{
    QFile xmlSettingsFile("CodeGenerator.xml");
    if (!xmlSettingsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(xmlSettingsFile.fileName())
                             .arg(xmlSettingsFile.errorString()));
        return;
    }
    QXmlStreamReader reader(&xmlSettingsFile);
    while(!reader.atEnd())
    {
        reader.readNext();
        if(reader.isStartElement() && (reader.name() == "namespace"))
        {
            switch(reader.attributes().value("number").toString().toInt())
            {
                case 1: setNamespaceLineEdit1->setText(reader.readElementText()); break;
                case 2: setNamespaceLineEdit2->setText(reader.readElementText()); break;
                case 3: setNamespaceLineEdit3->setText(reader.readElementText()); break;
                case 4: setNamespaceLineEdit4->setText(reader.readElementText()); break;
                case 5: setNamespaceLineEdit5->setText(reader.readElementText()); break;
                default: break;
            }
        }
        if(reader.isStartElement() && (reader.name() == "template"))
        {
            switch(reader.attributes().value("number").toString().toInt())
            {
                case 1: setTemplateNameLineEdit1->setText(reader.attributes().value("name").toString());
                        setTemplatePathLineEdit1->setText(reader.readElementText());
                        break;
                case 2: setTemplateNameLineEdit2->setText(reader.attributes().value("name").toString());
                        setTemplatePathLineEdit2->setText(reader.readElementText());
                        break;
                case 3: setTemplateNameLineEdit3->setText(reader.attributes().value("name").toString());
                        setTemplatePathLineEdit3->setText(reader.readElementText());
                        break;
                default: break;
            }
        }
        if(reader.isStartElement() && (reader.name() == "lib"))
        {
            switch(reader.attributes().value("number").toString().toInt())
            {
                case 1: setLibsLineEdit1->setText(reader.readElementText()); break;
                case 2: setLibsLineEdit2->setText(reader.readElementText()); break;
                case 3: setLibsLineEdit3->setText(reader.readElementText()); break;
                default: break;
            }
        }
        if(reader.isStartElement() && (reader.name() == "corePath"))
        {
            setRootPathLineEdit->setText(reader.readElementText());
        }
    }
}

//end test
void SettingsDialog::slotTemplateBrowse1()
{
    setTemplatePathLineEdit1->home(true);
    setTemplatePathLineEdit1->insert(QFileDialog::getOpenFileName(this, "Open File", "", "Header files (*.h)"));
}
void SettingsDialog::slotTemplateBrowse2()
{
    setTemplatePathLineEdit2->home(true);
    setTemplatePathLineEdit2->insert(QFileDialog::getOpenFileName(this, "Open File", "", "Header files (*.h)"));
}
void SettingsDialog::slotTemplateBrowse3()
{
    setTemplatePathLineEdit3->home(true);
    setTemplatePathLineEdit3->insert(QFileDialog::getOpenFileName(this, "Open File", "", "Header files (*.h)"));
}

void SettingsDialog::slotTempPathChanged1()
{
    setTemplateNameLineEdit1->setText(setTemplatePathLineEdit1->text().section('/',setTemplatePathLineEdit1->text().count('/')));
    setTemplateNameLineEdit1->backspace();
    setTemplateNameLineEdit1->backspace();
}
void SettingsDialog::slotTempPathChanged2()
{
    setTemplateNameLineEdit2->setText(setTemplatePathLineEdit2->text().section('/',setTemplatePathLineEdit2->text().count('/')));
    setTemplateNameLineEdit2->backspace();
    setTemplateNameLineEdit2->backspace();
}
void SettingsDialog::slotTempPathChanged3()
{
    setTemplateNameLineEdit3->setText(setTemplatePathLineEdit3->text().section('/',setTemplatePathLineEdit3->text().count('/')));
    setTemplateNameLineEdit3->backspace();
    setTemplateNameLineEdit3->backspace();
}
