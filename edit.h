#ifndef EDIT_H
#define EDIT_H

/** Abstract console declarations **/

typedef struct SysConsole Console;

Console* InitConsole();
void ReleaseConsole( Console** consolePtr );

/** Event handler declarations **/

typedef unsigned (*KeyHandler)();
unsigned SetKeyHandler( Console* console, KeyHandler onKey );
void ResetKeyHandler( Console* console );

typedef unsigned (*MouseHandler)();
unsigned SetMouseHandler( Console* console, MouseHandler onMouse );
void ResetMouseHandler( Console* console );

/** App handler declarations **/

unsigned IsActive( Console* console );
void RouteEvents( Console* console );
void UpdateUI( Console* console );

#endif
