#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QGridLayout>
#include <QMessageBox>
#include <QFileDialog>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    virtual ~SettingsDialog();
    void readSettings();
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



};
#endif // SETTINGSDIALOG_H
