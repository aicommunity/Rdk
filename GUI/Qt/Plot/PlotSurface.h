#ifndef PLOT_SURFACE_H
#define PLOT_SURFACE_H

#include "../UWatchChart.h"

namespace NMSDK
{
namespace Plot
{

/// Thin Qt Charts surface used by Watch panels (canonical render path).
class PlotSurface : public UWatchChart
{
    Q_OBJECT
public:
    explicit PlotSurface(QWidget* parent = nullptr)
        : UWatchChart(parent)
    {
        setObjectName(QStringLiteral("PlotSurface"));
    }
};

} // namespace Plot
} // namespace NMSDK

#endif // PLOT_SURFACE_H
