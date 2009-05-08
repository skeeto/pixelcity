/*-----------------------------------------------------------------------------

  Building.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------

  This module contains the class to construct the buildings.  

-----------------------------------------------------------------------------*/

#define MAX_VBUFFER         256

#include <windows.h>
#include <math.h>
#include <gl\gl.h>
#include "glTypes.h"

#include "building.h"
#include "deco.h"
#include "light.h"
#include "mesh.h"
#include "macro.h"
#include "math.h"
#include "random.h"
#include "texture.h"
#include "world.h"
#include "win.h"

//This is used by the recursive roof builder to decide what items may be added.
enum
{
  ADDON_NONE,
  ADDON_LOGO,
  ADDON_TRIM,
  ADDON_LIGHTS,
  ADDON_COUNT
};

static GLvector         vector_buffer[MAX_VBUFFER];

/*-----------------------------------------------------------------------------

  This is the constructor for our building constructor.

-----------------------------------------------------------------------------*/

CBuilding::CBuilding (int type, int x, int y, int height, int width, int depth, int seed, GLrgba color)
{

  _x = x;
  _y = y;
  _width = width;
  _depth = depth;
  _height = height;
  _center = glVector ((float)(_x + width / 2), 0.0f, (float)(_y + depth / 2));
  _seed = seed;
  _texture_type = RandomVal ();
  _color = color;
  _color.alpha = 0.1f;
  _have_lights = false;
  _have_logo = false;
  _have_trim = false;
  _roof_tiers = 0;
  //Pick a color for logos & roof lights
  _trim_color = WorldLightColor (seed);
  _mesh = new CMesh; //The main textured mesh for the building
  _mesh_flat = new CMesh; //Flat-color mesh for untextured detail items.
  switch (type) {
  case BUILDING_SIMPLE:
    CreateSimple ();
    break;  
  case BUILDING_MODERN: 
    CreateModern (); 
    break;
  case BUILDING_TOWER: 
    CreateTower (); 
    break;
  case BUILDING_BLOCKY:
    CreateBlocky (); 
    break;
  }

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CBuilding::~CBuilding ()
{

  if (_mesh)
    delete _mesh;
  if (_mesh_flat)
    delete _mesh_flat;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

unsigned CBuilding::Texture ()
{

  return TextureRandomBuilding (_texture_type);

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int CBuilding::PolyCount ()
{

  return _mesh->PolyCount () + _mesh_flat->PolyCount ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CBuilding::Render ()
{ 

  glColor3fv (&_color.red);
  _mesh->Render ();

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CBuilding::RenderFlat (bool colored)
{ 

  if (colored)
    glColor3fv (&_color.red);
  _mesh_flat->Render ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CBuilding::ConstructCube (int left, int right, int front, int back, int bottom, int top)
{

  GLvertex    p[10];
  float       x1, x2, z1, z2, y1, y2;
  int         i;
  cube        c;
  float       u, v1, v2;
  float       mapping;
  int         base_index;
  int         height;

  height = top - bottom;
  x1 = (float)left;
  x2 = (float)right;
  y1 = (float)bottom;
  y2 = (float)top;
  z1 = (float)front;
  z2 = (float)back;
  base_index = _mesh->VertexCount ();

  mapping = (float)SEGMENTS_PER_TEXTURE;
  u = (float)(RandomVal () % SEGMENTS_PER_TEXTURE) / (float)SEGMENTS_PER_TEXTURE;
  v1 = (float)bottom / (float)mapping;
  v2 = (float)top / (float)mapping;

  p[0].position = glVector (x1, y1, z1);  p[0].uv = glVector (u, v1);
  p[1].position = glVector (x1, y2, z1);  p[1].uv = glVector (u, v2);
  u += (float)_width / mapping;
  p[2].position = glVector (x2, y1, z1);  p[2].uv = glVector (u, v1);
  p[3].position = glVector (x2, y2, z1);  p[3].uv = glVector (u, v2);
  u += (float)_depth / mapping;
  p[4].position = glVector (x2, y1, z2);  p[4].uv = glVector (u, v1);
  p[5].position = glVector (x2, y2, z2);  p[5].uv = glVector (u, v2);
  u += (float)_width / mapping;
  p[6].position = glVector (x1, y1, z2);  p[6].uv = glVector (u, v1);
  p[7].position = glVector (x1, y2, z2);  p[7].uv = glVector (u, v2);
  u += (float)_width / mapping;
  p[8].position = glVector (x1, y1, z1);  p[8].uv = glVector (u, v1);
  p[9].position = glVector (x1, y2, z1);  p[9].uv = glVector (u, v2);
  for (i = 0; i < 10; i++) {
    p[i].uv.x = (p[i].position.x + p[i].position.z) / (float)SEGMENTS_PER_TEXTURE;
    _mesh->VertexAdd (p[i]);
    c.index_list.push_back(base_index + i);
  }
  _mesh->CubeAdd (c);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CBuilding::ConstructCube (float left, float right, float front, float back, float bottom, float top)
{

  GLvertex    p[10];
  float       x1, x2, z1, z2, y1, y2;
  int         i;
  cube        c;
  int         base_index;

  x1 = left;
  x2 = right;
  y1 = bottom;
  y2 = top;
  z1 = front;
  z2 = back;
  base_index = _mesh_flat->VertexCount ();

  p[0].position = glVector (x1, y1, z1);  p[0].uv = glVector (0.0f, 0.0f);
  p[1].position = glVector (x1, y2, z1);  p[1].uv = glVector (0.0f, 0.0f);
  p[2].position = glVector (x2, y1, z1);  p[2].uv = glVector (0.0f, 0.0f);
  p[3].position = glVector (x2, y2, z1);  p[3].uv = glVector (0.0f, 0.0f);
  p[4].position = glVector (x2, y1, z2);  p[4].uv = glVector (0.0f, 0.0f);
  p[5].position = glVector (x2, y2, z2);  p[5].uv = glVector (0.0f, 0.0f);
  p[6].position = glVector (x1, y1, z2);  p[6].uv = glVector (0.0f, 0.0f);
  p[7].position = glVector (x1, y2, z2);  p[7].uv = glVector (0.0f, 0.0f);
  p[8].position = glVector (x1, y1, z1);  p[8].uv = glVector (0.0f, 0.0f);
  p[9].position = glVector (x1, y2, z1);  p[9].uv = glVector (0.0f, 0.0f);
  for (i = 0; i < 10; i++) {
    p[i].uv.x = (p[i].position.x + p[i].position.z) / (float)SEGMENTS_PER_TEXTURE;
    _mesh_flat->VertexAdd (p[i]);
    c.index_list.push_back(base_index + i);
  }
  _mesh_flat->CubeAdd (c);

}

/*-----------------------------------------------------------------------------

  This will take the given area and populate it with rooftop stuff like
  air conditioners or light towers.

-----------------------------------------------------------------------------*/

void CBuilding::ConstructRoof (float left, float right, float front, float back, float bottom)
{

  int       air_conditioners;
  int       i;
  int       width, depth, height;
  int       face;
  int       addon;
  int       max_tiers;
  float     ac_x;
  float     ac_y;
  float     ac_base;
  float     ac_size;
  float     ac_height;
  float     tower_height;
  float     logo_offset;
  CDeco*    d;
  GLvector2 start, end;

  _roof_tiers++;
  max_tiers = _height / 10;
  width = (int)(right - left);
  depth = (int)(back - front);
  height = 5 - _roof_tiers;
  logo_offset = 0.2f;
  //See if this building is special and worthy of fancy roof decorations.
  if (bottom > 35.0f)
    addon = RandomVal (ADDON_COUNT);
  //Build the roof slab
  ConstructCube (left, right, front, back, bottom, bottom + (float)height);
  //Consider putting a logo on the roof, if it's tall enough
  if (addon == ADDON_LOGO && !_have_logo) {
    d = new CDeco;
    if (width > depth)
      face = COIN_FLIP ? NORTH : SOUTH;
    else
      face = COIN_FLIP ? EAST : WEST;
    switch (face) {
    case NORTH:
      start = glVector ((float)left, (float)back + logo_offset);
      end = glVector ((float)right, (float)back + logo_offset);
      break;
    case SOUTH:
      start = glVector ((float)right, (float)front - logo_offset);
      end = glVector ((float)left, (float)front - logo_offset);
      break;
    case EAST:
      start = glVector ((float)right + logo_offset, (float)back);
      end = glVector ((float)right + logo_offset, (float)front);
      break;
    case WEST:
    default:
      start = glVector ((float)left - logo_offset, (float)front);
      end = glVector ((float)left - logo_offset, (float)back);
      break;
    }
    d->CreateLogo (start, end, bottom, WorldLogoIndex (), _trim_color);
    _have_logo = true;
  } else if (addon == ADDON_TRIM) {
    d = new CDeco;
    vector_buffer[0] = glVector (left, bottom, back);
    vector_buffer[1] = glVector (left, bottom, front);
    vector_buffer[2] = glVector (right, bottom, front);
    vector_buffer[3] = glVector (right, bottom, back);
    d->CreateLightTrim (vector_buffer, 4, (float)RandomVal (2) + 1.0f, _seed, _trim_color);
  } else if (addon == ADDON_LIGHTS && !_have_lights) {
    new CLight (glVector (left, (float)(bottom + 2), front), _trim_color, 2);
    new CLight (glVector (right, (float)(bottom + 2), front), _trim_color, 2);
    new CLight (glVector (right, (float)(bottom + 2), back), _trim_color, 2);
    new CLight (glVector (left, (float)(bottom + 2), back), _trim_color, 2);
    _have_lights = true;
  }
  bottom += (float)height;
  //If the roof is big enough, consider making another layer 
  if (width > 7 && depth > 7 && _roof_tiers < max_tiers) {
    ConstructRoof (left + 1, right - 1, front + 1, back - 1, bottom);
    return;
  }
  //1 air conditioner block for every 15 floors sounds reasonble
  air_conditioners = _height / 15;
  for (i = 0; i < air_conditioners; i++) {
    ac_size = (float)(10 + RandomVal (30)) / 10;
    ac_height = (float)RandomVal (20) / 10 + 1.0f;
    ac_x = left + (float)RandomVal (width);
    ac_y = front + (float)RandomVal (depth);
    //make sure the unit doesn't hang off the right edge of the building
    if (ac_x + ac_size > (float)right)
      ac_x = (float)right - ac_size;
    //make sure the unit doesn't hang off the back edge of the building
    if (ac_y + ac_size > (float)back)
      ac_y = (float)back - ac_size;
    ac_base = (float)bottom;
    //make sure it doesn't hang off the edge
    ConstructCube (ac_x, ac_x + ac_size, ac_y, ac_y + ac_size, ac_base, ac_base + ac_height);
  }

  if (_height > 45) {
    d = new CDeco;
    tower_height = (float)(12 + RandomVal (8));
    d->CreateRadioTower (glVector ((float)(left + right) / 2.0f, (float)bottom, (float)(front + back) / 2.0f), 15.0f);
  }
  

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CBuilding::ConstructSpike (int left, int right, int front, int back, int bottom, int top)
{

  GLvertex    p;
  fan         f;
  int         i;
  GLvector    center;

  for (i = 0; i < 5; i++)
    f.index_list.push_back(_mesh_flat->VertexCount () + i);
  f.index_list.push_back(f.index_list[1]);
  p.uv = glVector (0.0f, 0.0f);
  center.x = ((float)left + (float)right) / 2.0f;
  center.z = ((float)front + (float)back) / 2.0f;
  p.position = glVector (center.x, (float)top, center.z);
  _mesh_flat->VertexAdd (p);
 
  p.position = glVector ((float)left, (float)bottom, (float)back);
  _mesh_flat->VertexAdd (p);

  p.position = glVector ((float)right, (float)bottom, (float)back);
  _mesh_flat->VertexAdd (p);

  p.position = glVector ((float)right, (float)bottom, (float)front);
  _mesh_flat->VertexAdd (p);
  
  p.position = glVector ((float)left, (float)bottom, (float)front);
  _mesh_flat->VertexAdd (p);
  
  _mesh_flat->FanAdd (f);

}

/*-----------------------------------------------------------------------------

  This builds an outer wall of a building, with blank (windowless) areas
  deliberately left.  It creates a chain of segments that alternate
  between windowed and windowless, and it always makes sure the wall
  is symetrical.  window_groups tells it how many windows to place in a row.

-----------------------------------------------------------------------------*/

float CBuilding::ConstructWall (int start_x, int start_y, int start_z, int direction, int length, int height, int window_groups, float uv_start, bool blank_corners)
{

  int         x, z;
  int         step_x, step_z;
  int         i;
  quad_strip  qs;
  int         column;
  int         mid;
  int         odd;
  GLvertex    v;
  bool        blank;
  bool        last_blank;

  qs.index_list.reserve(100);

  switch (direction) {
  case NORTH:
    step_z = 1; step_x = 0; break;
  case WEST:
    step_z = 0; step_x = -1; break;
  case SOUTH:
    step_z = -1; step_x = 0; break;
  case EAST:
    step_z = 0; step_x = 1; break;
  }
  x = start_x;;
  z = start_z;
  mid = (length / 2) - 1;
  odd = 1 - (length % 2);
  if (length % 2) 
    mid++;
  //mid = (length / 2);
  v.uv.x = (float)(x + z) / SEGMENTS_PER_TEXTURE;
  v.uv.x = uv_start;
  blank = false;
  for (i = 0; i <= length; i++) {
    //column counts up to the mid point, then back down, to make it symetrical
    if (i <= mid)
      column = i - odd;
    else 
      column = (mid) - (i - (mid));
    last_blank = blank;
    blank = (column % window_groups) > window_groups / 2;
    if (blank_corners && i == 0)
      blank = true;
    if (blank_corners && i == (length - 1))
      blank = true;
    if (last_blank != blank || i == 0 || i == length) {
      v.position = glVector ((float)x, (float)start_y, (float)z);
      v.uv.y = (float)start_y / SEGMENTS_PER_TEXTURE;
      _mesh->VertexAdd (v);
      qs.index_list.push_back(_mesh->VertexCount () - 1);
      v.position.y = (float)(start_y + height);
      v.uv.y = (float)(start_y + height) / SEGMENTS_PER_TEXTURE;;
      _mesh->VertexAdd (v);
      qs.index_list.push_back(_mesh->VertexCount () - 1);
    }
    //if (!blank && i != 0 && i != (length - 1))
    if (!blank && i != length)
      v.uv.x += 1.0f / SEGMENTS_PER_TEXTURE;
    x += step_x;
    z += step_z;
  }
  _mesh->QuadStripAdd (qs);  
  return v.uv.x;

}

/*-----------------------------------------------------------------------------

  This makes a big chunky building of intersecting cubes.  

-----------------------------------------------------------------------------*/

void CBuilding::CreateBlocky ()
{

  int         min_height;
  int         left, right, front, back;
  int         max_left, max_right, max_front, max_back;
  int         height;
  int         mid_x, mid_z;
  int         half_depth, half_width;
  int         tiers;
  int         max_tiers;
  int         grouping;
  float       lid_height;
  float       uv_start;
  bool        skip;
  bool        blank_corners;

  //Choose if the corners of the building are to be windowless.
  blank_corners = COIN_FLIP;
  //Choose a random column on our texture;
  uv_start = (float)RandomVal (SEGMENTS_PER_TEXTURE) / SEGMENTS_PER_TEXTURE;
  //Choose how the windows are grouped
  grouping = 2 + RandomVal (4);
  //Choose how tall the lid should be on top of each section
  lid_height = (float)(RandomVal (3) + 1);
  //find the center of the building.
  mid_x = _x + _width / 2;
  mid_z = _y + _depth / 2;
  max_left = max_right = max_front = max_back = 1;
  height = _height;
  min_height = _height / 2;
  min_height = 3;
  half_depth = _depth / 2;
  half_width = _width / 2;
  tiers = 0;
  if (_height > 40)
    max_tiers = 15;
  else if (_height > 30)
    max_tiers = 10;
  else if (_height > 20)
    max_tiers = 5;
  else if (_height > 10)
    max_tiers = 2;
  else
    max_tiers = 1;
  //We begin at the top of the building, and work our way down.
  //Viewed from above, the sections of the building are randomly sized
  //rectangles that ALWAYS include the center of the building somewhere within 
  //their area.  
  while (1) {
    if (height < min_height)
      break;
    if (tiers >= max_tiers)
      break;
    //pick new locationsfor our four outer walls
    left = (RandomVal () % half_width) + 1;
    right = (RandomVal () % half_width) + 1;
    front = (RandomVal () % half_depth) + 1;
    back = (RandomVal () % half_depth) + 1;
    skip = false;
    //At least ONE of the walls must reach out beyond a previous maximum.
    //Otherwise, this tier would be completely hidden within a previous one.
    if (left <= max_left && right <= max_right && front <= max_front && back <= max_back) 
      skip = true;
    //If any of the four walls is in the same position as the previous max,then
    //skip this tier, or else the two walls will end up z-fightng.
    if (left == max_left || right == max_right || front == max_front || back == max_back) 
      skip = true;
    if (!skip) {
      //if this is the top, then put some lights up here
      max_left = MAX (left, max_left);
      max_right = MAX (right, max_right);
      max_front = MAX (front, max_front);
      max_back = MAX (back, max_back);
      //Now build the four walls of this part
      uv_start = ConstructWall (mid_x - left, 0, mid_z + back, SOUTH, front + back, height, grouping, uv_start, blank_corners) - ONE_SEGMENT;
      uv_start = ConstructWall (mid_x - left, 0, mid_z - front, EAST, right + left, height, grouping, uv_start, blank_corners) - ONE_SEGMENT;
      uv_start = ConstructWall (mid_x + right, 0, mid_z - front, NORTH, front + back, height, grouping, uv_start, blank_corners) - ONE_SEGMENT;
      uv_start = ConstructWall (mid_x + right, 0, mid_z + back, WEST, right + left, height, grouping, uv_start, blank_corners) - ONE_SEGMENT;
      if (!tiers)
        ConstructRoof ((float)(mid_x - left), (float)(mid_x + right), (float)(mid_z - front), (float)(mid_z + back), (float)height);
      else //add a flat-color lid onto this section
        ConstructCube ((float)(mid_x - left), (float)(mid_x + right), (float)(mid_z - front), (float)(mid_z + back), (float)height, (float)height + lid_height);
      height -= (RandomVal () % 10) + 1;
      tiers++;
    }
    height--;
  }
  ConstructCube (mid_x - half_width, mid_x + half_width, mid_z - half_depth, mid_z + half_depth, 0, 2);
  _mesh->Compile ();
  _mesh_flat->Compile ();

}

/*-----------------------------------------------------------------------------

  A single-cube building.  Good for low-rise buildings and stuff that will be 
  far from the camera;

-----------------------------------------------------------------------------*/

void CBuilding::CreateSimple ()
{

  GLvertex    p;
  float       x1, x2, z1, z2, y1, y2;
  quad_strip  qs;
  float       u, v1, v2;
  float       cap_height;
  float       ledge;

  for(int i=0; i<=10; i++)
    qs.index_list.push_back(i);

  //How tall the flat-color roof is
  cap_height = (float)(1 + RandomVal (4));
  //how much the ledge sticks out
  ledge = (float)RandomVal (10) / 30.0f;

  x1 = (float)_x;
  x2 = (float)(_x + _width);
  y1 = (float)0.0f;
  y2 = (float)_height;
  z2 = (float)_y;
  z1 = (float)(_y + _depth);

  u = (float)(RandomVal (SEGMENTS_PER_TEXTURE)) / SEGMENTS_PER_TEXTURE;
  v1 = (float)(RandomVal (SEGMENTS_PER_TEXTURE)) / SEGMENTS_PER_TEXTURE;
  v2 = v1 + (float)_height * ONE_SEGMENT;

  p.position = glVector (x1, y1, z1);  p.uv = glVector (u, v1);
  _mesh->VertexAdd (p);
  p.position = glVector (x1, y2, z1);  p.uv = glVector (u, v2);
  _mesh->VertexAdd (p);
  u += (float)_depth / SEGMENTS_PER_TEXTURE;

  p.position = glVector (x1, y1, z2);  p.uv = glVector (u, v1);
  _mesh->VertexAdd (p);
  p.position = glVector (x1, y2, z2);  p.uv = glVector (u, v2);
  _mesh->VertexAdd (p);
  u += (float)_width / SEGMENTS_PER_TEXTURE;
  
  p.position = glVector (x2, y1, z2);  p.uv = glVector (u, v1);
  _mesh->VertexAdd (p);
  p.position = glVector (x2, y2, z2);  p.uv = glVector (u, v2);
  _mesh->VertexAdd (p);
  u += (float)_depth / SEGMENTS_PER_TEXTURE;

  p.position = glVector (x2, y1, z1);  p.uv = glVector (u, v1);
  _mesh->VertexAdd (p);
  p.position = glVector (x2, y2, z1);  p.uv = glVector (u, v2);
  _mesh->VertexAdd (p);
  u += (float)_depth / SEGMENTS_PER_TEXTURE;

  p.position = glVector (x1, y1, z1);  p.uv = glVector (u, v1);
  _mesh->VertexAdd (p);
  p.position = glVector (x1, y2, z1);  p.uv = glVector (u, v2);
  _mesh->VertexAdd (p);

  _mesh->QuadStripAdd (qs);
  ConstructCube (x1 - ledge, x2 + ledge, z2 - ledge, z1 + ledge, (float)_height, (float)_height + cap_height);
  _mesh->Compile ();

}


/*-----------------------------------------------------------------------------

  This makes a deformed cylinder building.  

-----------------------------------------------------------------------------*/

void CBuilding::CreateModern ()
{

  GLvertex    p;
  GLvector    center;
  GLvector    pos;
  GLvector2   radius;
  GLvector2   start, end;
  int         angle;
  int         windows;
  int         cap_height;
  int         half_depth, half_width;
  float       dist;
  float       length;
  quad_strip  qs;
  fan         f;
  int         points;
  int         skip_interval;
  int         skip_counter;
  int         skip_delta;
  int         i;
  bool        logo_done;
  bool        do_trim;
  CDeco*      d;

  logo_done = false;
  //How tall the windowless section on top will be.
  cap_height = 1 + RandomVal (5);
  //How many 10-degree segments to build before the next skip.
  skip_interval = 1 + RandomVal (8);
  //When a skip happens, how many degrees should be skipped
  skip_delta = (1 + RandomVal (2)) * 30; //30 60 or 90
  //See if this is eligible for fancy lighting trim on top
  if (_height > 48 && RandomVal (3) == 0)
    do_trim = true;
  else
    do_trim = false;
  //Get the center and radius of the circle
  half_depth = _depth / 2;
  half_width = _width / 2;
  center = glVector ((float)(_x + half_width), 0.0f, (float)(_y + half_depth));
  radius = glVector ((float)half_width, (float)half_depth);
  dist = 0;
  windows = 0;
  p.uv.x = 0.0f;
  points = 0;
  skip_counter = 0;
  for (angle = 0; angle <= 360; angle += 10) {
    if (skip_counter >= skip_interval && (angle + skip_delta < 360)) {
      angle += skip_delta;
      skip_counter = 0;
    }
    pos.x = center.x - sinf ((float)angle * DEGREES_TO_RADIANS) * radius.x;
    pos.z = center.z + cosf ((float)angle * DEGREES_TO_RADIANS) * radius.y;
    if (angle > 0 && skip_counter == 0) {
      length = MathDistance (p.position.x, p.position.z, pos.x, pos.z);
      windows += (int)length;
      if (length > 10 && !logo_done) {
        logo_done = true;
        start = glVector (pos.x, pos.z);
        end = glVector (p.position.x, p.position.z);
        d = new CDeco;
        d->CreateLogo (start, end, (float)_height, WorldLogoIndex (), RANDOM_COLOR);
      }
    } else if (skip_counter != 1)
      windows++;
    p.position = pos;
    p.uv.x = (float)windows / (float)SEGMENTS_PER_TEXTURE;
    p.uv.y = 0.0f;
    p.position.y = 0.0f;
    _mesh->VertexAdd (p);
    p.position.y = (float)_height;
    p.uv.y = (float)_height / (float)SEGMENTS_PER_TEXTURE;
    _mesh->VertexAdd (p);
    _mesh_flat->VertexAdd (p);
    p.position.y += (float)cap_height;
    _mesh_flat->VertexAdd (p);
    vector_buffer[points / 2] = p.position;
    vector_buffer[points / 2].y = (float)_height + cap_height / 4;
    points += 2;
    skip_counter++;
  }
  //if this is a big building and it didn't get a logo, consider giving it a light strip
  if (!logo_done && do_trim) {
    d = new CDeco;
    d->CreateLightTrim (vector_buffer, (points / 2) - 2, (float)cap_height / 2, _seed, RANDOM_COLOR);
  }
  qs.index_list.reserve(points);   
  //Add the outer walls
  for (i = 0; i < points; i++)
    qs.index_list.push_back(i);
  _mesh->QuadStripAdd (qs);
  _mesh_flat->QuadStripAdd (qs);
  //add the fan to cap the top of the buildings
  f.index_list.push_back(points);
  for (i = 0; i < points / 2; i++)
    f.index_list.push_back(points - (1 + i * 2));
  p.position.x = _center.x;
  p.position.z = _center.z;
  _mesh_flat->VertexAdd (p);
  _mesh_flat->FanAdd (f);
  radius /= 2.0f;
  //ConstructRoof ((int)(_center.x - radius), (int)(_center.x + radius), (int)(_center.z - radius), (int)(_center.z + radius), _height + cap_height);
  _mesh->Compile ();
  _mesh_flat->Compile ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CBuilding::CreateTower ()
{

  int         left, right, front, back, bottom;
  int         section_height, section_width, section_depth;
  int         remaining_height;
  int         ledge_height;
  int         tier_fraction;
  int         grouping;
  int         foundation;
  int         narrowing_interval;
  int         tiers;
  float       ledge;
  float       uv_start;
  bool        blank_corners;
  bool        roof_spike;
  bool        tower;

  //How much ledges protrude from the building
  ledge = (float)RandomVal (3) * 0.25f;
  //How tall the ledges are, in stories
  ledge_height = RandomVal (4) + 1;
  //How the windows are grouped
  grouping = RandomVal (3) + 2;
  //if the corners of the building have no windows
  blank_corners = RandomVal (4) > 0;
  //if the roof is pointed or has infrastructure on it
  roof_spike = RandomVal (3) == 0;
  //What fraction of the remaining height should be given to each tier
  tier_fraction = 2 + RandomVal (4);
  //How often (in tiers) does the building get narrorwer?
  narrowing_interval = 1 + RandomVal (10);
  //The height of the windowsless slab at the bottom
  foundation = 2 + RandomVal (3);
  //The odds that we'll have a big fancy spikey top
  tower = RandomVal (5) != 0 && _height > 40;
  //set our initial parameters
  left = _x; 
  right = _x + _width;
  front = _y;
  back = _y + _depth;
  bottom = 0;
  tiers = 0;
  //build the foundations.
  ConstructCube ((float)left - ledge, (float)right + ledge, (float)front - ledge, (float)back + ledge, (float)bottom, (float)foundation);
  bottom += foundation;
  //now add tiers until we reach the top
  while (1) {
    remaining_height = _height - bottom;
    section_depth = back - front;
    section_width = right - left;
    section_height = MAX (remaining_height / tier_fraction, 2);
    if (remaining_height < 10)
      section_height = remaining_height;
    //Build the four walls
    uv_start = (float)RandomVal (SEGMENTS_PER_TEXTURE) / SEGMENTS_PER_TEXTURE;
    uv_start = ConstructWall (left, bottom, back, SOUTH, section_depth, section_height, grouping, uv_start, blank_corners) - ONE_SEGMENT;
    uv_start = ConstructWall (left, bottom, front, EAST, section_width, section_height, grouping, uv_start, blank_corners) - ONE_SEGMENT;
    uv_start = ConstructWall (right, bottom, front, NORTH, section_depth, section_height, grouping, uv_start, blank_corners) - ONE_SEGMENT;
    uv_start = ConstructWall (right, bottom, back, WEST, section_width, section_height, grouping, uv_start, blank_corners) - ONE_SEGMENT;
    bottom += section_height;
    //Build the slab / ledges to cap this section.
    if (bottom + ledge_height > _height)
      break;
    ConstructCube ((float)left - ledge, (float)right + ledge, (float)front - ledge, (float)back + ledge, (float)bottom, (float)(bottom + ledge_height));
    bottom += ledge_height;
    if (bottom > _height)
      break;
    tiers++;
    if ((tiers % narrowing_interval) == 0) {
      if (section_width > 7) {
        left+=1;
        right-=1;
      }
      if (section_depth > 7) {
        front+=1;
        back-=1;
      }
    }
  }
  ConstructRoof ((float)left, (float)right, (float)front, (float)back, (float)bottom);
  _mesh->Compile ();
  _mesh_flat->Compile ();

}

