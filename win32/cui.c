
#include <stdlib.h>

#include "windows.h"

#ifndef ENABLE_EXTENDED_FLAGS
#define ENABLE_EXTENDED_FLAGS 0x0080
#endif

#include "..\cui.h"

const KeyCodeType KeyCode = {
};

/** Abstract console Win32 implementation **/

typedef struct SysConsole {
  // App state: 0 if closed/closing; non-zero if active
  unsigned isActive;

  // Standard handles
  HANDLE stdinHandle;
  HANDLE stdoutHandle;
  HANDLE stderrHandle;

  // Output buffer for Edit's CUI
  HANDLE outBuffer;

  // OS input mode
  DWORD callerInputMode;

  // App event handlers
  KeyHandler onKey;
  void* onKeyData;

  MouseHandler onMouse;
  void* onMouseData;

  CloseHandler onClose;
  void* onCloseData;
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
  if( SetConsoleMode(stdinHandle, ENABLE_EXTENDED_FLAGS |
      ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT |
      ENABLE_WINDOW_INPUT) == 0 ) {
    goto ReturnError;
  }

  newConsole->isActive = 1;

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
 * unsigned SetKeyHandler( Console* console, KeyHandler onKey )
 *
 * Sets the application defined keyboard event handler.
 *
 * Parameters:
 * - console: App console object
 * - onKey: Developer handler which returns non-zero if it
 *   processes the event.
 * - handlerData: Developer data pointer or NULL.
 *
 * Key handler is defined as:
 *   unsigned onKey( void* handlerData )
 *
 * Returns:
 * - Success: 0
 * - Error: Parameter number, or internal failed code index.
 *
 */
unsigned SetKeyHandler( Console* console,
  KeyHandler onKey, void* handlerData ) {

  if( !(console && (console->onKey == NULL)) ) { return 1; }
  if( onKey == NULL ) { return 2; }

  console->onKey = onKey;
  console->onKeyData = handlerData;

  return 0;
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
  if( console ) {
    console->onKey = NULL;
  }
}

/*
 * unsigned SetMouseHandler( Console* console,
 *   MouseHandler onMouse, void* handlerData )
 *
 * Sets the application defined mouse event handler.
 *
 * Parameters:
 * - console: App console object
 * - onMouse: Developer handler which returns non-zero if it
 *   processes the event.
 * - handlerData: Developer data pointer or NULL.
 *
 * Mouse handler is defined as:
 *   unsigned onMouse( void* handlerData )
 *
 * Returns:
 * - Success: 0
 * - Error: Parameter number, or internal failed code index.
 *
 */
unsigned SetMouseHandler( Console* console,
  MouseHandler onMouse, void* handlerData ) {

  if( !(console && (console->onMouse == NULL)) ) { return 1; }
  if( onMouse == NULL ) { return 2; }

  console->onMouse = onMouse;
  console->onMouseData = handlerData;

  return 0;
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
  if( console ) {
    console->onMouse = NULL;
  }
}

/*
 * unsigned SetCloseHandler( Console* console,
 *   CloseHandler onClose, void* handlerData )
 *
 * Sets the application defined Close event handler.
 *
 * Parameters:
 * - console: App console object
 * - onClose: Developer handler which returns non-zero if it
 *   processes the event.
 * - handlerData: Developer data pointer or NULL.
 *
 * Close handler is defined as:
 *   unsigned onClose( void* handlerData )
 *
 * Returns:
 * - Success: 0
 * - Error: Parameter number, or internal failed code index.
 *
 */
unsigned SetCloseHandler( Console* console,
  CloseHandler onClose, void* handlerData ) {

  if( !(console && (console->onClose == NULL)) ) { return 1; }
  if( onClose == NULL ) { return 2; }

  console->onClose = onClose;
  console->onCloseData = handlerData;

  return 0;
}

/*
 * void ResetCloseHandler( Console* console )
 *
 * Removes the application defined Close event handler.
 *
 * Returns: Nothing
 *
 */
void ResetCloseHandler( Console* console ) {
  if( console ) {
    console->onClose = NULL;
  }
}

/** App Win32 implementation **/

/*
 * unsigned IsActive( Console* console )
 *
 * Tests whether or not the console is active.
 *
 * Returns
 * - Success: non-zero
 * - Error: 0
 *
 */
unsigned IsActive( Console* console ) {
  if( console && (console->outBuffer != INVALID_HANDLE_VALUE) &&
      console->isActive ) {
    return 1;
  }

  return 0;
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
#include <stdio.h>

void RouteEvents( Console* console ) {
  DWORD eventsRead = 1;
  INPUT_RECORD event;

  if( !(console && (console->stdinHandle != INVALID_HANDLE_VALUE) &&
      console->isActive) ) {
    return;
  }

  while( eventsRead ) {
    memset( &event, 0, sizeof(event) );

    if( (PeekConsoleInput(console->stdinHandle,
        &event, 1, &eventsRead) == 0) || (eventsRead == 0) ) {
      return;
    }

    if( (ReadConsoleInput(console->stdinHandle,
        &event, 1, &eventsRead) == 0) ) {
      return;
    }

    switch( event.EventType ) {
    case KEY_EVENT:
      if( console->onKey && console->onKey(console->onKeyData) ) {
        return; // Developer processed event
      }
      break;

    case MOUSE_EVENT:
      break;

    case WINDOW_BUFFER_SIZE_EVENT:
      break;

    case FOCUS_EVENT:
      break;

    case MENU_EVENT:
      break;
    }
  }
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

/*
 * void ExitApp( Console* console )
 *
 * Calls onClose, if defined, to confirm whether it should break
 *   the main loop or do nothing.
 *
 * Returns: Nothing
 *
 */
void ExitApp( Console* console ) {
  if( console && console->isActive ) {
    // If onClose is defined, and developer returns 0 then cancel
    if( console->onClose && (console->onClose(console) == 0) ) {
      return;
    }

    // Else initiate app exit
    console->isActive = 0;
  }
}
