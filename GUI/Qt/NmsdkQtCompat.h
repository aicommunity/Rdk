#ifndef NMSDK_QT_COMPAT_H
#define NMSDK_QT_COMPAT_H

#include <QtGlobal>
#include <QTextStream>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

#include <QStringConverter>
#include <QLayout>

inline void nmsdkQtTextStreamSetCodec(QTextStream& stream, const char* codecName)
{
    if (qstricmp(codecName, "UTF-8") == 0) {
        stream.setEncoding(QStringConverter::Utf8);
        return;
    }
    stream.setEncoding(QStringConverter::encodingForName(codecName));
}

inline void nmsdkQtLayoutSetMargin(QLayout* layout, int margin)
{
    layout->setContentsMargins(margin, margin, margin, margin);
}

#define NMSDK_QT_TEXT_STREAM_SET_CODEC(stream, codec) nmsdkQtTextStreamSetCodec((stream), (codec))
#define NMSDK_QT_LAYOUT_SET_MARGIN(layout, margin) nmsdkQtLayoutSetMargin((layout), (margin))

// Qt 6 Charts types live in the global namespace.
#define NMSDK_QT_CHARTS_USE_NS
#define NMSDK_QT_CHARTS_BASE(type) type

#else

#include <QLayout>

#define NMSDK_QT_TEXT_STREAM_SET_CODEC(stream, codec) (stream).setCodec(codec)
#define NMSDK_QT_LAYOUT_SET_MARGIN(layout, margin) (layout)->setMargin(margin)
#define NMSDK_QT_CHARTS_USE_NS using namespace QtCharts;
#define NMSDK_QT_CHARTS_BASE(type) QtCharts::type

#endif

#endif // NMSDK_QT_COMPAT_H
