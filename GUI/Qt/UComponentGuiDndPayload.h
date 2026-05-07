#ifndef UCOMPONENTGUIDNDPAYLOAD_H
#define UCOMPONENTGUIDNDPAYLOAD_H

#include <QString>
#include <QByteArray>

#include "UComponentGuiContext.h"

class QMimeData;

namespace UComponentGuiDndPayload
{
const char* mimeType();
QByteArray encode(const UComponentGuiContext& context,
                  const QString& sourceGridId = QString(),
                  int sourceRow = -1,
                  int sourceCol = -1);
bool decode(const QMimeData* mimeData,
            UComponentGuiContext& context,
            QString& sourceGridId,
            int& sourceRow,
            int& sourceCol);
}

#endif // UCOMPONENTGUIDNDPAYLOAD_H
