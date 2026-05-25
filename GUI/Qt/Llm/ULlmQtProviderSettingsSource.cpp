#include "ULlmQtProviderSettingsSource.h"

#include <QSettings>

#include "../../../LLM/Core/Settings/ULLMProviderCatalog.h"

static QString profileKey(const std::string& profile_id, const char* suffix)
{
    return QStringLiteral("LLM/Providers/%1/%2")
        .arg(QString::fromStdString(profile_id))
        .arg(QString::fromLatin1(suffix));
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

    for(const RDK::LLM::LLMProviderProfile& profile : RDK::LLM::ULLMProviderCatalog::builtInProfiles())
    {
        const QString key = profileKey(profile.profile_id, "api_key");
        if(settings.contains(key))
        {
            const QString value = settings.value(key).toString();
            if(!value.isEmpty())
                runtime.api_keys_by_profile_id[profile.profile_id] = value.toStdString();
        }
    }
    return runtime;
}

void ULlmQtProviderSettingsSource::save(const RDK::LLM::LLMRuntimeProviderSettings& settings)
{
    QSettings qsettings;
    qsettings.setValue(QStringLiteral("LLM/active_profile_id"),
                       QString::fromStdString(settings.active_profile_id));
    qsettings.setValue(QStringLiteral("LLM/allow_cloud_providers"), settings.allow_cloud_providers);
    qsettings.setValue(QStringLiteral("LLM/llm_write_enabled"), settings.llm_write_enabled);

    for(const auto& entry : settings.api_keys_by_profile_id)
    {
        qsettings.setValue(profileKey(entry.first, "api_key"),
                         QString::fromStdString(entry.second));
    }
    qsettings.sync();
}
