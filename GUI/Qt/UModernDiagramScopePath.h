#ifndef UMODERNDIAGRAMSCOPEPATH_H
#define UMODERNDIAGRAMSCOPEPATH_H

#include <QSet>
#include <QString>

/// Pure path helpers for Modern Diagram scope resolution (unit-testable).
namespace UModernDiagramScopePath {

/// Strip current scope prefix from a model-root or relative endpoint path.
/// Returns empty if path equals the scope itself.
inline QString relativeUnderScope(const QString& path,
                                  const QString& scopeLongName,
                                  const QString& scopeShortName)
{
    if(path.isEmpty())
        return QString();

    QString relative = path;
    if(!scopeLongName.isEmpty())
    {
        if(relative == scopeLongName)
            return QString();
        const QString scopePrefix = scopeLongName + QLatin1Char('.');
        if(relative.startsWith(scopePrefix))
            relative = relative.mid(scopePrefix.size());
        else if(relative.contains(QLatin1Char('.')) && !scopeShortName.isEmpty())
        {
            if(relative.startsWith(scopeShortName + QLatin1Char('.')))
                relative = relative.mid(scopeShortName.size() + 1);
        }
    }
    return relative;
}

/// Top-level child name under the current diagram scope (first segment of relative path).
inline QString topChildName(const QString& path,
                            const QString& scopeLongName,
                            const QString& scopeShortName)
{
    const QString relative = relativeUnderScope(path, scopeLongName, scopeShortName);
    if(relative.isEmpty())
        return QString();
    const int dot = relative.indexOf(QLatin1Char('.'));
    return dot >= 0 ? relative.left(dot) : relative;
}

/// True if endpoint maps to a visible node on the current diagram.
inline bool resolvesToVisibleNode(const QString& pathOrName,
                                  const QString& scopeLongName,
                                  const QString& scopeShortName,
                                  const QSet<QString>& visibleNodeNames)
{
    if(pathOrName.isEmpty() || visibleNodeNames.isEmpty())
        return false;
    if(visibleNodeNames.contains(pathOrName))
        return true;
    const QString top = topChildName(pathOrName, scopeLongName, scopeShortName);
    return !top.isEmpty() && visibleNodeNames.contains(top);
}

/// Path relative to current diagram scope, suitable for sink labels
/// (drops the current level prefix, e.g. Dendrite1_85.ExcChannel → ExcChannel).
inline QString labelPathUnderScope(const QString& itemId,
                                   const QString& scopeLongName,
                                   const QString& scopeShortName)
{
    if(itemId.isEmpty())
        return QString();
    const QString relative = relativeUnderScope(itemId, scopeLongName, scopeShortName);
    if(!relative.isEmpty())
        return relative;
    // Path equals scope or could not strip — keep last segment only.
    const int dot = itemId.lastIndexOf(QLatin1Char('.'));
    return dot >= 0 ? itemId.mid(dot + 1) : itemId;
}

/// Local producer with at least one consumer not resolvable on the current diagram.
inline bool isOutgoingExternalExport(const QString& itemName,
                                     const QString& itemId,
                                     const QString& connName,
                                     const QString& connId,
                                     const QString& scopeLongName,
                                     const QString& scopeShortName,
                                     const QSet<QString>& visibleNodeNames)
{
    const bool localItem =
        resolvesToVisibleNode(itemId, scopeLongName, scopeShortName, visibleNodeNames)
        || resolvesToVisibleNode(itemName, scopeLongName, scopeShortName, visibleNodeNames);
    if(!localItem)
        return false;

    const bool localConn =
        resolvesToVisibleNode(connId, scopeLongName, scopeShortName, visibleNodeNames)
        || resolvesToVisibleNode(connName, scopeLongName, scopeShortName, visibleNodeNames);
    return !localConn;
}

} // namespace UModernDiagramScopePath

#endif // UMODERNDIAGRAMSCOPEPATH_H
