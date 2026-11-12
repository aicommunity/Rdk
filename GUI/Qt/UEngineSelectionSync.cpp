#include "UEngineSelectionSync.h"

#include <QStringList>

#include <rdk_cpp_init.h>
#include <rdk_engine_support.h>
#include <rdk_init.h>

#include "../../Core/Engine/UComponent.h"
#include "../../Core/Engine/UContainer.h"
#include "../../Core/Engine/UEnvironment.h"
#include "../../Core/Engine/UEngine.h"
#include "../../Core/Engine/UStorage.h"
#include "../../Core/Engine/UNet.h"
#include "../../Core/Serialize/USerStorageXML.h"
#include "../../Core/System/rdk_system.h"

namespace
{
RDK::UEPtr<RDK::UContainer> containerFromModelScope(RDK::UContainer* model, const QByteArray& scope_utf8)
{
    if(!model)
        return {};
    if(scope_utf8.isEmpty())
        return model;
    return model->GetComponentL(scope_utf8.constData(), true);
}

QString pathRelativeToModelRoot(RDK::UContainer* model, const QString& component_long_name)
{
    const QString trimmed = component_long_name.trimmed();
    if(!model || trimmed.isEmpty())
        return QString();

    const QString modelName = QString::fromUtf8(model->GetName().c_str());
    if(modelName.isEmpty())
        return trimmed;

    if(trimmed == modelName)
        return QString();
    if(trimmed.startsWith(modelName + QLatin1Char('.')))
        return trimmed.mid(modelName.length() + 1);
    return trimmed;
}

void syncEnvironmentCurrentComponent(RDK::UEngine* engine, const QString& component_long_name)
{
    if(!engine)
        return;

    RDK::UEnvironment* env = engine->GetEnvironment();
    if(!env)
        return;

    const QString trimmed = component_long_name.trimmed();
    if(trimmed.isEmpty())
    {
        engine->Env_ResetCurrentComponent("");
        return;
    }

    RDK::UEPtr<RDK::UContainer> model = env->GetModel();
    if(!model)
        return;

    const QString pathFromRoot = pathRelativeToModelRoot(model.Get(), trimmed);
    if(pathFromRoot.isEmpty())
    {
        engine->Env_ResetCurrentComponent("");
        return;
    }

    engine->Env_SelectCurrentComponent(pathFromRoot.toUtf8().constData());
}

DiagramAddComponentResult addComponentUnderModelScopeImpl(RDK::UEngine* eng,
                                                          RDK::UStorage* stor,
                                                          const QString& parent_scope_long_name,
                                                          const QByteArray& class_name_utf8)
{
    DiagramAddComponentResult result;
    if(!eng || !stor || class_name_utf8.isEmpty())
        return result;

    RDK::UEnvironment* env = eng->GetEnvironment();
    if(!env)
        return result;
    RDK::UEPtr<RDK::UContainer> model = env->GetModel();
    if(!model)
        return result;

    const QByteArray parentScope = parent_scope_long_name.trimmed().toUtf8();
    RDK::UEPtr<RDK::UContainer> parent = containerFromModelScope(model.Get(), parentScope);
    if(!parent)
        return result;

    RDK::UEPtr<RDK::UContainer> cont =
        RDK::dynamic_pointer_cast<RDK::UContainer>(stor->TakeObject(class_name_utf8.constData()));
    if(!cont)
        return result;

    if(!parent->AddComponent(cont))
    {
        stor->ReturnObject(cont);
        return result;
    }

    if(!cont->IsInit())
        cont->Init();
    else
        cont->Reset();

    std::string buffer;
    result.long_name = QString::fromUtf8(cont->GetLongName(model.Get(), buffer).c_str());
    result.short_name = QString::fromUtf8(cont->GetName().c_str());
    result.ok = true;
    return result;
}
} // namespace

QString resolveComponentLongNameFromModelRoot(int channel_index, const QString& path)
{
    const QString trimmed = path.trimmed();
    if(trimmed.isEmpty())
        return QString();

    RDK::UELockPtr<RDK::UContainer> model =
        RDK::GetModelLockTimeout<RDK::UContainer>(channel_index, 250);
    if(model)
    {
        RDK::UEPtr<RDK::UContainer> cont =
            model->GetComponentL(trimmed.toUtf8().constData(), true);
        if(cont)
            return trimmed;
    }

    const char* longRaw =
        MModel_GetComponentLongName(channel_index, trimmed.toUtf8().constData(), "");
    if(longRaw && longRaw[0] != '\0')
    {
        const QString resolved = QString::fromUtf8(longRaw);
        Engine_FreeBufString(longRaw);
        return resolved;
    }
    Engine_FreeBufString(longRaw);
    return trimmed;
}

void syncEngineCurrentComponent(const QString& component_long_name)
{
    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLock();
    if(!eng)
        return;
    syncEngineCurrentComponentWithEngine(eng.Get(), component_long_name);
}

void syncEngineCurrentComponentWithEngine(RDK::UEngine* engine, const QString& component_long_name)
{
    if(!engine)
        return;
    syncEnvironmentCurrentComponent(engine, component_long_name);
}

QStringList childComponentShortNamesFromModelScope(int channel_index, const QString& scope_long_name)
{
    QStringList out;
    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(channel_index);
    if(!model)
        return out;
    RDK::UEPtr<RDK::UContainer> cont;
    const QByteArray scope = scope_long_name.trimmed().toUtf8();
    if(scope.isEmpty())
        cont = model.Get();
    else
        cont = model->GetComponentL(scope.constData(), true);
    if(!cont)
        return out;
    std::vector<std::string> names;
    cont->GetComponentsList(names);
    out.reserve(static_cast<int>(names.size()));
    for(const std::string& n : names)
        out.append(QString::fromUtf8(n.c_str()));
    return out;
}

QString componentClassNameFromModelScope(int channel_index, const QString& component_long_name)
{
    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(channel_index);
    if(!model)
        return {};
    RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock(channel_index);
    if(!storage)
        return {};

    const QByteArray path = component_long_name.trimmed().toUtf8();
    RDK::UEPtr<RDK::UContainer> cont = containerFromModelScope(model.Get(), path);
    if(!cont)
        return {};
    return QString::fromUtf8(storage->FindClassName(cont->GetClass()).c_str());
}

std::string internalLinksXmlFromModelScope(int channel_index, const QString& scope_long_name)
{
    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(channel_index);
    if(!model)
        return {};

    const QByteArray scope = scope_long_name.trimmed().toUtf8();
    RDK::UEPtr<RDK::UNet> cont = RDK::dynamic_pointer_cast<RDK::UNet>(
        containerFromModelScope(model.Get(), scope));
    if(!cont)
        return {};

    RDK::USerStorageXML xml;
    xml.Create("Links");
    // UNet::GetComponentInternalLinks returns 0 on success, non-zero on error.
    if(cont->GetComponentInternalLinks(&xml, cont.Get()) != 0)
        return {};
    std::string out;
    xml.Save(out);
    return out;
}

bool propertyValueFromModelScope(RDK::UContainer* model_root,
                                 const QString& component_long_name,
                                 const QByteArray& param_name,
                                 std::string& out_value)
{
    out_value.clear();
    if(!model_root || param_name.isEmpty())
        return false;
    RDK::UEPtr<RDK::UContainer> cont =
        containerFromModelScope(model_root, component_long_name.trimmed().toUtf8());
    if(!cont)
        return false;
    cont->GetPropertyValue(param_name.constData(), out_value);
    return true;
}

bool setPropertyValueFromModelScope(RDK::UContainer* model_root,
                                    const QString& component_long_name,
                                    const QByteArray& param_name,
                                    const std::string& value)
{
    if(!model_root || param_name.isEmpty())
        return false;
    RDK::UEPtr<RDK::UContainer> cont =
        containerFromModelScope(model_root, component_long_name.trimmed().toUtf8());
    if(!cont)
        return false;
    cont->SetPropertyValue(param_name.constData(), value);
    return true;
}

QString propertiesLookupListFromModelScope(RDK::UContainer* model_root,
                                           const QString& component_long_name,
                                           unsigned int type_mask)
{
    if(!model_root)
        return {};
    RDK::UEPtr<RDK::UContainer> cont =
        containerFromModelScope(model_root, component_long_name.trimmed().toUtf8());
    if(!cont)
        return {};

    QStringList entries;
    const RDK::UContainer::VariableMapT props = cont->GetPropertiesList();
    const std::vector<RDK::NameT>& order = cont->GetPropertiesOrder();
    for(size_t oi = 0; oi < order.size(); ++oi)
    {
        RDK::UContainer::VariableMapCIteratorT it = props.find(order[oi]);
        if(it == props.end())
            continue;
        if(!it->second.CheckMask(type_mask))
            continue;
        entries.append(QString::fromUtf8(it->first.c_str()) + QLatin1Char(':') + QString::number(0));
    }
    return entries.join(QLatin1Char(','));
}

DiagramAddComponentResult addComponentUnderModelScopeWithLocks(RDK::UEngine* engine,
                                                               RDK::UStorage* storage,
                                                               const QString& parent_scope_long_name,
                                                               const QByteArray& class_name_utf8)
{
    return addComponentUnderModelScopeImpl(engine, storage, parent_scope_long_name, class_name_utf8);
}

DiagramAddComponentResult addComponentUnderModelScope(int channel_index,
                                                      const QString& parent_scope_long_name,
                                                      const QByteArray& class_name_utf8)
{
    DiagramAddComponentResult result;
    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLock(channel_index);
    if(!eng)
        return result;
    RDK::UELockPtr<RDK::UStorage> storLock = RDK::GetStorageLock(channel_index);
    if(!storLock)
        return result;
    return addComponentUnderModelScopeImpl(eng.Get(), storLock.Get(), parent_scope_long_name, class_name_utf8);
}
