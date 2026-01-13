#include "UModernDiagramTooltipGenerator.h"
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramLinkItem.h"
#include "UModernDiagramPort.h"

#include <QCoreApplication>

QString UModernDiagramTooltipGenerator::getPortCategoryName(UModernDiagramPortCategory category)
{
    switch(category)
    {
    case UModernDiagramPortCategory::Own:
        return QCoreApplication::translate("UModernDiagramTooltipGenerator", "Own");
    case UModernDiagramPortCategory::Child:
        return QCoreApplication::translate("UModernDiagramTooltipGenerator", "Child");
    case UModernDiagramPortCategory::Alias:
        return QCoreApplication::translate("UModernDiagramTooltipGenerator", "Alias");
    default:
        return QCoreApplication::translate("UModernDiagramTooltipGenerator", "Unknown");
    }
}

QString UModernDiagramTooltipGenerator::generateNodeTooltip(const UModernDiagramNodeItem* node)
{
    if(!node)
        return QString();

    QString tooltip = QCoreApplication::translate("UModernDiagramTooltipGenerator",
        "<b>%1</b><br/>"
        "<i>Class: %2</i><br/><br/>"
        "<b>Actions:</b><br/>"
        "• Left Click - Select<br/>"
        "• Double Left Click - Enter component<br/>"
        "• Right Click - Context menu<br/>"
        "• Drag - Move node<br/>"
        "• Drag selected - Move group<br/><br/>"
        "<b>Keys:</b><br/>"
        "• Delete - Delete (with confirmation)<br/>"
        "• Shift + Delete - Delete without confirmation<br/>"
        "• Esc - Cancel operation"
    ).arg(node->nodeName, node->className);

    return tooltip;
}

// Explicitly mark as non-inline to ensure proper linking on Windows
QString UModernDiagramTooltipGenerator::generatePortTooltip(const UModernDiagramPort& port)
{
    QString portType = port.isInput
        ? QCoreApplication::translate("UModernDiagramTooltipGenerator", "Input")
        : QCoreApplication::translate("UModernDiagramTooltipGenerator", "Output");
    QString categoryName = getPortCategoryName(port.category);
    QString actionText = port.isInput
        ? QCoreApplication::translate("UModernDiagramTooltipGenerator", "• Left Click - Complete connection")
        : QCoreApplication::translate("UModernDiagramTooltipGenerator", "• Left Click - Start connection");

    QString tooltip = QCoreApplication::translate("UModernDiagramTooltipGenerator",
        "<b>%1</b><br/>"
        "<i>%2</i><br/>"
        "Type: %3 | Category: %4<br/><br/>"
        "<b>Actions:</b><br/>"
        "%5<br/>"
        "• Shift + Left Click - Show nested ports<br/>"
        "• Enter - Select port (in selection window)"
    ).arg(port.displayName, port.fullPath, portType, categoryName, actionText);

    return tooltip;
}

QString UModernDiagramTooltipGenerator::generateLinkTooltip(const UModernDiagramLinkItem* link)
{
    if(!link || !link->getSourceNode())
        return QString();

    QString srcName = link->getSourceNode()->nodeName;
    const UModernDiagramNodeItem* dstNode = link->getDestinationNode();
    QString dstName = dstNode ? dstNode->nodeName : QCoreApplication::translate("UModernDiagramTooltipGenerator", "(temporary)");

    QString tooltip = QCoreApplication::translate("UModernDiagramTooltipGenerator",
        "<b>Connection</b><br/>"
        "From: %1<br/>"
        "To: %2<br/><br/>"
        "<b>Actions:</b><br/>"
        "• Right Click - Context menu"
    ).arg(srcName, dstName);

    return tooltip;
}

QString UModernDiagramTooltipGenerator::generateCanvasTooltip()
{
    QString tooltip = QCoreApplication::translate("UModernDiagramTooltipGenerator",
        "<b>Diagram Canvas</b><br/><br/>"
        "<b>Selection:</b><br/>"
        "• Left Click + Drag - Select rectangle<br/>"
        "• Shift + Left Click + Drag - Add to selection<br/><br/>"
        "<b>Navigation:</b><br/>"
        "• Ctrl + Left Click + Drag - Pan canvas<br/>"
        "• Mouse Wheel - Zoom<br/>"
        "• Double Right Click - Go up level<br/><br/>"
        "<b>Connections:</b><br/>"
        "• Right Click - Cancel connection"
    );

    return tooltip;
}

