
#include <stdlib.h>

#include "windows.h"

#include "..\edit.h"

/** Abstract console Win32 implementation **/

typedef struct SysConsole {
  // Standard handles
  HANDLE stdinHandle;
  HANDLE stdoutHandle;
  HANDLE stderrHandle;

  // Output buffer for Edit's CUI
  HANDLE outBuffer;

  // OS input mode
  DWORD callerInputMode;

  // Application event handlers
  KeyHandler onKey;
  MouseHandler onMouse;
} SysConsole;

/*
 * Console* InitConsole()
 *
 * Initializes Edit's Win32 console environment.
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

  // Alloc and set Edit output buffer
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

  // Set Edit input mode
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
 * Releases Edit's Win32 console environment resources.
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

    // Release Edit output buffer
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

/** Event handler Win32 implementation **/

/*
 * unsigned ConfirmClose( console )
 *
 * Called from any point where the program is closed (eg: menu,
 *   key combination, etc).
 *
 * Returns:
 * - Exit: Non-zero
 * - Cancel: 0
 *
 */
unsigned ConfirmClose( console ) {
  return 2;
}

/*
 * unsigned SetKeyHandler( Console* console, KeyHandler onKey )
 *
 * Sets the application defined keyboard event handler. onKey returns
 *   non-zero value if it processed the event.
 *
 * Returns:
 * - Success: 0
 * - Error: Parameter number, or internal failed code index.
 *
 */
unsigned SetKeyHandler( Console* console, KeyHandler onKey ) {
  return 3;
}

/*
 * void ResetKeyHandler( Console* console )
 *
 * Removes the application defined keyboard event handler.
 *
 * Returns: Nothing
 *
 */
void ResetKeyHandler( Console* console ) {
}

/*
 * unsigned SetMouseHandler( Console* console, MouseHandler onMouse )
 *
 * Sets the application defined mouse event handler. onMouse returns
 *   non-zero value if it processed the event.
 *
 * Returns:
 * - Success: 0
 * - Error: Parameter number, or internal failed code index.
 *
 */
unsigned SetMouseHandler( Console* console, MouseHandler onMouse ) {
  return 3;
}

/*
 * void ResetMouseHandler( Console* console )
 *
 * Removes the application defined mouse event handler.
 *
 * Returns: Nothing
 *
 */
void ResetMouseHandler( Console* console ) {
}

/** App handler Win32 implementation **/

/*
 * unsigned IsActive( Console* console )
 *
 * Tests whether or not the console is active.
 *
 * Returns
 * - Success: 0
 * - Error: Parameter number, or internal failed code index.
 *
 */
unsigned IsActive( Console* console ) {
  if( !(console && (console->outBuffer != INVALID_HANDLE_VALUE)) ) {
    return 0;
  }

  return 2;
}

/*
 * void RouteEvents( Console* console )
 *
 * Sends event down a hierarchy of UI components. Event is discarded
 *   if unhandled.
 *
 * Returns: Nothing
 *
 */
void RouteEvents( Console* console ) {
}

/*
 * void DrawUI( Console* console )
 *
 * Draws user interface components that are visible.
 *
 * Returns: Nothing
 *
 */
void UpdateUI( Console* console ) {
}
