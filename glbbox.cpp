/*-----------------------------------------------------------------------------

  glBbox.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------
  
  This module has a few functions useful for manipulating the bounding-box 
  structs.

-----------------------------------------------------------------------------*/

#define MAX_VALUE               999999999999999.9f

#include <math.h>

#include "macro.h"
#include "glTypes.h"

/*-----------------------------------------------------------------------------
Does the given point fall within the given Bbox?
-----------------------------------------------------------------------------*/

bool glBboxTestPoint (GLbbox box, GLvector point)
{

  if (point.x > box.max.x || point.x < box.min.x)
    return false;
  if (point.y > box.max.y || point.y < box.min.y)
    return false;
  if (point.z > box.max.z || point.z < box.min.z)
    return false;
  return true;

}

/*-----------------------------------------------------------------------------
Expand Bbox (if needed) to contain given point
-----------------------------------------------------------------------------*/

GLbbox glBboxContainPoint (GLbbox box, GLvector point)
{

  box.min.x = MIN (box.min.x, point.x);
  box.min.y = MIN (box.min.y, point.y);
  box.min.z = MIN (box.min.z, point.z);
  box.max.x = MAX (box.max.x, point.x);
  box.max.y = MAX (box.max.y, point.y);
  box.max.z = MAX (box.max.z, point.z);
  return box;
  
}

/*-----------------------------------------------------------------------------
This will invalidate the bbox. 
-----------------------------------------------------------------------------*/

GLbbox glBboxClear (void)
{

  GLbbox      result;

  result.max = glVector (-MAX_VALUE, -MAX_VALUE, -MAX_VALUE);
  result.min = glVector ( MAX_VALUE,  MAX_VALUE,  MAX_VALUE);
  return result;

}


