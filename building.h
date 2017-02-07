#ifndef ENTITY
#include "entity.h"
#endif

enum
{
  BUILDING_SIMPLE,
  BUILDING_BLOCKY,
  BUILDING_MODERN,
  BUILDING_TOWER
};

class CBuilding : public CEntity
{
private:

  int                     _x;
  int                     _y;
  int                     _width;
  int                     _depth;
  int                     _height;
  int                     _texture_type;
  int                     _seed;
  int                     _roof_tiers;
  GLrgba                  _color;
  GLrgba                  _trim_color;
  class CMesh*            _mesh;
  class CMesh*            _mesh_flat;
  bool                    _have_lights;
  bool                    _have_trim;
  bool                    _have_logo;


  void                    CreateSimple ();
  void                    CreateBlocky ();
  void                    CreateModern ();
  void                    CreateTower ();
  
  float                   ConstructWall (int start_x, int start_y, int start_z, int direction, int length, int height, int window_groups, float uv_start, bool blank_corners);
  void                    ConstructSpike (int left, int right, int front, int back, int bottom, int top);
  void                    ConstructCube (int left, int right, int front, int back, int bottom, int top);
  void                    ConstructCube (float left, float right, float front, float back, float bottom, float top);
  void                    ConstructRoof (float left, float right, float front, float back, float bottom);

public:
                          CBuilding (int type, int x, int y, int height, int width, int depth, int seed, GLrgba color);
                          ~CBuilding ();
  void                    Render (void);
  int                     PolyCount ();
  void                    RenderFlat (bool colored);
  unsigned                Texture ();

};
