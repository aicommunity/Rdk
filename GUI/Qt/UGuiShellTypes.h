#ifndef UGUISHELLTYPES_H
#define UGUISHELLTYPES_H

#include <QString>

/// App chrome / session-anchor presets for NeuroModeler GUI shell.
enum class GuiShellPreset
{
    Studio = 0,
    ControlBar = 1
};

inline QString toString(GuiShellPreset preset)
{
    switch(preset)
    {
    case GuiShellPreset::ControlBar:
        return QStringLiteral("ControlBar");
    case GuiShellPreset::Studio:
    default:
        return QStringLiteral("Studio");
    }
}

inline GuiShellPreset guiShellPresetFromString(const QString& value)
{
    const QString v = value.trimmed();
    if(v.compare(QStringLiteral("ControlBar"), Qt::CaseInsensitive) == 0
       || v.compare(QStringLiteral("Control Bar"), Qt::CaseInsensitive) == 0)
        return GuiShellPreset::ControlBar;
    return GuiShellPreset::Studio;
}

#endif // UGUISHELLTYPES_H
