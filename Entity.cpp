/*-----------------------------------------------------------------------------

  Entity.cpp

  Copyright (c) 2005 Shamus Young
  All Rights Reserved

-------------------------------------------------------------------------------

  An entity is any renderable stationary object in the world.  This is an 
  abstract class.  This module gathers up the Entities, sorts them by 
  texture use and location, and then stores them in OpenGL render lists
  for faster rendering.    

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <math.h>
#include <gl\gl.h>

#include "camera.h"
#include "entity.h"
#include "macro.h"
#include "math.h"
#include "render.h"
#include "texture.h"
#include "world.h"
#include "visible.h"
#include "win.h"

struct entity
{
  CEntity*            object;
};


struct cell
{
  unsigned        list_textured;
  unsigned        list_flat;
  unsigned        list_flat_wireframe;
  unsigned        list_alpha;
  GLvector        pos;
};

static cell           cell_list[GRID_SIZE][GRID_SIZE];
static int            entity_count;
static entity*        entity_list;
static bool           sorted;
static bool           compiled;
static int            polycount;
static int            compile_x;
static int            compile_y;
static int            compile_count;
static int            compile_end;

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static int do_compare (const void *arg1, const void *arg2 )
{

  struct entity*  e1 = (struct entity*)arg1;
  struct entity*  e2 = (struct entity*)arg2;

  if (e1->object->Alpha () && !e2->object->Alpha ())
    return 1;
  if (!e1->object->Alpha () && e2->object->Alpha ())
    return -1;
  if (e1->object->Texture () > e2->object->Texture ())
    return 1;
  else if (e1->object->Texture () < e2->object->Texture ())
    return -1;
  return 0;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void add (CEntity* b)
{

  entity_list = (entity*)realloc (entity_list, sizeof (entity) * (entity_count + 1));
  entity_list[entity_count].object = b;
  entity_count++;
  polycount = 0;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static void do_compile ()
{

  int       i;
  int       x, y;

  if (compiled)
    return;
  x = compile_x;
  y = compile_y;
  //Changing textures is pretty expensive, and thus sorting the entites so that
  //they are grouped by texture used can really improve framerate.
  //qsort (entity_list, entity_count, sizeof (struct entity), do_compare);
  //sorted = true;
  //Now group entites on the grid 
  //make a list for the textured objects in this region
  if (!cell_list[x][y].list_textured)
    cell_list[x][y].list_textured = glGenLists(1);
  glNewList (cell_list[x][y].list_textured, GL_COMPILE);
  cell_list[x][y].pos = glVector (GRID_TO_WORLD(x), 0.0f, (float)y * GRID_RESOLUTION);
  for (i = 0; i < entity_count; i++) {
    GLvector pos = entity_list[i].object->Center ();
    if (WORLD_TO_GRID(pos.x) == x && WORLD_TO_GRID(pos.z) == y && !entity_list[i].object->Alpha ()) {
      glBindTexture(GL_TEXTURE_2D, entity_list[i].object->Texture ());
      entity_list[i].object->Render ();
    }
  }
  glEndList();	

  //Make a list of flat-color stuff (A/C units, ledges, roofs, etc.)
  if (!cell_list[x][y].list_flat)
    cell_list[x][y].list_flat = glGenLists(1);
  glNewList (cell_list[x][y].list_flat, GL_COMPILE);
  glEnable (GL_CULL_FACE);
  cell_list[x][y].pos = glVector (GRID_TO_WORLD(x), 0.0f, (float)y * GRID_RESOLUTION);
  for (i = 0; i < entity_count; i++) {
    GLvector pos = entity_list[i].object->Center ();
    if (WORLD_TO_GRID(pos.x) == x && WORLD_TO_GRID(pos.z) == y && !entity_list[i].object->Alpha ()) {
      entity_list[i].object->RenderFlat (false);
    }
  }
  glEndList();	
  //Now a list of flat-colored stuff that will be wireframe friendly
  if (!cell_list[x][y].list_flat_wireframe)
    cell_list[x][y].list_flat_wireframe = glGenLists(1);
  glNewList (cell_list[x][y].list_flat_wireframe, GL_COMPILE);
  glEnable (GL_CULL_FACE);
  cell_list[x][y].pos = glVector (GRID_TO_WORLD(x), 0.0f, (float)y * GRID_RESOLUTION);
  for (i = 0; i < entity_count; i++) {
    GLvector pos = entity_list[i].object->Center ();
    if (WORLD_TO_GRID(pos.x) == x && WORLD_TO_GRID(pos.z) == y && !entity_list[i].object->Alpha ()) {
      entity_list[i].object->RenderFlat (true);
    }
  }
  glEndList();	
  //Now a list of stuff to be alpha-blended, and thus rendered last
  if (!cell_list[x][y].list_alpha)
    cell_list[x][y].list_alpha = glGenLists(1);
  glNewList (cell_list[x][y].list_alpha, GL_COMPILE);
  cell_list[x][y].pos = glVector (GRID_TO_WORLD(x), 0.0f, (float)y * GRID_RESOLUTION);
  glDepthMask (false);
  glEnable (GL_BLEND);
  glDisable (GL_CULL_FACE);
  for (i = 0; i < entity_count; i++) {
    GLvector pos = entity_list[i].object->Center ();
    if (WORLD_TO_GRID(pos.x) == x && WORLD_TO_GRID(pos.z) == y && entity_list[i].object->Alpha ()) {
      glBindTexture(GL_TEXTURE_2D, entity_list[i].object->Texture ());
      entity_list[i].object->Render ();
    }
  }
  glDepthMask (true);
  glEndList();	

  //now walk the grid
  compile_x++;
  if (compile_x == GRID_SIZE) {
    compile_x = 0;
    compile_y++;
    if (compile_y == GRID_SIZE)
      compiled = true;
    compile_end = GetTickCount ();
  } 
  compile_count++;


}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool EntityReady ()
{

  return compiled;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

float EntityProgress ()
{

  return (float)compile_count / (GRID_SIZE * GRID_SIZE);

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityUpdate ()
{

  unsigned    stop_time;

  if (!TextureReady ()) {
    sorted = false;
    return;
  } 
  if (!sorted) {
    qsort (entity_list, entity_count, sizeof (struct entity), do_compare);
    sorted = true;
  }
  //We want to do several cells at once. Enough to get things done, but
  //not so many that the program is unresponsive.
  if (LOADING_SCREEN) {  //If we're using a loading screen, we want to build as fast as possible
    stop_time = GetTickCount () + 100;
    while (!compiled && GetTickCount () < stop_time)
      do_compile ();
  } else //Take it slow
    do_compile ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityRender ()
{

  int       polymode[2];
  bool      wireframe;
  int       x, y;
  int       elapsed;

  //Draw all textured objects
  glGetIntegerv (GL_POLYGON_MODE, &polymode[0]);
  wireframe = polymode[0] != GL_FILL;
  if (RenderFlat ())
    glDisable (GL_TEXTURE_2D);
  //If we're not using a loading screen, make the wireframe fade out via fog
  if (!LOADING_SCREEN && wireframe) {
    elapsed = 6000 - WorldSceneElapsed ();
    if (elapsed >= 0 && elapsed <= 6000)
      RenderFogFX ((float)elapsed / 6000.0f);
    else
      return;
  }
  for (x = 0; x < GRID_SIZE; x++) {
    for (y = 0; y < GRID_SIZE; y++) {
      if (Visible (x,y))
        glCallList (cell_list[x][y].list_textured);
    }
  }
  //draw all flat colored objects
  glBindTexture(GL_TEXTURE_2D, 0);
  glColor3f (0, 0, 0);
  for (x = 0; x < GRID_SIZE; x++) {
    for (y = 0; y < GRID_SIZE; y++) {
      if (Visible (x, y)) {
        if (wireframe)
          glCallList (cell_list[x][y].list_flat_wireframe);
        else 
          glCallList (cell_list[x][y].list_flat);
      }
    }
  }
  //draw all alpha-blended objects
  glBindTexture(GL_TEXTURE_2D, 0);
  glColor3f (0, 0, 0);
  glEnable (GL_BLEND);
  for (x = 0; x < GRID_SIZE; x++) {
    for (y = 0; y < GRID_SIZE; y++) {
      if (Visible (x,y)) {
        glCallList (cell_list[x][y].list_alpha);
      }
    }
  }
  
}




/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityClear ()
{

  for (int i = 0; i < entity_count; i++) {
    delete entity_list[i].object;
  }
  if (entity_list)
    free (entity_list);
  entity_list = NULL;
  entity_count = 0;
  compile_x = 0;
  compile_y = 0;
  compile_count = 0;
  compiled = false;
  sorted = false;

  int  x, y;

  for (x = 0; x < GRID_SIZE; x++) {
    for (y = 0; y < GRID_SIZE; y++) {
      glNewList (cell_list[x][y].list_textured, GL_COMPILE);
      glEndList();	
      glNewList (cell_list[x][y].list_alpha, GL_COMPILE);
      glEndList();	
      glNewList (cell_list[x][y].list_flat_wireframe, GL_COMPILE);
      glEndList();	
      glNewList (cell_list[x][y].list_flat, GL_COMPILE);
      glEndList();	
    }
  }


}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int EntityCount ()
{

  return entity_count;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityInit (void)
{

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int EntityPolyCount (void)
{

  if (!sorted)
    return 0;
  if (polycount)
    return polycount;
  for (int i = 0; i < entity_count; i++) 
    polycount += entity_list[i].object->PolyCount ();
  return polycount;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CEntity::CEntity (void)
{

  add (this);

}

void CEntity::Render (void)
{

}

void CEntity::RenderFlat (bool wireframe)
{

}

void CEntity::Update (void)
{


}


