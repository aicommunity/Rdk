#ifndef UHELPWINDOW_H
#define UHELPWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QToolBar>
#include <QUrl>
#include <rdk_application.h>

class UHelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UHelpWindow(QWidget *parent = nullptr, RDK::UApplication* app = nullptr);
    ~UHelpWindow();
    
    void showHelp(const QString& topic = QString());
    void setLanguage(const QString& langCode);
    
private slots:
    void onHomeClicked();
    void onBackClicked();
    void onForwardClicked();
    void onSearchTextChanged(const QString& text);
    void onLinkClicked(const QUrl& url);
    void updateNavigationButtons();
    
private:
    void loadHelpContent(const QString& topic);
    QString getHelpPath() const;
    QString getCurrentLanguage() const;
    void setupUI();
    void setupToolbar();
    
    QTextBrowser* m_textBrowser;
    QLineEdit* m_searchEdit;
    QPushButton* m_homeButton;
    QPushButton* m_backButton;
    QPushButton* m_forwardButton;
    QString m_currentLanguage;
    QString m_helpBasePath;
    RDK::UApplication* m_application;
};

#endif // UHELPWINDOW_H

