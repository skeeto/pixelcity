/*-----------------------------------------------------------------------------

  Light.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------

  This tracks and renders the light sources. (Note that they do not really 
  CAST light in the OpenGL sense of the world, these are just simple panels.) 
  These are NOT subclassed to entities because these are dynamic.  Some lights 
  blink, and thus they can't go into the fixed render lists managed by 
  Entity.cpp.  

-----------------------------------------------------------------------------*/

#define MAX_SIZE            5

#include <windows.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "glTypes.h"

#include "camera.h"
#include "entity.h"
#include "light.h"
#include "macro.h"
#include "math.h"
#include "random.h"
#include "render.h"
#include "texture.h"
#include "visible.h"

static GLvector2      angles[5][360];
static CLight*        head;
static bool           angles_done;
static int            count;

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void LightClear ()
{

  CLight*   l;

  while (head) {
    l = head;
    head = l->_next;
    delete l;
  }
  count = 0;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int LightCount ()
{

  return count;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void LightRender ()
{

  CLight*     l;

  if (!EntityReady ())
    return;
  if (!angles_done) {
    for (int size = 0; size < MAX_SIZE; size++) {
      for (int i = 0 ;i < 360; i++) {
        angles[size][i].x = cosf ((float)i * DEGREES_TO_RADIANS) * ((float)size + 0.5f);
        angles[size][i].y = sinf ((float)i * DEGREES_TO_RADIANS) * ((float)size + 0.5f);
      }
    }
  }
  glDepthMask (false);
  glEnable (GL_BLEND);
  glDisable (GL_CULL_FACE);
  glBlendFunc (GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D, TextureId (TEXTURE_LIGHT));
  glDisable (GL_CULL_FACE);
  glBegin (GL_QUADS);
  for (l = head; l; l = l->_next) 
    l->Render ();
  glEnd ();
  glDepthMask (true);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CLight::CLight (GLvector pos, GLrgba color, int size)
{

  _position = pos;
  _color = color;
  _size = CLAMP (size, 0, (MAX_SIZE - 1));
  _vert_size = (float)_size + 0.5f;
  _flat_size = _vert_size + 0.5f;
  _blink = false;
  _cell_x = WORLD_TO_GRID(pos.x);
  _cell_z = WORLD_TO_GRID(pos.z);
  _next = head;
  head = this;
  count++;


}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CLight::Blink ()
{

  _blink = true;
  //we don't want blinkers to be in sync, so have them blink at 
  //slightly different rates. (Milliseconds)
  _blink_interval = 1500 + RandomVal (500);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CLight::Render ()
{

  int       angle;
  GLvector  pos;
  GLvector  camera;
  GLvector  camera_position;
  GLvector2 offset;

  if (!Visible (_cell_x, _cell_z))
    return;
  camera = CameraAngle ();
  camera_position = CameraPosition ();
  if (fabs (camera_position.x - _position.x) > RenderFogDistance ())
    return;
  if (fabs (camera_position.z - _position.z) > RenderFogDistance ())
    return;
  if (_blink && (GetTickCount () % _blink_interval) > 200)
    return;
  angle = (int)MathAngle (camera.y);
  offset = angles[_size][angle];
  pos = _position;
  glColor4fv (&_color.red);
  glTexCoord2f (0, 0);   
  glVertex3f (pos.x + offset.x, pos.y - _vert_size, pos.z + offset.y);
  glTexCoord2f (0, 1);   
  glVertex3f (pos.x - offset.x, pos.y - _vert_size, pos.z - offset.y);
  glTexCoord2f (1, 1);   
  glVertex3f (pos.x - offset.x, pos.y + _vert_size, pos.z - offset.y);
  glTexCoord2f (1, 0);   
  glVertex3f (pos.x + offset.x, pos.y + _vert_size, pos.z + offset.y);

}