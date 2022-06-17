
#include <stdlib.h>

#include "windows.h"

#include "..\edit.h"

/*
 * Abstract console Win32 implementation
 */

typedef struct SysConsole {
  // Standard handles
  HANDLE stdinHandle;
  HANDLE stdoutHandle;
  HANDLE stderrHandle;

  // Output buffer for Qux's CUI
  HANDLE outBuffer;

  // OS input mode
  DWORD callerInputMode;
} SysConsole;

/*
 * Console* InitConsole()
 *
 * Initializes Qux's Win32 console environment.
 *
 * Returns:
 * - Success: Console environment state pointer
 * - Error: NULL
 *
 */
Console* InitConsole() {
  Console* newConsole;
  HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE stderrHandle = GetStdHandle(STD_ERROR_HANDLE);

  newConsole = calloc(1, sizeof(SysConsole));
  if( newConsole == NULL ) { goto ReturnError; }

  // Initialize handle values for proper cleanup
  newConsole->stdinHandle = stdinHandle;
  newConsole->stdoutHandle = stdoutHandle;
  newConsole->stderrHandle = stderrHandle;
  newConsole->outBuffer = INVALID_HANDLE_VALUE;

  if( (stdinHandle == INVALID_HANDLE_VALUE) ||
      (stdoutHandle == INVALID_HANDLE_VALUE) ||
      (stderrHandle == INVALID_HANDLE_VALUE) ) {
    goto ReturnError;
  }

  // Alloc and set Qux output buffer
  newConsole->outBuffer = CreateConsoleScreenBuffer(
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    CONSOLE_TEXTMODE_BUFFER,
    NULL);
  if( newConsole->outBuffer == INVALID_HANDLE_VALUE ) {
    goto ReturnError;
  }

  if( SetConsoleActiveScreenBuffer(newConsole->outBuffer) == 0 ) {
    goto ReturnError;
  }

  // Save OS input mode
  if( GetConsoleMode(stdinHandle, &newConsole->callerInputMode) == 0 ) {
    goto ReturnError;
  }

  // Set Qux input mode
  if( SetConsoleMode(stdinHandle, 
      ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT) == 0 ) {
    goto ReturnError;
  }

  return newConsole;

ReturnError:
  ReleaseConsole( &newConsole );
  return NULL;
}

/*
 * void ReleaseConsole( Console** consolePtr )
 *
 * Releases Qux's Win32 console environment resources.
 *
 * Returns: Nothing
 *
 */
void ReleaseConsole( Console** consolePtr ) {
  if( !(consolePtr && (*consolePtr)) ) { return; }

  if( (*consolePtr)->stdinHandle && (*consolePtr)->callerInputMode ) {

    // Restore OS input mode
    SetConsoleMode( (*consolePtr)->stdinHandle,
      (*consolePtr)->callerInputMode );

    // Restore OS output buffer
    if( (*consolePtr)->stdinHandle != INVALID_HANDLE_VALUE ) {
      SetConsoleActiveScreenBuffer( (*consolePtr)->stdinHandle );
    }

    // Release Qux output buffer
    if( (*consolePtr)->outBuffer != INVALID_HANDLE_VALUE ) {
      CloseHandle( (*consolePtr)->outBuffer );
      (*consolePtr)->outBuffer = INVALID_HANDLE_VALUE;
    }

    // Clear standard handles
    (*consolePtr)->stdinHandle = INVALID_HANDLE_VALUE;
    (*consolePtr)->stdoutHandle = INVALID_HANDLE_VALUE;
    (*consolePtr)->stderrHandle = INVALID_HANDLE_VALUE;
  }

  free( (*consolePtr) );
}
