#define SKY_GRID      21
#define SKY_HALF      (SKY_GRID / 2)

struct sky_point
{
  GLrgba        color;
  GLvector      position;
};

class CSky 
{
private:
  int                     m_list;
  int                     m_stars_list;
  sky_point               m_grid[SKY_GRID][SKY_GRID];

public:
                          CSky ();
  void                    Render (void);

};

void SkyRender ();
void SkyClear ();
