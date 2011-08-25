/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MUIOStreamH
#define MUIOStreamH

#include <iostream>
#include "MVector.h"
#include "MDyad.h"
#include "MTensor.h"
#include "MTheormec.h"
#include "MBody.h"
#include "MGeometry.h"
#include "MCSystem.h"
#include "../Serialize/UIOStream.h"

namespace RDK {

//namespace IO {

// Вывод MVector в поток
template<typename CharT, typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MVector<T> &data)
{
 stream<<"{"<<(T)data.x<<","<<(T)data.y<<","<<(T)data.z<<"}";
 return stream;
}

// Ввод из потока
template<typename CharT, typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MVector<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>data.x;
 ch=stream.get();
 stream>>data.y;
 ch=stream.get();
 stream>>data.z;
 ch=stream.get();
 return stream;
}


// MDyad
template<typename CharT, typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MDyad<T> &data)
{
 stream<<"{"<<data.d1<<","<<data.d2<<"}";
 return stream;
}

template<typename CharT, typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MDyad<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>data.d1;
 ch=stream.get();
 stream>>data.y;
 ch=stream.get();
 return stream;
}

// MTensor
template<typename CharT, typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MTensor<T> &data)
{
 stream<<data.Dyads;
 return stream;
}


template<typename CharT, typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MTensor<T> &data)
{
 stream>>data.Dyads;
 return stream;
}

// Theormec
template<typename CharT, typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MInertiaTensor<T> &data)
{
 stream<<"{"<<data.d1<<","<<data.d2<<","<<data.d3<<","<<data.m1<<","<<data.m2<<","<<data.m3<<"}";
 return stream;
}


template<typename CharT, typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MInertiaTensor<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>data.d1;
 ch=stream.get();
 stream>>data.d2;
 ch=stream.get();
 stream>>data.d3;
 ch=stream.get();
 stream>>data.m1;
 ch=stream.get();
 stream>>data.m2;
 ch=stream.get();
 stream>>data.m3;
 ch=stream.get();
 return stream;
}

// MRotationTensor
template<typename CharT, typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MRotationTensor<T> &data)
{
 stream<<"{"<<data.m<<","<<data.angle<<"}";
 return stream;
}


template<typename CharT, typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MRotationTensor<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>data.m;
 ch=stream.get();
 stream>>data.angle;
 ch=stream.get();
 return stream;
}

// MBody
template<typename CharT,typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MBody<T> &data)
{
 stream<<"{"<<data.Location<<"}";
 return stream;
}

template<typename CharT,typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MBody<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>data.Location;
 ch=stream.get();
 return stream;
}

// MKinematicBody
template<typename CharT,typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MKinematicBody<T> &data)
{
 stream<<"{"<<static_cast<const MBody<T>&>(data)<<","<<data.Rotation<<","<<data.TranslationSpeed<<","
  <<data.AngleSpeed<<"}";
 return stream;
}

// Ввод из потока
template<typename CharT,typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MKinematicBody<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>static_cast<MBody<T>&>(data);
 ch=stream.get();
 stream>>data.Rotation;
 ch=stream.get();
 stream>>data.TranslationSpeed;
 ch=stream.get();
 stream>>data.AngleSpeed;
 ch=stream.get();
 return stream;
}

// MechanicalBody
template<typename CharT,typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MMechanicalBody<T> &data)
{
 stream<<"{"<<static_cast<const MKinematicBody<T> >(data)<<",";
 stream<<data.MassCenter<<","<<data.Mass<<","<<data.CIT<<"}";
 return stream;
}

template<typename CharT,typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MMechanicalBody<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>static_cast<MKinematicBody<T>&>(data);
 ch=stream.get();
 stream>>data.MassCenter;
 ch=stream.get();
 stream>>data.Mass;
 ch=stream.get();
 stream>>data.CIT;
 ch=stream.get();
 return stream;
}

// MRay
template<typename CharT, typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MRay<T> &data)
{
 stream<<"{"<<data.Origin<<","<<data.Direction<<"}";
 return stream;
}


template<typename CharT, typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MRay<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>data.Origin;
 ch=stream.get();
 stream>>data.Direction;
 ch=stream.get();
}

// MPlane
template<typename CharT, typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MPlane<T> &data)
{
 stream<<"{"<<data.Normal<<","<<data.Distance<<"}";
 return stream;
}


template<typename CharT, typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MPlane<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>data.Normal;
 ch=stream.get();
 stream>>data.Distance;
 ch=stream.get();
}

// MBorder
template<typename CharT>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MBorder &data)
{
 stream<<data.GetVertexIndex();
 return stream;
}


template<typename CharT>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MBorder &data)
{
 std::vector<int> temp;

 stream>>temp;
 data=temp;
 return stream;
}

// MVertex
template<typename CharT,typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MVertex<T> &data)
{
 stream<<"{"<<data.GetVertex()<<","<<data.GetNames()<<"}";
 return stream;
}


template<typename CharT,typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MVertex<T> &data)
{
 std::vector<MVector<T> > temp;
 std::vector<std::string> temp2;

 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>temp;
 ch=stream.get();
 stream>>temp2;
 ch=stream.get();

 data=temp;
 data=temp2;
 return stream;
}

// MGeometry
template<typename CharT,typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MGeometry<T> &data)
{
 stream<<"{"<<data.GetVertex()<<","<<data.GetBorders()<<"}";
 return stream;
}


template<typename CharT,typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MGeometry<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 std::vector<MVector<T> > vtemp;
 std::vector<MBorder> btemp;

 stream>>vtemp;
 ch=stream.get();
 stream>>btemp;
 ch=stream.get();

 data.SetVertex(vtemp);
 data.SetBorders(btemp);
 return stream;
}

// MCSystem
template<typename CharT,typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MCSystem<T> &data)
{
 stream<<"{"<<data.Location<<data.Basis[0]<<","<<data.Basis[1]<<","<<data.Basis[2]<<"}";
 return stream;
}

template<typename CharT,typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MCSystem<T> &data)
{
 CharT ch;
 ch=stream.get();
 if(ch != '{')
  return stream;

 ch=stream.get();
 if(ch == '}')
  return stream;
 else
  stream.unget();

 stream>>data.Location;
 ch=stream.get();
 stream>>data.Basis[0];
 ch=stream.get();
 stream>>data.Basis[1];
 ch=stream.get();
 stream>>data.Basis[2];
 ch=stream.get();
 return stream;
}
        /*
// MCartesianCSystem
template<typename CharT,typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MCartesianCSystem<T> &data)
{
 operator << (stream,static_cast<const MCSystem<T> &>(data));
 return stream;
}

template<typename CharT,typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MCartesianCSystem<T> &data)
{
 operator >> (stream,static_cast<const MCSystem<T> &>(data));
 return stream;
}

// MEulerCSystem
template<typename CharT,typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MEulerCSystem<T> &data)
{
 operator << (stream,static_cast<const MCSystem<T> &>(data));
 return stream;
}

template<typename CharT,typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MEulerCSystem<T> &data)
{
 operator >> (stream,static_cast<const MCSystem<T> &>(data));
 return stream;
}

// MPolarCSystem
template<typename CharT,typename T>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const MPolarCSystem<T> &data)
{
 operator << (stream,static_cast<const MCSystem<T> &>(data));
 return stream;
}

template<typename CharT,typename T>
std::basic_istream<CharT>& operator >> (std::basic_istream<CharT>& stream, MPolarCSystem<T> &data)
{
 operator >> (stream,static_cast<const MCSystem<T> &>(data));
 return stream;
}                     */

//}
}
#endif

