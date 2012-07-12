/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2012.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru
Version:        1.0.0

This file - part of the project: RDK

File License:        BSD License
Project License:    BSD License
See file license.txt for more information
*********************************************************** */
#ifndef CVMMATRIX_CPP
#define CVMMATRIX_CPP

namespace RDK {

// Отправляет данные объекта MMatrix в CvMat
template<class T, int Rows, int Cols>
void operator >> (MMatrix<T,Rows,Cols> &source, CvMat* &target)
{     
 int type;
 if(source.Data1D[0].typeid().name() == float.typeid().name())
  type=CV_32FC1;
 else
 if(source.Data1D[0].typeid().name() == double.typeid().name())
  type=CV_64FC1;
 else
 if(source.Data1D[0].typeid().name() == int.typeid().name())
  type=CV_32SC1;
 else
  type=0;
    
 if(target && (target->rows != source.GetRows() ||
                target->cols != source.GetCols() ||
                (type != target.type)))
 {
    cvReleaseMat( &target );
    target=0;
 }

 if(!target)
  target=cvCreateMat(Rows, Cols, type);

 memcpy(target->data.ptr,source.Data1D,Rows*Cols*sizeof(T));
}

// Отправляет данные объекта CVMat в MMatrix
template<class T, int Rows, int Cols>
void operator << (MMatrix<T,Rows,Cols> &target, const CvMat *source)
{         
 if(!source)
  return;

 if(Rows != source->rows || Cols != source->cols)
  return;

 int type;
 if(target.Data1D[0].typeid().name() == float.typeid().name())
  type=CV_32FC1;
 else
 if(target.Data1D[0].typeid().name() == double.typeid().name())
  type=CV_64FC1;
 else
 if(target.Data1D[0].typeid().name() == int.typeid().name())
  type=CV_32SC1;
 else
  type=0;

 if(source->type != type)
  return;
 memcpy(target.Data1D, source->ptr,Rows*Cols*sizeof(T));
}

}
#endif

