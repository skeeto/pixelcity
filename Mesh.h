
#include <vector>

struct cube
{
  std::vector<int> index_list;   // probably always .size() == 10...
};

struct quad_strip
{
  std::vector<int> index_list;
};

struct fan
{
  std::vector<int> index_list;
};

class CMesh
{
public:
                          CMesh ();
                          ~CMesh ();
  unsigned                _list;
  int                     _polycount;
  std::vector<GLvertex>   _vertex;  
  std::vector<cube>       _cube;
  std::vector<quad_strip> _quad_strip;
  std::vector<fan>        _fan;
  bool                    _compiled;

  void        VertexAdd (const GLvertex& v);
  int         VertexCount () { return _vertex.size(); }
  int         PolyCount () { return _polycount; }
  void        CubeAdd (const cube& c);
  void        QuadStripAdd (const quad_strip& qs);
  void        FanAdd (const fan& f);
  void        Render ();
  void        Compile ();

};
