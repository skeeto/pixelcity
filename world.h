
GLrgba    WorldBloomColor ();
char      WorldCell (int x, int y);
GLrgba    WorldLightColor (unsigned index);
int       WorldLogoIndex ();
GLbbox    WorldHotZone ();
void      WorldInit (void);
float     WorldFade (void);
void      WorldRender ();
void      WorldReset (void);
int       WorldSceneBegin ();
int       WorldSceneElapsed ();
void      WorldTerm (void);
void      WorldUpdate (void);


