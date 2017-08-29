#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <stdio.h>
#include <iostream.h>
#include <conio.h>
#include "MDMatrix.h"


 int _tmain(int argc, _TCHAR* argv[])
{
 double mat_init[3][3]={{1,2,3},{4,5,6},{7,8,9}};
 double *pmat_init=&mat_init[0][0];
 RDK::MDMatrix<double> m,m2;

 m.Assign(3,3,pmat_init);
 m.Print(cout);
 cout<<endl;

 m2=m;
 m2.Resize(3,4,0.0);
 m2.Print(cout);

 m2=m;
 m2.Resize(4,3,0.0);
 m2.Print(cout);

 m2=m;
 m2.Resize(4,4,0.0);
 m2.Print(cout);

 m2=m;
 m2.Resize(3,2,0.0);
 m2.Print(cout);

 m2=m;
 m2.Resize(2,2,0.0);
 m2.Print(cout);

 m2=m;
 m2.Resize(3,6,0.0);
 m2.Print(cout);

 m2=m;
 m2.Resize(2,7,0.0);
 m2.Print(cout);

 m2=m;
 m2.Resize(30,3,0.0);
 m2.Print(cout);

 getch();
	return 0;
}
