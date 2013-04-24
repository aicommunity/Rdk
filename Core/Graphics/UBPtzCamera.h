#ifndef UBPtzCameraInfo_H
#define UBPtzCameraInfo_H

#include <string>

namespace RDK {

class UBPtzCameraInfo
{
public:
/// Минимальная условная величина
static const int MinCValue=-10000;

/// Максимальная условная величина
static const int MaxCValue=10000;

public:
/// Минимальное значение поворота по оси X в единицах камеры
double MinPan;

/// Максимальное значение поворота по оси X в единицах камеры
double MaxPan;

/// Минимальное значение поворота по оси X в градусах
double MinPanAngle;

/// Максимальное значение поворота по оси X в градусах
double MaxPanAngle;

/// Минимальное значение скорости поворота по оси X в единицах камеры
double MinPanSpeed;

/// Максимальное значение скорости поворота по оси X в единицах камеры
double MaxPanSpeed;

/// Минимальное значение поворота по оси Y в единицах камеры
double MinTilt;

/// Максимальное значение поворота по оси Y в единицах камеры
double MaxTilt;

/// Минимальное значение поворота по оси Y в градусах
double MinTiltAngle;

/// Максимальное значение поворота по оси Y в градусах
double MaxTiltAngle;

/// Минимальное значение скорости поворота по оси Y в единицах камеры
double MinTiltSpeed;

/// Максимальное значение скорости поворота по оси Y в единицах камеры
double MaxTiltSpeed;

/// Минимальное значение зума в единицах камеры
double MinZoom;

/// Максимальное значение зума в единицах камеры
double MaxZoom;

/// Минимальное значение зума в градусах
double MinZoomAngle;

/// Максимальное значение зума в градусах
double MaxZoomAngle;

/// Минимальное значение скорости зума в единицах камеры
double MinZoomSpeed;

/// Максимальное значение скорости зума в единицах камеры
double MaxZoomSpeed;

/// Минимальное значение скорости фокусировки в единицах камеры
double MinFocusSpeed;

/// Максимальное значение скорости фокусировки в единицах камеры
double MaxFocusSpeed;

/// Минимальное значение скорости изменения диафрагмы в единицах камеры
double MinIrisSpeed;

/// Максимальное значение скорости изменения диафрагмы в единицах камеры
double MaxIrisSpeed;

/// Минимальное значение скорости изменения яркости в единицах камеры
double MinBrightnessSpeed;

/// Максимальное значение скорости изменения яркости в единицах камеры
double MaxBrightnessSpeed;

/// Минимальный интервал времени между командами (мс)
int MinCommandDelay;

public: // Методы
UBPtzCameraInfo(void);

/// Возвращает значение величины во внешних единицах по значению во внутренних
double CalcExternalValue(double value, double min_internal, double max_internal, double min_external, double max_external) const;

/// Возвращает значение величины во внутренних единицах по значению во внешних
double CalcInternalValue(double value, double min_internal, double max_internal, double min_external, double max_external) const;

/// Возвращает угол зрения в градусах по углу в попугаях
double CalcZoomAngle(double zoom) const;

/// Возвращает угол зрения в попугаях по углу в градусах
double CalcZoom(double zoom) const;

/// Инициализация
void InitCanonVBM40(void);


bool operator == (const UBPtzCameraInfo &copy);
bool operator != (const UBPtzCameraInfo &copy);

};

class UBPtzCameraData: public UBPtzCameraInfo
{
protected:
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

/// Текущее значение фокуса
double Focus;

/// Текущая скорости фокусировки
double FocusSpeed;

/// Текущее значение диафрагмиы
double Iris;

/// Текущая скорости диафрагмы
double IrisSpeed;

/// Текущее значение яркости
double Brightness;

/// Текущая скорости изменения яркости
double BrightnessSpeed;

public: // Методы
UBPtzCameraData(void);
virtual ~UBPtzCameraData(void);

std::string& GenerateCanonVBM40Command(const std::string &addr, std::string &result);

// -----------------------------------
// Методы доступа к данным
// -----------------------------------
/// Текущее значение поворота по оси X
double GetPan(void) const;
double GetPanNative(void) const;
void SetPanNative(double value);

/// Текущая скорость поворота по оси X
double GetPanSpeed(void) const;
double GetPanSpeedNative(void) const;
void SetPanSpeedNative(double value);

/// Текущее значение поворота по оси Y
double GetTilt(void) const;
double GetTiltNative(void) const;
void SetTiltNative(double value);

/// Текущая скорость поворота по оси Y
double GetTiltSpeed(void) const;
double GetTiltSpeedNative(void) const;
void SetTiltSpeedNative(double value);

/// Текущее значение зума
double GetZoom(void) const;
double GetZoomNative(void) const;
void SetZoomNative(double value);

/// Текущая скорость зуммирования
double GetZoomSpeed(void) const;
double GetZoomSpeedNative(void) const;
void SetZoomSpeedNative(double value);

/// Текущее значение фокуса
double GetFocus(void) const;
double GetFocusNative(void) const;
void SetFocusNative(double value);

/// Текущая скорости фокусировки
double GetFocusSpeed(void) const;
double GetFocusSpeedNative(void) const;
void SetFocusSpeedNative(double value);

/// Текущее значение диафрагмиы
double GetIris(void) const;
double GetIrisNative(void) const;
void SetIrisNative(double value);

/// Текущая скорости диафрагмы
double GetIrisSpeed(void) const;
double GetIrisSpeedNative(void) const;
void SetIrisSpeedNative(double value);

/// Текущее значение яркости
double GetBrightness(void) const;
double GetBrightnessNative(void) const;
void SetBrightnessNative(double value);

/// Текущая скорости изменения яркости
double GetBrightnessSpeed(void) const;
double GetBrightnessSpeedNative(void) const;
void SetBrightnessSpeedNative(double value);
// -----------------------------------

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

virtual void ContinuesMove(double pan_speed, double tilt_speed);
virtual void ContinuesZoom(double speed);

virtual void ContinuesFocus(double speed);
virtual void ContinuesIris(double speed);
virtual void ContinuesBrightness(double speed);

bool operator == (const UBPtzCameraData &copy);
bool operator != (const UBPtzCameraData &copy);

UBPtzCameraData& operator = (const UBPtzCameraInfo &info);
};

}
#endif
