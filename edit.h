#ifndef EDIT_H
#define EDIT_H

/*
 * Abstract console declarations
 */
typedef struct SysConsole Console;

Console* InitConsole();
void ReleaseConsole( Console** consolePtr );

#endif
