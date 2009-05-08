/*-----------------------------------------------------------------------------

  Deco.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------

  This handles building and rendering decoration objects - infrastructure & 
  such around the city.

-----------------------------------------------------------------------------*/

#define LOGO_OFFSET           0.2f //How far a logo sticks out from the given surface

#include <windows.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "glTypes.h"

#include "deco.h"
#include "light.h"
#include "mesh.h"
#include "macro.h"
#include "math.h"
#include "random.h"
#include "render.h"
#include "texture.h"
#include "world.h"
#include "visible.h"


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CDeco::~CDeco ()
{

  delete _mesh;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CDeco::CDeco ()
{

  _mesh = new CMesh ();
  _use_alpha = false;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CDeco::Render ()
{

  glColor3fv (&_color.red);
  _mesh->Render ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CDeco::RenderFlat (bool colored)
{


}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool CDeco::Alpha ()
{ 
  
  return _use_alpha; 

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int CDeco::PolyCount ()
{

  return _mesh->PolyCount ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

unsigned CDeco::Texture ()
{

  return _texture;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CDeco::CreateRadioTower (GLvector pos, float height)
{

  CLight*   l;
  float     offset;
  GLvertex  v;
  fan       f;

  for(int i=0; i<6; i++)
    f.index_list.push_back(i);

  offset = height / 15.0f;
  _center = pos;
  _use_alpha = true;
  //Radio tower
  v.position = glVector (_center.x, _center.y + height, _center.z);  v.uv = glVector (0,1);
  _mesh->VertexAdd (v);
  v.position = glVector (_center.x - offset, _center.y, _center.z - offset);  v.uv = glVector (1,0);
  _mesh->VertexAdd (v);
  v.position = glVector (_center.x + offset, _center.y, _center.z - offset);  v.uv = glVector (0,0);
  _mesh->VertexAdd (v);
  v.position = glVector (_center.x + offset, _center.y, _center.z + offset);  v.uv = glVector (1,0);
  _mesh->VertexAdd (v);
  v.position = glVector (_center.x - offset, _center.y, _center.z + offset);  v.uv = glVector (0,0);
  _mesh->VertexAdd (v);
  v.position = glVector (_center.x - offset, _center.y, _center.z - offset);  v.uv = glVector (1,0);
  _mesh->VertexAdd (v);
  _mesh->FanAdd (f);
  l = new CLight (glVector (_center.x, _center.y + height + 1.0f, _center.z), glRgba (255,192,160), 1);
  l->Blink ();
  _texture = TextureId (TEXTURE_LATTICE);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CDeco::CreateLogo (GLvector2 start, GLvector2 end, float bottom, int seed, GLrgba color)
{

  GLvertex   p;
  quad_strip qs;
  float      u1, u2, v1, v2;
  float      top;
  float      height, length;
  GLvector2  center2d;
  GLvector   to;
  GLvector   out;
  int        logo_index;

  qs.index_list.push_back(0);
  qs.index_list.push_back(1);
  qs.index_list.push_back(3);
  qs.index_list.push_back(2);

  _use_alpha = true;
  _color = color;
  logo_index = seed % LOGO_ROWS;
  to = glVector (start.x, 0.0f, start.y) - glVector (end.x, 0.0f, end.y);
  to = glVectorNormalize (to);
  out = glVectorCrossProduct (glVector (0.0f, 1.0f, 0.0f), to) * LOGO_OFFSET;
  center2d = (start + end) / 2;
  _center = glVector (center2d.x, bottom, center2d.y);
  length = glVectorLength (start - end);
  height = (length / 8.0f) * 1.5f;
  top = bottom + height;
  u1 = 0.0f;
  u2 = 0.5f;//We actually only use the left half of the texture
  v1 = (float)logo_index / LOGO_ROWS;
  v2 = v1 + (1.0f / LOGO_ROWS);
  p.position = glVector (start.x, bottom, start.y) + out;  p.uv = glVector (u1,v1);
  _mesh->VertexAdd (p);
  p.position = glVector (end.x, bottom, end.y) + out;  p.uv = glVector (u2, v1);
  _mesh->VertexAdd (p);
  p.position = glVector (end.x, top, end.y) + out;  p.uv = glVector (u2, v2);
  _mesh->VertexAdd (p);
  p.position = glVector (start.x, top, start.y) + out;  p.uv = glVector (u1, v2);
  _mesh->VertexAdd (p);
  _mesh->QuadStripAdd (qs);
  _texture = TextureId (TEXTURE_LOGOS);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CDeco::CreateLightStrip (float x, float z, float width, float depth, float height, GLrgba color)
{

  GLvertex   p;
  quad_strip qs1;
  float      u, v;
  
  qs1.index_list.push_back(0);
  qs1.index_list.push_back(1);
  qs1.index_list.push_back(3);
  qs1.index_list.push_back(2);
  _color = color;
  _use_alpha = true;
  _center = glVector (x + width / 2, height, z + depth / 2);
  if (width < depth) {
    u = 1.0f;
    v = (float)((int)(depth / width));
  } else {
    v = 1.0f;
    u = (float)((int)(width / depth));
  }
  _texture = TextureId (TEXTURE_LIGHT);
  p.position = glVector (x, height, z);  p.uv = glVector (0.0f, 0.0f);
  _mesh->VertexAdd (p);
  p.position = glVector (x, height, z + depth);  p.uv = glVector (0.0f, v);
  _mesh->VertexAdd (p);
  p.position = glVector (x + width, height, z + depth);  p.uv = glVector (u, v);
  _mesh->VertexAdd (p);
  p.position = glVector (x + width, height, z);  p.uv = glVector (u, 0.0f);
  _mesh->VertexAdd (p);
  _mesh->QuadStripAdd (qs1);
  _mesh->Compile ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CDeco::CreateLightTrim (GLvector* chain, int count, float height, int seed, GLrgba color)
{

  GLvertex   p;
  GLvector   to;
  GLvector   out;
  int        i;
  int        index;
  int        prev, next;
  float      u, v1, v2;
  float      row;
  quad_strip qs;

  _color = color;
  _center = glVector (0.0f, 0.0f, 0.0f);
  qs.index_list.reserve(count * 2 + 2);
  for (i = 0; i < count; i++) 
    _center += chain[i];
  _center /= (float)count;
  row = (float)(seed % TRIM_ROWS);
  v1 = row * TRIM_SIZE;
  v2 = (row + 1.0f) * TRIM_SIZE;
  index = 0;
  u = 0.0f;
  for (i = 0; i < count + 1; i++) {
    if (i)
      u += glVectorLength (chain[i % count] - p.position) * 0.1f;
    //Add the bottom point      
    prev = i - 1;
    if (prev < 0)
      prev = count + prev;
    next = (i + 1) % count;
    to = glVectorNormalize (chain[next] - chain[prev]);
    out = glVectorCrossProduct (glVector (0.0f, 1.0f, 0.0f), to) * LOGO_OFFSET;
    p.position = chain[i % count] + out; p.uv = glVector (u, v2);
    _mesh->VertexAdd (p);
    qs.index_list.push_back(index++);
    //Top point
    p.position.y += height;p.uv = glVector (u, v1);
    _mesh->VertexAdd (p);
    qs.index_list.push_back(index++);
  }
  _mesh->QuadStripAdd (qs);
  _texture = TextureId (TEXTURE_TRIM);
  _mesh->Compile ();

}