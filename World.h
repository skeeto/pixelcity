#define SHOW_DEBUG_GROUND     0

#define WORLD_SIZE            1024
#define WORLD_HALF            (WORLD_SIZE / 2)

#define CLAIM_ROAD            1
#define CLAIM_WALK            2
#define CLAIM_BUILDING        4
#define MAP_ROAD_NORTH        8
#define MAP_ROAD_SOUTH        16
#define MAP_ROAD_EAST         32
#define MAP_ROAD_WEST         64
#define RANDOM_COLOR          (glRgbaFromHsl ((float)RandomVal (255)/255,1.0f, 0.75f))

enum
{
  NORTH,
  EAST,
  SOUTH,
  WEST
};

GLrgba    WorldBloomColor ();
char      WorldCell (int x, int y);
GLrgba    WorldLightColor (unsigned index);
int       WorldLogoIndex ();
GLbbox    WorldHotZone ();
void      WorldInit (void);
float     WorldFade (void);
void      WorldRender ();
void      WorldReset (void);
void      WorldTerm (void);
void      WorldUpdate (void);


