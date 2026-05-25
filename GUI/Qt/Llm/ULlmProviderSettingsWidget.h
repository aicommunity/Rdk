#ifndef RDK_ULLM_PROVIDER_SETTINGS_WIDGET_H
#define RDK_ULLM_PROVIDER_SETTINGS_WIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace RDK {
class UApplication;
}

class ULlmProviderSettingsWidget : public QDialog {
    Q_OBJECT
public:
    explicit ULlmProviderSettingsWidget(QWidget* parent, RDK::UApplication* app);

private slots:
    void onProfileChanged(int index);
    void onSaveClicked();
    void onTestConnectionClicked();
    void onResetEndpointsClicked();

private:
    void loadFromStore();
    void saveToStore();

    RDK::UApplication* m_app = nullptr;
    QComboBox* m_profiles = nullptr;
    QLineEdit* m_api_key = nullptr;
    QLineEdit* m_base_url = nullptr;
    QLineEdit* m_model = nullptr;
    QCheckBox* m_allow_cloud = nullptr;
    QCheckBox* m_allow_write = nullptr;
    QComboBox* m_response_language = nullptr;
    QComboBox* m_send_shortcut = nullptr;
    QLabel* m_status = nullptr;
};

#endif
