class CCar
{
  GLvector        m_position;
  GLvector        m_drive_position;
  bool            m_ready;
  bool            m_front;
  int             m_drive_angle;
  int             m_row;
  int             m_col;
  int             m_direction;
  int             m_change;
  int             m_stuck;
  float           m_speed;
  float           m_max_speed;

public:
                  CCar ();
  bool            TestPosition (int row, int col);
  void            Render ();
  void            Update ();
  void            Park () { m_ready = false;}
  class CCar*     m_next;

};

void  CarClear ();
int   CarCount ();
void  CarRender ();
void  CarUpdate ();

