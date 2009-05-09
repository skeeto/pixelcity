/*-----------------------------------------------------------------------------

  Render.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------
  
  This is the core of the gl rendering functions.  This contains the main 
  rendering function RenderUpdate (), which initiates the various 
  other renders in the other modules. 

-----------------------------------------------------------------------------*/

#define RENDER_DISTANCE     1280
#define MAX_TEXT            256
#define YOUFAIL(message)    {WinPopup (message);return;}
#define HELP_SIZE           sizeof(help)
#define COLOR_CYCLE_TIME    10000 //milliseconds
#define COLOR_CYCLE         (COLOR_CYCLE_TIME / 4)
#define FONT_COUNT          (sizeof (fonts) / sizeof (struct glFont))
#define FONT_SIZE           (LOGO_PIXELS - LOGO_PIXELS / 8)
#define BLOOM_SCALING       0.07f

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>

#include <gl\gl.h>
#include <gl\glu.h>

#include "gltypes.h"
#include "entity.h"
#include "car.h"
#include "camera.h"
#include "ini.h"
#include "light.h"
#include "macro.h"
#include "math.h"
#include "render.h"
#include "sky.h"
#include "texture.h"
#include "world.h"
#include "win.h"

static	PIXELFORMATDESCRIPTOR pfd =			
{
	sizeof(PIXELFORMATDESCRIPTOR),			
	1,											  // Version Number
	PFD_DRAW_TO_WINDOW |			// Format Must Support Window
	PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
	PFD_DOUBLEBUFFER,					// Must Support Double Buffering
	PFD_TYPE_RGBA,						// Request An RGBA Format
	32,										    // Select Our glRgbaDepth
	0, 0, 0, 0, 0, 0,					// glRgbaBits Ignored
	0,											  // No Alpha Buffer
	0,											  // Shift Bit Ignored
	0,											  // Accumulation Buffers
	0, 0, 0, 0,								// Accumulation Bits Ignored
	16,											  // Z-Buffer (Depth Buffer)  bits
	0,											  // Stencil Buffers
	1,											  // Auxiliary Buffers
	PFD_MAIN_PLANE,						// Main Drawing Layer
	0,											  // Reserved
	0, 0, 0										// Layer Masks Ignored
};

static char             help[] = 
  "ESC - Exit!\n" 
  "F1  - Show this help screen\n" 
  "R   - Rebuild city\n" 
  "L   - Toggle 'letterbox' mode\n"
  "F   - Show Framecounter\n"
  "W   - Toggle Wireframe\n"
  "E   - Change full-scene effects\n"
  "T   - Toggle Textures\n"
  "G   - Toggle Fog\n"
;

struct glFont
{
  char*         name;
  unsigned		  base_char;
} fonts[] = 
{
  "Courier New",      0,
  "Arial",            0,
  "Times New Roman",  0,
  "Arial Black",      0,
  "Impact",           0,
  "Agency FB",        0,
  "Book Antiqua",     0,
};

#if SCREENSAVER
enum
{
  EFFECT_NONE,
  EFFECT_BLOOM,
  EFFECT_BLOOM_RADIAL,
  EFFECT_COLOR_CYCLE,
  EFFECT_GLASS_CITY,
  EFFECT_COUNT,
  EFFECT_DEBUG,
  EFFECT_DEBUG_OVERBLOOM,
};
#else
enum
{
  EFFECT_NONE,
  EFFECT_BLOOM,
  EFFECT_COUNT,
  EFFECT_DEBUG_OVERBLOOM,
  EFFECT_DEBUG,
  EFFECT_BLOOM_RADIAL,
  EFFECT_COLOR_CYCLE,
  EFFECT_GLASS_CITY,
};
#endif 

static HDC			        hDC;
static HGLRC		        hRC;
static float            render_aspect;
static float            fog_distance;
static int              render_width;
static int              render_height;
static bool             letterbox;
static int              letterbox_offset;
static int              effect;
static unsigned         next_fps;
static unsigned         current_fps;
static unsigned         frames;
static bool             show_wireframe;
static bool             flat;
static bool             show_fps;
static bool             show_fog;
static bool             show_help;

/*-----------------------------------------------------------------------------

  Draw a clock-ish progress.. widget... thing.  It's cute.

-----------------------------------------------------------------------------*/

static void do_progress (float center_x, float center_y, float radius, float opacity, float progress)
{

  int     i;
  int     end_angle;
  float   inner, outer;
  float   angle;
  float   s, c;
  float   gap;

  //Outer Ring
  gap = radius * 0.05f;
  outer = radius;
  inner = radius - gap * 2;
  glColor4f (1,1,1, opacity);
  glBegin (GL_QUAD_STRIP);
  for (i = 0; i <= 360; i+= 15) {
    angle = (float)i * DEGREES_TO_RADIANS;
    s = sinf (angle);
    c = -cosf (angle);
    glVertex2f (center_x + s * outer, center_y + c * outer);
    glVertex2f (center_x + s * inner, center_y + c * inner);
  }
  glEnd ();
  //Progress indicator
  glColor4f (1,1,1, opacity);
  end_angle = (int)(360 * progress);
  outer = radius - gap * 3;
  glBegin (GL_TRIANGLE_FAN);
  glVertex2f (center_x, center_y);
  for (i = 0; i <= end_angle; i+= 3) {
    angle = (float)i * DEGREES_TO_RADIANS;
    s = sinf (angle);
    c = -cosf (angle);
    glVertex2f (center_x + s * outer, center_y + c * outer);
  }
  glEnd ();
  //Tic lines
  glLineWidth (2.0f);
  outer = radius - gap * 1;
  inner = radius - gap * 2;
  glColor4f (0,0,0, opacity);
  glBegin (GL_LINES);
  for (i = 0; i <= 360; i+= 15) {
    angle = (float)i * DEGREES_TO_RADIANS;
    s = sinf (angle);
    c = -cosf (angle);
    glVertex2f (center_x + s * outer, center_y + c * outer);
    glVertex2f (center_x + s * inner, center_y + c * inner);
  }
  glEnd ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static void do_effects (int type)
{

  float           hue1, hue2, hue3, hue4;
  GLrgba          color;
  float           fade;
  int             radius;
  int             x, y;
  int             i;
  int             bloom_radius;
  int             bloom_step;
  
  fade = WorldFade ();
  bloom_radius = 15;
  bloom_step = bloom_radius / 3;
  if (!TextureReady ())
    return;
  //Now change projection modes so we can render full-screen effects
  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  glOrtho (0, render_width, render_height, 0, 0.1f, 2048);
	glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity();
  glTranslatef(0, 0, -1.0f);				
  glDisable (GL_CULL_FACE);
  glDisable (GL_FOG);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //Render full-screen effects
  glBlendFunc (GL_ONE, GL_ONE);
  glEnable (GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDepthMask (false);
  glBindTexture(GL_TEXTURE_2D, TextureId (TEXTURE_BLOOM));
  switch (type) {
  case EFFECT_DEBUG:
    glBindTexture(GL_TEXTURE_2D, TextureId (TEXTURE_LOGOS));
    glDisable (GL_BLEND);
    glBegin (GL_QUADS);
    glColor3f (1, 1, 1);
    glTexCoord2f (0, 0);  glVertex2i (0, render_height / 4);
    glTexCoord2f (0, 1);  glVertex2i (0, 0);
    glTexCoord2f (1, 1);  glVertex2i (render_width / 4, 0);
    glTexCoord2f (1, 0);  glVertex2i (render_width / 4, render_height / 4);

    glTexCoord2f (0, 0);  glVertex2i (0, 512);
    glTexCoord2f (0, 1);  glVertex2i (0, 0);
    glTexCoord2f (1, 1);  glVertex2i (512, 0);
    glTexCoord2f (1, 0);  glVertex2i (512, 512);
    glEnd ();
    break;
  case EFFECT_BLOOM_RADIAL:
    //Psychedelic bloom
    glEnable (GL_BLEND);
    glBegin (GL_QUADS);
    color = WorldBloomColor () * BLOOM_SCALING * 2;
    glColor3fv (&color.red);
    for (i = 0; i <= 100; i+=10) {
      glTexCoord2f (0, 0);  glVertex2i (-i, i + render_height);
      glTexCoord2f (0, 1);  glVertex2i (-i, -i);
      glTexCoord2f (1, 1);  glVertex2i (i + render_width, -i);
      glTexCoord2f (1, 0);  glVertex2i (i + render_width, i + render_height);
    }
    glEnd ();
    break;
  case EFFECT_COLOR_CYCLE:
    //Oooh. Pretty colors.  Tint the scene according to screenspace.
    hue1 = (float)(GetTickCount () % COLOR_CYCLE_TIME) / COLOR_CYCLE_TIME;
    hue2 = (float)((GetTickCount () + COLOR_CYCLE) % COLOR_CYCLE_TIME) / COLOR_CYCLE_TIME;
    hue3 = (float)((GetTickCount () + COLOR_CYCLE * 2) % COLOR_CYCLE_TIME) / COLOR_CYCLE_TIME;
    hue4 = (float)((GetTickCount () + COLOR_CYCLE * 3) % COLOR_CYCLE_TIME) / COLOR_CYCLE_TIME;
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE);
    glBlendFunc (GL_DST_COLOR, GL_SRC_COLOR);
    glBegin (GL_QUADS);
    color = glRgbaFromHsl (hue1, 1.0f, 0.6f);
    glColor3fv (&color.red);
    glTexCoord2f (0, 0);  glVertex2i (0, render_height);
    color = glRgbaFromHsl (hue2, 1.0f, 0.6f);
    glColor3fv (&color.red);
    glTexCoord2f (0, 1);  glVertex2i (0, 0);
    color = glRgbaFromHsl (hue3, 1.0f, 0.6f);
    glColor3fv (&color.red);
    glTexCoord2f (1, 1);  glVertex2i (render_width, 0);
    color = glRgbaFromHsl (hue4, 1.0f, 0.6f);
    glColor3fv (&color.red);
    glTexCoord2f (1, 0);  glVertex2i (render_width, render_height);
    glEnd ();
    break;
  case EFFECT_BLOOM:
    //Simple bloom effect
    glBegin (GL_QUADS);
    color = WorldBloomColor () * BLOOM_SCALING;
    glColor3fv (&color.red);
    for (x = -bloom_radius; x <= bloom_radius; x += bloom_step) {
      for (y = -bloom_radius; y <= bloom_radius; y += bloom_step) {
        if (abs (x) == abs (y) && x)
          continue;
        glTexCoord2f (0, 0);  glVertex2i (x, y + render_height);
        glTexCoord2f (0, 1);  glVertex2i (x, y);
        glTexCoord2f (1, 1);  glVertex2i (x + render_width, y);
        glTexCoord2f (1, 0);  glVertex2i (x + render_width, y + render_height);
      }
    }
    glEnd ();
    break;
  case EFFECT_DEBUG_OVERBLOOM:
    //This will punish that uppity GPU. Good for testing low frame rate behavior.
    glBegin (GL_QUADS);
    color = WorldBloomColor () * 0.01f;
    glColor3fv (&color.red);
    for (x = -50; x <= 50; x+=5) {
      for (y = -50; y <= 50; y+=5) {
        glTexCoord2f (0, 0);  glVertex2i (x, y + render_height);
        glTexCoord2f (0, 1);  glVertex2i (x, y);
        glTexCoord2f (1, 1);  glVertex2i (x + render_width, y);
        glTexCoord2f (1, 0);  glVertex2i (x + render_width, y + render_height);
      }
    }
    glEnd ();
    break;
  }
  //Do the fade to / from darkness used to hide scene transitions
  if (LOADING_SCREEN) {
    if (fade > 0.0f) {
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable (GL_BLEND);
      glDisable (GL_TEXTURE_2D);
      glColor4f (0, 0, 0, fade);
      glBegin (GL_QUADS);
      glVertex2i (0, 0);
      glVertex2i (0, render_height);
      glVertex2i (render_width, render_height);
      glVertex2i (render_width, 0);
      glEnd ();
    }
    if (TextureReady () && !EntityReady () && fade != 0.0f) {
      radius = render_width / 16;
      do_progress ((float)render_width / 2, (float)render_height / 2, (float)radius, fade, EntityProgress ());
      RenderPrint (render_width / 2 - LOGO_PIXELS, render_height / 2 + LOGO_PIXELS, 0, glRgba (0.5f), "%1.2f%%", EntityProgress () * 100.0f);
      RenderPrint (1, "%s v%d.%d.%03d", APP_TITLE, VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
    }
  }
  glPopMatrix ();
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int RenderMaxTextureSize ()
{

  int mts;

  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mts);
  mts = MIN (mts, render_width);
  return MIN (mts, render_height);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderPrint (int x, int y, int font, GLrgba color, const char *fmt, ...)				
{

  char		  text[MAX_TEXT];	
  va_list		ap;					
  
  text[0] = 0;
  if (fmt == NULL)			
		  return;						
  va_start(ap, fmt);		
  vsprintf(text, fmt, ap);				
  va_end(ap);		
  glPushAttrib(GL_LIST_BIT);				
  glListBase(fonts[font % FONT_COUNT].base_char - 32);				
  glColor3fv (&color.red);
	glRasterPos2i (x, y);
  glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderPrint (int line, const char *fmt, ...)				
{

  char		  text[MAX_TEXT];	
	va_list		ap;			
	
  text[0] = 0;	
  if (fmt == NULL)			
		  return;						
  va_start (ap, fmt);		
  vsprintf (text, fmt, ap);				
  va_end (ap);		
  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  glOrtho (0, render_width, render_height, 0, 0.1f, 2048);
  glDisable(GL_DEPTH_TEST);
  glDepthMask (false);
	glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity();
  glTranslatef(0, 0, -1.0f);				
  glDisable (GL_BLEND);
  glDisable (GL_FOG);
  glDisable (GL_TEXTURE_2D);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  RenderPrint (0, line * FONT_SIZE - 2, 0, glRgba (0.0f), text);
  RenderPrint (4, line * FONT_SIZE + 2, 0, glRgba (0.0f), text);
  RenderPrint (2, line * FONT_SIZE, 0, glRgba (1.0f), text);
  glPopAttrib();						
  glPopMatrix ();
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void static do_help (void)
{

  char*     text;
  int       line;
  char      parse[HELP_SIZE];
  int       x;
  
  strcpy (parse, help);
  line = 0;
  text = strtok (parse, "\n");
  x = 10;
  while (text) {
    RenderPrint (line + 2, text);
    text = strtok (NULL, "\n");
    line++;
  }

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void do_fps ()
{

  LIMIT_INTERVAL (1000);
  current_fps = frames;
  frames = 0;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderResize (void)		
{

  float     fovy;

  render_width = WinWidth ();
  render_height = WinHeight ();
  if (letterbox) {
    letterbox_offset = render_height / 6;
    render_height = render_height - letterbox_offset * 2;
  } else 
    letterbox_offset = 0;
  //render_aspect = (float)render_height / (float)render_width;
  glViewport (0, letterbox_offset, render_width, render_height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  render_aspect = (float)render_width / (float)render_height;
  fovy = 60.0f;
  if (render_aspect > 1.0f) 
    fovy /= render_aspect; 
  gluPerspective (fovy, render_aspect, 0.1f, RENDER_DISTANCE);
	glMatrixMode (GL_MODELVIEW);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderTerm (void)
{

  if (!hRC)
    return;
  wglDeleteContext (hRC);
  hRC = NULL;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderInit (void)
{

  HWND              hWnd;
	unsigned		      PixelFormat;
  HFONT	            font;		
	HFONT	            oldfont;

  hWnd = WinHwnd ();
  if (!(hDC = GetDC (hWnd))) 
		YOUFAIL ("Can't Create A GL Device Context.") ;
	if (!(PixelFormat = ChoosePixelFormat(hDC,&pfd)))
		YOUFAIL ("Can't Find A Suitable PixelFormat.") ;
  if(!SetPixelFormat(hDC,PixelFormat,&pfd))
		YOUFAIL ("Can't Set The PixelFormat.");
	if (!(hRC = wglCreateContext (hDC)))	
		YOUFAIL ("Can't Create A GL Rendering Context.");
  if(!wglMakeCurrent(hDC,hRC))	
		YOUFAIL ("Can't Activate The GL Rendering Context.");
  //Load the fonts for printing debug info to the window.
  for (int i = 0; i < FONT_COUNT; i++) {
	  fonts[i].base_char = glGenLists(96); 
	  font = CreateFont (FONT_SIZE,	0, 0,	0,	
				  FW_BOLD, FALSE,	FALSE, FALSE,	DEFAULT_CHARSET,	OUT_TT_PRECIS,		
				  CLIP_DEFAULT_PRECIS,	ANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH,
				  fonts[i].name);
	  oldfont = (HFONT)SelectObject(hDC, font);	
	  wglUseFontBitmaps(hDC, 32, 96, fonts[i].base_char);
	  SelectObject(hDC, oldfont);
	  DeleteObject(font);		
  }
  //If the program is running for the first time, set the defaults.
  if (!IniInt ("SetDefaults")) {
    IniIntSet ("SetDefaults", 1);
    IniIntSet ("Effect", EFFECT_BLOOM);
    IniIntSet ("ShowFog", 1);
  }
  //load in our settings
  letterbox = IniInt ("Letterbox") != 0;
  show_wireframe = IniInt ("Wireframe") != 0;
  show_fps = IniInt ("ShowFPS") != 0;
  show_fog = IniInt ("ShowFog") != 0;
  effect = IniInt ("Effect");
  flat = IniInt ("Flat") != 0;
  fog_distance = WORLD_HALF;
  //clear the viewport so the user isn't looking at trash while the program starts
  glViewport (0, 0, WinWidth (), WinHeight ());
  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SwapBuffers (hDC);
  RenderResize ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderFPSToggle ()
{

  show_fps = !show_fps;
  IniIntSet ("ShowFPS", show_fps ? 1 : 0);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool RenderFog ()
{

  return show_fog;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderFogToggle ()
{

  show_fog = !show_fog;
  IniIntSet ("ShowFog", show_fog ? 1 : 0);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderLetterboxToggle ()
{

  letterbox = !letterbox;
  IniIntSet ("Letterbox", letterbox ? 1 : 0);
  RenderResize ();


}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderWireframeToggle ()
{

  show_wireframe = !show_wireframe;
  IniIntSet ("Wireframe", show_wireframe ? 1 : 0);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool RenderWireframe ()
{

  return show_wireframe;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderEffectCycle ()
{

  effect = (effect + 1) % EFFECT_COUNT;
  IniIntSet ("Effect", effect);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool RenderBloom ()
{

  return effect == EFFECT_BLOOM || effect == EFFECT_BLOOM_RADIAL 
    || effect == EFFECT_DEBUG_OVERBLOOM || effect == EFFECT_COLOR_CYCLE;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool RenderFlat ()
{

  return flat;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderFlatToggle ()
{

  flat = !flat;
  IniIntSet ("Flat", flat ? 1 : 0);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderHelpToggle ()
{

  show_help = !show_help;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

float RenderFogDistance ()
{

  return fog_distance;

}

/*-----------------------------------------------------------------------------

  This is used to set a gradient fog that goes from camera to some portion of 
  the normal fog distance.  This is used for making wireframe outlines and
  flat surfaces fade out after rebuild.  Looks cool.

-----------------------------------------------------------------------------*/

void RenderFogFX (float scalar)
{

  if (scalar >= 1.0f) {
    glDisable (GL_FOG);
    return;
  }
  glFogf (GL_FOG_START, 0.0f);
  glFogf (GL_FOG_END, fog_distance * 2.0f * scalar);
  glEnable (GL_FOG);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderUpdate (void)		
{

  GLvector        pos;
  GLvector        angle;
  GLrgba          color;
  int             elapsed;

  frames++;
  do_fps ();
  glViewport (0, 0, WinWidth (), WinHeight ());
  glDepthMask (true);
  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (letterbox) 
    glViewport (0, letterbox_offset, render_width, render_height);
  if (LOADING_SCREEN && TextureReady () && !EntityReady ()) {
    do_effects (EFFECT_NONE);
    SwapBuffers (hDC);
    return;
  }
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glShadeModel(GL_SMOOTH);
  glFogi (GL_FOG_MODE, GL_LINEAR);
	glDepthFunc(GL_LEQUAL);
  glEnable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glMatrixMode (GL_TEXTURE);
  glLoadIdentity();
	glMatrixMode (GL_MODELVIEW);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glLoadIdentity();
  glLineWidth (1.0f);
  pos = CameraPosition ();
  angle = CameraAngle ();
  glRotatef (angle.x, 1.0f, 0.0f, 0.0f);
  glRotatef (angle.y, 0.0f, 1.0f, 0.0f);
  glRotatef (angle.z, 0.0f, 0.0f, 1.0f);
  glTranslatef (-pos.x, -pos.y, -pos.z);
  glEnable (GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //Render all the stuff in the whole entire world.
  glDisable (GL_FOG);
  SkyRender ();
  if (show_fog) {
    glEnable (GL_FOG);
    glFogf (GL_FOG_START, fog_distance - 100);
    glFogf (GL_FOG_END, fog_distance);
    color = glRgba (0.0f);
    glFogfv (GL_FOG_COLOR, &color.red);
  }
  WorldRender ();
  if (effect == EFFECT_GLASS_CITY) {
    glDisable (GL_CULL_FACE);
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE);
    glDepthFunc (false);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode (GL_TEXTURE);
    glTranslatef ((pos.x + pos.z) / SEGMENTS_PER_TEXTURE, 0, 0);
	  glMatrixMode (GL_MODELVIEW);
  } else {
    glEnable (GL_CULL_FACE);
    glDisable (GL_BLEND);
  }
  EntityRender ();
  if (!LOADING_SCREEN) {
    elapsed = 3000 - WorldSceneElapsed ();
    if (elapsed >= 0 && elapsed <= 3000) {
      RenderFogFX ((float)elapsed / 3000.0f);
      glDisable (GL_TEXTURE_2D);
      glEnable (GL_BLEND);
      glBlendFunc (GL_ONE, GL_ONE);
      EntityRender ();
    }
  } 
  if (EntityReady ())
    LightRender ();
  CarRender ();
  if (show_wireframe) {
    glDisable (GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    EntityRender ();
  }
  do_effects (effect);
  //Framerate tracker
  if (show_fps) 
    RenderPrint (1, "FPS=%d : Entities=%d : polys=%d", current_fps, EntityCount () + LightCount () + CarCount (), EntityPolyCount () + LightCount () + CarCount ());
  //Show the help overlay
  if (show_help)
    do_help ();
  SwapBuffers (hDC);

}
