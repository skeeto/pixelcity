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
#include <GL/gl.h>
#include <GL/glu.h>

#include <vector>
#include "glTypes.h"
#include "Mesh.h"

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CMesh::CMesh ()
{

  _list = glGenLists(1);
  _compiled = false;
  _polycount = 0;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CMesh::~CMesh ()
{

  glDeleteLists (_list, 1);
  _vertex.clear ();
  _fan.clear ();
  _quad_strip.clear ();
  _cube.clear ();


}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::VertexAdd (const GLvertex& v)
{

  _vertex.push_back(v);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::CubeAdd (const cube& c)
{

  _cube.push_back(c);
  _polycount += 5;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::QuadStripAdd (const quad_strip& qs)
{

  _quad_strip.push_back(qs);
  _polycount += (qs.index_list.size() - 2) / 2;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::FanAdd (const fan& f)
{

  _fan.push_back(f);
  _polycount += f.index_list.size() - 2;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::Render ()
{

  std::vector<quad_strip>::iterator qsi;
  std::vector<cube>::iterator ci;
  std::vector<fan>::iterator fi;
  std::vector<int>::iterator n;

  if (_compiled) {
    glCallList (_list);
    return;
  }
  for (qsi = _quad_strip.begin(); qsi < _quad_strip.end(); ++qsi) {
    glBegin (GL_QUAD_STRIP);
    for (n = qsi->index_list.begin(); n < qsi->index_list.end(); ++n) {
      glTexCoord2fv (&_vertex[*n].uv.x);
      glVertex3fv (&_vertex[*n].position.x);
    }
    glEnd ();
  }
  for (ci = _cube.begin(); ci < _cube.end(); ++ci) {
    glBegin (GL_QUAD_STRIP);
    for (n = ci->index_list.begin(); n < ci->index_list.end(); ++n) {
      glTexCoord2fv (&_vertex[*n].uv.x);
      glVertex3fv (&_vertex[*n].position.x);
    }
    glEnd ();
    
    glBegin (GL_QUADS);
    glTexCoord2fv (&_vertex[ci->index_list[7]].uv.x);
    glVertex3fv (&_vertex[ci->index_list[7]].position.x);
    glVertex3fv (&_vertex[ci->index_list[5]].position.x);
    glVertex3fv (&_vertex[ci->index_list[3]].position.x);
    glVertex3fv (&_vertex[ci->index_list[1]].position.x);
    glEnd ();
    
    glBegin (GL_QUADS);
    glTexCoord2fv (&_vertex[ci->index_list[6]].uv.x);
    glVertex3fv (&_vertex[ci->index_list[0]].position.x);
    glVertex3fv (&_vertex[ci->index_list[2]].position.x);
    glVertex3fv (&_vertex[ci->index_list[4]].position.x);
    glVertex3fv (&_vertex[ci->index_list[6]].position.x);
    glEnd ();

  
  }
  for (fi = _fan.begin(); fi < _fan.end(); ++fi) {
    glBegin (GL_TRIANGLE_FAN);
    for (n = fi->index_list.begin(); n < fi->index_list.end(); ++n) {
      glTexCoord2fv (&_vertex[*n].uv.x);
      glVertex3fv (&_vertex[*n].position.x);
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
