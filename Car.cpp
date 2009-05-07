/*-----------------------------------------------------------------------------

  Car.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------

  This creates the little two-triangle cars and moves them around the map.

-----------------------------------------------------------------------------*/

#define DEAD_ZONE       200
#define STUCK_TIME      230
#define UPDATE_INTERVAL 50 //milliseconds
#define MOVEMENT_SPEED  0.61f
#define CAR_SIZE        3.0f

#include <windows.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "glTypes.h"

#include "building.h"
#include "car.h"
#include "camera.h"
#include "mesh.h"
#include "macro.h"
#include "math.h"
#include "random.h"
#include "render.h"
#include "texture.h"
#include "world.h"
#include "visible.h"
#include "win.h"

static GLvector           direction[] = 
{
  0.0f, 0.0f, -1.0f,
  1.0f, 0.0f,  0.0f,
  0.0f, 0.0f,  1.0f,
 -1.0f, 0.0f,  0.0f,
};

static int                dangles[] = { 0, 90, 180, 270};

static GLvector2          angles[360];
static bool               angles_done;
static unsigned char      carmap[WORLD_SIZE][WORLD_SIZE];
static CCar*              head;
static unsigned           next_update;
static int                count;

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int CarCount ()
{

  return count;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CarClear ()
{

  CCar*       c;

  for (c = head; c; c = c->m_next)
    c->Park ();
  ZeroMemory (carmap, sizeof (carmap));
  count = 0;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CarRender ()
{

  CCar*       c;

  if (!angles_done) {
    for (int i = 0 ;i < 360; i++) {
      angles[i].x = cosf ((float)i * DEGREES_TO_RADIANS) * CAR_SIZE;
      angles[i].y = sinf ((float)i * DEGREES_TO_RADIANS) * CAR_SIZE;
    }
  }
  glDepthMask (false);
  glEnable (GL_BLEND);
  glDisable (GL_CULL_FACE);
  glBlendFunc (GL_ONE, GL_ONE);
  glBindTexture (GL_TEXTURE_2D, 0);
  glBindTexture(GL_TEXTURE_2D, TextureId (TEXTURE_HEADLIGHT));
  for (c = head; c; c = c->m_next)
    c->Render ();
  glDepthMask (true);

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CarUpdate ()
{

  CCar*       c;
  unsigned    now;

  if (!TextureReady () || !EntityReady ())
    return;
  now = GetTickCount ();
  if (next_update > now)
    return;
  next_update = now + UPDATE_INTERVAL;
  for (c = head; c; c = c->m_next)
    c->Update ();

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CCar::CCar ()
{

  m_ready = false;
  m_next = head;
  head = this;
  count++;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool CCar::TestPosition (int row, int col)
{

  //test the given position and see if it's already occupied
  if (carmap[row][col])
    return false;
  //now make sure that the lane is going the right direction
  if (WorldCell (row, col) != WorldCell (m_row, m_col))
    return false;
  return true;

}
 
/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CCar::Update (void)
{

  int       new_row, new_col;
  GLvector  old_pos;    
  GLvector  camera;

  //If the car isn't ready, place it on the map and get it moving
  camera = CameraPosition ();
  if (!m_ready) {
    //if the car isn't ready, we need to place it somewhere on the map
    m_row = DEAD_ZONE + RandomVal (WORLD_SIZE - DEAD_ZONE * 2);
    m_col = DEAD_ZONE + RandomVal (WORLD_SIZE - DEAD_ZONE * 2);
    //if there is already a car here, forget it.  
    if (carmap[m_row][m_col] > 0)
      return;
    //if this spot is not a road, forget it
    if (!(WorldCell (m_row, m_col) & CLAIM_ROAD)) 
      return;
    if (!Visible (glVector ((float)m_row, 0.0f, (float)m_col)))
      return;
    //good spot. place the car
    m_position = glVector ((float)m_row, 0.1f, (float)m_col);
    m_drive_position = m_position;
    m_ready = true;
    if (WorldCell (m_row, m_col) & MAP_ROAD_NORTH) 
      m_direction = NORTH;
    if (WorldCell (m_row, m_col) & MAP_ROAD_EAST) 
      m_direction = EAST;
    if (WorldCell (m_row, m_col) & MAP_ROAD_SOUTH) 
      m_direction = SOUTH;
    if (WorldCell (m_row, m_col) & MAP_ROAD_WEST) 
      m_direction = WEST;
    m_drive_angle = dangles[m_direction];
    m_max_speed = (float)(4 + RandomVal (6)) / 10.0f;
    m_speed = 0.0f;
    m_change = 3;
    m_stuck = 0;
    carmap[m_row][m_col]++;
  }
  //take the car off the map and move it
  carmap[m_row][m_col]--;
  old_pos = m_position;
  m_speed += m_max_speed * 0.05f;
  m_speed = MIN (m_speed, m_max_speed);
  m_position += direction[m_direction] * MOVEMENT_SPEED * m_speed;
  //If the car has moved out of view, there's no need to keep simulating it. 
  if (!Visible (glVector ((float)m_row, 0.0f, (float)m_col))) 
    m_ready = false;
  //if the car is far away, remove it.  We use manhattan units because buildings almost always
  //block views of cars on the diagonal.
  if (fabs (camera.x - m_position.x) + fabs (camera.z - m_position.z) > RenderFogDistance ())
    m_ready = false;
  //if the car gets too close to the edge of the map, take it out of play
  if (m_position.x < DEAD_ZONE || m_position.x > (WORLD_SIZE - DEAD_ZONE))
    m_ready = false;
  if (m_position.z < DEAD_ZONE || m_position.z > (WORLD_SIZE - DEAD_ZONE))
    m_ready = false;
  if (m_stuck >= STUCK_TIME)
    m_ready = false;
  if (!m_ready)
    return;
  //Check the new position and make sure its not in another car
  new_row = (int)m_position.x;
  new_col = (int)m_position.z;
  if (new_row != m_row || new_col != m_col) {
    //see if the new position places us on top of another car
    if (carmap[new_row][new_col]) {
      m_position = old_pos;
      m_speed = 0.0f;
      m_stuck++;
    } else {
      //look at the new position and decide if we're heading towards or away from the camera
      m_row = new_row;
      m_col = new_col;
      m_change--;
      m_stuck = 0;
      if (m_direction == NORTH)
        m_front = camera.z < m_position.z;
      else if (m_direction == SOUTH)
        m_front = camera.z > m_position.z;
      else if (m_direction == EAST)
        m_front = camera.x > m_position.x;
      else 
        m_front = camera.x < m_position.x;
    }
  }
  m_drive_position = (m_drive_position + m_position) / 2.0f;
  //place the car back on the map
  carmap[m_row][m_col]++;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CCar::Render ()
{

  GLvector  pos;
  int       angle;
  int       turn;
  float     top;

  if (!m_ready)
    return;
  if (!Visible (m_drive_position))
    return;
  if (m_front) {
    glColor3f (1, 1, 0.8f);
    top = CAR_SIZE;
  } else {
    glColor3f (0.5, 0.2f, 0);
    top = 0.0f;
  }

  glBegin (GL_QUADS);

  angle = dangles[m_direction];
  pos = m_drive_position;// 
  angle = 360 - (int)MathAngle (m_position.x, m_position.z, pos.x, pos.z);
  angle %= 360;
  turn = (int)MathAngleDifference ((float)m_drive_angle, (float)angle);
  m_drive_angle += SIGN (turn);
  pos += glVector (0.5f, 0.0f, 0.5f);
  
  glTexCoord2f (0, 0);   
  glVertex3f (pos.x + angles[angle].x, -CAR_SIZE, pos.z + angles[angle].y);
  glTexCoord2f (1, 0);   
  glVertex3f (pos.x - angles[angle].x, -CAR_SIZE, pos.z - angles[angle].y);
  glTexCoord2f (1, 1);   
  glVertex3f (pos.x - angles[angle].x,  top, pos.z - angles[angle].y);
  glTexCoord2f (0, 1);   
  glVertex3f (pos.x + angles[angle].x,  top, pos.z +  angles[angle].y);
  
  glEnd ();

}