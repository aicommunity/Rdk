#ifndef UBPtzCameraInfo_H
#define UBPtzCameraInfo_H

#include <string>

namespace RDK {

class UBPtzCameraInfo
{
public:
/// Минимальное значение поворота по оси X
double MinPan;

/// Максимальное значение поворота по оси X
double MaxPan;

/// Минимальное значение скорости поворота по оси X
double MinPanSpeed;

/// Максимальное значение скорости поворота по оси X
double MaxPanSpeed;

/// Минимальное значение поворота по оси Y
double MinTilt;

/// Максимальное значение поворота по оси Y
double MaxTilt;

/// Минимальное значение скорости поворота по оси Y
double MinTiltSpeed;

/// Максимальное значение скорости поворота по оси Y
double MaxTiltSpeed;

/// Минимальное значение зума
double MinZoom;

/// Максимальное значение зума
double MaxZoom;

/// Минимальное значение зума в градусах
double MinZoomAngle;

/// Максимальное значение зума в градусах
double MaxZoomAngle;

/// Минимальное значение скорости зума
double MinZoomSpeed;

/// Максимальное значение скорости зума
double MaxZoomSpeed;

public: // Методы
/// Инициализация
void InitCanonVBM40(void);


bool operator == (const UBPtzCameraInfo &copy);
bool operator != (const UBPtzCameraInfo &copy);

};

class UBPtzCameraData: public UBPtzCameraInfo
{
public:
/// Текущее значение поворота по оси X
double Pan;

/// Текущая скорость поворота по оси X
double PanSpeed;

/// Текущее значение поворота по оси Y
double Tilt;

/// Текущая скорость поворота по оси Y
double TiltSpeed;

/// Текущее значение зума
double Zoom;

/// Текущая скорость зуммирования
double ZoomSpeed;

public: // Методы
UBPtzCameraData(void);
virtual ~UBPtzCameraData(void);

std::string& GenerateCanonVBM40Command(const std::string &addr, std::string &result);

/// Возвращает текущий угол зрения в градусах
double CalcCurrentZoomAngle(void);

/// Устанавливает камеру в нулевую позицию с минимальным зумом
/// Устанавливает максимальные скорости перемещения
virtual void ZeroPosition(void);

/// Перемещает камеру по оси X в зависимости от знака shift
virtual void MovePan(double shift, double speed=-1);

/// Перемещает камеру по оси Y в зависимости от знака shift
virtual void MoveTilt(double shift, double speed=-1);

/// Зуммирует камеру в зависимости от знака shift
virtual void MoveZoom(double shift, double speed=-1);

virtual void MoveUp(double shift, double speed=-1);

virtual void MoveDown(double shift, double speed=-1);

virtual void MoveLeft(double shift, double speed=-1);

virtual void MoveRight(double shift, double speed=-1);

virtual void ZoomIn(double shift, double speed=-1);

virtual void ZoomOut(double shift, double speed=-1);

bool operator == (const UBPtzCameraData &copy);
bool operator != (const UBPtzCameraData &copy);

UBPtzCameraData& operator = (const UBPtzCameraInfo &info);




};

}
#endif
