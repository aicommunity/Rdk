// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <string.h>
#include <time.h>

using namespace cv;
using namespace std;
/*
static bool runCalibration( vector<vector<Point2f> > imagePoints,
                    Size imageSize, Size boardSize, Pattern patternType,
                    float squareSize, float aspectRatio,
                    int flags, Mat& cameraMatrix, Mat& distCoeffs,
                    vector<Mat>& rvecs, vector<Mat>& tvecs,
                    vector<float>& reprojErrs,
                    double& totalAvgErr)
{
    cameraMatrix = Mat::eye(3, 3, CV_64F);
    if( flags & CV_CALIB_FIX_ASPECT_RATIO )
        cameraMatrix.at<double>(0,0) = aspectRatio;
    
    distCoeffs = Mat::zeros(8, 1, CV_64F);
    
    vector<vector<Point3f> > objectPoints(1);
    calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

    objectPoints.resize(imagePoints.size(),objectPoints[0]);
    
    double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                    distCoeffs, rvecs, tvecs, flags|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
                    ///|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
    printf("RMS error reported by calibrateCamera: %g\n", rms);
    
    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
    
    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

    return ok;
}*/

extern "C" {

double __declspec(dllexport) CameraCalibrate(double* objects_points, double* image_points, int num_points, int image_width, int image_height, 
		double* icc, double* dist_coeffs, int* num_dist_coeffs, double* ecc)
{
 double totalAvgErr=0;
 int flags=0;
 Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
 //   if( flags & CV_CALIB_FIX_ASPECT_RATIO )
 //       cameraMatrix.at<double>(0,0) = aspectRatio;
    
 Mat distCoeffs = Mat::zeros(8, 1, CV_64F);
    
 vector<vector<Point3f> > objectPoints(1);
 vector<vector<Point2f> > imagePoints(1);
	 
	 //calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

 objectPoints.resize(imagePoints.size(),objectPoints[0]);

 CvSize imageSize;
 imageSize.height=image_height;
 imageSize.width=image_width;
 vector<Mat> rvecs;
 vector<Mat> tvecs;
    
    double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                    distCoeffs, rvecs, tvecs, flags|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
                    ///|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
  //  printf("RMS error reported by calibrateCamera: %g\n", rms);
    
    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
    
 //   totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
 //               rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

 return totalAvgErr;	
}

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

