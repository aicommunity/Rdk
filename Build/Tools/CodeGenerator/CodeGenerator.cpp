#include "CodeGenerator.h"
#include "ui_CodeGenerator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), maxHistorySize(100)
{
    ui->setupUi(this);

    Settings = new SettingsDialog;

    dstPathLineEdit = new QLineEdit;
    classNameLineEdit = new QLineEdit;
    baseNameLineEdit = new QLineEdit;
    namespaceLineEdit = new QLineEdit;
    headerHighlighter = new MyHighlighter(ui->headerEdit->document());
    sourceHighlighter = new MyHighlighter(ui->sourceEdit->document());

    libModel = new QStringListModel;

    readXMLSettings();

    ui->templateCBox->addItems(templates);
    ui->dstPathCBox->addItems(paths);
    ui->dstPathCBox->setEditable(true);
    ui->dstPathCBox->setMaxCount(maxHistorySize);
    ui->dstPathCBox->setMaxVisibleItems(5);
    dstPathLineEdit->setText(ui->dstPathCBox->currentText());
    ui->classNameCBox->addItem("");//
    ui->classNameCBox->addItems(classNameHistory);
    ui->classNameCBox->setEditable(true);
    ui->classNameCBox->setMaxCount(maxHistorySize);
    ui->classNameCBox->setMaxVisibleItems(5);
    ui->baseNameCBox->addItems(baseNameHistory);
    ui->baseNameCBox->setEditable(true);
    ui->baseNameCBox->setMaxCount(maxHistorySize);
    ui->baseNameCBox->setMaxVisibleItems(5);
    ui->namespaceCBox->addItems(namespaces);
    namespaceLineEdit = new QLineEdit(ui->namespaceCBox->currentText());

    slotBaseChanged();

    libModel->setStringList(libList);
    ui->libListView->setModel(libModel);

    dstModel = new QFileSystemModel;
    dstModel->setRootPath(corePath);
    dstModel->setReadOnly(true);
    ui->tree->setModel(dstModel);
    ui->tree->setColumnHidden(1,true);
    ui->tree->setColumnHidden(2,true);
    ui->tree->setColumnHidden(3,true);
    ui->tree->setHeaderHidden(true);
    ui->tree->setRootIndex(dstModel->index(dstPathLineEdit->text()));

    connect(ui->browseButton, SIGNAL(clicked()), SLOT(slotBrowse()));
    connect(classNameLineEdit, SIGNAL(textChanged(QString)), SLOT(slotClassChanged()));
    connect(ui->classNameCBox, SIGNAL(currentTextChanged(QString)), SLOT(slotClassNameChanged()));
    connect(ui->baseNameCBox, SIGNAL(currentTextChanged(QString)), SLOT(slotBaseChanged()));
    connect(ui->dstPathCBox, SIGNAL(currentTextChanged(QString)), SLOT(slotDstChanged()));

    connect(ui->tree, SIGNAL(clicked(QModelIndex)),ui->tree, SLOT(expand(QModelIndex)));
    connect(ui->libListView, SIGNAL(clicked(QModelIndex)), SLOT(slotLibChanged()));
    connect(ui->expandButton, SIGNAL(clicked()), ui->tree, SLOT(expandAll()));
    connect(ui->selectButton, SIGNAL(clicked()), SLOT(slotPathSelected()));
    connect(ui->tree, SIGNAL(clicked(QModelIndex)), SLOT(slotPathSelected()));
    connect(ui->settingsButton, SIGNAL(clicked()), SLOT(slotSettings()));
    connect(ui->createButton, SIGNAL(clicked()), SLOT(slotaccept()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(close()));
    connect(ui->templateCBox, SIGNAL(customContextMenuRequested(QPoint)), SLOT(slotSettingsChanged()));

    connect(Settings->buttons, SIGNAL(accepted()), SLOT(slotSettingsAccepted()));
    connect(Settings->buttons, SIGNAL(rejected()), Settings, SLOT(reject()));

    connect(ui->hRedoButton, SIGNAL(clicked()), ui->headerEdit, SLOT(redo()));
    connect(ui->hUndoButton, SIGNAL(clicked()), ui->headerEdit, SLOT(undo()));
    connect(ui->sRedoButton, SIGNAL(clicked()), ui->sourceEdit, SLOT(redo()));
    connect(ui->sUndoButton, SIGNAL(clicked()), ui->sourceEdit, SLOT(undo()));
    connect(ui->hSaveButton, SIGNAL(clicked()), SLOT(slotSaveHChanges()));
    connect(ui->sSaveButton, SIGNAL(clicked()), SLOT(slotSaveCPPChanges()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotBrowse()
{
    ui->dstPathCBox->setLineEdit(dstPathLineEdit);
    dstPathLineEdit->home(true);
    dstPathLineEdit->insert(QFileDialog::getExistingDirectory());

}

void MainWindow::slotClassChanged()
{
    ui->fileNameLineEdit->setText(classNameLineEdit->text());
    ui->componentNameLineEdit->setText(classNameLineEdit->text());
}

void MainWindow::slotClassNameChanged()
{
    classNameLineEdit->setText(ui->classNameCBox->currentText());
}

void MainWindow::slotBaseChanged()
{
    baseNameLineEdit->setText(ui->baseNameCBox->currentText());
    ui->baseFileNameLineEdit->setText(ui->baseNameCBox->currentText()+".h");
}

void MainWindow::slotDstChanged()
{
    dstPathLineEdit->setText(ui->dstPathCBox->currentText());
}

void MainWindow::slotNamespaceChanged()
{
    namespaceLineEdit->setText(ui->namespaceCBox->currentText());
}

void MainWindow::slotLibChanged()
{
    ui->tree->setRootIndex(dstModel->index(corePath+"\\"+libModel->data(ui->libListView->currentIndex(),0).toString()));
    emit expand();
}

void MainWindow::slotPathSelected()
{
    QString dstTemporary(dstModel->filePath(ui->tree->currentIndex()));
    if(dstTemporary.contains(".h")||dstTemporary.contains(".cpp"))
    {
        dstTemporary = dstTemporary.left(dstTemporary.lastIndexOf("/"));
    }
    ui->dstPathCBox->setLineEdit(dstPathLineEdit);
    dstPathLineEdit->setText(dstTemporary);
}

void MainWindow::slotSettings()
{
    Settings->exec();
}

void MainWindow::writeSettingsToXML()
{
    QFile xmlSettingsFile("CodeGenerator.xml");
    if (!xmlSettingsFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(xmlSettingsFile.fileName())
                             .arg(xmlSettingsFile.errorString()));
        return;
    }
    QXmlStreamWriter stream(&xmlSettingsFile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("settings");
    stream.writeTextElement("corePath", corePath);

    stream.writeStartElement("namespaces");
    for(int i=0; i<=namespaces.indexOf(namespaces.last()); i++)
    {
        QVariant temp = i+1;
        stream.writeStartElement("namespace");
        stream.writeAttribute("number", temp.toString());
        stream.writeCharacters(namespaces[i]);
        stream.writeEndElement();
    }
    stream.writeEndElement();
    stream.writeStartElement("templates");
    for(int i=0; i<=templates.indexOf(templates.last()); i++)
    {
        QVariant temp = i+1;
        stream.writeStartElement("template");
        stream.writeAttribute("number", temp.toString());
        stream.writeAttribute("name", templates[i]);
        stream.writeCharacters(templatePaths[i]+"h");
        stream.writeEndElement();
    }
    stream.writeEndElement();
    stream.writeStartElement("libList");
    for(int i=0; i<=libList.indexOf(libList.last()); i++)
    {
        QVariant temp = i+1;
        stream.writeStartElement("lib");
        stream.writeAttribute("number", temp.toString());
        stream.writeCharacters(libList[i]);
        stream.writeEndElement();
    }
    stream.writeEndElement();
    stream.writeEndElement();
    stream.writeEndDocument();

    xmlSettingsFile.close();

}

void MainWindow::writeHistoryToXML()
{
    if(!dstPathLineEdit->text().isEmpty())
    {
        paths.push_front(dstPathLineEdit->text());
    }
    paths.removeDuplicates();
    while(paths.size()>maxHistorySize) paths.removeLast();
    if(!classNameLineEdit->text().isEmpty())
    {
        classNameHistory.push_front(classNameLineEdit->text());
    }
    classNameHistory.removeDuplicates();
    while(classNameHistory.size()>maxHistorySize) classNameHistory.removeLast();
    if(!baseNameLineEdit->text().isEmpty())
    {
        baseNameHistory.push_front(baseNameLineEdit->text());
    }
    baseNameHistory.removeDuplicates();
    while(baseNameHistory.size()>maxHistorySize) baseNameHistory.removeLast();

    QFile xmlHistoryFile("CodeGenerator.history.xml");
      if (!xmlHistoryFile.open(QIODevice::WriteOnly | QIODevice::Text)){
//        QMessageBox::warning(0, QObject::tr("Wizard"),
//                             QObject::tr("Cannot open file %1:\n%2")
//                             .arg(xmlHistoryFile.fileName())
//                             .arg(xmlHistoryFile.errorString()));
        return;
    }
    QXmlStreamWriter stream(&xmlHistoryFile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("history");
    stream.writeStartElement("classNames");
    for(int i=0; i<=classNameHistory.indexOf(classNameHistory.last()); i++)
    {
        QVariant temp = i+1;
        stream.writeStartElement("className");
        stream.writeAttribute("number", temp.toString());
        stream.writeCharacters(classNameHistory[i]);
        stream.writeEndElement();
    }
    stream.writeEndElement();
    stream.writeStartElement("paths");
    for(int i=0; i<=paths.indexOf(paths.last()); i++)
    {
        QVariant temp = i+1;
        stream.writeStartElement("path");
        stream.writeAttribute("number", temp.toString());
        stream.writeCharacters(paths[i]);
        stream.writeEndElement();
    }
    stream.writeEndElement();
    stream.writeStartElement("baseNames");
    for(int i=0; i<=baseNameHistory.indexOf(baseNameHistory.last()); i++)
    {
        QVariant temp = i+1;
        stream.writeStartElement("baseName");
        stream.writeAttribute("number", temp.toString());
        stream.writeCharacters(baseNameHistory[i]);
        stream.writeEndElement();
    }
    stream.writeEndElement();
    stream.writeEndElement();
    stream.writeEndDocument();

    xmlHistoryFile.close();

}

void MainWindow::readXMLSettings()
{
      QFile xmlSettingsFile("CodeGenerator.xml");
      if (!xmlSettingsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
//        QMessageBox::warning(0, QObject::tr("Wizard"),
//                             QObject::tr("Cannot open file %1:\n%2")
//                             .arg(xmlSettingsFile.fileName())
//                             .arg(xmlSettingsFile.errorString()));
          return;
      }
     QFile xmlHistoryFile("CodeGenerator.history.xml");
      if (!xmlHistoryFile.open(QIODevice::ReadOnly | QIODevice::Text)){
//        QMessageBox::warning(0, QObject::tr("Wizard"),
//                             QObject::tr("Cannot open file %1:\n%2")
//                             .arg(xmlHistoryFile.fileName())
//                             .arg(xmlHistoryFile.errorString()));
        return;
       }
    QXmlStreamReader setReader(&xmlSettingsFile);
    QXmlStreamReader histReader(&xmlHistoryFile);
    while(!setReader.atEnd())
    {
        setReader.readNext();
        if(setReader.isStartElement() && (setReader.name() == "namespace"))
        {
            namespaces.append(setReader.readElementText());
        }
        if(setReader.isStartElement() && (setReader.name() == "template"))
        {
            templates.append(setReader.attributes().value("name").toString());
            QString tempPathString = setReader.readElementText();
            tempPathString.chop(1);
            templatePaths.append(tempPathString);
        }
        if(setReader.isStartElement() && (setReader.name() == "lib"))
        {
            libList.append(setReader.readElementText());
        }
        if(setReader.isStartElement() && (setReader.name() == "corePath"))
        {
            corePath.append(setReader.readElementText());
        }
    }
    xmlSettingsFile.close();
    while(!histReader.atEnd())
    {
        histReader.readNext();
        if(histReader.isStartElement() && (histReader.name() == "className"))
        {
            classNameHistory.append(histReader.readElementText());
        }
        if(histReader.isStartElement() && (histReader.name() == "baseName"))
        {
            baseNameHistory.append(histReader.readElementText());
        }
        if(histReader.isStartElement() && (histReader.name() == "path"))
        {
            paths.append(histReader.readElementText());
        }
    }
    xmlHistoryFile.close();

}

void MainWindow::slotaccept()
{
    writeSettingsToXML();
    writeHistoryToXML();
    AddToULib();


    //Creating H file
    QFile srcHfile(templatePaths.value(ui->templateCBox->currentIndex()) + "h");
    if (!srcHfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(srcHfile.fileName())
                             .arg(srcHfile.errorString()));
        return;
    }

    QFile dstHfile(dstPathLineEdit->text()+"/"+ui->fileNameLineEdit->text()+".h");
        if(QFile::exists(dstHfile.fileName()))
        {
            QMessageBox::warning(0,QObject::tr("Writing err0r"),
                                 QObject::tr("File %1 already exists")
                                 .arg(dstHfile.fileName()));
            return;
        }
        if (!dstHfile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(0, QObject::tr("Wizard"),
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
              line.replace("@CLASS_NAME@",classNameLineEdit->text());
          }
          if(line.contains("@FILE_NAME@"))
          {
              line.replace("@FILE_NAME@",ui->fileNameLineEdit->text());
          }
          if(line.contains("@INHERITANCE_NAME@"))
          {
              line.replace("@INHERITANCE_NAME@", baseNameLineEdit->text());
          }
          if(line.contains("@INCLUDE_FILE_NAME@"))
          {
              line.replace("@INCLUDE_FILE_NAME@", ui->baseFileNameLineEdit->text());
          }
          if(line.contains("@NAMESPACE_NAME@"))
          {
              line.replace("@NAMESPACE_NAME@", namespaceLineEdit->text());
          }
          if(line.contains("@COPYRIGHT_NAME@"))
          {
              if(!(ui->componentNameLineEdit->text().isEmpty()))
                  line.replace("@COPYRIGHT_NAME@",ui->componentNameLineEdit->text());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_YEAR@"))
          {
              if(!(ui->copyrightYearLineEdit->text().isEmpty()))
                  line.replace("@COPYRIGHT_YEAR@",ui->copyrightYearLineEdit->text());
              else
                  line.clear();
          }
          if(line.contains("@COPYRIGHT_EMAIL@"))
          {
              if(!(ui->copyrightEmailLineEdit->text().isEmpty()))
                  line.replace("@COPYRIGHT_EMAIL@",ui->copyrightEmailLineEdit->text());
              else
                  line.replace("@COPYRIGHT_EMAIL@","");
          }
          if(line.contains("@COPYRIGHT_URL@"))
          {
              if(!(ui->copyrightUrlLineEdit->text().isEmpty()))
                  line.replace("@COPYRIGHT_URL@",ui->copyrightUrlLineEdit->text());
              else
                  line.replace("@COPYRIGHT_URL@","");
          }
          if(line.contains("@COPYRIGHT_PROJECT_NAME@"))
          {
              if(!(ui->copyrightProjectNameLineEdit->text().isEmpty()))
                  line.replace("@COPYRIGHT_PROJECT_NAME@",ui->copyrightProjectNameLineEdit->text());
              else
                  line.replace("@COPYRIGHT_PROJECT_NAME@","");
          }
          //if(!(line.isEmpty()))
          {
              outH<<line<<'\n';
          }
      }

      //Creating CPP file
      QFile srcCPPfile(templatePaths.value(ui->templateCBox->currentIndex()) + "cpp");
      if (!srcCPPfile.open(QIODevice::ReadOnly | QIODevice::Text)){
          QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                               QObject::tr("Cannot open file %1:\n%2")
                               .arg(srcCPPfile.fileName())
                               .arg(srcCPPfile.errorString()));
          return;
      }

      QFile dstCPPfile(dstPathLineEdit->text()+"/"+ui->fileNameLineEdit->text()+".cpp");
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
                line.replace("@CLASS_NAME@",classNameLineEdit->text());
            }
            if(line.contains("@FILE_NAME@"))
            {
                line.replace("@FILE_NAME@",ui->fileNameLineEdit->text());
            }
            if(line.contains("@INHERITANCE_NAME@"))
            {
                line.replace("@INHERITANCE_NAME@",baseNameLineEdit->text());
            }
            if(line.contains("@INCLUDE_FILE_NAME@"))
            {
                line.replace("@INCLUDE_FILE_NAME@",ui->baseFileNameLineEdit->text());
            }
            if(line.contains("@NAMESPACE_NAME@"))
            {
                line.replace("@NAMESPACE_NAME@", namespaceLineEdit->text());
            }
            if(line.contains("@COPYRIGHT_NAME@"))
            {
                if(!(ui->componentNameLineEdit->text().isEmpty()))
                    line.replace("@COPYRIGHT_NAME@",ui->componentNameLineEdit->text());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_YEAR@"))
            {
                if(!(ui->copyrightYearLineEdit->text().isEmpty()))
                    line.replace("@COPYRIGHT_YEAR@",ui->copyrightYearLineEdit->text());
                else
                    line.clear();
            }
            if(line.contains("@COPYRIGHT_EMAIL@"))
            {
                if(!(ui->copyrightEmailLineEdit->text().isEmpty()))
                    line.replace("@COPYRIGHT_EMAIL@",ui->copyrightEmailLineEdit->text());
                else
                    line.replace("@COPYRIGHT_EMAIL@","");
            }
            if(line.contains("@COPYRIGHT_URL@"))
            {
                if(!(ui->copyrightUrlLineEdit->text().isEmpty()))
                    line.replace("@COPYRIGHT_URL@",ui->copyrightUrlLineEdit->text());
                else
                    line.replace("@COPYRIGHT_URL@","");
            }
            if(line.contains("@COPYRIGHT_PROJECT_NAME@"))
            {
                if(!(ui->copyrightProjectNameLineEdit->text().isEmpty()))
                    line.replace("@COPYRIGHT_PROJECT_NAME@",ui->copyrightProjectNameLineEdit->text());
                else
                    line.replace("@COPYRIGHT_PROJECT_NAME@","");
            }

            //if(!(line.isEmpty()))
            {
                outCPP<<line<<'\n';

            }
        }
        srcCPPfile.close();
        srcHfile.close();
        dstCPPfile.close();
        dstHfile.close();
        viewFiles();
}

void MainWindow::slotSettingsAccepted()
{
    templates.clear();
    if (!Settings->setTemplateNameLineEdit1->text().isEmpty()) templates<<Settings->setTemplateNameLineEdit1->text();
    if (!Settings->setTemplateNameLineEdit2->text().isEmpty()) templates<<Settings->setTemplateNameLineEdit2->text();
    if (!Settings->setTemplateNameLineEdit3->text().isEmpty()) templates<<Settings->setTemplateNameLineEdit3->text();

    namespaces.clear();
    if (!Settings->setNamespaceLineEdit1->text().isEmpty()) namespaces<<Settings->setNamespaceLineEdit1->text();
    if (!Settings->setNamespaceLineEdit2->text().isEmpty()) namespaces<<Settings->setNamespaceLineEdit2->text();
    if (!Settings->setNamespaceLineEdit3->text().isEmpty()) namespaces<<Settings->setNamespaceLineEdit3->text();
    if (!Settings->setNamespaceLineEdit4->text().isEmpty()) namespaces<<Settings->setNamespaceLineEdit4->text();
    if (!Settings->setNamespaceLineEdit5->text().isEmpty()) namespaces<<Settings->setNamespaceLineEdit5->text();

    libList.clear();
    if (!Settings->setLibsLineEdit1->text().isEmpty()) libList<<Settings->setLibsLineEdit1->text();
    if (!Settings->setLibsLineEdit2->text().isEmpty()) libList<<Settings->setLibsLineEdit2->text();
    if (!Settings->setLibsLineEdit3->text().isEmpty()) libList<<Settings->setLibsLineEdit3->text();

    if (!Settings->setRootPathLineEdit->text().isEmpty())
    {
        corePath.clear();
        corePath.append(Settings->setRootPathLineEdit->text());
    }

    QString temp;
    templatePaths.clear();
    if(!Settings->setTemplatePathLineEdit1->text().isEmpty())
    {
        temp = Settings->setTemplatePathLineEdit1->text();
        temp.chop(1);
        templatePaths<<temp;
    }
    if(!Settings->setTemplatePathLineEdit2->text().isEmpty())
    {
        temp = Settings->setTemplatePathLineEdit2->text();
        temp.chop(1);
        templatePaths<<temp;
    }
    if(!Settings->setTemplatePathLineEdit3->text().isEmpty())
    {
        temp = Settings->setTemplatePathLineEdit3->text();
        temp.chop(1);
        templatePaths<<temp;
    }


    writeSettingsToXML();
    emit ui->templateCBox->customContextMenuRequested(QPoint(20,20));
    Settings->accept();
}

void MainWindow::slotSettingsChanged()
{
    while(ui->templateCBox->count()>0)
    {
        ui->templateCBox->removeItem(ui->templateCBox->count()-1);
    }
    ui->templateCBox->addItems(templates);

    while(ui->namespaceCBox->count()>0)
    {
        ui->namespaceCBox->removeItem(ui->namespaceCBox->count()-1);
    }
    ui->namespaceCBox->addItems(namespaces);

    libModel->setStringList(libList);

}

void MainWindow::viewFiles()
{
    QFile srcHfile(dstPathLineEdit->text()+"/"+ui->fileNameLineEdit->text()+".h");
    if (!srcHfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(srcHfile.fileName())
                             .arg(srcHfile.errorString()));
        return;
    }
    QTextStream inH(&srcHfile);
    inH.setCodec("CP1251");
    ui->headerEdit->setPlainText(QString (inH.readAll()));

    QFile srcCPPfile(dstPathLineEdit->text()+"/"+ui->fileNameLineEdit->text()+".cpp");
    if (!srcCPPfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(srcCPPfile.fileName())
                             .arg(srcCPPfile.errorString()));
        return;
    }
    QTextStream inCPP(&srcCPPfile);
    inCPP.setCodec("CP1251");
    ui->sourceEdit->setPlainText(QString (inCPP.readAll()));

    srcCPPfile.close();
    srcHfile.close();
}

void MainWindow::AddToULib()
{
    bool exists = false;
    QString uLibFileName;
    QModelIndex parentIndex = dstModel->index(dstPathLineEdit->text());
    int numOfChilds = dstModel->rowCount(parentIndex);
    for(int i=0; i<numOfChilds; i++)
    {
        QString childFileName = dstModel->fileName(dstModel->index(i,0,parentIndex));
        if(childFileName.contains(".h"))
        {
            QFile childFile(dstPathLineEdit->text()+"/"+childFileName);
            if (!childFile.open(QIODevice::ReadOnly | QIODevice::Text)){
                QMessageBox::warning(0, QObject::tr("Wizard"),
                                     QObject::tr("Cannot open file %1:\n%2")
                                     .arg(childFile.fileName())
                                     .arg(childFile.errorString()));
                return;
            }
            QTextStream childFileStream(&childFile);
            while (!childFileStream.atEnd())
            {
                if(childFileStream.readLine().contains("public ULibrary"))
                {
                    uLibFileName = childFileName;
                    uLibFileName.chop(2);
                    break;
                }
            }
            childFile.close();

        }
    }
    QFile childFile(dstPathLineEdit->text()+"/"+uLibFileName+".h");
    if (!childFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(childFile.fileName())
                             .arg(childFile.errorString()));
        return;
    }
    QTextStream uLibFileStream(&childFile);
    QString name = ui->fileNameLineEdit->text();
    while (!uLibFileStream.atEnd())
    {
        if(uLibFileStream.readLine().contains(name))
        {
            exists = true;
            return;
        }
    }

    QFile uLibHFile(dstPathLineEdit->text()+"/"+uLibFileName+".h");
    if (!uLibHFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Code Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(uLibHFile.fileName())
                             .arg(uLibHFile.errorString()));
        return;
    }
    QTextStream uLibInHStream(&uLibHFile);
    QByteArray tempArray;
    QTextStream uLibOutHStream(&uLibHFile);
    while (!uLibInHStream.atEnd())
    {
        QString uLibStreamLine = uLibInHStream.readLine();

        if(uLibStreamLine.contains("namespace"))
        {
            uLibStreamLine.prepend("#include \""+ui->fileNameLineEdit->text()+".h\"\n\n");
            tempArray.chop(1);
        }
        tempArray.append(uLibStreamLine);
        tempArray.append("\n");
    }
    uLibHFile.close();
    if (!uLibHFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(uLibHFile.fileName())
                             .arg(uLibHFile.errorString()));
        return;
    }
    uLibOutHStream<<tempArray;//
    uLibHFile.close();

    //CPP
    QFile uLibCppFile(dstPathLineEdit->text()+"/"+uLibFileName+".cpp");
    if (!uLibCppFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(uLibCppFile.fileName())
                             .arg(uLibCppFile.errorString()));
        return;
    }

    QTextStream uLibInCppStream(&uLibCppFile);
    QByteArray tempCppArray;
    QTextStream uLibOutCppStream(&uLibCppFile);
//    QString tempStreamLine;
    //
    int braceCount = 0;
    while(!uLibInCppStream.atEnd())
    {
        if(uLibInCppStream.readLine().contains("}"))
        {
            braceCount++;
        }
    }
    uLibCppFile.close();

    if (!uLibCppFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(uLibCppFile.fileName())
                             .arg(uLibCppFile.errorString()));
        return;
    }
    //
    while (!uLibInCppStream.atEnd())
    {
        QString uLibStreamLine = uLibInCppStream.readLine();
        //QString temp2;
        //temp2.append(uLibStreamLine);
        if(uLibStreamLine.contains("namespace")&&!exists)
        {
            uLibStreamLine.prepend("#include \""+ui->fileNameLineEdit->text()+".cpp\"\n\n");
            tempCppArray.chop(1);

        }
        if(uLibStreamLine.contains("}"))
        {
            braceCount--;
            if ((braceCount == 1))
            {
                QString uploadClassString("\n{\n UEPtr<UContainer> generated_cont=new "+
                                          ui->fileNameLineEdit->text()+";"+
                                          "\n generated_cont->SetName(\""+
                                          ui->componentNameLineEdit->text()+"\");"+
                                          "\n generated_cont->Default();"+
                                          "\n UploadClass(\""+ui->fileNameLineEdit->text()+"\",generated_cont);\n}\n");//
                uLibStreamLine.push_front(uploadClassString);
            }
        }

        //temp2.append("\n"); //для Win
        tempCppArray.append(uLibStreamLine);
        tempCppArray.append("\n");
    }
    uLibCppFile.close();
    if (!uLibCppFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(0, QObject::tr("Simple Wizard"),
                             QObject::tr("Cannot open file %1:\n%2")
                             .arg(uLibCppFile.fileName())
                             .arg(uLibCppFile.errorString()));
        return;
    }
    uLibOutCppStream<<tempCppArray;//

}

void MainWindow::slotSaveHChanges()
{
    QFile dstfile((dstPathLineEdit->text()+"/"+ui->fileNameLineEdit->text()+".h"));

        if (!dstfile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(0, QObject::tr("Wizard"),
                                 QObject::tr("Cannot write file %1:\n%2")
                                 .arg(dstfile.fileName())
                                 .arg(dstfile.errorString()));
            return;
        }
    QTextStream out(&dstfile);
    out<<ui->headerEdit->toPlainText();
}

void MainWindow::slotSaveCPPChanges()
{
    QFile dstfile((dstPathLineEdit->text()+"/"+ui->fileNameLineEdit->text()+".cpp"));

        if (!dstfile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(0, QObject::tr("Wizard"),
                                 QObject::tr("Cannot write file %1:\n%2")
                                 .arg(dstfile.fileName())
                                 .arg(dstfile.errorString()));
            return;
        }
    QTextStream out(&dstfile);
    out<<ui->sourceEdit->toPlainText();
}
