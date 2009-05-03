#define APP_TITLE       "PixelCity"
#define APP             "pixelcity"
#define SCREENSAVER     0


HWND  WinHwnd (void);
void  WinPopup (char* message, ...);
void  WinTerm (void);
bool  WinInit (void);
int   WinWidth (void);
int   WinHeight (void);
void  WinMousePosition (int* x, int* y);