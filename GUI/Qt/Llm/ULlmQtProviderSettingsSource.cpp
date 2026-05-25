#include "ULlmQtProviderSettingsSource.h"

#include <QSettings>

#include "../../../LLM/Core/Settings/ULLMProviderCatalog.h"

static QString profileKey(const std::string& profile_id, const char* suffix)
{
    return QStringLiteral("LLM/Providers/%1/%2")
        .arg(QString::fromStdString(profile_id))
        .arg(QString::fromLatin1(suffix));
}

static void loadProfileKeys(QSettings& settings, RDK::LLM::LLMRuntimeProviderSettings& runtime,
                            const RDK::LLM::LLMProviderProfile& profile)
{
    const QString api_key_key = profileKey(profile.profile_id, "api_key");
    if(settings.contains(api_key_key))
    {
        const QString value = settings.value(api_key_key).toString();
        if(!value.isEmpty())
            runtime.api_keys_by_profile_id[profile.profile_id] = value.toStdString();
    }

    RDK::LLM::LLMProfileEndpointOverride endpoint;
    const QString base_url_key = profileKey(profile.profile_id, "base_url");
    const QString model_key = profileKey(profile.profile_id, "model");
    if(settings.contains(base_url_key))
        endpoint.base_url = settings.value(base_url_key).toString().toStdString();
    if(settings.contains(model_key))
        endpoint.model = settings.value(model_key).toString().toStdString();
    if(!endpoint.base_url.empty() || !endpoint.model.empty())
        runtime.endpoint_overrides_by_profile_id[profile.profile_id] = endpoint;
}

RDK::LLM::LLMRuntimeProviderSettings ULlmQtProviderSettingsSource::load() const
{
    QSettings settings;
    RDK::LLM::LLMRuntimeProviderSettings runtime;
    runtime.active_profile_id =
        settings.value(QStringLiteral("LLM/active_profile_id"), QStringLiteral("ollama-local"))
            .toString()
            .toStdString();
    runtime.allow_cloud_providers =
        settings.value(QStringLiteral("LLM/allow_cloud_providers"), false).toBool();
    runtime.llm_write_enabled = settings.value(QStringLiteral("LLM/llm_write_enabled"), true).toBool();

    runtime.preferred_response_language =
        settings.value(QStringLiteral("LLM/preferred_response_language"), QString())
            .toString()
            .toStdString();

    const QString send_shortcut =
        settings.value(QStringLiteral("LLM/send_shortcut"), QStringLiteral("ctrl_enter")).toString();
    runtime.send_shortcut = send_shortcut == QStringLiteral("enter")
                                ? RDK::LLM::LLMSendShortcutMode::Enter
                                : RDK::LLM::LLMSendShortcutMode::CtrlEnter;

    for(const RDK::LLM::LLMProviderProfile& profile : RDK::LLM::ULLMProviderCatalog::builtInProfiles())
        loadProfileKeys(settings, runtime, profile);
    return runtime;
}

void ULlmQtProviderSettingsSource::save(const RDK::LLM::LLMRuntimeProviderSettings& settings)
{
    QSettings qsettings;
    qsettings.setValue(QStringLiteral("LLM/active_profile_id"),
                       QString::fromStdString(settings.active_profile_id));
    qsettings.setValue(QStringLiteral("LLM/allow_cloud_providers"), settings.allow_cloud_providers);
    qsettings.setValue(QStringLiteral("LLM/llm_write_enabled"), settings.llm_write_enabled);
    qsettings.setValue(QStringLiteral("LLM/preferred_response_language"),
                       QString::fromStdString(settings.preferred_response_language));
    qsettings.setValue(QStringLiteral("LLM/send_shortcut"),
                       settings.send_shortcut == RDK::LLM::LLMSendShortcutMode::Enter
                           ? QStringLiteral("enter")
                           : QStringLiteral("ctrl_enter"));

    for(const auto& entry : settings.api_keys_by_profile_id)
    {
        qsettings.setValue(profileKey(entry.first, "api_key"),
                         QString::fromStdString(entry.second));
    }

    for(const RDK::LLM::LLMProviderProfile& preset :
        RDK::LLM::ULLMProviderCatalog::builtInProfiles())
    {
        const QString base_url_key = profileKey(preset.profile_id, "base_url");
        const QString model_key = profileKey(preset.profile_id, "model");
        if(auto it = settings.endpoint_overrides_by_profile_id.find(preset.profile_id);
           it != settings.endpoint_overrides_by_profile_id.end())
        {
            if(!it->second.base_url.empty())
                qsettings.setValue(base_url_key, QString::fromStdString(it->second.base_url));
            else
                qsettings.remove(base_url_key);
            if(!it->second.model.empty())
                qsettings.setValue(model_key, QString::fromStdString(it->second.model));
            else
                qsettings.remove(model_key);
        }
        else
        {
            qsettings.remove(base_url_key);
            qsettings.remove(model_key);
        }
    }
    qsettings.sync();
}
