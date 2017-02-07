/*-----------------------------------------------------------------------------

  Math.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------
  
  Various useful math functions.

-----------------------------------------------------------------------------*/

#include <math.h>

#include "macro.h"
#include "math.h"

/*-----------------------------------------------------------------------------
Keep an angle between 0 and 360
-----------------------------------------------------------------------------*/

float MathAngle (float angle)
{

  if (angle < 0.0f) 
    angle = 360.0f - (float)fmod (fabs (angle), 360.0f);
  else
    angle = (float)fmod (angle, 360.0f);
  return angle;

}

/*-----------------------------------------------------------------------------
Get an angle between two given points on a grid
-----------------------------------------------------------------------------*/

float MathAngle (float x1, float y1, float x2, float y2)
{

  float   x_delta;
  float   z_delta;
  float   angle;

  z_delta = (y1 - y2);
  x_delta = (x1 - x2); 
  if (x_delta == 0) {
    if (z_delta > 0)
      return 0.0f;
    else
      return 180.0f;
  }
  if (fabs (x_delta) < fabs (z_delta)) {
    angle = 90 - (float)atan (z_delta / x_delta) * RADIANS_TO_DEGREES;
    if (x_delta < 0)
      angle -= 180.0f;
  } else {
    angle = (float)atan (x_delta / z_delta) * RADIANS_TO_DEGREES;
    if (z_delta < 0.0f)
      angle += 180.0f;
  }
  if (angle< 0.0f)
    angle += 360.0f;
  return angle;

}

/*-----------------------------------------------------------------------------
Get distance (squared) between 2 points on a plane
-----------------------------------------------------------------------------*/

float MathDistance2 (float x1, float y1, float x2, float y2)
{

  float     dx;
  float     dy;

  dx = x1 - x2;
  dy = y1 - y2;
  return dx * dx + dy * dy;

}

/*-----------------------------------------------------------------------------
Get distance between 2 points on a plane. This is slightly slower than 
MathDistance2 ()
-----------------------------------------------------------------------------*/

float MathDistance (float x1, float y1, float x2, float y2)
{

  float     dx;
  float     dy;

  dx = x1 - x2;
  dy = y1 - y2;
  return (float)sqrt (dx * dx + dy * dy);

}

/*-----------------------------------------------------------------------------
difference between two angles
-----------------------------------------------------------------------------*/

float MathAngleDifference (float a1, float a2)

{

  float         result;

  result = (float)fmod (a1 - a2, 360.0f);
  if (result > 180.0)
    return result - 360.0F;
  if (result < -180.0)
    return result + 360.0F;
  return result;

}

/*-----------------------------------------------------------------------------
interpolate between two values
-----------------------------------------------------------------------------*/

float MathInterpolate (float n1, float n2, float delta)
{

  return n1 * (1.0f - delta) + n2 * delta;

}

/*-----------------------------------------------------------------------------
return a scalar of 0.0 to 1.0, based an the given values position within a range
-----------------------------------------------------------------------------*/

float MathSmoothStep (float val, float a, float b)
{

  if (b == a)
    return 0.0f;
  val -= a;
  val /= (b - a);
  return CLAMP (val, 0.0f, 1.0f);

}

/*-----------------------------------------------------------------------------
Average two values
-----------------------------------------------------------------------------*/

float MathAverage (float n1, float n2)
{

  return (n1 + n2) / 2.0f;

}

/*-----------------------------------------------------------------------------
  This will take linear input values from 0.0 to 1.0 and convert them to 
  values along a curve.  This could also be acomplished with sin (), but this 
  way avoids converting to radians and back.
-----------------------------------------------------------------------------*/

float MathScalarCurve (float val)
{
 
  float   sign;

  val = (val - 0.5f) * 2.0f;
  if (val < 0.0f)
    sign = -1.0f;
  else 
    sign = 1.0f;
  if (val < 0.0f) 
    val = -val;
  val = 1.0f - val;
  val *= val;
  val = 1.0f - val;
  val *= sign;
  val = (val + 1.0f) / 2.0f;
  return val;

}