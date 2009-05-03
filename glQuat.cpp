/*-----------------------------------------------------------------------------

  glQuat.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------

  Functions for dealing with Quaternions

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <float.h>
#include <math.h>
#include <gl\gl.h>

#include "math.h"
#include "glTypes.h"

enum QuatPart {X, Y, Z, W};

/*-----------------------------------------------------------------------------
                           
-----------------------------------------------------------------------------*/

GLquat glQuat (float x, float y, float z, float w)
{

  GLquat result;

  result.x = x;
  result.y = y;
  result.z = z;
  result.w = w;
  return result;

}


/* Convert quaternion to Euler angles (in radians). */
/*
EulerAngles Eul_FromQuat(Quat q, int order)
{
    HMatrix M;
    double Nq = q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
    double s = (Nq > 0.0) ? (2.0 / Nq) : 0.0;
    double xs = q.x*s,	  ys = q.y*s,	 zs = q.z*s;
    double wx = q.w*xs,	  wy = q.w*ys,	 wz = q.w*zs;
    double xx = q.x*xs,	  xy = q.x*ys,	 xz = q.x*zs;
    double yy = q.y*ys,	  yz = q.y*zs,	 zz = q.z*zs;
    M[X][X] = 1.0 - (yy + zz); M[X][Y] = xy - wz; M[X][Z] = xz + wy;
    M[Y][X] = xy + wz; M[Y][Y] = 1.0 - (xx + zz); M[Y][Z] = yz - wx;
    M[Z][X] = xz - wy; M[Z][Y] = yz + wx; M[Z][Z] = 1.0 - (xx + yy);
    M[W][X]=M[W][Y]=M[W][Z]=M[X][W]=M[Y][W]=M[Z][W]=0.0; M[W][W]=1.0;
    return (Eul_FromHMatrix(M, order));
}
*/

GLvector glQuatToEuler (GLquat q, int order)
{
    GLmatrix M;

    float Nq = q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
    float s = (Nq > 0.0f) ? (2.0f / Nq) : 0.0f;
    float xs = q.x*s,	  ys = q.y*s,	 zs = q.z*s;
    float wx = q.w*xs,	  wy = q.w*ys,	 wz = q.w*zs;
    float xx = q.x*xs,	  xy = q.x*ys,	 xz = q.x*zs;
    float yy = q.y*ys,	  yz = q.y*zs,	 zz = q.z*zs;
    M.elements[X][X] = 1.0f - (yy + zz); M.elements[X][Y] = xy - wz;         M.elements[X][Z] = xz + wy;
    M.elements[Y][X] = xy + wz;         M.elements[Y][Y] = 1.0f - (xx + zz); M.elements[Y][Z] = yz - wx;
    M.elements[Z][X] = xz - wy;         M.elements[Z][Y] = yz + wx;         M.elements[Z][Z] = 1.0f - (xx + yy);
    M.elements[W][X] = M.elements[W][Y] = 
      M.elements[W][Z] = M.elements[X][W] = 
      M.elements[Y][W] = M.elements[Z][W] = 0.0f; 
    M.elements[W][W] = 1.0f;
    return (glMatrixToEuler(M, order));
}
