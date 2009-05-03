/*-----------------------------------------------------------------------------

  Vector2.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------

  Functions for dealing with 2d (usually texture mapping) values.

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <float.h>
#include <math.h>
#include <gl\gl.h>

#include "glTypes.h"
#include "math.h"
#include "macro.h"

/*-----------------------------------------------------------------------------
                           
-----------------------------------------------------------------------------*/

GLvector2 glVectorNormalize (GLvector2 v)
{

  float length;

  length = glVectorLength (v);
  if (length < 0.000001f)
    return v;
  return v * (1.0f / length);

}

/*-----------------------------------------------------------------------------
                           
-----------------------------------------------------------------------------*/

float glVectorLength (GLvector2 v)
{

  return (float)sqrt (v.x * v.x + v.y * v.y);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector2 glVectorSinCos (float a)
{

  GLvector2      val;

  a *= DEGREES_TO_RADIANS;
  val.x = sinf (a);
  val.y = cosf (a);
  return val;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector2 glVector (float x, float y)
{

  GLvector2      val;

  val.x = x;
  val.y = y;
  return val;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector2 glVectorAdd (GLvector2 val1, GLvector2 val2)
{

  GLvector2      result;

  result.x = val1.x + val2.x;
  result.y = val1.y + val2.y;
  return result;

}


/*-----------------------------------------------------------------------------
                           
-----------------------------------------------------------------------------*/

GLvector2 glVectorInterpolate (GLvector2 v1, GLvector2 v2, float scalar)
{

  GLvector2 result;

  result.x = MathInterpolate (v1.x, v2.x, scalar);
  result.y = MathInterpolate (v1.y, v2.y, scalar);
  return result;

}  

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector2 glVectorSubtract (GLvector2 val1, GLvector2 val2)
{

  GLvector2      result;

  result.x = val1.x - val2.x;
  result.y = val1.y - val2.y;
  return result;

}

/*-----------------------------------------------------------------------------
+                           
-----------------------------------------------------------------------------*/

GLvector2 GLvector2::operator+ (const GLvector2& c)
{
  return glVector (x + c.x, y + c.y);
}

GLvector2 GLvector2::operator+ (const float& c)
{
  return glVector (x + c, y + c);
}

void GLvector2::operator+= (const GLvector2& c)
{
  x += c.x;
  y += c.y;
}

void GLvector2::operator+= (const float& c)
{
  x += c;
  y += c;
}

GLvector2 GLvector2::operator- (const GLvector2& c)
{
  return glVector (x - c.x, y - c.y);
}

GLvector2 GLvector2::operator- (const float& c)
{
  return glVector (x - c, y - c);
}

void GLvector2::operator-= (const GLvector2& c)
{
  x -= c.x;
  y -= c.y;
}

void GLvector2::operator-= (const float& c)
{
  x -= c;
  y -= c;
}

GLvector2 GLvector2::operator* (const GLvector2& c)
{
  return glVector (x * c.x, y * c.y);
}

GLvector2 GLvector2::operator* (const float& c)
{
  return glVector (x * c, y * c);
}

void GLvector2::operator*= (const GLvector2& c)
{
  x *= c.x;
  y *= c.y;
}

void GLvector2::operator*= (const float& c)
{
  x *= c;
  y *= c;
}

GLvector2 GLvector2::operator/ (const GLvector2& c)
{
  return glVector (x / c.x, y / c.y);
}

GLvector2 GLvector2::operator/ (const float& c)
{
  return glVector (x / c, y / c);
}

void GLvector2::operator/= (const GLvector2& c)
{
  x /= c.x;
  y /= c.y;
}

void GLvector2::operator/= (const float& c)
{
  x /= c;
  y /= c;
}

bool GLvector2::operator== (const GLvector2& c)
{
  if (x == c.x && y == c.y)
    return true;
  return false;
}