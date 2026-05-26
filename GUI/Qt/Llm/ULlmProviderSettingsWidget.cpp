#include "ULlmProviderSettingsWidget.h"

#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>

#include "../../../LLM/Core/LlmPublicApi.h"
#include "../../../LLM/Core/Providers/UOllamaModelInfo.h"
#include "../../../LLM/Core/Providers/ULLMProviderFactory.h"
#include "../../../LLM/Core/Settings/ULLMProviderAuth.h"
#include "../../../LLM/Core/Settings/ULLMProviderCatalog.h"
#include "../../../LLM/Core/Settings/ULLMResponseLanguage.h"
#include "ULlmGuiLocale.h"

ULlmProviderSettingsWidget::ULlmProviderSettingsWidget(QWidget* parent, RDK::UApplication* app)
    : QDialog(parent)
    , m_app(app)
{
    setWindowTitle(tr("AI Assistant Settings"));
    setMinimumWidth(480);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("Provider profile:"), this));
    m_profiles = new QComboBox(this);
    layout->addWidget(m_profiles);

    auto* endpoint_form = new QFormLayout();
    m_base_url = new QLineEdit(this);
    m_base_url->setPlaceholderText(tr("e.g. http://127.0.0.1:11434/v1"));
    m_model = new QComboBox(this);
    m_model->setEditable(true);
    m_model->lineEdit()->setPlaceholderText(tr("e.g. qwen2.5:7b"));
    endpoint_form->addRow(tr("Base URL:"), m_base_url);
    endpoint_form->addRow(tr("Model:"), m_model);
    layout->addLayout(endpoint_form);

    m_refresh_ollama_models = new QPushButton(tr("Refresh Ollama model list"), this);
    layout->addWidget(m_refresh_ollama_models);

    auto* reset_btn = new QPushButton(tr("Reset URL and model to defaults"), this);
    layout->addWidget(reset_btn);

    layout->addWidget(new QLabel(tr("API key (stored locally, not in project files):"), this));
    m_api_key = new QLineEdit(this);
    m_api_key->setEchoMode(QLineEdit::Password);
    layout->addWidget(m_api_key);

    m_allow_cloud = new QCheckBox(tr("Allow cloud providers (DeepSeek, OpenAI)"), this);
    layout->addWidget(m_allow_cloud);

    m_allow_write = new QCheckBox(
        tr("Allow LLM write tools (create/load/save configuration, add/set components)"), this);
    layout->addWidget(m_allow_write);

    m_auto_apply_writes = new QCheckBox(
        tr("Apply write tools automatically (no confirmation step for each tool call)"), this);
    layout->addWidget(m_auto_apply_writes);

    connect(m_allow_write, &QCheckBox::toggled, m_auto_apply_writes, &QWidget::setEnabled);

    auto* lang_form = new QFormLayout();
    m_response_language = new QComboBox(this);
    m_response_language->addItem(tr("Auto (system)"), QString());
    m_response_language->addItem(tr("English"), QStringLiteral("en"));
    m_response_language->addItem(tr("Russian"), QStringLiteral("ru"));
    m_response_language->addItem(tr("German"), QStringLiteral("de"));
    m_response_language->addItem(tr("French"), QStringLiteral("fr"));
    m_response_language->addItem(tr("Chinese (Simplified)"), QStringLiteral("zh"));
    lang_form->addRow(tr("Response language:"), m_response_language);

    m_send_shortcut = new QComboBox(this);
    m_send_shortcut->addItem(tr("Ctrl+Enter"), QStringLiteral("ctrl_enter"));
    m_send_shortcut->addItem(tr("Enter"), QStringLiteral("enter"));
    lang_form->addRow(tr("Send message:"), m_send_shortcut);
    layout->addLayout(lang_form);

    m_status = new QLabel(this);
    m_status->setWordWrap(true);
    layout->addWidget(m_status);

    auto* test_btn = new QPushButton(tr("Test connection"), this);
    auto* save_btn = new QPushButton(tr("Save and apply"), this);
    layout->addWidget(test_btn);
    layout->addWidget(save_btn);

    connect(m_profiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ULlmProviderSettingsWidget::onProfileChanged);
    connect(reset_btn, &QPushButton::clicked, this, &ULlmProviderSettingsWidget::onResetEndpointsClicked);
    connect(save_btn, &QPushButton::clicked, this, &ULlmProviderSettingsWidget::onSaveClicked);
    connect(test_btn, &QPushButton::clicked, this,
            &ULlmProviderSettingsWidget::onTestConnectionClicked);
    connect(m_refresh_ollama_models, &QPushButton::clicked, this,
            &ULlmProviderSettingsWidget::onRefreshOllamaModelsClicked);

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
    m_auto_apply_writes->setChecked(store.runtime().llm_auto_apply_writes);
    m_auto_apply_writes->setEnabled(store.runtime().llm_write_enabled);

    const QString lang =
        QString::fromStdString(store.runtime().preferred_response_language);
    const int lang_index = m_response_language->findData(lang);
    m_response_language->setCurrentIndex(lang_index >= 0 ? lang_index : 0);

    const QString send_mode =
        store.runtime().send_shortcut == RDK::LLM::LLMSendShortcutMode::Enter
            ? QStringLiteral("enter")
            : QStringLiteral("ctrl_enter");
    const int send_index = m_send_shortcut->findData(send_mode);
    m_send_shortcut->setCurrentIndex(send_index >= 0 ? send_index : 0);

    onProfileChanged(select_index);
}

void ULlmProviderSettingsWidget::onProfileChanged(int index)
{
    if(index < 0)
        return;
    const std::string profile_id = m_profiles->currentData().toString().toStdString();
    auto& store = RDK::LLM::LLMServices::instance().settings();
    const auto preset = store.presetProfile(profile_id);
    const auto& runtime = store.runtime();

    auto api_it = runtime.api_keys_by_profile_id.find(profile_id);
    if(api_it != runtime.api_keys_by_profile_id.end())
        m_api_key->setText(QString::fromStdString(api_it->second));
    else
        m_api_key->clear();

    const RDK::LLM::LLMProfileEndpointOverride override = store.endpointOverride(profile_id);
    if(preset.kind == RDK::LLM::LLMProviderKind::EmbeddedLlama)
    {
        m_base_url->setEnabled(false);
        m_base_url->setPlaceholderText(tr("In-process (no URL)"));
        m_model->setEnabled(true);
        m_model->lineEdit()->setPlaceholderText(tr("Path to .gguf model file"));
    }
    else
    {
        m_base_url->setEnabled(true);
        m_base_url->setPlaceholderText(tr("e.g. http://127.0.0.1:11434/v1"));
        m_model->lineEdit()->setPlaceholderText(tr("e.g. qwen2.5:7b"));
    }

    m_base_url->setText(override.base_url.empty() ? QString::fromStdString(preset.base_url)
                                                  : QString::fromStdString(override.base_url));
    const QString model_text = override.model.empty() ? QString::fromStdString(preset.model)
                                                      : QString::fromStdString(override.model);
    m_model->setEditText(model_text);

    updateOllamaModelRefreshVisibility();

    QString hint;
    if(preset.kind == RDK::LLM::LLMProviderKind::EmbeddedLlama)
    {
        hint = tr("Offline GGUF. Env: NMSDK_LLM_GGUF_PATH, NMSDK_LLM_CTX, NMSDK_LLM_GPU_LAYERS");
    }
    else if(preset.kind == RDK::LLM::LLMProviderKind::OllamaOpenAICompat
            || preset.kind == RDK::LLM::LLMProviderKind::OllamaNative)
    {
        hint = tr("Ollama: chat template auto (qwen/llama3/mistral). Tools need Ollama 0.3+.");
        hint += tr("\nDefaults: %1 · %2")
                    .arg(QString::fromStdString(preset.base_url))
                    .arg(QString::fromStdString(preset.model));
        hint += tr("\nEnv: NMSDK_LLM_OLLAMA_BASE_URL, NMSDK_LLM_OLLAMA_MODEL");
    }
    else
    {
        if(!preset.api_key_env.empty())
            hint = tr("Env fallback: %1").arg(QString::fromStdString(preset.api_key_env));
        hint += tr("\nDefaults: %1 · %2")
                    .arg(QString::fromStdString(preset.base_url))
                    .arg(QString::fromStdString(preset.model));
    }
    const std::string configured =
        m_response_language->currentData().toString().toStdString();
    const std::string effective = RDK::LLM::resolveResponseLanguage(
        configured, LlmGui::systemResponseLanguageCode().toStdString());
    hint += tr("\nResponse language: %1")
                .arg(QString::fromStdString(
                    RDK::LLM::responseLanguageDisplayName(effective)));
    if(configured.empty())
        hint += tr(" (from system)");

    m_status->setText(hint.trimmed());

    if(m_refresh_ollama_models->isVisible())
        onRefreshOllamaModelsClicked();
}

void ULlmProviderSettingsWidget::updateOllamaModelRefreshVisibility()
{
    const std::string profile_id = m_profiles->currentData().toString().toStdString();
    const auto preset =
        RDK::LLM::LLMServices::instance().settings().presetProfile(profile_id);
    const bool ollama = preset.kind == RDK::LLM::LLMProviderKind::OllamaOpenAICompat
                        || preset.kind == RDK::LLM::LLMProviderKind::OllamaNative;
    m_refresh_ollama_models->setVisible(ollama);
}

RDK::LLM::LLMProviderProfile ULlmProviderSettingsWidget::profileFromFields() const
{
    RDK::LLM::LLMProviderProfile profile =
        RDK::LLM::LLMServices::instance().settings().presetProfile(
            m_profiles->currentData().toString().toStdString());
    profile.base_url = m_base_url->text().trimmed().toStdString();
    profile.model = m_model->currentText().trimmed().toStdString();
    profile.api_key = m_api_key->text().toStdString();
    return profile;
}

void ULlmProviderSettingsWidget::onRefreshOllamaModelsClicked()
{
    const RDK::LLM::LLMProviderProfile profile = profileFromFields();
    if(profile.kind != RDK::LLM::LLMProviderKind::OllamaOpenAICompat
       && profile.kind != RDK::LLM::LLMProviderKind::OllamaNative)
        return;

    m_refresh_ollama_models->setEnabled(false);
    const QString previous = m_model->currentText();
    auto* watcher = new QFutureWatcher<std::vector<std::string>>(this);
    connect(watcher, &QFutureWatcher<std::vector<std::string>>::finished, this,
            [this, watcher, previous]() {
                const std::vector<std::string> models = watcher->result();
                watcher->deleteLater();
                m_refresh_ollama_models->setEnabled(true);
                m_model->clear();
                for(const std::string& name : models)
                    m_model->addItem(QString::fromStdString(name));
                if(!previous.isEmpty())
                {
                    const int idx = m_model->findText(previous);
                    if(idx >= 0)
                        m_model->setCurrentIndex(idx);
                    else
                        m_model->setEditText(previous);
                }
                if(models.empty())
                    m_status->setText(tr("No models returned from Ollama /api/tags"));
                else
                    m_status->setText(tr("Ollama models: %1 listed").arg(static_cast<int>(models.size())));
            });
    watcher->setFuture(QtConcurrent::run([profile]() {
        return RDK::LLM::listOllamaTagModels(profile);
    }));
}

void ULlmProviderSettingsWidget::onResetEndpointsClicked()
{
    const std::string profile_id = m_profiles->currentData().toString().toStdString();
    const auto preset =
        RDK::LLM::LLMServices::instance().settings().presetProfile(profile_id);
    m_base_url->setText(QString::fromStdString(preset.base_url));
    m_model->setEditText(QString::fromStdString(preset.model));
}

void ULlmProviderSettingsWidget::saveToStore()
{
    auto& store = RDK::LLM::LLMServices::instance().settings();
    const std::string profile_id = m_profiles->currentData().toString().toStdString();
    const auto preset = store.presetProfile(profile_id);

    store.setActiveProfileId(profile_id);
    store.setApiKeyForProfile(profile_id, m_api_key->text().toStdString());

    const std::string base_url = m_base_url->text().trimmed().toStdString();
    const std::string model = m_model->currentText().trimmed().toStdString();
    if(base_url == preset.base_url && model == preset.model)
        store.clearEndpointOverride(profile_id);
    else
        store.setEndpointOverride(profile_id, base_url, model);

    store.setAllowCloudProviders(m_allow_cloud->isChecked());
    store.setLlmWriteEnabled(m_allow_write->isChecked());
    store.setLlmAutoApplyWrites(m_allow_write->isChecked() && m_auto_apply_writes->isChecked());
    store.setPreferredResponseLanguage(m_response_language->currentData().toString().toStdString());
    store.setSendShortcut(m_send_shortcut->currentData().toString() == QStringLiteral("enter")
                              ? RDK::LLM::LLMSendShortcutMode::Enter
                              : RDK::LLM::LLMSendShortcutMode::CtrlEnter);
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
        m_status->setText(tr("Connection OK — %1 / %2")
                              .arg(QString::fromStdString(profile.base_url))
                              .arg(QString::fromStdString(profile.model)));
    else
        m_status->setText(tr("Failed: %1").arg(QString::fromStdString(err)));
}
