#ifndef UPROPERTYLISTOPTIONS_H
#define UPROPERTYLISTOPTIONS_H

#include <QFlags>

/// Presentation / filtering options for Modern components property list.
enum class PropertyListPresentation
{
    Tabbed = 0,
    UnifiedGrouped = 1
};

enum PropertyGroupFlag
{
    GroupNone       = 0,
    GroupParameters = 1 << 0,
    GroupState      = 1 << 1,
    GroupInputs     = 1 << 2,
    GroupOutputs    = 1 << 3,
    GroupFavorites  = 1 << 4,
    GroupAllIO      = GroupParameters | GroupState | GroupInputs | GroupOutputs
};
Q_DECLARE_FLAGS(PropertyGroupFlags, PropertyGroupFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(PropertyGroupFlags)

enum class PropertySubtitleMode
{
    None = 0,
    Type = 1,
    Path = 2,
    TypeAndPath = 3
};

enum class PropertySortMode
{
    Declaration = 0,
    Alphabetical = 1
};

/// Компоновка Modern ComponentsList под контекст встраивания.
enum class ComponentsListLayoutPreset
{
    /// Главная панель Properties: вертикальный splitter, дерево в popup (☰).
    Inspector = 0,
    /// Мастер/диалог выбора свойства: дерево слева, свойства справа;
    /// канал компактно сверху слева; класс над свойствами; Name тянется.
    PropertyPicker = 1
};

struct PropertyListOptions
{
    PropertyListPresentation presentation = PropertyListPresentation::Tabbed;
    PropertyGroupFlags visibleGroups = GroupAllIO;
    bool watchableOnly = false;
    PropertySubtitleMode subtitleMode = PropertySubtitleMode::Type;
    bool allowInlineEdit = true;
    bool showValueColumn = true;
    bool expandGroups = true;
    bool hideTabBarInUnified = true;
    bool selectLeavesOnly = true;
    PropertySortMode sortWithinGroup = PropertySortMode::Declaration;
};

#endif // UPROPERTYLISTOPTIONS_H
