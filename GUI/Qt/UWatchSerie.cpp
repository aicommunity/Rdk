#include "UWatchSerie.h"
#include <QPen>

UWatchSerie::UWatchSerie()
{
    isOnline = true;
}

void UWatchSerie::setOnlineStatus(bool online)
{
    if (isOnline == online) {
        return; // Статус не изменился
    }

    isOnline = online;
    QPen pen = this->pen();
    
    if (online) {
        // Активная серия: сплошная линия
        pen.setStyle(Qt::SolidLine);
    } else {
        // Неактивная серия: пунктирная линия
        pen.setStyle(Qt::DashLine);
    }
    
    this->setPen(pen);
}
