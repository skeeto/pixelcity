#define LIMIT_INTERVAL(interval)  { static unsigned next_update; if (next_update > GetTickCount ()) return; next_update = GetTickCount () + interval;}
#define DEGREES_TO_RADIANS        .017453292F
#define RADIANS_TO_DEGREES        57.29577951F
#define PI                        ((double)3.1415926535F)
#define PI2                       PI*PI
#define GRAVITY                   9.5f
#define CLAMP(a,b,c)              (a < b ? b : (a > c ? c : a))
#define WRAP(x,y)                 ((unsigned)x % y)
#define SIGN(x)                   (((x) > 0) ? 1 : ((x) < 0) ? -1 : 0)
#define ABS(x)                    (((x) < 0 ? (-x) : (x)))
#define SMALLEST(x,y)             (ABS(x) < ABS(y) ? 0 : x)                
#define MIN(x,y)                  ((x) < (y) ? x : y)                
#define MAX(x,y)                  ((x) > (y) ? x : y)                
#define POW(x,y)                  (float)pow(x,y)
#define SWAP(a,b)                 {int temp = a;a = b; b = temp;}
