#ifndef CODEWIZARD_H
#define CODEWIZARD_H

#include <QWizard>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include <QMenuBar>
#include <QMenu>
#include <QDialogButtonBox>
#include <QFileSystemModel>
#include <QTreeView>
#include <QStringListModel>
#include <QListView>
#include <QSplitter>

class InitialPage;
class SettingsDialog;


class CodeWizard : public QWizard
{
    Q_OBJECT

public:
    CodeWizard(QWidget *parent = 0);
    void accept();

public slots:
    void slotaccept();
    void slotSettings();
    void slotSettingsAccepted();
private:
    InitialPage *IP;
    SettingsDialog *Settings;
};

class InitialPage : public QWizardPage
{
    Q_OBJECT
public:
    InitialPage(QWidget *parent = 0);
    virtual ~InitialPage();
    void saveSettings();
    void loadSettings();
    friend class SettingsDialog;
signals:
    void expand();
public slots:
    void slotBrowse();
    void slotClassChanged();
    void slotBaseChanged();
    void slotDstChanged();//-------------------------------------
    void slotClassNameChanged();
    void slotNamespaceChanged();
    void slotSettingsChanged();
    void slotLibChanged();
    void slotPathSelected();
//private:
public://
    QLabel *templateLabel;
    QLabel *fileNameLabel;
    QLabel *classNameLabel;
    QLabel *componentNameLabel;
    QLabel *dstPathLabel;
    QLabel *baseNameLabel;
    QLabel *baseFileNameLabel;
    QLabel *namespaceLabel;
    QLabel *copyrightNameLabel;
    QLabel *copyrightYearLabel;
    QLabel *copyrightEmailLabel;
    QLabel *copyrightUrlLabel;
    QLabel *copyrightProjectNameLabel;

    QComboBox *templateCBox;
    QStringList templates;

    QLineEdit *fileNameLineEdit;

    QLineEdit *classNameLineEdit;
    QComboBox *classNameCBox;

    QLineEdit *componentNameLineEdit;

    QLineEdit *dstPathLineEdit;
    QComboBox *dstPathCBox;//

    QLineEdit *baseNameLineEdit;
    QComboBox *baseNameCBox;

    QLineEdit *baseFileNameLineEdit;

    QComboBox *namespaceCBox;
    QLineEdit *namespaceLineEdit;

    QLineEdit *copyrightNameLineEdit;
    QLineEdit *copyrightYearLineEdit;
    QLineEdit *copyrightEmailLineEdit;
    QLineEdit *copyrightUrlLineEdit;
    QLineEdit *copyrightProjectNameLineEdit;
    QPushButton *browseBut;

    QSettings settings;
    QStringList paths;
    QString srcPath;
    QStringList classNameHistory;
    QStringList baseNameHistory;
    QStringList namespaces;

    QStringList templatePaths;


    QString corePath;

    QFileSystemModel *dstModel;
    QTreeView *tree;
    QStringListModel *libModel;
    QStringList libList;
    QListView *libListView;

    QSplitter *splitter;

    const int maxHistorySize;

    void AddToULib();//test
};

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    void saveSettingsDialog();
    void loadSettingsDialog();
public slots:
    //void slotAccepted();
    void slotRootBrowse();
    void slotTemplateBrowse1();
    void slotTemplateBrowse2();
    void slotTemplateBrowse3();
    void slotTempPathChanged1();
    void slotTempPathChanged2();
    void slotTempPathChanged3();
//private:
public://
    QLabel *setTemplateNameLabel;
    QLabel *setTemplatePathLabel;
    QLineEdit *setTemplateNameLineEdit1;
    QLineEdit *setTemplateNameLineEdit2;
    QLineEdit *setTemplateNameLineEdit3;
    QLineEdit *setTemplatePathLineEdit1;
    QLineEdit *setTemplatePathLineEdit2;
    QLineEdit *setTemplatePathLineEdit3;

    QLabel *setNamespaceLabel;
    QLineEdit *setNamespaceLineEdit1;
    QLineEdit *setNamespaceLineEdit2;
    QLineEdit *setNamespaceLineEdit3;
    QLineEdit *setNamespaceLineEdit4;
    QLineEdit *setNamespaceLineEdit5;

    QDialogButtonBox *buttons;

    QLabel *setRootPathLabel;
    QLineEdit *setRootPathLineEdit;
    QLabel *setLibsLabel;
    QLineEdit *setLibsLineEdit1;
    QLineEdit *setLibsLineEdit2;
    QLineEdit *setLibsLineEdit3;

    QSettings presets;

};

#endif // CODEWIZARD_H
