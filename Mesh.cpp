/*-----------------------------------------------------------------------------

  Mesh.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------

  This class is used to make constructing objects easier. It handles
  allocating vertex lists, polygon lists, and suchlike. 

  If you were going to implement vertex buffers, this would be the place to 
  do it.  Take away the _vertex member variable and store verts for ALL meshes
  in a common list, which could then be unloaded onto the good 'ol GPU.

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "glTypes.h"

#include "mesh.h"

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CMesh::CMesh ()
{

  _vertex_count = 0;
  _triangle_count = 0;
  _quad_strip_count = 0;
  _fan_count = 0;
  _cube_count = 0;
  _polycount = 0;
  _list = glGenLists(1);
  _compiled = false;
  _vertex = NULL;
  _normal = NULL;
  _triangle = NULL;
  _cube = NULL;
  _quad_strip = NULL;
  _fan = NULL;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CMesh::~CMesh ()
{

  unsigned    i;
  
  if (_vertex)
    free (_vertex);
  if (_normal)
    free (_normal);
  if (_triangle)
    free (_triangle);
  if (_cube)
    free (_cube);
  for (i = 0; i < _quad_strip_count; i++) 
    delete _quad_strip[i].index_list;
  if (_quad_strip)
    delete _quad_strip;
  for (i = 0; i < _fan_count; i++) 
    delete _fan[i].index_list;
  if (_fan)
    delete _fan;
  if (_list)
    glDeleteLists (_list, 1);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::VertexAdd (GLvertex v)
{

  _vertex = (GLvertex*)realloc (_vertex, sizeof (GLvertex) * (_vertex_count + 1));
  _vertex[_vertex_count] = v;
  _vertex_count++;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::CubeAdd (int* index)
{

  _cube = (cube*)realloc (_cube, sizeof (cube) * (_cube_count + 1));
  memcpy (&_cube[_cube_count].index_list[0], index, sizeof (int) * 10);
  _cube_count++;
  _polycount += 5;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::QuadStripAdd (int* index, int count)
{

  _quad_strip = (quad_strip*)realloc (_quad_strip, sizeof (quad_strip) * (_quad_strip_count + 1));
  _quad_strip[_quad_strip_count].index_list = (int*)malloc (sizeof (int) * count);
  _quad_strip[_quad_strip_count].count = count;
  memcpy (&_quad_strip[_quad_strip_count].index_list[0], &index[0], sizeof (int) * count);
  _quad_strip_count++;
  _polycount += (count - 2) / 2;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::FanAdd (int* index, int count)
{

  _fan = (fan*)realloc (_fan, sizeof (fan) * (_fan_count + 1));
  _fan[_fan_count].index_list = (int*)malloc (sizeof (int) * count);
  _fan[_fan_count].count = count;
  memcpy (&_fan[_fan_count].index_list[0], &index[0], sizeof (int) * count);
  _fan_count++;
  _polycount += count - 2;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::Render ()
{

  unsigned  i, n;
  int*      index;

  if (_compiled) {
    glCallList (_list);
    return;
  }
  for (i = 0; i < _quad_strip_count; i++) {
    index = &_quad_strip[i].index_list[0];
    glBegin (GL_QUAD_STRIP);
    for (n = 0; n < _quad_strip[i].count; n++) {
      glTexCoord2fv (&_vertex[index[n]].uv.x);
      glVertex3fv (&_vertex[index[n]].position.x);
    }
    glEnd ();
  }
  for (i = 0; i < _cube_count; i++) {
    index = &_cube[i].index_list[0];
    glBegin (GL_QUAD_STRIP);
    for (n = 0; n < 10; n++) {
      glTexCoord2fv (&_vertex[index[n]].uv.x);
      glVertex3fv (&_vertex[index[n]].position.x);
    }
    glEnd ();
    
    glBegin (GL_QUADS);
    glTexCoord2fv (&_vertex[index[7]].uv.x);
    glVertex3fv (&_vertex[index[7]].position.x);
    glVertex3fv (&_vertex[index[5]].position.x);
    glVertex3fv (&_vertex[index[3]].position.x);
    glVertex3fv (&_vertex[index[1]].position.x);
    glEnd ();
    
    glBegin (GL_QUADS);
    glTexCoord2fv (&_vertex[index[6]].uv.x);
    glVertex3fv (&_vertex[index[0]].position.x);
    glVertex3fv (&_vertex[index[2]].position.x);
    glVertex3fv (&_vertex[index[4]].position.x);
    glVertex3fv (&_vertex[index[6]].position.x);
    glEnd ();

  
  }
  for (i = 0; i < _fan_count; i++) {
    index = &_fan[i].index_list[0];
    glBegin (GL_TRIANGLE_FAN);
    for (n = 0; n < _fan[i].count; n++) {
      glTexCoord2fv (&_vertex[index[n]].uv.x);
      glVertex3fv (&_vertex[index[n]].position.x);
    }
    glEnd ();
  }

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::Compile ()
{

  glNewList (_list, GL_COMPILE);
  Render ();
  glEndList();	
  _compiled = true;

}