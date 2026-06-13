#ifndef NMSDK_QT_COMPAT_H
#define NMSDK_QT_COMPAT_H

#include <QtGlobal>
#include <QTextStream>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

#include <QStringConverter>
#include <QStringDecoder>
#include <QLayout>

inline void nmsdkQtTextStreamSetCodec(QTextStream& stream, const char* codecName)
{
    if (qstricmp(codecName, "UTF-8") == 0) {
        stream.setEncoding(QStringConverter::Utf8);
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    const auto encoding = QStringConverter::encodingForName(codecName);
    stream.setEncoding(encoding.value_or(QStringConverter::Utf8));
#else
    stream.setEncoding(QStringConverter::encodingForName(codecName));
#endif
}

inline QString nmsdkQtDecodeFromCodec(const QByteArray& bytes, const char* codecName)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    const auto encoding = QStringConverter::encodingForName(codecName);
    if (encoding) {
        return QStringDecoder(*encoding)(bytes);
    }
#else
    const QStringDecoder decoder(codecName);
    if (decoder.isValid()) {
        return decoder(bytes);
    }
#endif
    return QString::fromUtf8(bytes);
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
#include <QTextCodec>

inline QString nmsdkQtDecodeFromCodec(const QByteArray& bytes, const char* codecName)
{
    QTextCodec* codec = QTextCodec::codecForName(codecName);
    if (codec) {
        return codec->toUnicode(bytes);
    }
    return QString::fromUtf8(bytes);
}

#define NMSDK_QT_TEXT_STREAM_SET_CODEC(stream, codec) (stream).setCodec(codec)
#define NMSDK_QT_LAYOUT_SET_MARGIN(layout, margin) (layout)->setMargin(margin)
#define NMSDK_QT_CHARTS_USE_NS using namespace QtCharts;
#define NMSDK_QT_CHARTS_BASE(type) QtCharts::type

#endif

#endif // NMSDK_QT_COMPAT_H
