/*-----------------------------------------------------------------------------

  Texture.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------
  
  This procedurally builds all of the textures.  

  I apologize in advance for the apalling state of this module. It's the victim 
  of iterative and experimental development.  It has cruft, poorly named
  functions, obscure code, poorly named variables, and is badly organized. Even
  the formatting sucks in places. Its only saving grace is that it works.
  
-----------------------------------------------------------------------------*/

#define RANDOM_COLOR_SHIFT  ((float)(RandomVal (10)) / 50.0f)
#define RANDOM_COLOR_VAL    ((float)(RandomVal (256)) / 256.0f)
#define RANDOM_COLOR_LIGHT  ((float)(200 + RandomVal (56)) / 256.0f)
#define SKY_BANDS           (sizeof (sky_pos) / sizeof (int))
#define PREFIX_COUNT        (sizeof (prefix) / sizeof (char*))
#define SUFFIX_COUNT        (sizeof (suffix) / sizeof (char*))
#define NAME_COUNT          (sizeof (name) / sizeof (char*))

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#include "gltypes.h"
#include "building.h"
#include "camera.h"
#include "car.h"
#include "light.h"
#include "macro.h"
#include "random.h"
#include "render.h"
#include "sky.h"
#include "texture.h"
#include "world.h"
#include "win.h"

static char*        prefix[] = 
{
  "i", 
  "Green ",
  "Mega",
  "Super ",
  "Omni",
  "e",
  "Hyper",
  "Global ",
  "Vital", 
  "Next ",
  "Pacific ",
  "Metro",
  "Unity ",
  "G-",
  "Trans",
  "Infinity ", 
  "Superior ",
  "Monolith ",
  "Best ",
  "Atlantic ",
  "First ",
  "Union ",
  "National ",
};

static char*        name[] = 
{
  "Biotic",
  "Info",
  "Data",
  "Solar",
  "Aerospace",
  "Motors",
  "Nano",
  "Online",
  "Circuits",
  "Energy",
  "Med",
  "Robotic",
  "Exports",
  "Security",
  "Systems",
  "Financial",
  "Industrial",
  "Media",
  "Materials",
  "Foods",
  "Networks",
  "Shipping",
  "Tools",
  "Medical",
  "Publishing",
  "Enterprises",
  "Audio",
  "Health",
  "Bank",
  "Imports",
  "Apparel",
  "Petroleum", 
  "Studios",
};

static char*        suffix[] = 
{
  "Corp",
  " Inc.",
  "Co",
  "World",
  ".Com",
  " USA",
  " Ltd.",
  "Net",
  " Tech",
  " Labs",
  " Mfg.",
  " UK",
  " Unlimited",
  " One",
  " LLC"
};
  
class CTexture
{
public:
  int               _my_id;
  unsigned          _glid;
  int               _desired_size;
  int               _size;
  int               _half;
  int               _segment_size;
  bool              _ready;
  bool              _masked;
  bool              _mipmap;
  bool              _clamp;
public:
  CTexture*         _next;
                    CTexture (int id, int size, bool mipmap, bool clamp, bool masked);
  void              Clear () { _ready = false; }
  void              Rebuild ();
  void              DrawWindows ();
  void              DrawSky ();
  void              DrawHeadlight ();
};

static CTexture*    head;
static bool         textures_done;
static bool         prefix_used[PREFIX_COUNT];
static bool         name_used[NAME_COUNT];
static bool         suffix_used[SUFFIX_COUNT];
static int          build_time;

/*-----------------------------------------------------------------------------
                          
-----------------------------------------------------------------------------*/

void drawrect_simple (int left, int top, int right, int bottom, GLrgba color)
{

  glColor3fv (&color.red);
  glBegin (GL_QUADS);
  glVertex2i (left, top);
  glVertex2i (right, top);
  glVertex2i (right, bottom);
  glVertex2i (left, bottom);
  glEnd ();

}


/*-----------------------------------------------------------------------------
                          
-----------------------------------------------------------------------------*/

void drawrect_simple (int left, int top, int right, int bottom, GLrgba color1, GLrgba color2)
{

  glColor3fv (&color1.red);
  glBegin (GL_TRIANGLE_FAN);
  glVertex2i ((left + right) / 2, (top + bottom) / 2);
  glColor3fv (&color2.red);
  glVertex2i (left, top);
  glVertex2i (right, top);
  glVertex2i (right, bottom);
  glVertex2i (left, bottom);
  glVertex2i (left, top);
  glEnd ();

}


/*-----------------------------------------------------------------------------
                          
-----------------------------------------------------------------------------*/

void drawrect (int left, int top, int right, int bottom, GLrgba color)
{

  float     average;
  float     hue;
  int       potential;
  int       repeats;
  int       height;
  int       i, j;
  bool      bright;
  GLrgba    color_noise;

  glDisable (GL_CULL_FACE);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable (GL_BLEND);
  glLineWidth (1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glColor3fv (&color.red);

  if (left == right) { //in low resolution, a "rect" might be 1 pixel wide
    glBegin (GL_LINES);
    glVertex2i (left, top);
    glVertex2i (left, bottom);
    glEnd ();
  } if (top == bottom) { //in low resolution, a "rect" might be 1 pixel wide
    glBegin (GL_LINES);
    glVertex2i (left, top);
    glVertex2i (right, top);
    glEnd ();
  } else { // draw one of those fancy 2-dimensional rectangles
    glBegin (GL_QUADS);
    glVertex2i (left, top);
    glVertex2i (right, top);
    glVertex2i (right, bottom);
    glVertex2i (left, bottom);
    glEnd ();


    average = (color.red + color.blue + color.green) / 3.0f;
    bright = average > 0.5f;
    potential = (int)(average * 255.0f);

    if (bright) {
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBegin (GL_POINTS);
      for (i = left + 1; i < right - 1; i++) {
        for (j = top + 1; j < bottom - 1; j++) {
          glColor4i (255, 0, RandomVal (potential), 255);
          hue = 0.2f + (float)RandomVal (100) / 300.0f + (float)RandomVal (100) / 300.0f + (float)RandomVal (100) / 300.0f;
          color_noise = glRgbaFromHsl (hue, 0.3f, 0.5f);
          color_noise.alpha = (float)RandomVal (potential) / 144.0f;
          glColor4f (RANDOM_COLOR_VAL, RANDOM_COLOR_VAL, RANDOM_COLOR_VAL, (float)RandomVal (potential) / 144.0f);
          glColor4fv (&color_noise.red);
          glVertex2i (i, j);
        }
      }
      glEnd ();
    }
    repeats = RandomVal (6) + 1;
    height = (bottom - top) + (RandomVal (3) - 1) + (RandomVal(3) - 1);
    for (i = left; i < right; i++) {
      if (RandomVal (3) == 0)
        repeats = RandomVal (4) + 1;
      if (RandomVal (6) == 0) {
        height = bottom - top;
        height = RandomVal (height);
        height = RandomVal (height);
        height = RandomVal (height);
        height = ((bottom - top) + height) / 2;
      }
      for (j = 0; j < 1; j++) {
        glBegin (GL_LINES);
        glColor4f (0, 0, 0, (float)RandomVal (256) / 256.0f);
        glVertex2i (i, bottom - height);
        glColor4f (0, 0, 0, (float)RandomVal (256) / 256.0f);
        glVertex2i (i, bottom);
        glEnd ();
      }
    }
  }
}

/*-----------------------------------------------------------------------------
                          
-----------------------------------------------------------------------------*/

static void window (int x, int y, int size, int id, GLrgba color)
{

  int     margin;
  int     half;
  int     i;

  margin = size / 3;
  half = size / 2;
  switch (id) {
  case TEXTURE_BUILDING1: //filled, 1-pixel frame
    drawrect (x + 1, y + 1, x + size - 1, y + size - 1, color);
    break;
  case TEXTURE_BUILDING2: //vertical
    drawrect (x + margin, y + 1, x + size - margin, y + size - 1, color);
    break;
  case TEXTURE_BUILDING3: //side-by-side pair
    drawrect (x + 1, y + 1, x + half - 1, y + size - margin, color);
    drawrect (x + half + 1, y + 1, x + size - 1, y + size - margin,  color);
    break;
  case TEXTURE_BUILDING4: //windows with blinds
    drawrect (x + 1, y + 1, x + size - 1, y + size - 1, color);
    i = RandomVal (size - 2);
    drawrect (x + 1, y + 1, x + size - 1, y + i + 1, color * 0.3f);

    break;
  case TEXTURE_BUILDING5: //vert stripes
    drawrect (x + 1, y + 1, x + size - 1, y + size - 1, color);
    drawrect (x + margin, y + 1, x + margin, y + size - 1, color * 0.7f);
    drawrect (x + size - margin - 1, y + 1, x + size - margin - 1, y + size - 1, color * 0.3f);
    break;
  case TEXTURE_BUILDING6: //wide horz line
    drawrect (x + 1, y + 1, x + size - 1, y + size - margin, color);
    break;
  case TEXTURE_BUILDING7: //4-pane
    drawrect (x + 2, y + 1, x + size - 1, y + size - 1, color);
    drawrect (x + 2, y + half, x + size - 1, y + half, color * 0.2f);
    drawrect (x + half, y + 1, x + half, y + size - 1, color * 0.2f);
    break;
  case TEXTURE_BUILDING8: // Single narrow window
    drawrect (x + half - 1, y + 1, x + half + 1, y + size - margin, color);
    break;
  case TEXTURE_BUILDING9: //horizontal
    drawrect (x + 1, y + margin, x + size - 1, y + size - margin - 1, color);
    break;
  }

}

/*-----------------------------------------------------------------------------
                          
-----------------------------------------------------------------------------*/

static void do_bloom (CTexture* t)
{

  glBindTexture(GL_TEXTURE_2D, 0);		
  glViewport(0, 0, t->_size , t->_size);
  glCullFace (GL_BACK);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthMask (true);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable (GL_FOG);
  glFogf (GL_FOG_START, RenderFogDistance () / 2);
  glFogf (GL_FOG_END, RenderFogDistance ());
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable (GL_TEXTURE_2D);
  EntityRender ();
  CarRender ();
  LightRender ();
  glBindTexture(GL_TEXTURE_2D, t->_glid);		
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glCopyTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, t->_size, t->_size, 0);

}

/*-----------------------------------------------------------------------------
                          
-----------------------------------------------------------------------------*/

CTexture::CTexture (int id, int size, bool mipmap, bool clamp, bool masked)
{

  glGenTextures (1, &_glid); 
  _my_id = id;
  _mipmap = mipmap;
  _clamp = clamp;
  _masked = masked;
  _desired_size = size;
  _size = size;
  _half = size / 2;
  _segment_size = size / SEGMENTS_PER_TEXTURE;
  _ready = false;
  _next = head;
  head = this;

}

/*-----------------------------------------------------------------------------

  This draws all of the windows on a building texture. lit_density controls 
  how many lights are on. (1 in n chance that the light is on. Higher values 
  mean less lit windows. run_length controls how often it will consider 
  changing the lit / unlit status. 1 produces a complete scatter, higher
  numbers make long strings of lights.
  
-----------------------------------------------------------------------------*/

void CTexture::DrawWindows ()
{


  int         x, y;
  int         run;
  int         run_length;
  int         lit_density;
  GLrgba      color;
  bool        lit;

  //color = glRgbaUnique (_my_id);
  for (y = 0; y < SEGMENTS_PER_TEXTURE; y++)  {
    //Every few floors we change the behavior
    if (!(y % 8)) {
      run = 0;
      run_length = RandomVal (9) + 2;
      lit_density = 2 + RandomVal(2) + RandomVal(2);
      lit = false;
    }
    for (x = 0; x < SEGMENTS_PER_TEXTURE; x++) {
      //if this run is over reroll lit and start a new one
      if (run < 1) {
        run = RandomVal (run_length);
        lit = RandomVal (lit_density) == 0;
        //if (lit)
          //color = glRgba (0.5f + (float)(RandomVal () % 128) / 256.0f) + glRgba (RANDOM_COLOR_SHIFT, RANDOM_COLOR_SHIFT, RANDOM_COLOR_SHIFT);
      }
      if (lit) 
        color = glRgba (0.5f + (float)(RandomVal () % 128) / 256.0f) + glRgba (RANDOM_COLOR_SHIFT, RANDOM_COLOR_SHIFT, RANDOM_COLOR_SHIFT);
       else 
        color = glRgba ((float)(RandomVal () % 40) / 256.0f);
      window (x * _segment_size, y * _segment_size, _segment_size, _my_id, color);
      run--;

    }
  }

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CTexture::DrawSky ()
{

  GLrgba          color;
  float           grey;
  float           scale, inv_scale;
  int             i, x, y;
  int             width, height;
  int             offset;
  int             width_adjust;
  int             height_adjust;

  color = WorldBloomColor ();
  grey = (color.red + color.green + color.blue) / 3.0f;
  //desaturate, slightly dim
  color = (color + glRgba (grey) * 2.0f) / 15.0f;
  glDisable (GL_BLEND);
  glBegin (GL_QUAD_STRIP);
  glColor3f (0,0,0);
  glVertex2i (0, _half);
  glVertex2i (_size, _half);
  glColor3fv (&color.red);
  glVertex2i (0, _size - 2);  
  glVertex2i (_size, _size - 2);  
  glEnd ();
  //Draw a bunch of little faux-buildings on the horizon.
  for (i = 0; i < _size; i += 5) 
    drawrect (i, _size - RandomVal (8) - RandomVal (8) - RandomVal (8), i + RandomVal (9), _size, glRgba (0.0f));
  //Draw the clouds
  for (i = _size - 30; i > 5; i -= 2) {

    x = RandomVal (_size);
    y = i;

    scale = 1.0f - ((float)y / (float)_size);
    width = RandomVal (_half / 2) + (int)((float)_half * scale) / 2;
    scale = 1.0f - (float)y / (float)_size;
    height = (int)((float)(width) * scale);
    height = MAX (height, 4);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable (GL_CULL_FACE);
    glEnable (GL_TEXTURE_2D);
    glBindTexture (GL_TEXTURE_2D, TextureId (TEXTURE_SOFT_CIRCLE));
    glDepthMask (false);
    glBegin (GL_QUADS);
    for (offset = -_size; offset <= _size; offset += _size) {
      for (scale = 1.0f; scale > 0.0f; scale -= 0.25f) {

        inv_scale = 1.0f - (scale);
        if (scale < 0.4f)
          color = WorldBloomColor () * 0.1f;
        else
          color = glRgba (0.0f);
        color.alpha = 0.2f;
        glColor4fv (&color.red);
        width_adjust = (int)((float)width / 2.0f + (int)(inv_scale * ((float)width / 2.0f)));
        height_adjust = height + (int)(scale * (float)height * 0.99f);
        glTexCoord2f (0, 0);   glVertex2i (offset + x - width_adjust, y + height - height_adjust);
        glTexCoord2f (0, 1);   glVertex2i (offset + x - width_adjust, y + height);
        glTexCoord2f (1, 1);   glVertex2i (offset + x + width_adjust, y + height);
        glTexCoord2f (1, 0);   glVertex2i (offset + x + width_adjust, y + height - height_adjust);
      }

    }
  }
  glEnd ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CTexture::DrawHeadlight ()
{

  float           radius;
  int             i, x, y;
  GLvector2       pos;

  //Make a simple circle of light, bright in the center and fading out
  radius = ((float)_half) - 20;
  x = _half - 20;
  y = _half;
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin (GL_TRIANGLE_FAN);
  glColor4f (0.8f, 0.8f, 0.8f, 0.6f);
  glVertex2i (_half - 5, y);
  glColor4f (0, 0, 0, 0);
  for (i = 0; i <= 360; i += 36) {
    pos.x = sinf ((float)(i % 360) * DEGREES_TO_RADIANS) * radius;
    pos.y = cosf ((float)(i % 360) * DEGREES_TO_RADIANS) * radius;
    glVertex2i (x + (int)pos.x, _half + (int)pos.y);
  }
  glEnd ();
  x = _half + 20;
  glBegin (GL_TRIANGLE_FAN);
  glColor4f (0.8f, 0.8f, 0.8f, 0.6f);
  glVertex2i (_half + 5, y);
  glColor4f (0, 0, 0, 0);
  for (i = 0; i <= 360; i += 36) {
    pos.x = sinf ((float)(i % 360) * DEGREES_TO_RADIANS) * radius;
    pos.y = cosf ((float)(i % 360) * DEGREES_TO_RADIANS) * radius;
    glVertex2i (x + (int)pos.x, _half + (int)pos.y);
  }
  glEnd ();
  x = _half - 6;
  drawrect_simple (x - 3, y - 2, x + 2, y + 2, glRgba (1.0f));
  x = _half + 6;
  drawrect_simple (x - 2, y - 2, x + 3, y + 2, glRgba (1.0f));

}

/*-----------------------------------------------------------------------------

  Here is where ALL of the procedural textures are created.  It's filled with 
  obscure logic, magic numbers, and messy code. Part of this is because 
  there is a lot of "art" being done here, and lots of numbers that could be 
  endlessly tweaked.  Also because I'm lazy.
                    
-----------------------------------------------------------------------------*/

void CTexture::Rebuild ()
{

  int             i, j;
  int             x, y;
  int             name_num, prefix_num, suffix_num;
  int             max_size;
  float           radius;
  GLvector2       pos;
  bool            use_framebuffer;
  unsigned char*  bits;
  unsigned        start;
  int             lapsed;

  start = GetTickCount ();
  //Since we make textures by drawing into the viewport, we can't make them bigger 
  //than the current view.
  _size = _desired_size;
  max_size = RenderMaxTextureSize ();
  while (_size > max_size)
    _size /= 2;
  glBindTexture(GL_TEXTURE_2D, _glid);
  //Set up the texture
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, _size, _size, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  if (_clamp) {
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  //Set up our viewport so that drawing into our texture will be as easy 
  //as possible.  We make the viewport and projection simply match the given 
  //texture size. 
  glViewport(0, 0, _size , _size);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0, _size, _size, 0, 0.1f, 2048);
	glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity();
  glDisable (GL_CULL_FACE);
  glDisable (GL_FOG);
  glBindTexture(GL_TEXTURE_2D, 0);
  glTranslatef(0, 0, -10.0f);
  glClearColor (0, 0, 0, _masked ? 0.0f : 1.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  use_framebuffer = true;
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  switch (_my_id) {
  case TEXTURE_LATTICE:
    glLineWidth (2.0f);

    glColor3f (0,0,0);
    glBegin (GL_LINES);
    glVertex2i (0, 0);  glVertex2i (_size, _size);//diagonal
    glVertex2i (0, 0);  glVertex2i (0, _size);//vertical
    glVertex2i (0, 0);  glVertex2i (_size, 0);//vertical
    glEnd ();
    glBegin (GL_LINE_STRIP);
    glVertex2i (0, 0);    
    for (i = 0; i < _size; i += 9) {
      if (i % 2)
        glVertex2i (0, i);    
      else
        glVertex2i (i, i);    
    }
    for (i = 0; i < _size; i += 9) {
      if (i % 2)
        glVertex2i (i, 0);    
      else
        glVertex2i (i, i);    
    }
    glEnd ();
    break;
  case TEXTURE_SOFT_CIRCLE:
    //Make a simple circle of light, bright in the center and fading out
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    radius = ((float)_half) - 3;
    glBegin (GL_TRIANGLE_FAN);
    glColor4f (1, 1, 1, 1);
    glVertex2i (_half, _half);
    glColor4f (0, 0, 0, 0);
    for (i = 0; i <= 360; i++) {
      pos.x = sinf ((float)i * DEGREES_TO_RADIANS) * radius;
      pos.y = cosf ((float)i * DEGREES_TO_RADIANS) * radius;
      glVertex2i (_half + (int)pos.x, _half + (int)pos.y);
    }
    glEnd ();
    break;
  case TEXTURE_LIGHT:
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    radius = ((float)_half) - 3;
    for (j = 0; j < 2; j++) {
      glBegin (GL_TRIANGLE_FAN);
      glColor4f (1, 1, 1, 1);
      glVertex2i (_half, _half);
      if (!j)
        radius = ((float)_half / 2);
      else
        radius = 8;
      glColor4f (1, 1, 1, 0);
      for (i = 0; i <= 360; i++) {
        pos.x = sinf ((float)i * DEGREES_TO_RADIANS) * radius;
        pos.y = cosf ((float)i * DEGREES_TO_RADIANS) * radius;
        glVertex2i (_half + (int)pos.x, _half + (int)pos.y);
      }
      glEnd ();
    }
    break;
  case TEXTURE_HEADLIGHT:
    DrawHeadlight ();
    break;
  case TEXTURE_LOGOS:
    i = 0;
    glDepthMask (false);
    glDisable (GL_BLEND);
    name_num = RandomVal (NAME_COUNT);
    prefix_num = RandomVal (PREFIX_COUNT);
    suffix_num = RandomVal (SUFFIX_COUNT);
    glColor3f (1,1,1);
    while (i < _size) {
      //randomly use a prefix OR suffix, but not both.  Too verbose.
      if (COIN_FLIP)
        RenderPrint (2, _size - i - LOGO_PIXELS / 4, RandomVal(), glRgba (1.0f), "%s%s", prefix[prefix_num], name[name_num]);
      else
        RenderPrint (2, _size - i - LOGO_PIXELS / 4, RandomVal(), glRgba (1.0f), "%s%s", name[name_num], suffix[suffix_num]);
      name_num = (name_num + 1) % NAME_COUNT;
      prefix_num = (prefix_num + 1) % PREFIX_COUNT;
      suffix_num = (suffix_num + 1) % SUFFIX_COUNT;
      i += LOGO_PIXELS;
    }
    break;
  case TEXTURE_TRIM:
    int     margin;
    y = 0;
    margin = MAX (TRIM_PIXELS / 4, 1);
    for (x = 0; x < _size; x += TRIM_PIXELS) 
      drawrect_simple (x + margin, y + margin, x + TRIM_PIXELS - margin, y + TRIM_PIXELS - margin, glRgba (1.0f), glRgba (0.5f));
    y += TRIM_PIXELS;
    for (x = 0; x < _size; x += TRIM_PIXELS * 2) 
      drawrect_simple (x + margin, y + margin, x + TRIM_PIXELS - margin, y + TRIM_PIXELS - margin, glRgba (1.0f), glRgba (0.5f));
    y += TRIM_PIXELS;
    for (x = 0; x < _size; x += TRIM_PIXELS * 3) 
      drawrect_simple (x + margin, y + margin, x + TRIM_PIXELS - margin, y + TRIM_PIXELS - margin, glRgba (1.0f), glRgba (0.5f));
    y += TRIM_PIXELS;
    for (x = 0; x < _size; x += TRIM_PIXELS) 
      drawrect_simple (x + margin, y + margin * 2, x + TRIM_PIXELS - margin, y + TRIM_PIXELS - margin, glRgba (1.0f), glRgba (0.5f));
    break;
  case TEXTURE_SKY:
    DrawSky ();
    break;
  default: //building textures
    DrawWindows ();
    break;
  }
  glPopMatrix ();
  //Now blit the finished image into our texture  
  if (use_framebuffer) {
    glBindTexture(GL_TEXTURE_2D, _glid);		
	  glCopyTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, _size, _size, 0);
  }
  if (_mipmap) {
    bits = (unsigned char*)malloc (_size * _size * 4);
    glGetTexImage (GL_TEXTURE_2D,	0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, _size, _size, GL_RGBA, GL_UNSIGNED_BYTE, bits);
    free (bits);
	  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  } else
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  //cleanup and restore the viewport
  RenderResize ();  
  _ready = true;
  lapsed = GetTickCount () - start;
  build_time += lapsed;
    

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

unsigned TextureId (int id)
{

  for (CTexture* t = head; t; t = t->_next) {
    if (t->_my_id == id)
      return t->_glid;
  }
  return 0;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

unsigned TextureRandomBuilding (int index)
{

  index = abs (index) % BUILDING_COUNT;
  return TextureId (TEXTURE_BUILDING1 + index);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void TextureReset (void)
{

  textures_done = false;
  build_time = 0;
  for (CTexture* t = head; t; t = t->_next)
    t->Clear ();
  ZeroMemory (prefix_used, sizeof (prefix_used));
  ZeroMemory (name_used, sizeof (name_used));
  ZeroMemory (suffix_used, sizeof (suffix_used));

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool TextureReady ()
{

  return textures_done;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void TextureUpdate (void)
{

  if (textures_done) {
    if (!RenderBloom ())
      return;
    CTexture*   t;

    for (t = head; t; t = t->_next) {
      if (t->_my_id != TEXTURE_BLOOM) 
        continue;
      do_bloom (t);
      return;
    }
  }
  for (CTexture* t = head; t; t = t->_next) {
    if (!t->_ready) {
      t->Rebuild();
      return;
    }
  } 
  textures_done = true;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void TextureTerm (void)
{

  CTexture*    t;

  while (head) {
    t = head->_next;
    free (head);
    head = t;
  }

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void TextureInit (void)
{

  new CTexture (TEXTURE_SKY,          512,  true,  false, false);
  new CTexture (TEXTURE_LATTICE,      128,  true,  true,  true);
  new CTexture (TEXTURE_LIGHT,        128,  false, false, true);
  new CTexture (TEXTURE_SOFT_CIRCLE,  128,  false, false, true);
  new CTexture (TEXTURE_HEADLIGHT,    128,  false, false, true);
  new CTexture (TEXTURE_TRIM,  TRIM_RESOLUTION,  true, false, false);
  new CTexture (TEXTURE_LOGOS, LOGO_RESOLUTION,  true, false, true);
  for (int i = TEXTURE_BUILDING1; i <= TEXTURE_BUILDING9; i++)
    new CTexture (i, 512, true, false, false);
  new CTexture (TEXTURE_BLOOM,  512,  true, false, false);
  int  names = PREFIX_COUNT * NAME_COUNT + SUFFIX_COUNT * NAME_COUNT;

}
