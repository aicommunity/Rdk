#include "codewizard.h"

CodeWizard::CodeWizard(QWidget *parent)
    : QWizard(parent)
{
    Settings = new SettingsDialog;
    addPage(IP = new InitialPage);
    setOption(QWizard::NoBackButtonOnStartPage, true);
    setOption(QWizard::HaveCustomButton1, true);
    setOption(QWizard::HaveCustomButton3, true);
    setWizardStyle(ModernStyle);
    setWindowTitle("CodeWizard");
    setMinimumWidth(500);
    setGeometry(500,300,800,500);

    QPushButton *createBut = new QPushButton;
    QPushButton *setBut = new QPushButton;
    createBut->setText("Create");
    setBut->setText("Settings");
    setButton(CustomButton3, createBut);
    setButton(CustomButton1, setBut);

    connect(createBut, SIGNAL(clicked()), SLOT(slotaccept()));
    connect(setBut, SIGNAL(clicked()), SLOT(slotSettings()));

    connect(Settings->buttons, SIGNAL(accepted()), SLOT(slotSettingsAccepted()));
    connect(Settings->buttons, SIGNAL(rejected()), Settings, SLOT(reject()));

    emit IP->expand();

}

InitialPage::InitialPage(QWidget *parent)
    :QWizardPage(parent), settings("Obald", "CodeWizard"), maxHistorySize(100)
{

    templateLabel = new QLabel("Template:");
    templateCBox = new QComboBox;

    //templates = new QStringList;
    //templates<<"EngineClassSimple";

    fileNameLabel = new QLabel("File Name:");
    fileNameLineEdit = new QLineEdit;
    classNameLabel = new QLabel("Class Name:");
    classNameLineEdit = new QLineEdit;
    classNameCBox = new QComboBox;
    componentNameLabel = new QLabel("Component Name:");
    componentNameLineEdit = new QLineEdit;
    dstPathLabel = new QLabel("Path:");
    dstPathLineEdit = new QLineEdit;
    dstPathCBox = new QComboBox;//
    //
    baseNameLabel = new QLabel("Base Class:");
    baseNameLineEdit = new QLineEdit;
    baseFileNameLabel = new QLabel("Include file:");
    baseFileNameLineEdit = new QLineEdit;
    namespaceLabel = new QLabel("Namespace:");
    namespaceCBox = new QComboBox;
    splitter = new QSplitter(Qt::Vertical);


    //
    loadSettings();
    templateCBox->addItems(templates);
    //dstPathCBox->addItem(dstPathLineEdit->text());
    srcPath;
    dstPathCBox->addItems(paths);
    dstPathCBox->setEditable(true);
    dstPathCBox->setMaxCount(maxHistorySize);
    dstPathCBox->setMaxVisibleItems(5);
    classNameCBox->addItem("");
    classNameCBox->addItems(classNameHistory);
    classNameCBox->setEditable(true);
    classNameCBox->setMaxCount(maxHistorySize);
    classNameCBox->setMaxVisibleItems(5);
    //dstPathCBox->setLineEdit(dstPathLineEdit);
    //
    //namespaces<<"RDK"<<"RTV"<<"NMSDK"<<"DVA";
    namespaceCBox->addItems(namespaces);
    namespaceLineEdit = new QLineEdit(namespaceCBox->currentText());

    //Copyrights
    copyrightNameLabel = new QLabel("Copyright name:");
    copyrightYearLabel = new QLabel("Copyright year:");
    copyrightEmailLabel = new QLabel("Copyright e-mail:");
    copyrightUrlLabel = new QLabel("Copyright URL:");
    copyrightProjectNameLabel = new QLabel("Copyright project:"); //name
    copyrightNameLineEdit = new QLineEdit;
    copyrightYearLineEdit = new QLineEdit;
    copyrightEmailLineEdit = new QLineEdit;
    copyrightUrlLineEdit = new QLineEdit;
    copyrightProjectNameLineEdit = new QLineEdit;

    browseBut = new QPushButton("Browse");
    QPushButton *expandAllBut = new QPushButton("Expand All");
    QPushButton *selectBut = new QPushButton("Select");

    //Registering fields
    registerField("template", templateCBox);
    registerField("className*", classNameLineEdit);
    registerField("componentName", componentNameLineEdit);
    registerField("fileName", fileNameLineEdit);
    registerField("dstPath", dstPathLineEdit);
    registerField("baseName", baseNameLineEdit);
    registerField("baseFile", baseFileNameLineEdit);
    registerField("namespace", namespaceLineEdit);
    registerField("crName", copyrightNameLineEdit);
    registerField("crYear", copyrightYearLineEdit);
    registerField("crEmail", copyrightEmailLineEdit);
    registerField("crUrl", copyrightUrlLineEdit);
    registerField("crPrName", copyrightProjectNameLineEdit);

    //Layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(templateLabel,0,0);
    layout->addWidget(templateCBox,0,1);

    layout->addWidget(classNameLabel,1,0);
    //layout->addWidget(classNameLineEdit,1,1);
    layout->addWidget(classNameCBox,1,1);

    layout->addWidget(fileNameLabel,2,0);
    layout->addWidget(fileNameLineEdit,2,1);

    layout->addWidget(componentNameLabel,3,0);
    layout->addWidget(componentNameLineEdit,3,1);

    layout->addWidget(dstPathLabel,4,0);
    //layout->addWidget(dstPathLineEdit,4,1);
    layout->addWidget(dstPathCBox,4,1);

    layout->addWidget(baseNameLabel,5,0);
    layout->addWidget(baseNameLineEdit,5,1);

    layout->addWidget(baseFileNameLabel,6,0);
    layout->addWidget(baseFileNameLineEdit,6,1);

    layout->addWidget(namespaceLabel,7,0);
    layout->addWidget(namespaceCBox,7,1);

    layout->addWidget(copyrightNameLabel,8,0);
    layout->addWidget(copyrightNameLineEdit,8,1);

    layout->addWidget(copyrightYearLabel,9,0);
    layout->addWidget(copyrightYearLineEdit,9,1);

    layout->addWidget(copyrightEmailLabel,10,0);
    layout->addWidget(copyrightEmailLineEdit,10,1);

    layout->addWidget(copyrightUrlLabel,11,0);
    layout->addWidget(copyrightUrlLineEdit,11,1);

    layout->addWidget(copyrightProjectNameLabel,12,0);
    layout->addWidget(copyrightProjectNameLineEdit,12,1);

    layout->addWidget(browseBut,4,2);

    //corePath.insert(0,"C:\\Users\\Obald\\Documents\\РТК\\Core");

    libModel = new QStringListModel;
    libModel->setStringList(libList);
    libListView = new  QListView;
    libListView->setModel(libModel);


    dstModel = new QFileSystemModel;
    dstModel->setRootPath(corePath);
    dstModel->setReadOnly(true);
    tree = new QTreeView;
    tree->setModel(dstModel);
    tree->setColumnHidden(1,true);
    tree->setColumnHidden(2,true);
    tree->setColumnHidden(3,true);
    tree->setHeaderHidden(true);
    tree->setRootIndex(dstModel->index(corePath/*+"\\Engine\\Libraries"*/));//


    QHBoxLayout *hboxlayout = new QHBoxLayout;
    //QSplitter *hspltr = new QSplitter(Qt::Horizontal);

    QHBoxLayout *h2boxlayout = new QHBoxLayout;
    QVBoxLayout *vboxlayout = new QVBoxLayout;

    h2boxlayout->addWidget(expandAllBut);
    h2boxlayout->addWidget(selectBut);
    splitter->addWidget(libListView);
    splitter->addWidget(tree);
    vboxlayout->addWidget(splitter);
    vboxlayout->addLayout(h2boxlayout);

    hboxlayout->addLayout(vboxlayout);
    hboxlayout->addLayout(layout);

    setLayout(hboxlayout);

    connect(browseBut, SIGNAL(clicked()), SLOT(slotBrowse()));
    connect(classNameLineEdit, SIGNAL(textChanged(QString)), SLOT(slotClassChanged()));
    connect(baseNameLineEdit, SIGNAL(textChanged(QString)), SLOT(slotBaseChanged()));
    connect(dstPathCBox, SIGNAL(currentTextChanged(QString)), SLOT(slotDstChanged()));
    connect(classNameCBox, SIGNAL(currentTextChanged(QString)), SLOT(slotClassNameChanged()));
    connect(namespaceCBox,SIGNAL(currentTextChanged(QString)), SLOT(slotNamespaceChanged()));
    connect(templateCBox, SIGNAL(customContextMenuRequested(QPoint)), SLOT(slotSettingsChanged()));
    //
    //connect(this, SIGNAL(expand()),tree, SLOT(expandAll()));//придумать получше
    //connect(tree, SIGNAL(clicked(QModelIndex)),tree, SLOT(expandAll()));//придумать получше
    connect(libListView, SIGNAL(clicked(QModelIndex)), SLOT(slotLibChanged()));
    connect(expandAllBut, SIGNAL(clicked()), tree, SLOT(expandAll()));
    connect(selectBut, SIGNAL(clicked()), SLOT(slotPathSelected()));

}
/*virtual*/ InitialPage::~InitialPage()
{
    saveSettings();
}

SettingsDialog::SettingsDialog(QWidget *parent)
    :QDialog(parent),presets("Obald", "CodeWizard")
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

    loadSettingsDialog();

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


void CodeWizard::accept()
{

    //Creating H file
    QFile srcHfile(IP->templatePaths.value(IP->templateCBox->currentIndex()) + "h");
    if (!srcHfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(srcHfile.fileName())
                             .arg(srcHfile.errorString()));
        return;
    }

    QFile dstHfile(field("dstPath").toString()+"/"+field("fileName").toString()+".h");

        if(QFile::exists(dstHfile.fileName()))
        {
            QMessageBox::warning(0,QObject::tr("Writing err0r"),
                                 QObject::tr("File %1 already exists")
                                 .arg(dstHfile.fileName()));
            return;
        }
        if (!dstHfile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(0, QObject::tr("Writing err0r"),
                                 QObject::tr("Cannot write to file %1:\n%2")
                                 .arg(dstHfile.fileName())
                                 .arg(dstHfile.errorString()));
            return;
        }

    QTextStream outH(&dstHfile);
    QTextStream inH(&srcHfile);
      while (!inH.atEnd())
      {
          QString line = inH.readLine();
          if(line.contains("@CLASS_NAME@"))
          {
              line.replace("@CLASS_NAME@",field("className").toString());
          }
          if(line.contains("@FILE_NAME@"))
          {
              line.replace("@FILE_NAME@",field("fileName").toString());
          }
          if(line.contains("@INHERITANCE_NAME@"))
          {
              line.replace("@INHERITANCE_NAME@",field("baseName").toString());
          }
          if(line.contains("@INCLUDE_FILE_NAME@"))
          {
              line.replace("@INCLUDE_FILE_NAME@",field("baseFile").toString());
          }
          if(line.contains("@NAMESPACE_NAME@"))
          {
              line.replace("@NAMESPACE_NAME@", field("namespace").toString());
          }
          if(line.contains("@COPYRIGHT_NAME@"))
          {
              if(!(field("crName").toString().isEmpty()))
                  line.replace("@COPYRIGHT_NAME@",field("crName").toString());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_YEAR@"))
          {
              if(!(field("crYear").toString().isEmpty()))
                  line.replace("@COPYRIGHT_YEAR@",field("crYear").toString());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_EMAIL@"))
          {
              if(!(field("crEmail").toString().isEmpty()))
                  line.replace("@COPYRIGHT_EMAIL@",field("crEmail").toString());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_URL@"))
          {
              if(!(field("crUrl").toString().isEmpty()))
                  line.replace("@COPYRIGHT_URL@",field("crUrl").toString());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_PROJECT_NAME@"))
          {
              if(!(field("crPrName").toString().isEmpty()))
                  line.replace("@COPYRIGHT_PROJECT_NAME@",field("crPrName").toString());
              else
                  line.clear();
          }
          if(!(line.isEmpty()))
          {
              outH<<line<<'\n';
          }
      }

      //Creating CPP file
      QFile srcCPPfile(IP->templatePaths.value(IP->templateCBox->currentIndex()) + "cpp");
      if (!srcCPPfile.open(QIODevice::ReadOnly | QIODevice::Text)){
          QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                               QObject::tr("Cannot open file %1:\n%2")
                               .arg(srcCPPfile.fileName())
                               .arg(srcCPPfile.errorString()));
          return;
      }

      QFile dstCPPfile(field("dstPath").toString()+"/"+field("fileName").toString()+".cpp");
          if (!dstCPPfile.open(QIODevice::WriteOnly | QIODevice::Text)){
              QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                                   QObject::tr("Cannot write file %1:\n%2")
                                   .arg(dstCPPfile.fileName())
                                   .arg(dstCPPfile.errorString()));
              return;
          }
      QTextStream outCPP(&dstCPPfile);
      QTextStream inCPP(&srcCPPfile);
        while (!inCPP.atEnd())
        {
            QString line = inCPP.readLine();
            if(line.contains("@CLASS_NAME@"))
            {
                line.replace("@CLASS_NAME@",field("className").toString());
            }
            if(line.contains("@FILE_NAME@"))
            {
                line.replace("@FILE_NAME@",field("fileName").toString());
            }
            if(line.contains("@INHERITANCE_NAME@"))
            {
                line.replace("@INHERITANCE_NAME@",field("baseName").toString());
            }
            if(line.contains("@INCLUDE_FILE_NAME@"))
            {
                line.replace("@INCLUDE_FILE_NAME@",field("baseFile").toString());
            }
            if(line.contains("@NAMESPACE_NAME@"))
            {
                line.replace("@NAMESPACE_NAME@", field("namespace").toString());
            }
            if(line.contains("@COPYRIGHT_NAME@"))
            {
                if(!(field("crName").toString().isEmpty()))
                    line.replace("@COPYRIGHT_NAME@",field("crName").toString());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_YEAR@"))
            {
                if(!(field("crYear").toString().isEmpty()))
                    line.replace("@COPYRIGHT_YEAR@",field("crYear").toString());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_EMAIL@"))
            {
                if(!(field("crEmail").toString().isEmpty()))
                    line.replace("@COPYRIGHT_EMAIL@",field("crEmail").toString());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_URL@"))
            {
                if(!(field("crUrl").toString().isEmpty()))
                    line.replace("@COPYRIGHT_URL@",field("crUrl").toString());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_PROJECT_NAME@"))
            {
                if(!(field("crPrName").toString().isEmpty()))
                    line.replace("@COPYRIGHT_PROJECT_NAME@",field("crPrName").toString());
                else
                    line.clear();
            }
            if(!(line.isEmpty()))
            {
                outCPP<<line<<'\n';
            }
        }
      QDialog::accept();

}

void CodeWizard::slotaccept()
{
    //Creating H file
    QFile srcHfile(IP->templatePaths.value(IP->templateCBox->currentIndex()) + "h");
    if (!srcHfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(srcHfile.fileName())
                             .arg(srcHfile.errorString()));
        return;
    }

    QFile dstHfile(field("dstPath").toString()+"/"+field("fileName").toString()+".h");
        if(QFile::exists(dstHfile.fileName()))
        {
            QMessageBox::warning(0,QObject::tr("Writing err0r"),
                                 QObject::tr("File %1 already exists")
                                 .arg(dstHfile.fileName()));
            return;
        }
        if (!dstHfile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                                 QObject::tr("Cannot write file %1:\n%2")
                                 .arg(dstHfile.fileName())
                                 .arg(dstHfile.errorString()));
            return;
        }
    QTextStream outH(&dstHfile);
    QTextStream inH(&srcHfile);
      while (!inH.atEnd())
      {
          QString line = inH.readLine();
          if(line.contains("@CLASS_NAME@"))
          {
              line.replace("@CLASS_NAME@",field("className").toString());
          }
          if(line.contains("@FILE_NAME@"))
          {
              line.replace("@FILE_NAME@",field("fileName").toString());
          }
          if(line.contains("@INHERITANCE_NAME@"))
          {
              line.replace("@INHERITANCE_NAME@",field("baseName").toString());
          }
          if(line.contains("@INCLUDE_FILE_NAME@"))
          {
              line.replace("@INCLUDE_FILE_NAME@",field("baseFile").toString());
          }
          if(line.contains("@NAMESPACE_NAME@"))
          {
              line.replace("@NAMESPACE_NAME@", field("namespace").toString());
          }
          if(line.contains("@COPYRIGHT_NAME@"))
          {
              if(!(field("crName").toString().isEmpty()))
                  line.replace("@COPYRIGHT_NAME@",field("crName").toString());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_YEAR@"))
          {
              if(!(field("crYear").toString().isEmpty()))
                  line.replace("@COPYRIGHT_YEAR@",field("crYear").toString());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_EMAIL@"))
          {
              if(!(field("crEmail").toString().isEmpty()))
                  line.replace("@COPYRIGHT_EMAIL@",field("crEmail").toString());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_URL@"))
          {
              if(!(field("crUrl").toString().isEmpty()))
                  line.replace("@COPYRIGHT_URL@",field("crUrl").toString());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_PROJECT_NAME@"))
          {
              if(!(field("crPrName").toString().isEmpty()))
                  line.replace("@COPYRIGHT_PROJECT_NAME@",field("crPrName").toString());
              else
                  line.clear();
          }
          if(!(line.isEmpty()))
          {
              outH<<line<<'\n';
          }
      }

      //Creating CPP file
      QFile srcCPPfile(IP->templatePaths.value(IP->templateCBox->currentIndex()) + "cpp");
      if (!srcCPPfile.open(QIODevice::ReadOnly | QIODevice::Text)){
          QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                               QObject::tr("Cannot open file %1:\n%2")
                               .arg(srcCPPfile.fileName())
                               .arg(srcCPPfile.errorString()));
          return;
      }

      QFile dstCPPfile(field("dstPath").toString()+"/"+field("fileName").toString()+".cpp");
          if (!dstCPPfile.open(QIODevice::WriteOnly | QIODevice::Text)){
              QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                                   QObject::tr("Cannot write file %1:\n%2")
                                   .arg(dstCPPfile.fileName())
                                   .arg(dstCPPfile.errorString()));
              return;
          }
      QTextStream outCPP(&dstCPPfile);
      QTextStream inCPP(&srcCPPfile);
        while (!inCPP.atEnd())
        {
            QString line = inCPP.readLine();
            if(line.contains("@CLASS_NAME@"))
            {
                line.replace("@CLASS_NAME@",field("className").toString());
            }
            if(line.contains("@FILE_NAME@"))
            {
                line.replace("@FILE_NAME@",field("fileName").toString());
            }
            if(line.contains("@INHERITANCE_NAME@"))
            {
                line.replace("@INHERITANCE_NAME@",field("baseName").toString());
            }
            if(line.contains("@INCLUDE_FILE_NAME@"))
            {
                line.replace("@INCLUDE_FILE_NAME@",field("baseFile").toString());
            }
            if(line.contains("@NAMESPACE_NAME@"))
            {
                line.replace("@NAMESPACE_NAME@", field("namespace").toString());
            }
            if(line.contains("@COPYRIGHT_NAME@"))
            {
                if(!(field("crName").toString().isEmpty()))
                    line.replace("@COPYRIGHT_NAME@",field("crName").toString());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_YEAR@"))
            {
                if(!(field("crYear").toString().isEmpty()))
                    line.replace("@COPYRIGHT_YEAR@",field("crYear").toString());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_EMAIL@"))
            {
                if(!(field("crEmail").toString().isEmpty()))
                    line.replace("@COPYRIGHT_EMAIL@",field("crEmail").toString());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_URL@"))
            {
                if(!(field("crUrl").toString().isEmpty()))
                    line.replace("@COPYRIGHT_URL@",field("crUrl").toString());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_PROJECT_NAME@"))
            {
                if(!(field("crPrName").toString().isEmpty()))
                    line.replace("@COPYRIGHT_PROJECT_NAME@",field("crPrName").toString());
                else
                    line.clear();
            }

            if(!(line.isEmpty()))
            {
                outCPP<<line<<'\n';
            }
        }
        QWizard::restart();

}

void CodeWizard::slotSettings()
{
    Settings->exec();
}

void InitialPage::slotBrowse()
{
    dstPathCBox->setLineEdit(dstPathLineEdit);
    dstPathLineEdit->home(true);
    dstPathLineEdit->insert(QFileDialog::getExistingDirectory());

}

void InitialPage::saveSettings()
{
    settings.beginGroup("/Settings");
    paths.push_front(field("dstPath").toString());
    while(paths.size()>maxHistorySize) paths.removeLast();
    classNameHistory.push_front(field("className").toString());
    while(classNameHistory.size()>maxHistorySize) classNameHistory.removeLast();
    //paths.clear();
    settings.setValue("/path", paths);
    settings.setValue("/className", classNameHistory);
    settings.setValue("/corePath", corePath);
    settings.setValue("/libList", libList);
    settings.setValue("/namespaces", namespaces);
    settings.setValue("/templates", templates);
    settings.setValue("/split", splitter->saveState());
    settings.setValue("/tempPaths", templatePaths);
    settings.setValue("/dstPathString", dstPathLineEdit->text());
    settings.endGroup();
}

void InitialPage::loadSettings()
{
    settings.beginGroup("/Settings");
    paths<<settings.value("/path", "").toStringList();
    classNameHistory<<settings.value("/className", "").toStringList();
    corePath.append(settings.value("/corePath","").toString());
    libList<<settings.value("/libList","").toStringList();
    namespaces<<settings.value("/namespaces", "").toStringList();
    templates<<settings.value("/templates", "").toStringList();
    splitter->restoreState(settings.value("/split").toByteArray());
    templatePaths<<settings.value("/tempPaths", "").toStringList();
    dstPathLineEdit->setText(settings.value("/dstPathString", "").toString());

    settings.endGroup();
}


void InitialPage::slotClassChanged()
{
    fileNameLineEdit->setText(classNameLineEdit->text());
    componentNameLineEdit->setText(classNameLineEdit->text());
}

void InitialPage::slotBaseChanged()
{
    baseFileNameLineEdit->setText(baseNameLineEdit->text()+".h");
}
void InitialPage::slotDstChanged()
{
    dstPathLineEdit->setText(dstPathCBox->currentText());
}
void InitialPage::slotClassNameChanged()
{
    classNameLineEdit->setText(classNameCBox->currentText());
}
void InitialPage::slotNamespaceChanged()
{
    namespaceLineEdit->setText(namespaceCBox->currentText());
}
void InitialPage::slotSettingsChanged()
{
    while(templateCBox->count()>0)
    {
        templateCBox->removeItem(templateCBox->count()-1);
    }
    templateCBox->addItems(templates);

    while(namespaceCBox->count()>0)
    {
        namespaceCBox->removeItem(namespaceCBox->count()-1);
    }
    namespaceCBox->addItems(namespaces);

    libModel->setStringList(libList);

}

void InitialPage::slotLibChanged()
{
    tree->setRootIndex(dstModel->index(corePath+"\\"+libModel->data(libListView->currentIndex(),0).toString()));
    emit expand();
}

void CodeWizard::slotSettingsAccepted()
{
    IP->templates.clear();
    if (!Settings->setTemplateNameLineEdit1->text().isEmpty()) IP->templates<<Settings->setTemplateNameLineEdit1->text();
    if (!Settings->setTemplateNameLineEdit2->text().isEmpty()) IP->templates<<Settings->setTemplateNameLineEdit2->text();
    if (!Settings->setTemplateNameLineEdit3->text().isEmpty()) IP->templates<<Settings->setTemplateNameLineEdit3->text();

    IP->namespaces.clear();
    if (!Settings->setNamespaceLineEdit1->text().isEmpty()) IP->namespaces<<Settings->setNamespaceLineEdit1->text();
    if (!Settings->setNamespaceLineEdit2->text().isEmpty()) IP->namespaces<<Settings->setNamespaceLineEdit2->text();
    if (!Settings->setNamespaceLineEdit3->text().isEmpty()) IP->namespaces<<Settings->setNamespaceLineEdit3->text();
    if (!Settings->setNamespaceLineEdit4->text().isEmpty()) IP->namespaces<<Settings->setNamespaceLineEdit4->text();
    if (!Settings->setNamespaceLineEdit5->text().isEmpty()) IP->namespaces<<Settings->setNamespaceLineEdit5->text();

    IP->libList.clear();
    if (!Settings->setLibsLineEdit1->text().isEmpty()) IP->libList<<Settings->setLibsLineEdit1->text();
    if (!Settings->setLibsLineEdit2->text().isEmpty()) IP->libList<<Settings->setLibsLineEdit2->text();
    if (!Settings->setLibsLineEdit3->text().isEmpty()) IP->libList<<Settings->setLibsLineEdit3->text();

    if (!Settings->setRootPathLineEdit->text().isEmpty())
    {
        IP->corePath.clear();
        IP->corePath.append(Settings->setRootPathLineEdit->text());
    }

    QString temp;
    IP->templatePaths.clear();
    if(!Settings->setTemplatePathLineEdit1->text().isEmpty())
    {
        temp = Settings->setTemplatePathLineEdit1->text();
        temp.chop(1);
        IP->templatePaths<<temp;
    }
    if(!Settings->setTemplatePathLineEdit2->text().isEmpty())
    {
        temp = Settings->setTemplatePathLineEdit2->text();
        temp.chop(1);
        IP->templatePaths<<temp;
    }
    if(!Settings->setTemplatePathLineEdit3->text().isEmpty())
    {
        temp = Settings->setTemplatePathLineEdit3->text();
        temp.chop(1);
        IP->templatePaths<<temp;
    }


    Settings->saveSettingsDialog();
    emit IP->templateCBox->customContextMenuRequested(QPoint(20,20));
    Settings->accept();
}

void SettingsDialog::slotRootBrowse()
{
    setRootPathLineEdit->home(true);
    setRootPathLineEdit->insert(QFileDialog::getExistingDirectory());
}
void SettingsDialog::saveSettingsDialog()
{
    presets.beginGroup("/Presets");
    presets.setValue("/root", setRootPathLineEdit->text());
    presets.setValue("/tempName1", setTemplateNameLineEdit1->text());
    presets.setValue("/tempName2", setTemplateNameLineEdit2->text());
    presets.setValue("/tempName3", setTemplateNameLineEdit3->text());
    presets.setValue("/tempPath1", setTemplatePathLineEdit1->text());
    presets.setValue("/tempPath2", setTemplatePathLineEdit2->text());
    presets.setValue("/tempPath3", setTemplatePathLineEdit3->text());
    presets.setValue("/namespace1", setNamespaceLineEdit1->text());
    presets.setValue("/namespace2", setNamespaceLineEdit2->text());
    presets.setValue("/namespace3", setNamespaceLineEdit3->text());
    presets.setValue("/namespace4", setNamespaceLineEdit4->text());
    presets.setValue("/namespace5", setNamespaceLineEdit5->text());
    presets.setValue("/Lib1", setLibsLineEdit1->text());
    presets.setValue("/Lib2", setLibsLineEdit2->text());
    presets.setValue("/Lib3", setLibsLineEdit3->text());
    presets.endGroup();
}
void SettingsDialog::loadSettingsDialog()
{
    presets.beginGroup("/Presets");
    setRootPathLineEdit->setText(presets.value("/root", "").toString());
    setTemplateNameLineEdit1->setText(presets.value("/tempName1", "").toString());
    setTemplateNameLineEdit2->setText(presets.value("/tempName2", "").toString());
    setTemplateNameLineEdit3->setText(presets.value("/tempName3", "").toString());
    setTemplatePathLineEdit1->setText(presets.value("/tempPath1", "").toString());
    setTemplatePathLineEdit2->setText(presets.value("/tempPath2", "").toString());
    setTemplatePathLineEdit3->setText(presets.value("/tempPath3", "").toString());
    setNamespaceLineEdit1->setText(presets.value("/namespace1", "").toString());
    setNamespaceLineEdit2->setText(presets.value("/namespace2", "").toString());
    setNamespaceLineEdit3->setText(presets.value("/namespace3", "").toString());
    setNamespaceLineEdit4->setText(presets.value("/namespace4", "").toString());
    setNamespaceLineEdit5->setText(presets.value("/namespace5", "").toString());
    setLibsLineEdit1->setText(presets.value("/Lib1", "").toString());
    setLibsLineEdit2->setText(presets.value("/Lib2", "").toString());
    setLibsLineEdit3->setText(presets.value("/Lib3", "").toString());
    presets.endGroup();

}

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

void InitialPage::slotPathSelected()
{
    QString dstTemporary(dstModel->filePath(tree->currentIndex()));
    if(dstTemporary.contains(".h")||dstTemporary.contains(".cpp"))
    {
        dstTemporary = dstTemporary.left(dstTemporary.lastIndexOf("/"));
    }
    dstPathCBox->setLineEdit(dstPathLineEdit);
    dstPathLineEdit->setText(dstTemporary);
}
