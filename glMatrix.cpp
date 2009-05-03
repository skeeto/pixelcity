/*-----------------------------------------------------------------------------

  glMatrix.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------
  
  Functions useful for manipulating the Matrix struct

-----------------------------------------------------------------------------*/


#define M(e,x,y)                (e.elements[x][y])

/*** Order type constants, constructors, extractors ***/

    /* There are 24 possible conventions, designated by:    */
    /*	  o EulAxI = axis used initially		    */
    /*	  o EulPar = parity of axis permutation		    */
    /*	  o EulRep = repetition of initial axis as last	    */
    /*	  o EulFrm = frame from which axes are taken	    */
    /* Axes I,J,K will be a permutation of X,Y,Z.	    */
    /* Axis H will be either I or K, depending on EulRep.   */
    /* Frame S takes axes from initial static frame.	    */
    /* If ord = (AxI=X, Par=Even, Rep=No, Frm=S), then	    */
    /* {a,b,c,ord} means Rz(c)Ry(b)Rx(a), where Rz(c)v	    */
    /* rotates v around Z by c radians.			    */

#define EulFrmS	     0
#define EulFrmR	     1
#define EulFrm(ord)  ((unsigned)(ord)&1)
#define EulRepNo     0
#define EulRepYes    1
#define EulRep(ord)  (((unsigned)(ord)>>1)&1)
#define EulParEven   0
#define EulParOdd    1
#define EulPar(ord)  (((unsigned)(ord)>>2)&1)
#define EulSafe	     "\000\001\002\000"
#define EulNext	     "\001\002\000\001"
#define EulAxI(ord)  ((int)(EulSafe[(((unsigned)(ord)>>3)&3)]))
#define EulAxJ(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)==EulParOdd)]))
#define EulAxK(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)!=EulParOdd)]))
#define EulAxH(ord)  ((EulRep(ord)==EulRepNo)?EulAxK(ord):EulAxI(ord))
    /* EulGetOrd unpacks all useful information about order simultaneously. */
#define EulGetOrd(ord,i,j,k,h,n,s,f) {unsigned o=ord;f=o&1;o>>=1;s=o&1;o>>=1;\
    n=o&1;o>>=1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];h=s?k:i;}
    /* EulOrd creates an order value between 0 and 23 from 4-tuple choices. */
#define EulOrd(i,p,r,f)	   (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))
    /* Static axes */
#define EulOrdXYZs    EulOrd(X,EulParEven,EulRepNo,EulFrmS)
#define EulOrdXYXs    EulOrd(X,EulParEven,EulRepYes,EulFrmS)
#define EulOrdXZYs    EulOrd(X,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdXZXs    EulOrd(X,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdYZXs    EulOrd(Y,EulParEven,EulRepNo,EulFrmS)
#define EulOrdYZYs    EulOrd(Y,EulParEven,EulRepYes,EulFrmS)
#define EulOrdYXZs    EulOrd(Y,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdYXYs    EulOrd(Y,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdZXYs    EulOrd(Z,EulParEven,EulRepNo,EulFrmS)
#define EulOrdZXZs    EulOrd(Z,EulParEven,EulRepYes,EulFrmS)
#define EulOrdZYXs    EulOrd(Z,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdZYZs    EulOrd(Z,EulParOdd,EulRepYes,EulFrmS)
    /* Rotating axes */
#define EulOrdZYXr    EulOrd(X,EulParEven,EulRepNo,EulFrmR)
#define EulOrdXYXr    EulOrd(X,EulParEven,EulRepYes,EulFrmR)
#define EulOrdYZXr    EulOrd(X,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdXZXr    EulOrd(X,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdXZYr    EulOrd(Y,EulParEven,EulRepNo,EulFrmR)
#define EulOrdYZYr    EulOrd(Y,EulParEven,EulRepYes,EulFrmR)
#define EulOrdZXYr    EulOrd(Y,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdYXYr    EulOrd(Y,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdYXZr    EulOrd(Z,EulParEven,EulRepNo,EulFrmR)
#define EulOrdZXZr    EulOrd(Z,EulParEven,EulRepYes,EulFrmR)
#define EulOrdXYZr    EulOrd(Z,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdZYZr    EulOrd(Z,EulParOdd,EulRepYes,EulFrmR)

#include <math.h>
#include <float.h>

#include "macro.h"
#include "glTypes.h"

static float      identity[4][4] = 
{
  {1.0f, 0.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 0.0f, 1.0f},
};


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void* glMatrixCreate (void)
{

  GLmatrix*       m;
  int             x;
  int             y;

  m = new GLmatrix;
  for (x = 0; x < 4; x++) {
    for (y = 0; y < 4; y++) {
      m -> elements[x][y] = identity[x][y];
    }
  }
  return (void*)m;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLmatrix glMatrixIdentity (void)
{

  GLmatrix        m;
  int             x;
  int             y;

  for (x = 0; x < 4; x++) {
    for (y = 0; y < 4; y++) {
      M(m, x, y) = identity[x][y];
    }
  }
  return m;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void glMatrixElementsSet (GLmatrix* m, float* in)
{

  m -> elements[0][0] = in[0];
  m -> elements[0][1] = in[1];
  m -> elements[0][2] = in[2];
  m -> elements[0][3] = in[3];

  m -> elements[1][0] = in[4];
  m -> elements[1][1] = in[5];
  m -> elements[1][2] = in[6];
  m -> elements[1][3] = in[7];

  m -> elements[2][0] = in[8];
  m -> elements[2][1] = in[9];
  m -> elements[2][2] = in[10];
  m -> elements[2][3] = in[11];
  
  m -> elements[3][0] = in[12];
  m -> elements[3][1] = in[13];
  m -> elements[3][2] = in[14];
  m -> elements[3][3] = in[15];

}

/*---------------------------------------------------------------------------
A matrix multiplication (dot product) of two 4x4 matrices.
---------------------------------------------------------------------------*/

GLmatrix glMatrixMultiply (GLmatrix a, GLmatrix b)
{

  GLmatrix        result;
  
  M(result, 0,0) = M(a, 0,0) * M(b, 0, 0) + M(a, 1,0) * M(b, 0, 1) + M(a, 2,0) * M(b, 0, 2);
  M(result, 1,0) = M(a, 0,0) * M(b, 1, 0) + M(a, 1,0) * M(b, 1, 1) + M(a, 2,0) * M(b, 1, 2);
  M(result, 2,0) = M(a, 0,0) * M(b, 2, 0) + M(a, 1,0) * M(b, 2, 1) + M(a, 2,0) * M(b, 2, 2);
  M(result, 3,0) = M(a, 0,0) * M(b, 3, 0) + M(a, 1,0) * M(b, 3, 1) + M(a, 2,0) * M(b, 3, 2) + M(a, 3,0);
  
  M(result, 0,1) = M(a, 0,1) * M(b, 0, 0) + M(a, 1,1) * M(b, 0, 1) + M(a, 2,1) * M(b, 0, 2);
  M(result, 1,1) = M(a, 0,1) * M(b, 1, 0) + M(a, 1,1) * M(b, 1, 1) + M(a, 2,1) * M(b, 1, 2);
  M(result, 2,1) = M(a, 0,1) * M(b, 2, 0) + M(a, 1,1) * M(b, 2, 1) + M(a, 2,1) * M(b, 2, 2);
  M(result, 3,1) = M(a, 0,1) * M(b, 3, 0) + M(a, 1,1) * M(b, 3, 1) + M(a, 2,1) * M(b, 3, 2) + M(a, 3,1);

  M(result, 0,2) = M(a, 0,2) * M(b, 0, 0) + M(a, 1,2) * M(b, 0, 1) + M(a, 2,2) * M(b, 0, 2);
  M(result, 1,2) = M(a, 0,2) * M(b, 1, 0) + M(a, 1,2) * M(b, 1, 1) + M(a, 2,2) * M(b, 1, 2);
  M(result, 2,2) = M(a, 0,2) * M(b, 2, 0) + M(a, 1,2) * M(b, 2, 1) + M(a, 2,2) * M(b, 2, 2);
  M(result, 3,2) = M(a, 0,2) * M(b, 3, 0) + M(a, 1,2) * M(b, 3, 1) + M(a, 2,2) * M(b, 3, 2) + M(a, 3,2);
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector glMatrixTransformPoint (GLmatrix m, GLvector in)
{

  GLvector              out;

  out.x = M(m,0,0) * in.x + M(m,1,0) * in.y + M(m,2,0) * in.z + M(m,3,0);
  out.y = M(m,0,1) * in.x + M(m,1,1) * in.y + M(m,2,1) * in.z + M(m,3,1);
  out.z = M(m,0,2) * in.x + M(m,1,2) * in.y + M(m,2,2) * in.z + M(m,3,2);
  return out;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLmatrix glMatrixTranslate (GLmatrix m, GLvector in)
{

  GLvector  old;

  old.x = M(m,3,0);
  old.y = M(m,3,1);
  old.z = M(m,3,2);
  M(m, 3, 0) = 0.0f;
  M(m, 3, 1) = 0.0f;
  M(m, 3, 2) = 0.0f;
  in = glMatrixTransformPoint (m, in);
  M(m, 3, 0) = old.x;
  M(m, 3, 1) = old.y;
  M(m, 3, 2) = old.z;
  M(m,3,0) += in.x;
  M(m,3,1) += in.y;
  M(m,3,2) += in.z;
  return m;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLmatrix glMatrixRotate (GLmatrix m, float theta, float x, float y, float z)
{

  GLmatrix              r;
  float                 length;
  float                 s, c, t;
  GLvector              in;

  theta *= DEGREES_TO_RADIANS;
  r = glMatrixIdentity ();
  length = (float)sqrt (x * x + y * y + z * z); 
  if (length < 0.00001f)
    return m;
  x /= length;
  y /= length;
  z /= length;
  s = (float)sin (theta);
  c = (float)cos (theta);
  t = 1.0f - c;  
 
  in.x = in.y = in.z = 1.0f;
  M(r, 0,0) = t*x*x + c;
  M(r, 1,0) = t*x*y - s*z;
  M(r, 2,0) = t*x*z + s*y;
  M(r, 3,0) = 0;

  M(r, 0,1) = t*x*y + s*z;
  M(r, 1,1) = t*y*y + c;
  M(r, 2,1) = t*y*z - s*x;
  M(r, 3,1) = 0;

  M(r, 0,2) = t*x*z - s*y;
  M(r, 1,2) = t*y*z + s*x;
  M(r, 2,2) = t*z*z + c;
  M(r, 3,2) = 0;

  m = glMatrixMultiply (m, r);
  return m;

}

/* Convert matrix to Euler angles (in radians). */
GLvector glMatrixToEuler (GLmatrix mat, int order)
{
  GLvector    ea;
  int         i,j,k,h,n,s,f;

  EulGetOrd (order,i,j,k,h,n,s,f);
  if (s==EulRepYes) {
	  float sy = (float)sqrt(mat.elements[i][j]*mat.elements[i][j] + mat.elements[i][k]*mat.elements[i][k]);
	  if (sy > 16 * FLT_EPSILON) {
	      ea.x = (float)atan2(mat.elements[i][j], mat.elements[i][k]);
	      ea.y = (float)atan2(sy, mat.elements[i][i]);
	      ea.z = (float)atan2(mat.elements[j][i], -mat.elements[k][i]);
	  } else {
	      ea.x = (float)atan2(-mat.elements[j][k], mat.elements[j][j]);
	      ea.y = (float)atan2(sy, mat.elements[i][i]);
	      ea.z = 0;
	  }
  } else {
	  float cy = (float)sqrt(mat.elements[i][i]*mat.elements[i][i] + mat.elements[j][i]*mat.elements[j][i]);
	  if (cy > 16*FLT_EPSILON) {
	      ea.x = (float)atan2(mat.elements[k][j], mat.elements[k][k]);
	      ea.y = (float)atan2(-mat.elements[k][i], cy);
	      ea.z = (float)atan2(mat.elements[j][i], mat.elements[i][i]);
	  } else {
	      ea.x = (float)atan2(-mat.elements[j][k], mat.elements[j][j]);
	      ea.y = (float)atan2(-mat.elements[k][i], cy);
	      ea.z = 0;
	  }
  }
  if (n==EulParOdd) {
    ea.x = -ea.x;  
    ea.y = - ea.y; 
    ea.z = -ea.z;
  }
  if (f==EulFrmR) {
    float t = ea.x; 
    ea.x = ea.z; 
    ea.z = t;
  }
  //ea.w = order;
  return (ea);
}
