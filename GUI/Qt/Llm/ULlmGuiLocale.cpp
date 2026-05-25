#include "ULlmGuiLocale.h"

#include <QLocale>

namespace LlmGui {

QString systemResponseLanguageCode()
{
    switch(QLocale::system().language())
    {
    case QLocale::Russian:
        return QStringLiteral("ru");
    case QLocale::German:
        return QStringLiteral("de");
    case QLocale::French:
        return QStringLiteral("fr");
    case QLocale::Chinese:
    case QLocale::LiteraryChinese:
        return QStringLiteral("zh");
    case QLocale::English:
        return QStringLiteral("en");
    default:
        return QStringLiteral("en");
    }
}

} // namespace LlmGui
