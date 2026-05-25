#include "ULlmProviderSettingsWidget.h"

#include <QLabel>
#include <QVBoxLayout>

#include "../../../LLM/Core/LlmPublicApi.h"
#include "../../../LLM/Core/Providers/ULLMProviderFactory.h"
#include "../../../LLM/Core/Settings/ULLMProviderAuth.h"
#include "../../../LLM/Core/Settings/ULLMProviderCatalog.h"

ULlmProviderSettingsWidget::ULlmProviderSettingsWidget(QWidget* parent, RDK::UApplication* app)
    : QDialog(parent)
    , m_app(app)
{
    setWindowTitle(tr("AI Assistant Settings"));
    setMinimumWidth(420);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("Provider profile:"), this));
    m_profiles = new QComboBox(this);
    layout->addWidget(m_profiles);

    layout->addWidget(new QLabel(tr("API key (stored locally, not in project files):"), this));
    m_api_key = new QLineEdit(this);
    m_api_key->setEchoMode(QLineEdit::Password);
    layout->addWidget(m_api_key);

    m_allow_cloud = new QCheckBox(tr("Allow cloud providers (DeepSeek, OpenAI)"), this);
    layout->addWidget(m_allow_cloud);

    m_allow_write = new QCheckBox(tr("Allow LLM write tools (add/set property)"), this);
    layout->addWidget(m_allow_write);

    m_status = new QLabel(this);
    m_status->setWordWrap(true);
    layout->addWidget(m_status);

    auto* test_btn = new QPushButton(tr("Test connection"), this);
    auto* save_btn = new QPushButton(tr("Save and apply"), this);
    layout->addWidget(test_btn);
    layout->addWidget(save_btn);

    connect(m_profiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ULlmProviderSettingsWidget::onProfileChanged);
    connect(save_btn, &QPushButton::clicked, this, &ULlmProviderSettingsWidget::onSaveClicked);
    connect(test_btn, &QPushButton::clicked, this,
            &ULlmProviderSettingsWidget::onTestConnectionClicked);

    loadFromStore();
}

void ULlmProviderSettingsWidget::loadFromStore()
{
    auto& store = RDK::LLM::LLMServices::instance().settings();
    store.reload();
    m_profiles->clear();
    int select_index = 0;
    int index = 0;
    for(const RDK::LLM::LLMProviderProfile& profile : store.listProfiles())
    {
        const QString label =
            profile.is_cloud
                ? tr("%1 (cloud)").arg(QString::fromStdString(profile.profile_id))
                : tr("%1 (local)").arg(QString::fromStdString(profile.profile_id));
        m_profiles->addItem(label, QString::fromStdString(profile.profile_id));
        if(profile.profile_id == store.runtime().active_profile_id)
            select_index = index;
        ++index;
    }
    m_profiles->setCurrentIndex(select_index);
    m_allow_cloud->setChecked(store.runtime().allow_cloud_providers);
    m_allow_write->setChecked(store.runtime().llm_write_enabled);
    onProfileChanged(select_index);
}

void ULlmProviderSettingsWidget::onProfileChanged(int index)
{
    if(index < 0)
        return;
    const std::string profile_id = m_profiles->currentData().toString().toStdString();
    auto& store = RDK::LLM::LLMServices::instance().settings();
    const auto& runtime = store.runtime();
    auto it = runtime.api_keys_by_profile_id.find(profile_id);
    if(it != runtime.api_keys_by_profile_id.end())
        m_api_key->setText(QString::fromStdString(it->second));
    else
        m_api_key->clear();

    if(const RDK::LLM::LLMProviderProfile* preset =
           RDK::LLM::ULLMProviderCatalog::findById(profile_id))
    {
        if(!preset->api_key_env.empty())
        {
            m_status->setText(tr("Env fallback: %1")
                                  .arg(QString::fromStdString(preset->api_key_env)));
        }
        else
            m_status->clear();
    }
}

void ULlmProviderSettingsWidget::saveToStore()
{
    auto& store = RDK::LLM::LLMServices::instance().settings();
    const std::string profile_id = m_profiles->currentData().toString().toStdString();
    store.setActiveProfileId(profile_id);
    store.setApiKeyForProfile(profile_id, m_api_key->text().toStdString());
    store.setAllowCloudProviders(m_allow_cloud->isChecked());
    store.setLlmWriteEnabled(m_allow_write->isChecked());
    store.save();
    RDK::LLM::LLMServices::instance().applyActiveProvider();
}

void ULlmProviderSettingsWidget::onSaveClicked()
{
    saveToStore();
    accept();
}

void ULlmProviderSettingsWidget::onTestConnectionClicked()
{
    saveToStore();
    const RDK::LLM::LLMProviderProfile profile =
        RDK::LLM::LLMServices::instance().settings().activeProfile();
    RDK::LLM::LLMSessionContext session;
    session.allow_cloud_llm = true;
    const RDK::LLM::ProviderAccessCheck access = RDK::LLM::ULLMProviderAuth::checkProviderAccess(
        profile, session, RDK::LLM::LLMServices::instance().settings().runtime());
    if(!access.allowed)
    {
        m_status->setText(QString::fromStdString(access.deny_message));
        return;
    }

    std::string err;
    if(profile.kind == RDK::LLM::LLMProviderKind::Mock)
    {
        m_status->setText(tr("Mock provider — OK"));
        return;
    }

    auto provider = RDK::LLM::ULLMProviderFactory::create(profile);
    if(provider && provider->healthCheck(err))
        m_status->setText(tr("Connection OK"));
    else
        m_status->setText(tr("Failed: %1").arg(QString::fromStdString(err)));
}
