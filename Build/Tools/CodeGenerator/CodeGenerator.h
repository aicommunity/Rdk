#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QLineEdit>
#include <QStringListModel>
#include <QFileSystemModel>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include "settingsdialog.h"
#include <QTextStream>
#include <QTextEdit>
#include "myhighlighter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QLineEdit *dstPathLineEdit;
    QLineEdit *classNameLineEdit;
    QLineEdit *baseNameLineEdit;
    QLineEdit *namespaceLineEdit;

    QStringListModel *libModel;
    QFileSystemModel *dstModel;
    QStringList libList;
    QString corePath;
    QString *currentClassName;

    SettingsDialog *Settings;

    QStringList classNameHistory;
    QStringList baseNameHistory;
    QStringList namespaces;
    QStringList paths;
    QStringList templatePaths;
    QStringList templates;

    QList<QTextEdit::ExtraSelection> selectionsList;
    MyHighlighter *headerHighlighter;
    MyHighlighter *sourceHighlighter;

    const int maxHistorySize;

    void readXMLSettings();
    void AddToULib();
    void writeSettingsToXML();//test
    void writeHistoryToXML();
    void viewFiles();
//    void addProperty();

signals:
    void expand();

public slots:
    void slotBrowse();
    void slotClassChanged();
    void slotClassNameChanged();
    void slotBaseChanged();
    void slotDstChanged();
    void slotNamespaceChanged();
//    void slotSettingsChanged();
    void slotLibChanged();
    void slotPathSelected();
    void slotSettings();
    void slotaccept();
    void slotSettingsAccepted();
    void slotSettingsChanged();
    void slotSaveHChanges();
    void slotSaveCPPChanges();
    void slotAddProperty();
    void slotPropertyTypeChanged();
};

#endif // CODEGENERATOR_H
