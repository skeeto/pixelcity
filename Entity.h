#ifndef TYPES
#include "glTypes.h"
#endif

#ifndef ENTITY

#define ENTITY

class CEntity
{
private:
protected:

  GLvector                _center;

public:
                          CEntity (void);
  virtual                 ~CEntity () {};
  virtual void            Render (void);
  virtual void            RenderFlat (bool wirefame);
  virtual unsigned        Texture () { return 0; }
  virtual void            Update (void);
  virtual bool            Alpha () { return false; }
  virtual int             PolyCount () { return 0; }
  GLvector                Center () { return _center; }

};

void      EntityClear ();
int       EntityCount (void);
float     EntityProgress ();
bool      EntityReady ();
void      EntityRender (void);
void      EntityUpdate (void);
int       EntityPolyCount (void);

#endif