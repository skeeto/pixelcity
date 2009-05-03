/*-----------------------------------------------------------------------------

  Ini.cpp

  2009 Shamus Young


-------------------------------------------------------------------------------
  
  This takes various types of data and dumps them into a predefined ini file.

-----------------------------------------------------------------------------*/

#define FORMAT_VECTOR       "%f %f %f"
#define MAX_RESULT          256
#define FORMAT_FLOAT        "%1.2f"
#define INI_FILE            ".\\" APP ".ini"
#define SECTION             "Settings"

#include <windows.h>
#include <stdio.h>
#include "glTypes.h"

#include "ini.h"
#include "win.h"

static char                 result[MAX_RESULT];

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int IniInt (char* entry)
{

  int         result;

  result = GetPrivateProfileInt (SECTION, entry, 0, INI_FILE);
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void IniIntSet (char* entry, int val)
{

  char        buf[20];

  sprintf (buf, "%d", val);
  WritePrivateProfileString (SECTION, entry, buf, INI_FILE);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

float IniFloat (char* entry)
{

  float     f;

  GetPrivateProfileString (SECTION, entry, "", result, MAX_RESULT, INI_FILE);
  f = (float)atof (result);
  return f;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void IniFloatSet (char* entry, float val)
{

  char        buf[20];
  
  sprintf (buf, FORMAT_FLOAT, val);
  WritePrivateProfileString (SECTION, entry, buf, INI_FILE);

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

char* IniString (char* entry)
{

  GetPrivateProfileString (SECTION, entry, "", result, MAX_RESULT, INI_FILE);
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void IniStringSet (char* entry, char* val)
{

  WritePrivateProfileString (SECTION, entry, val, INI_FILE);

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void IniVectorSet (char* entry, GLvector v)
{
  
  sprintf (result, FORMAT_VECTOR, v.x, v.y, v.z);
  WritePrivateProfileString (SECTION, entry, result, INI_FILE);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector IniVector (char* entry)
{

  GLvector  v;

  v.x = v.y = v.z = 0.0f;
  GetPrivateProfileString (SECTION, entry, "0 0 0", result, MAX_RESULT, INI_FILE);
  sscanf (result, FORMAT_VECTOR, &v.x, &v.y, &v.z);
  return v;

}
