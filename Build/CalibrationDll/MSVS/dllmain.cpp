// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <string.h>
#include <time.h>

enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };
enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

using namespace cv;
using namespace std;


int NumFrames=0;
int ImageWidth=640;
int ImageHeight=480;

int CurrentFrame=0;

double ICC[3][3];
double ECC[4][4];
double Distortion[8];

Size boardSize, imageSize;
float squareSize = 1.f, aspectRatio = 1.f;
vector<Mat> cameraMatrix, distCoeffs;
const char* outputFilename = "out_camera_data.yml";
const char* inputFilename = 0;
    
int i, nframes = 10;
bool writeExtrinsics = false, writePoints = false;
bool undistortImage = false;
int flags = 0;
VideoCapture capture;
bool flipVertical = false;
bool showUndistorted = false;
bool videofile = false;
int delay = 3000;
vector<clock_t> prevTimestamp;
int mode = DETECTION;
int cameraId = 0;
vector<vector<vector<Point2f> > >imagePoints;
vector<string> imageList;
Pattern pattern = CHESSBOARD;
vector<Mat> rvecs, tvecs;
vector<float> reprojErrs;
double totalAvgErr = 0;
vector<Mat> views;
Mat view;

double computeReprojectionErrors(
        const vector<vector<Point3f> >& objectPoints,
        const vector<vector<Point2f> >& imagePoints,
        const vector<Mat>& rvecs, const vector<Mat>& tvecs,
        const Mat& cameraMatrix, const Mat& distCoeffs,
        vector<float>& perViewErrors )
{
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());
    
    for( i = 0; i < (int)objectPoints.size(); i++ )
    {
        projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
                      cameraMatrix, distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float)std::sqrt(err*err/n);
        totalErr += err*err;
        totalPoints += n;
    }
    
    return std::sqrt(totalErr/totalPoints);
}

static void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType = CHESSBOARD)
{
    corners.resize(0);
    
    switch(patternType)
    {
      case CHESSBOARD:
      case CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(Point3f(float(j*squareSize),
                                          float(i*squareSize), 0));
        break;

      case ASYMMETRIC_CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(Point3f(float((2*j + i % 2)*squareSize),
                                          float(i*squareSize), 0));
        break;

      default:
        CV_Error(CV_StsBadArg, "Unknown pattern type\n");
    }
}

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
                    ///*|CV_CALIB_FIX_K3*/|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
    printf("RMS error reported by calibrateCamera: %g\n", rms);
    
    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
    
    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

    return ok;
}


void saveCameraParams( const string& filename,
                       Size imageSize, Size boardSize,
                       float squareSize, float aspectRatio, int flags,
                       const Mat& cameraMatrix, const Mat& distCoeffs,
                       const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                       const vector<float>& reprojErrs,
                       const vector<vector<Point2f> >& imagePoints,
                       double totalAvgErr )
{
    FileStorage fs( filename, FileStorage::WRITE );
    
    time_t t;
    time( &t );
    struct tm *t2 = localtime( &t );
    char buf[1024];
    strftime( buf, sizeof(buf)-1, "%c", t2 );

    fs << "calibration_time" << buf;
    
    if( !rvecs.empty() || !reprojErrs.empty() )
        fs << "nframes" << (int)std::max(rvecs.size(), reprojErrs.size());
    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;
    fs << "board_width" << boardSize.width;
    fs << "board_height" << boardSize.height;
    fs << "square_size" << squareSize;
    
    if( flags & CV_CALIB_FIX_ASPECT_RATIO )
        fs << "aspectRatio" << aspectRatio;

    if( flags != 0 )
    {
        sprintf( buf, "flags: %s%s%s%s",
            flags & CV_CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
            flags & CV_CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
            flags & CV_CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
            flags & CV_CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "" );
        cvWriteComment( *fs, buf, 0 );
    }
    
    fs << "flags" << flags;

    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;

    fs << "avg_reprojection_error" << totalAvgErr;
    if( !reprojErrs.empty() )
        fs << "per_view_reprojection_errors" << Mat(reprojErrs);
    
    if( !rvecs.empty() && !tvecs.empty() )
    {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++ )
        {
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "extrinsic_parameters" << bigmat;
    }
    
    if( !imagePoints.empty() )
    {
        Mat imagePtMat((int)imagePoints.size(), imagePoints[0].size(), CV_32FC2);
        for( int i = 0; i < (int)imagePoints.size(); i++ )
        {
            Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
            Mat imgpti(imagePoints[i]);
            imgpti.copyTo(r);
        }
        fs << "image_points" << imagePtMat;
    }
}

static bool readStringList( const string& filename, vector<string>& l )
{
    l.resize(0);
    FileStorage fs(filename, FileStorage::READ);
    if( !fs.isOpened() )
        return false;
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
        return false;
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
        l.push_back((string)*it);
    return true;
}


bool runAndSave(const string& outputFilename,
                const vector<vector<Point2f> >& imagePoints,
                Size imageSize, Size boardSize, Pattern patternType, float squareSize,
                float aspectRatio, int flags, Mat& cameraMatrix,
                Mat& distCoeffs, bool writeExtrinsics, bool writePoints )
{
    
    bool ok = runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize,
                   aspectRatio, flags, cameraMatrix, distCoeffs,
                   rvecs, tvecs, reprojErrs, totalAvgErr);
//    printf("%s. avg reprojection error = %.2f\n",
//           ok ? "Calibration succeeded" : "Calibration failed",
//           totalAvgErr);
    
    if( ok )
	{
	}
/*        saveCameraParams( outputFilename, imageSize,
                         boardSize, squareSize, aspectRatio,
                         flags, cameraMatrix, distCoeffs,
                         writeExtrinsics ? rvecs : vector<Mat>(),
                         writeExtrinsics ? tvecs : vector<Mat>(),
                         writeExtrinsics ? reprojErrs : vector<float>(),
                         writePoints ? imagePoints : vector<vector<Point2f> >(),
                         totalAvgErr );*/
    return ok;
}

double CameraCalibrate(unsigned char *imagedata, double* objects_points, double* image_points, int num_points, int image_width, int image_height, 
		double* icc, double* dist_coeffs, int* num_dist_coeffs, double* ecc)
{
 double totalAvgErr=0;
 int flags=0;
 *num_dist_coeffs=0;
 Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
 //   if( flags & CV_CALIB_FIX_ASPECT_RATIO )
 //       cameraMatrix.at<double>(0,0) = aspectRatio;
    
 Mat distCoeffs = Mat::zeros(8, 1, CV_64F);
    
 vector<vector<Point3f> > objectPoints(1);
 vector<vector<Point2f> > imagePoints(1);
 
 int k=0;
 for(int i=0;i<4;i++,k+=3)
  objectPoints[0].push_back(Point3f(objects_points[k],objects_points[k+1], objects_points[k+2]));
	 
 k=0;
 for(int i=0;i<4;i++,k+=2)
  imagePoints[0].push_back(Point2f(image_points[k],image_points[k+1]));

 CvSize imageSize;
 imageSize.height=image_height;
 imageSize.width=image_width;
 vector<Mat> rvecs;
 vector<Mat> tvecs;
    
    double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                    distCoeffs, rvecs, tvecs, flags|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
                    ///|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
	
 k=0;
 for(int i=0;i<3;i++)
  for(int j=0;j<3;j++)
	  icc[k]=cameraMatrix.at<double>(i, j);
	  
  //  printf("RMS error reported by calibrateCamera: %g\n", rms);
    
 //   bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
    
    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

 return totalAvgErr;	
}


extern "C" {

__declspec(dllexport) int __cdecl CameraCalibrateInit(int num_frames, int width, int height, int board_width, int board_height, double board_size, int num_cameras)
{
 NumFrames=num_frames;
 ImageWidth=width;
 ImageHeight=height;
 CurrentFrame=-1;
 memset(ICC,0,3*3*sizeof(double));
 memset(ECC,0,4*4*sizeof(double));
 memset(Distortion,0,8*sizeof(double)); 

 imageList.resize(0);
 imagePoints.resize(num_cameras);
 for(size_t i=0;i<imagePoints.size();i++)
  imagePoints[i].resize(0);

 cameraMatrix.resize(num_cameras);
 distCoeffs.resize(num_cameras);
 prevTimestamp.resize(num_cameras);
 views.resize(num_cameras);

 imageSize.height=height;
 imageSize.width=width;
 boardSize.width=board_width;
 boardSize.height=board_height;
 squareSize=board_size;
 totalAvgErr=0;
 return 0;
}

__declspec(dllexport) int __cdecl SetCameraCalibrationStepDelay(int value)
{
 if(value>=0)
  delay=value;
 return 0;
}

__declspec(dllexport) int __cdecl CameraCalibrationStep(unsigned char *imagedata, int camera_index)
{
 if(camera_index<0 || camera_index >=imagePoints.size())
  return 1;
//    namedWindow( "Image View", 1 );
        vector<Point2f> pointbuf;
		Mat viewGray(Size(ImageWidth, ImageHeight), CV_8UC1, imagedata, Mat::AUTO_STEP);
        cvtColor(viewGray, view, CV_GRAY2BGR); 
        bool found;
        switch( pattern )
        {
            case CHESSBOARD:
                found = findChessboardCorners( view, boardSize, pointbuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
                break;
            case CIRCLES_GRID:
                found = findCirclesGrid( view, boardSize, pointbuf );
                break;
            case ASYMMETRIC_CIRCLES_GRID:
                found = findCirclesGrid( view, boardSize, pointbuf, CALIB_CB_ASYMMETRIC_GRID );
                break;
            default:
                return -1;
        }

/*		if(!found)
		{
         imshow("Image View", view);		
		 return -2;
		}
		*/
       // improve the found corners' coordinate accuracy
        if( pattern == CHESSBOARD && found) cornerSubPix( viewGray, pointbuf, Size(11,11),
            Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

        if(found && clock() - prevTimestamp[camera_index] > delay*1e-3*CLOCKS_PER_SEC) 
//        if(found) 
        {
            imagePoints[camera_index].push_back(pointbuf);
            drawChessboardCorners( view, boardSize, Mat(pointbuf), found );
			views[camera_index]=view;
            prevTimestamp[camera_index] = clock();

			// «десь вычисл€ем ошибку репроекции на этом кадре, предварительно вычисл€€ внешнюю калибровку
	//		void cvFindExtrinsicCameraParams2(const CvMat* objectPoints, pointbuf, const CvMat* cameraMatrix, const CvMat* distCoeffs, CvMat* rvec, CvMat* tvec);
        }
		else
		 return -3;
        
/*
        string msg = mode == CAPTURING ? "100/100" :
            mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
        int baseLine = 0;
        Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);        
        Point textOrigin(view.cols - 2*textSize.width - 10, view.rows - 2*baseLine - 10);

        if( mode == CAPTURING )
        {
            if(undistortImage)
                msg = format( "%d/%d Undist", (int)imagePoints.size(), nframes );
            else
                msg = format( "%d/%d", (int)imagePoints.size(), nframes );
        }

        putText( view, msg, textOrigin, 1, 1,
                 mode != CALIBRATED ? Scalar(0,0,255) : Scalar(0,255,0));

        if( mode == CALIBRATED && undistortImage )
        {
            Mat temp = view.clone();
            undistort(temp, view, cameraMatrix, distCoeffs);
        }
		*/
 //       imshow("Image View", view);		
		return 0;
}

__declspec(dllexport) int __cdecl CameraCalibrationUndoStep(int camera_index)
{
 if(imagePoints[camera_index].size()>0)
  imagePoints[camera_index].resize(imagePoints[camera_index].size()-1);
 return 0;
}

__declspec(dllexport) int __cdecl CameraCalibrateComplete(double* icc, double* dist_coeffs, int* num_dist_coeffs, double* ecc, double &error, int camera_index)
{
 if(camera_index<0 || camera_index>=cameraMatrix.size())
  return 1;
            
 if( runAndSave(outputFilename, imagePoints[camera_index], imageSize, boardSize, pattern, squareSize, aspectRatio,
                       flags, cameraMatrix[camera_index], distCoeffs[camera_index], writeExtrinsics, writePoints))
  mode = CALIBRATED;
 else
  mode = DETECTION;

 int k=0;
 for(int i=0;i<3;i++)
  for(int j=0;j<3;j++)
	  icc[k++]=cameraMatrix[camera_index].at<double>(i, j);

 *num_dist_coeffs=distCoeffs[camera_index].rows;
 for(int i=0;i<*num_dist_coeffs;i++)
	 dist_coeffs[i]=distCoeffs[camera_index].at<double>(i,0);

 Mat rotation(3,3,CV_64F);
 cvRodrigues2(&rvecs[0].operator CvMat(),&rotation.operator CvMat());

 k=0;
 for(int i=0;i<3;i++)
 {
  for(int j=0;j<3;j++)
  {
   ecc[k++]=rotation.at<double>(i, j);
  }
   ecc[k++]=tvecs[0].at<double>(i, 0);
 }
  for(int j=0;j<3;j++)
  {
   ecc[k++]=0;
  }
  ecc[k++]=1;

 error=totalAvgErr;
   
 return 0;
}

__declspec(dllexport) int __cdecl StereoCalibrateInit(double* icc1, double* dist_coeffs1, double* icc2, double* dist_coeffs2, int num_dist_coeffs)
{
 cameraMatrix[0] = Mat::eye(3, 3, CV_64F);
 if( flags & CV_CALIB_FIX_ASPECT_RATIO )
  cameraMatrix[0].at<double>(0,0) = aspectRatio;
    
 distCoeffs[0] = Mat::zeros(8, 1, CV_64F);

 cameraMatrix[1] = Mat::eye(3, 3, CV_64F);
 if( flags & CV_CALIB_FIX_ASPECT_RATIO )
  cameraMatrix[1].at<double>(0,0) = aspectRatio;
    
 distCoeffs[1] = Mat::zeros(8, 1, CV_64F);

 int k=0;
 for(int i=0;i<3;i++)
  for(int j=0;j<3;j++)
  {
   cameraMatrix[0].at<double>(i, j)=icc1[k];
   cameraMatrix[1].at<double>(i, j)=icc2[k++];
  }

 for(int i=0;i<num_dist_coeffs;i++)
 {
  distCoeffs[0].at<double>(i,0)=dist_coeffs1[i];
  distCoeffs[1].at<double>(i,0)=dist_coeffs2[i];
 }
 return 0;
}


__declspec(dllexport) int __cdecl StereoCalibrateComplete(double* r, double* t, double &error)
{
    vector<vector<Point3f> > objectPoints(1);
    calcChessboardCorners(boardSize, squareSize, objectPoints[0], pattern);

    objectPoints.resize(imagePoints[0].size(),objectPoints[0]);

 Mat R, T, E, F;

 R = Mat::eye(3, 3, CV_64F);
 T = Mat::zeros(3, 1, CV_64F);

 double test_icc1[9],test_icc2[9];
 int k=0;
 for(int i=0;i<3;i++)
 {
  for(int j=0;j<3;j++)
  {
   test_icc1[k]=cameraMatrix[0].at<double>(i, j);
   test_icc2[k++]=cameraMatrix[1].at<double>(i, j);
  }
 }


 error=stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1], cameraMatrix[0], distCoeffs[0], cameraMatrix[1], distCoeffs[1],
                                     imageSize, R, T, E, F,
                                     TermCriteria(TermCriteria::COUNT+
                                         TermCriteria::EPS, 30, 1e-6),
                                     CV_CALIB_FIX_INTRINSIC);

 k=0;
 for(int i=0;i<3;i++)
 {
  for(int j=0;j<3;j++)
  {
   r[k++]=R.at<double>(i, j);
  }
   t[i]=T.at<double>(i,0);
 }

	// cvStereoCalibrate(objectPoints, const CvMat* imagePoints1, const CvMat* imagePoints2, const CvMat* pointCounts, CvMat* cameraMatrix1, CvMat* distCoeffs1, CvMat* cameraMatrix2, CvMat* distCoeffs2, CvSize imageSize, CvMat* R, CvMat* T, CvMat* E=0, CvMat* F=0, CvTermCriteria term_crit=cvTermCriteria( CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 30, 1e-6), int flags=CV_CALIB_FIX_INTRINSIC)ґ

 return 0;
}

__declspec(dllexport) unsigned char* __cdecl GetCalibrationBoardImage(int camera_index)
{
 if(camera_index<0 || camera_index>=cameraMatrix.size())
  return 0;

 return (unsigned char*)views[camera_index].data;
}

__declspec(dllexport) void __cdecl Undistortion(char *source, char *dest, int imagewidth, int imageheight, double IntMat[3][3], double DistCoeff[5][1], double NewIntMat[3][3], double alpha)
{
	/*
 int k, j;
 cv::Mat FRAME(imageheight,imagewidth,CV_8UC3), FRAME2(imageheight,imagewidth,CV_8UC3);
 cv::Mat cammatrix(3,3,CV_64FC1);
 cv::Mat distvec(5,1,CV_64FC1);
 memcpy(FRAME.data, source, 3*imagewidth*imageheight);
 for(k=0; k<3; k++)
  for(j=0; j<3; j++)
  { 
   cammatrix.at<double>(k,j) = IntMat[k][j];
  }
 for(k=0; k<5; k++)
  distvec.at<double>(k,0)=DistCoeff[k][0];
 
 undistort(FRAME, FRAME2, cammatrix, distvec);
 memcpy(dest, FRAME2.data, 3*imagewidth*imageheight);
 
 */
	
        cv::Mat FRAME(imageheight,imagewidth,CV_8UC3,(void*)source), FRAME2(imageheight,imagewidth,CV_8UC3,(void*)dest);
        cv::Mat cammatrix(3,3,CV_64FC1, (void*)(IntMat)), cammatrix2;
        cv::Mat distvec(5,1,CV_64FC1, (void*)(DistCoeff));
cv::Size sz(imagewidth, imageheight);
if ((alpha<=1)&&(alpha>=0)) cammatrix2=cv::getOptimalNewCameraMatrix(cammatrix, distvec, sz, alpha,sz,0, true);
else cammatrix2=cammatrix;

        undistort(FRAME, FRAME2, cammatrix, distvec, cammatrix2);
        memcpy(dest, FRAME2.data, 3*imageheight*imagewidth);
memcpy(NewIntMat, cammatrix2.data, 3*3*sizeof(double));

	
 /*
 cv::Mat FRAME(Size(imagewidth, imageheight), CV_8UC3, source, Mat::AUTO_STEP), FRAME2;
 cv::Mat cammatrix(3,3,CV_64FC1, (void*)(IntMat)), cammatrix2;
 cv::Mat distvec(5,1,CV_64FC1, (void*)(DistCoeff));
 cv::Size sz(imagewidth, imageheight);

 Mat map1, map2;
 cv::Rect rect;
 cv::initUndistortRectifyMap(cammatrix, distvec, Mat(),
      cv::getOptimalNewCameraMatrix(cammatrix, distvec, sz, alpha, sz, &rect),
      sz, CV_16SC2, map1, map2);
 cv::remap(FRAME, FRAME2, map1, map2, CV_INTER_LINEAR);
*newimagewidth=FRAME2.cols;
*newimageheight=FRAME2.rows;
        memcpy(dest, FRAME2.data, 3*(*newimagewidth)*(*newimageheight));
//memcpy(NewIntMat, cammatrix2.data, 3*3*sizeof(double));
//cvNamedWindow("Image View",1);
//imshow("Image View", FRAME2);*/
}


// ќсуществл€ет инициализацию поиска маркера, определени€ его пространственной ориентации и ошибки репроекции
__declspec(dllexport) void __cdecl CameraMarkerSearchInit(double *icc, double *dist_coeff, int camera_index)
{
 cv::Mat cammatrix(3,3,CV_64FC1,icc);
 cv::Mat distvec(5,1,CV_64FC1,dist_coeff);
 cameraMatrix[camera_index]=cammatrix;
 distCoeffs[camera_index]=distvec;
}

// ќсуществл€ет определение внешней калибровки по последнему найденому калибровочному маркеру
__declspec(dllexport) int __cdecl ExternalCalibrationStep(unsigned char *imagedata, double* ecc, double *avg_error, double *max_error, double *min_error, int camera_index)
{
 Mat rotation(3,3,CV_64F);
 Mat rvec, tvec;

 vector<Point2f> imagePoints2;
 //int totalPoints = 0;
 //double totalErr = 0;

 vector<Point2f> pointbuf;
 Mat viewGray(Size(ImageWidth, ImageHeight), CV_8UC1, imagedata, Mat::AUTO_STEP);
 cvtColor(viewGray, view, CV_GRAY2BGR); 
 bool found;
 switch( pattern )
 {
 case CHESSBOARD:
  found = findChessboardCorners( view, boardSize, pointbuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
 break;
 case CIRCLES_GRID:
  found = findCirclesGrid( view, boardSize, pointbuf );
 break;
 case ASYMMETRIC_CIRCLES_GRID:
  found = findCirclesGrid( view, boardSize, pointbuf, CALIB_CB_ASYMMETRIC_GRID );
 break;
 default:
  return 0;
 }

 if(!found)
 {
  *avg_error=100;
  *max_error=100;
  *min_error=100; 
  return 0;
 }

 drawChessboardCorners( view, boardSize, Mat(pointbuf), found ); 
 views[camera_index]=view;

 vector<Point3f> objectPoints;
 calcChessboardCorners(boardSize, squareSize, objectPoints, pattern);

 Mat objectPointsMat(objectPoints);
 /*for(size_t i=0;i<objectPoints.size();i++)
 {
  objectPointsMat.at<double>(i,0)=objectPoints[i].x;  
  objectPointsMat.at<double>(i,1)=objectPoints[i].y;  
  objectPointsMat.at<double>(i,1)=objectPoints[i].z;  
 }*/

 Mat imagePointsMat(pointbuf);
 /*for(size_t i=0;i<pointbuf.size();i++)
 {
  imagePointsMat.at<double>(i,0)=pointbuf[i].x;  
  imagePointsMat.at<double>(i,1)=objectPoints[i].y;  
 }*/
 
 cv::solvePnP(objectPointsMat,imagePointsMat,cameraMatrix,distCoeffs,rvec, tvec);
 
 projectPoints(Mat(objectPoints), rvecs, tvecs,
                      cameraMatrix[camera_index], distCoeffs[camera_index], imagePoints2);

 *avg_error=0;
 *min_error=100000;
 *max_error=0;
 for(size_t i=0;i<pointbuf.size();i++)
 {
  Point2f sub=pointbuf[i]-imagePoints2[i];
  
  double error=sub.x*sub.x+sub.y*sub.y;
  if(*min_error>error)
   *min_error=error;
  if(*max_error<error)
   *max_error=error;

   *avg_error+=error;
 }
 *avg_error/=pointbuf.size();
    
 cv::Rodrigues(rvec,rotation);

 int k=0;
 for(int i=0;i<3;i++)
 {
  for(int j=0;j<3;j++)
  {
   ecc[k++]=rotation.at<double>(i, j);
  }
   ecc[k++]=tvec.at<double>(i, 0);
 }
  for(int j=0;j<3;j++)
  {
   ecc[k++]=0;
  }
  ecc[k++]=1;

 return 1;
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

