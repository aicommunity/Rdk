#ifndef CALIBRATION_DLL_H
#define CALIBRATION_DLL_H

struct CProjectedPoint
{
 double x,y;
 double error;
};

extern "C" {

__declspec(dllexport) int __cdecl CameraCalibrateInit(int num_frames, int width, int height, int board_width, int board_height, double board_size, int num_cameras=1);

__declspec(dllexport) int __cdecl SetCameraCalibrationStepDelay(int value);

__declspec(dllexport) int __cdecl CameraCalibrationStep(unsigned char *imagedata, int camera_index=0);

__declspec(dllexport) int __cdecl CameraCalibrateComplete(double* icc, double* dist_coeffs, int* num_dist_coeffs, double* ecc, double &error, int camera_index=0);

//__declspec(dllexport) double __cdecl CameraCalibrate(unsigned char *imagedata, double* objects_points, double* image_points, int num_points, int image_width, int image_height, 
//		double* icc, double* dist_coeffs, int* num_dist_coeffs, double* ecc);

__declspec(dllexport) int __cdecl StereoCalibrateInit(double* icc1, double* dist_coeffs1, double* icc2, double* dist_coeffs2, int num_dist_coeffs);

__declspec(dllexport) int __cdecl StereoCalibrateComplete(double* r, double* t, double &error);

__declspec(dllexport) void __cdecl Undistortion(char *data, int imagewidth, int imageheight, double IntMat[3][3], double DistCoeff[5][1], double NewIntMat[3][3], int *newimagewidth, int *newimageheight, double alpha)

// Осуществляет инициализацию поиска маркера, определения его пространственной ориентации и ошибки репроекции
__declspec(dllexport) void __cdecl CameraMarkerSearchInit(double *icc, double *dist_coeff, int camera_index=0);

// Осуществляет определение внешней калибровки по последнему найденому калибровочному маркеру
__declspec(dllexport) int __cdecl ExternalCalibrationStep(unsigned char *imagedata, double* ecc, double *avg_error, double *max_error, double *min_error, CProjectedPoint *all_errors, int camera_index);

}




#endif
