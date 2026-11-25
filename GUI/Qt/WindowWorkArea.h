#ifndef WINDOW_WORK_AREA_H
#define WINDOW_WORK_AREA_H

#include <QRect>

namespace NMSDK::Gui
{
inline QRect fitWindowFrameToWorkArea(const QRect& frame, const QRect& workArea)
{
    if (frame.isEmpty() || workArea.isEmpty())
        return frame;
    const QSize size = frame.size().boundedTo(workArea.size());
    return QRect(qBound(workArea.left(), frame.left(), workArea.right() - size.width() + 1),
                 qBound(workArea.top(), frame.top(), workArea.bottom() - size.height() + 1),
                 size.width(), size.height());
}
}

#endif
