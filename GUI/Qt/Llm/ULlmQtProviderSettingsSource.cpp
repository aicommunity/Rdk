#include "ULlmQtProviderSettingsSource.h"

#include <cstdlib>

#include <QProcess>
#include <QSettings>

#if defined(_WIN32)
#include <windows.h>
#include <wincred.h>
#endif

#include "../../../LLM/Core/Settings/ULLMProviderCatalog.h"

static QString profileKey(const std::string& profile_id, const char* suffix)
{
    return QStringLiteral("LLM/Providers/%1/%2")
        .arg(QString::fromStdString(profile_id))
        .arg(QString::fromLatin1(suffix));
}

static QSettings makeAppSettings()
{
    return QSettings(QStringLiteral("NeuroModeler"), QStringLiteral("NeuroModeler"));
}

static bool secureStoreEnabled()
{
    return std::getenv("NMSDK_LLM_DISABLE_SECURE_KEYSTORE") == nullptr;
}

static bool writeApiKeyToSecureStore(const std::string& profile_id, const std::string& api_key)
{
    if(!secureStoreEnabled())
        return false;
#if defined(__linux__)
    QProcess proc;
    QStringList args;
    args << QStringLiteral("store")
         << QStringLiteral("--label=NeuroModeler LLM API key")
         << QStringLiteral("service")
         << QStringLiteral("NeuroModelerLLM")
         << QStringLiteral("profile")
         << QString::fromStdString(profile_id);
    proc.start(QStringLiteral("secret-tool"), args);
    if(!proc.waitForStarted(2000))
        return false;
    proc.write(QByteArray::fromStdString(api_key));
    proc.write("\n");
    proc.closeWriteChannel();
    if(!proc.waitForFinished(4000))
        return false;
    return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
#elif defined(__APPLE__)
    QProcess proc;
    QStringList args;
    args << QStringLiteral("add-generic-password")
         << QStringLiteral("-U")
         << QStringLiteral("-a")
         << QString::fromStdString(profile_id)
         << QStringLiteral("-s")
         << QStringLiteral("NeuroModelerLLM")
         << QStringLiteral("-w")
         << QString::fromStdString(api_key);
    proc.start(QStringLiteral("security"), args);
    if(!proc.waitForFinished(4000))
        return false;
    return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
#elif defined(_WIN32)
    const std::wstring target =
        (QStringLiteral("NeuroModelerLLM/") + QString::fromStdString(profile_id)).toStdWString();
    const std::wstring secret = QString::fromStdString(api_key).toStdWString();
    CREDENTIALW cred{};
    cred.Type = CRED_TYPE_GENERIC;
    cred.TargetName = const_cast<LPWSTR>(target.c_str());
    cred.CredentialBlobSize = static_cast<DWORD>(secret.size() * sizeof(wchar_t));
    cred.CredentialBlob =
        reinterpret_cast<LPBYTE>(const_cast<wchar_t*>(secret.c_str()));
    cred.Persist = CRED_PERSIST_LOCAL_MACHINE;
    cred.UserName = const_cast<LPWSTR>(L"nmsdk");
    return CredWriteW(&cred, 0) == TRUE;
#else
    (void)profile_id;
    (void)api_key;
    return false;
#endif
}

static std::string readApiKeyFromSecureStore(const std::string& profile_id)
{
    if(!secureStoreEnabled())
        return {};
#if defined(__linux__)
    QProcess proc;
    QStringList args;
    args << QStringLiteral("lookup")
         << QStringLiteral("service")
         << QStringLiteral("NeuroModelerLLM")
         << QStringLiteral("profile")
         << QString::fromStdString(profile_id);
    proc.start(QStringLiteral("secret-tool"), args);
    if(!proc.waitForFinished(3000))
        return {};
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0)
        return {};
    const QString out = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
    return out.toStdString();
#elif defined(__APPLE__)
    QProcess proc;
    QStringList args;
    args << QStringLiteral("find-generic-password")
         << QStringLiteral("-a")
         << QString::fromStdString(profile_id)
         << QStringLiteral("-s")
         << QStringLiteral("NeuroModelerLLM")
         << QStringLiteral("-w");
    proc.start(QStringLiteral("security"), args);
    if(!proc.waitForFinished(3000))
        return {};
    if(proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0)
        return {};
    const QString out = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
    return out.toStdString();
#elif defined(_WIN32)
    const std::wstring target =
        (QStringLiteral("NeuroModelerLLM/") + QString::fromStdString(profile_id)).toStdWString();
    PCREDENTIALW cred = nullptr;
    if(CredReadW(target.c_str(), CRED_TYPE_GENERIC, 0, &cred) != TRUE || !cred)
        return {};
    const wchar_t* blob = reinterpret_cast<const wchar_t*>(cred->CredentialBlob);
    const size_t wchar_count = cred->CredentialBlobSize / sizeof(wchar_t);
    const std::wstring secret(blob, wchar_count);
    CredFree(cred);
    return QString::fromStdWString(secret).toStdString();
#else
    (void)profile_id;
    return {};
#endif
}

static void loadProfileKeys(QSettings& settings, RDK::LLM::LLMRuntimeProviderSettings& runtime,
                            const RDK::LLM::LLMProviderProfile& profile)
{
    const std::string secure_api_key = readApiKeyFromSecureStore(profile.profile_id);
    if(!secure_api_key.empty())
        runtime.api_keys_by_profile_id[profile.profile_id] = secure_api_key;

    const QString api_key_key = profileKey(profile.profile_id, "api_key");
    const bool allow_plaintext_settings_keys =
        std::getenv("NMSDK_LLM_ALLOW_PLAINTEXT_SETTINGS_KEYS") != nullptr;
    if(allow_plaintext_settings_keys && settings.contains(api_key_key))
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
    QSettings settings = makeAppSettings();
    RDK::LLM::LLMRuntimeProviderSettings runtime;
    runtime.active_profile_id =
        settings.value(QStringLiteral("LLM/active_profile_id"), QStringLiteral("ollama-thinking"))
            .toString()
            .toStdString();
    runtime.allow_cloud_providers =
        settings.value(QStringLiteral("LLM/allow_cloud_providers"), false).toBool();
    runtime.llm_write_enabled = settings.value(QStringLiteral("LLM/llm_write_enabled"), true).toBool();
    runtime.llm_auto_apply_writes =
        settings.value(QStringLiteral("LLM/llm_auto_apply_writes"), false).toBool();

    const QString autonomous =
        settings.value(QStringLiteral("LLM/autonomous_mode"), QStringLiteral("off")).toString();
    if(autonomous == QStringLiteral("strict"))
        runtime.autonomous_mode = RDK::LLM::LLMAutonomousMode::Strict;
    else if(autonomous == QStringLiteral("semi_auto"))
        runtime.autonomous_mode = RDK::LLM::LLMAutonomousMode::SemiAuto;
    else
        runtime.autonomous_mode = RDK::LLM::LLMAutonomousMode::Off;
    runtime.max_autonomous_steps =
        settings.value(QStringLiteral("LLM/max_autonomous_steps"), 3).toInt();
    runtime.translate_queries_to_en =
        settings.value(QStringLiteral("LLM/translate_queries_to_en"), true).toBool();

    const QString task_path =
        settings.value(QStringLiteral("LLM/task_path_mode"), QStringLiteral("hint_only")).toString();
    runtime.task_path_mode = task_path == QStringLiteral("fast_path")
                                 ? RDK::LLM::LLMTaskPathMode::FastPath
                                 : RDK::LLM::LLMTaskPathMode::HintOnly;

    runtime.preferred_response_language =
        settings.value(QStringLiteral("LLM/preferred_response_language"), QString())
            .toString()
            .toStdString();

    const QString send_shortcut =
        settings.value(QStringLiteral("LLM/send_shortcut"), QStringLiteral("ctrl_enter")).toString();
    runtime.send_shortcut = send_shortcut == QStringLiteral("enter")
                                ? RDK::LLM::LLMSendShortcutMode::Enter
                                : RDK::LLM::LLMSendShortcutMode::CtrlEnter;

    const QString acquisition =
        settings.value(QStringLiteral("LLM/context_acquisition_mode"), QStringLiteral("auto"))
            .toString();
    runtime.context_acquisition_mode =
        acquisition == QStringLiteral("minimal") ? RDK::LLM::LLMContextAcquisitionMode::Minimal
                                               : RDK::LLM::LLMContextAcquisitionMode::Auto;

    runtime.enable_ollama_thinking =
        settings.value(QStringLiteral("LLM/enable_ollama_thinking"), true).toBool();

    for(const RDK::LLM::LLMProviderProfile& profile : RDK::LLM::ULLMProviderCatalog::builtInProfiles())
        loadProfileKeys(settings, runtime, profile);
    return runtime;
}

void ULlmQtProviderSettingsSource::save(const RDK::LLM::LLMRuntimeProviderSettings& settings)
{
    QSettings qsettings = makeAppSettings();
    const bool allow_plaintext_settings_keys =
        std::getenv("NMSDK_LLM_ALLOW_PLAINTEXT_SETTINGS_KEYS") != nullptr;
    qsettings.setValue(QStringLiteral("LLM/active_profile_id"),
                       QString::fromStdString(settings.active_profile_id));
    qsettings.setValue(QStringLiteral("LLM/allow_cloud_providers"), settings.allow_cloud_providers);
    qsettings.setValue(QStringLiteral("LLM/llm_write_enabled"), settings.llm_write_enabled);
    qsettings.setValue(QStringLiteral("LLM/llm_auto_apply_writes"), settings.llm_auto_apply_writes);
    QString autonomous_mode = QStringLiteral("off");
    switch(settings.autonomous_mode)
    {
    case RDK::LLM::LLMAutonomousMode::Strict:
        autonomous_mode = QStringLiteral("strict");
        break;
    case RDK::LLM::LLMAutonomousMode::SemiAuto:
        autonomous_mode = QStringLiteral("semi_auto");
        break;
    case RDK::LLM::LLMAutonomousMode::Off:
    default:
        break;
    }
    qsettings.setValue(QStringLiteral("LLM/autonomous_mode"), autonomous_mode);
    qsettings.setValue(QStringLiteral("LLM/max_autonomous_steps"), settings.max_autonomous_steps);
    qsettings.setValue(QStringLiteral("LLM/translate_queries_to_en"),
                       settings.translate_queries_to_en);
    qsettings.setValue(QStringLiteral("LLM/task_path_mode"),
                       settings.task_path_mode == RDK::LLM::LLMTaskPathMode::FastPath
                           ? QStringLiteral("fast_path")
                           : QStringLiteral("hint_only"));
    qsettings.setValue(QStringLiteral("LLM/preferred_response_language"),
                       QString::fromStdString(settings.preferred_response_language));
    qsettings.setValue(QStringLiteral("LLM/send_shortcut"),
                       settings.send_shortcut == RDK::LLM::LLMSendShortcutMode::Enter
                           ? QStringLiteral("enter")
                           : QStringLiteral("ctrl_enter"));
    qsettings.setValue(QStringLiteral("LLM/context_acquisition_mode"),
                       settings.context_acquisition_mode == RDK::LLM::LLMContextAcquisitionMode::Minimal
                           ? QStringLiteral("minimal")
                           : QStringLiteral("auto"));
    qsettings.setValue(QStringLiteral("LLM/enable_ollama_thinking"), settings.enable_ollama_thinking);

    for(const auto& entry : settings.api_keys_by_profile_id)
    {
        const bool stored_securely = writeApiKeyToSecureStore(entry.first, entry.second);
        if(stored_securely)
        {
            qsettings.remove(profileKey(entry.first, "api_key"));
        }
        else if(allow_plaintext_settings_keys)
        {
            qsettings.setValue(profileKey(entry.first, "api_key"),
                               QString::fromStdString(entry.second));
        }
        else
        {
            qsettings.remove(profileKey(entry.first, "api_key"));
        }
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
