#define SEGMENTS_PER_TEXTURE  64
#define ONE_SEGMENT           (1.0f / SEGMENTS_PER_TEXTURE)
#define LANES_PER_TEXTURE     8
#define LANE_SIZE             (1.0f / LANES_PER_TEXTURE)
#define LANE_PIXELS           (_size / LANES_PER_TEXTURE)
#define TRIM_RESOLUTION       256
#define TRIM_ROWS             4
#define TRIM_SIZE             (1.0f / TRIM_ROWS)
#define TRIM_PIXELS           (TRIM_RESOLUTION / TRIM_ROWS)
#define LOGO_RESOLUTION       512
#define LOGO_ROWS             16
#define LOGO_SIZE             (1.0f / LOGO_ROWS)
#define LOGO_PIXELS           (LOGO_RESOLUTION / LOGO_ROWS)

enum
{
  TEXTURE_LIGHT,
  TEXTURE_SOFT_CIRCLE,
  TEXTURE_SKY,
  TEXTURE_LOGOS,
  TEXTURE_TRIM,
  TEXTURE_BLOOM,
  TEXTURE_HEADLIGHT,
  TEXTURE_LATTICE,
  TEXTURE_BUILDING1,
  TEXTURE_BUILDING2,
  TEXTURE_BUILDING3,
  TEXTURE_BUILDING4,
  TEXTURE_BUILDING5,
  TEXTURE_BUILDING6,
  TEXTURE_BUILDING7,
  TEXTURE_BUILDING8,
  TEXTURE_BUILDING9,
  TEXTURE_COUNT,
};

#define BUILDING_COUNT    ((TEXTURE_BUILDING9 - TEXTURE_BUILDING1) + 1)

unsigned  TextureFromName (char* name);
unsigned  TextureId (int id);
void      TextureInit (void);
void      TextureTerm (void);
unsigned  TextureRandomBuilding (int index);
bool      TextureReady ();
void      TextureReset (void);
void      TextureUpdate (void);

