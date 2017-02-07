#ifndef ENTITY
#include "entity.h"
#endif

class CDeco : CEntity
{
  GLrgba        _color;
  class CMesh*  _mesh;
  int           _type;
  unsigned      _texture;
  bool          _use_alpha;

public:

                CDeco ();
                ~CDeco ();
  void          CreateLogo (GLvector2 start, GLvector2 end, float base, int seed, GLrgba color);
  void          CreateLightStrip (float x, float z, float width, float depth, float height, GLrgba color);
  void          CreateLightTrim (GLvector* chain, int count, float height, int seed, GLrgba color);
  void          CreateRadioTower (GLvector pos, float height);
  void          Render (void);
  void          RenderFlat (bool colored);
  bool          Alpha ();
  int           PolyCount ();

  unsigned      Texture ();

};