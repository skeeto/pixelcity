#ifndef TYPES
#include "glTypes.h"
#endif

GLvector  CameraAngle (void);
void      CameraAngleSet (GLvector new_angle);
void      CameraAutoToggle ();
float     CameraDistance (void);
void      CameraDistanceSet (float new_distance);
void      CameraInit (void);
void      CameraNextBehavior (void);
GLvector  CameraPosition (void);
void      CameraPositionSet (GLvector new_pos);
void      CameraReset ();
void      CameraUpdate (void);	
void      CameraTerm (void);

void      CameraForward (float delta);
void      CameraPan (float delta_x);
void      CameraPitch (float delta_y);
void      CameraYaw (float delta_x);
void      CameraVertical (float val);
void      CameraLateral (float val);
void      CameraMedial (float val);