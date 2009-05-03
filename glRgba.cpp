/*-----------------------------------------------------------------------------

  glRgba.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------

  Functions for dealing with RGBA color values.

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>
#include <math.h>

#include "math.h"
#include "glTypes.h"
#include "macro.h"

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaFromHsl (float h, float sl, float l)
{
  
  float v;
  float r,g,b;
  
  
  r = l;   // default to gray
  g = l;
  b = l;
  v = (l <= 0.5f) ? (l * (1.0f + sl)) : (l + sl - l * sl);
  if (v > 0)  {
    float m;
    float sv;
    int sextant;
    float fract, vsf, mid1, mid2;
   
    m = l + l - v;
    sv = (v - m ) / v;
    h *= 6.0f;
    sextant = (int)h;
    fract = h - sextant;
    vsf = v * sv * fract;
    mid1 = m + vsf;
    mid2 = v - vsf;
    switch (sextant) {
    case 0:
      r = v;  g = mid1; b = m;
      break;
    case 1:
      r = mid2; g = v;  b = m;
      break;
    case 2:
      r = m;  g = v;  b = mid1;
      break;
    case 3:
      r = m; g = mid2; b = v;
      break;
    case 4:
      r = mid1; g = m; b = v;
      break;
    case 5:
      r = v;  g = m; b = mid2;
      break;
    }
  }
  return glRgba (r, g, b);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaInterpolate (GLrgba c1, GLrgba c2, float delta)
{

  GLrgba     result;

  result.red = MathInterpolate (c1.red, c2.red, delta);
  result.green = MathInterpolate (c1.green, c2.green, delta);
  result.blue = MathInterpolate (c1.blue, c2.blue, delta);
  result.alpha = MathInterpolate (c1.alpha, c2.alpha, delta);
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaAdd (GLrgba c1, GLrgba c2)
{

  GLrgba     result;

  result.red = c1.red + c2.red;
  result.green = c1.green + c2.green;
  result.blue = c1.blue + c2.blue;
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaSubtract (GLrgba c1, GLrgba c2)
{

  GLrgba     result;

  result.red = c1.red - c2.red;
  result.green = c1.green - c2.green;
  result.blue = c1.blue - c2.blue;
  return result;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaMultiply (GLrgba c1, GLrgba c2)
{

  GLrgba     result;

  result.red = c1.red * c2.red;
  result.green = c1.green * c2.green;
  result.blue = c1.blue * c2.blue;
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaScale (GLrgba c, float scale)
{

  c.red *= scale;
  c.green *= scale;
  c.blue *= scale;
  return c;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (char* string)
{

  long    color;
  char    buffer[10];
  char*   pound;
  GLrgba  result;

  strncmp (buffer, string, 10);
  if (pound = strchr (buffer, '#'))
    pound[0] = ' ';
  if (sscanf (string, "%x", &color) != 1)
	  return glRgba (0.0f);
  result.red = (float)GetBValue (color) / 255.0f;
  result.green = (float)GetGValue (color) / 255.0f;
  result.blue = (float)GetRValue (color) / 255.0f;
  result.alpha = 1.0f;
  return result;  

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (int red, int green, int blue)
{

  GLrgba     result;

  result.red = (float)red / 255.0f;
  result.green = (float)green / 255.0f;
  result.blue = (float)blue / 255.0f;
  result.alpha = 1.0f;
  return result;  

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (float red, float green, float blue)
{

  GLrgba     result;

  result.red = red;
  result.green = green;
  result.blue = blue;
  result.alpha = 1.0f;
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (float red, float green, float blue, float alpha)
{

  GLrgba     result;

  result.red = red;
  result.green = green;
  result.blue = blue;
  result.alpha = alpha;
  return result;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (long c)
{

  GLrgba     result;

  result.red = (float)GetRValue (c) / 255.0f;
  result.green = (float)GetGValue (c) / 255.0f;
  result.blue = (float)GetBValue (c) / 255.0f;
  result.alpha = 1.0f;
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (float luminance)
{

  GLrgba     result;

  result.red = luminance;
  result.green = luminance;
  result.blue = luminance;
  result.alpha = 1.0f;
  return result;

}

/*-----------------------------------------------------------------------------
Takes the given index and returns a "random" color unique for that index.
512 Unique values: #0 and #512 will be the same, as will #1 and #513, etc
Useful for visual debugging in some situations.
-----------------------------------------------------------------------------*/

GLrgba glRgbaUnique (int i)
{

  GLrgba    c;

  c.alpha = 1.0f;
  c.red   = 0.4f + ((i & 1) ? 0.2f : 0.0f) + ((i &  8) ? 0.3f : 0.0f) - ((i &  64) ? 0.3f : 0.0f);
  c.green = 0.4f + ((i & 2) ? 0.2f : 0.0f) + ((i & 32) ? 0.3f : 0.0f) - ((i & 128) ? 0.3f : 0.0f);
  c.blue  = 0.4f + ((i & 4) ? 0.2f : 0.0f) + ((i & 16) ? 0.3f : 0.0f) - ((i & 256) ? 0.3f : 0.0f);
  return c;

}

/*-----------------------------------------------------------------------------
  + operator                          
-----------------------------------------------------------------------------*/

GLrgba GLrgba::operator+ (const GLrgba& c)
{
  return glRgba (red + c.red, green + c.green, blue + c.blue, alpha);
}

GLrgba GLrgba::operator+ (const float& c)
{
  return glRgba (red + c, green + c, blue + c, alpha);
} 

void GLrgba::operator+= (const GLrgba& c)
{
  red += c.red;
  green += c.green;
  blue += c.blue;
}

void GLrgba::operator+= (const float& c)
{
  red += c;
  green += c;
  blue += c;
}

/*-----------------------------------------------------------------------------
  - operator                          
-----------------------------------------------------------------------------*/

GLrgba GLrgba::operator- (const GLrgba& c)
{
  return glRgba (red - c.red, green - c.green, blue - c.blue);
}

GLrgba GLrgba::operator- (const float& c)
{
  return glRgba (red - c, green - c, blue - c, alpha);
}

void GLrgba::operator-= (const GLrgba& c)
{
  red -= c.red;
  green -= c.green;
  blue -= c.blue;
}

void GLrgba::operator-= (const float& c)
{
  red -= c;
  green -= c;
  blue -= c;
}

/*-----------------------------------------------------------------------------
  * operator                          
-----------------------------------------------------------------------------*/

GLrgba GLrgba::operator* (const GLrgba& c)
{
  return glRgba (red * c.red, green * c.green, blue * c.blue);
}

GLrgba GLrgba::operator* (const float& c)
{
  return glRgba (red * c, green * c, blue * c, alpha);
}

void GLrgba::operator*= (const GLrgba& c)
{
  red *= c.red;
  green *= c.green;
  blue *= c.blue;
}

void GLrgba::operator*= (const float& c)
{
  red *= c;
  green *= c;
  blue *= c;
}

/*-----------------------------------------------------------------------------
  / operator                          
-----------------------------------------------------------------------------*/

GLrgba GLrgba::operator/ (const GLrgba& c)
{
  return glRgba (red / c.red, green / c.green, blue / c.blue);
}

GLrgba GLrgba::operator/ (const float& c)
{
  return glRgba (red / c, green / c, blue / c, alpha);
}

void GLrgba::operator/= (const GLrgba& c)
{
  red /= c.red;
  green /= c.green;
  blue /= c.blue;
}

void GLrgba::operator/= (const float& c)
{
  red /= c;
  green /= c;
  blue /= c;
}

bool GLrgba::operator==  (const GLrgba& c)
{
  return (red == c.red && green == c.green && blue == c.blue);
}