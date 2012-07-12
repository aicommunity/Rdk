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
#ifndef CVMMATRIX_H
#define CVMMATRIX_H

#include "cxcore.h"
#include "../../Core/Math/MMatrix.h"

namespace RDK {

// Отправляет данные объекта MMatrix в CvMat
template<class T, int Rows, int Cols>
void operator >> (MMatrix<T,Rows,Cols> &source, CvMat* &target);

// Отправляет данные объекта CVMat в MMatrix
template<class T, int Rows, int Cols>
void operator << (MMatrix<T,Rows,Cols> &target, const CvMat *source);
}
//---------------------------------------------------------------------------

#endif

