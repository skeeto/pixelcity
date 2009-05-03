

struct cube
{
  int         index_list[10];
};

struct quad_strip
{
  int*        index_list;
  unsigned    count;
};

struct fan
{
  int*        index_list;
  unsigned    count;
};

class CMesh
{
public:
              CMesh ();
              ~CMesh ();
  unsigned    _vertex_count;
  unsigned    _triangle_count;
  unsigned    _cube_count;
  unsigned    _quad_strip_count;
  unsigned    _fan_count;
  unsigned    _normal_count;
  unsigned    _list;
  int         _polycount;
  GLvertex*   _vertex;  
  GLvector*   _normal;
  GLtriangle* _triangle;
  cube*       _cube;
  quad_strip* _quad_strip;
  fan*        _fan;
  bool        _compiled;

  //void        TriangleRender (unsigned n);
  //GLtriangle* TriangleAdd (unsigned v1, int unsigned, int unsigned);
  //GLtriangle* TriangleAdd (GLtriangle c);
  void        NormalAdd (GLvector n);
  void        VertexAdd (GLvertex v);
  int         VertexCount () { return _vertex_count; }
  int         PolyCount () { return _polycount; }
  void        CubeAdd (int* index);
  void        QuadStripAdd (int* index, int count);
  void        FanAdd (int* index, int count);
  void        Render ();
  void        Compile ();

};
