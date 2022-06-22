
#include <stdlib.h>

#include "windows.h"

#ifndef ENABLE_EXTENDED_FLAGS
#define ENABLE_EXTENDED_FLAGS 0x0080
#endif

#include "..\cui.h"

/* Reference:
https://docs.microsoft.com/en-us/windows-hardware/customize/enterprise/keyboardfilter-key-names
*/

const unsigned keyEscape = VK_ESCAPE;
const unsigned keySpace = VK_SPACE;
const unsigned keyReturn = VK_RETURN;
const unsigned keyBackspace = VK_BACK;
const unsigned keyTab = VK_TAB;
const unsigned keyLeftShift = VK_LSHIFT;
const unsigned keyRightShift = VK_RSHIFT;
const unsigned keyShift = VK_SHIFT;
const unsigned keyLeftCtrl = VK_LCONTROL;
const unsigned keyRightCtrl = VK_RCONTROL;
const unsigned keyCtrl = VK_CONTROL;
const unsigned keyLeftMenu = VK_LMENU;
const unsigned keyRightMenu = VK_RMENU;
const unsigned keyMenu = VK_MENU;
const unsigned keyPause = VK_PAUSE;
const unsigned keyInsert = VK_INSERT;
const unsigned keyDelete = VK_DELETE;
const unsigned keyCapsLock = VK_CAPITAL;
const unsigned keyScrollLock = VK_SCROLL;
const unsigned keyNumLock = VK_NUMLOCK;
const unsigned keyF1 = VK_F1;
const unsigned keyF2 = VK_F2;
const unsigned keyF3 = VK_F3;
const unsigned keyF4 = VK_F4;
const unsigned keyF5 = VK_F5;
const unsigned keyF6 = VK_F6;
const unsigned keyF7 = VK_F7;
const unsigned keyF8 = VK_F8;
const unsigned keyF9 = VK_F9;
const unsigned keyF10 = VK_F10;
const unsigned keyF11 = VK_F11;
const unsigned keyF12 = VK_F12;
const unsigned keyF13 = VK_F13;
const unsigned keyF14 = VK_F14;
const unsigned keyF15 = VK_F15;
const unsigned keyF16 = VK_F16;
const unsigned keyF17 = VK_F17;
const unsigned keyF18 = VK_F18;
const unsigned keyF19 = VK_F19;
const unsigned keyF20 = VK_F20;
const unsigned keyF21 = VK_F21;
const unsigned keyF22 = VK_F22;
const unsigned keyF23 = VK_F23;
const unsigned keyF24 = VK_F24;
const unsigned keyPageUp = VK_NEXT;
const unsigned keyPageDown = VK_PRIOR;
const unsigned keyHome = VK_HOME;
const unsigned keyEnd = VK_END;
const unsigned keyLeft = VK_LEFT;
const unsigned keyUp = VK_UP;
const unsigned keyRight = VK_RIGHT;
const unsigned keyDown = VK_DOWN;
const unsigned keyPrintScreen = VK_SNAPSHOT;
const unsigned keyLeftWin = VK_LWIN;
const unsigned keyRightWin = VK_RWIN;
const unsigned keyWin = VK_LWIN;
const unsigned key0 = '0';
const unsigned key1 = '1';
const unsigned key2 = '2';
const unsigned key3 = '3';
const unsigned key4 = '4';
const unsigned key5 = '5';
const unsigned key6 = '6';
const unsigned key7 = '7';
const unsigned key8 = '8';
const unsigned key9 = '9';
const unsigned num0 = VK_NUMPAD0;
const unsigned num1 = VK_NUMPAD1;
const unsigned num2 = VK_NUMPAD2;
const unsigned num3 = VK_NUMPAD3;
const unsigned num4 = VK_NUMPAD4;
const unsigned num5 = VK_NUMPAD5;
const unsigned num6 = VK_NUMPAD6;
const unsigned num7 = VK_NUMPAD7;
const unsigned num8 = VK_NUMPAD8;
const unsigned num9 = VK_NUMPAD9;
const unsigned numMul = VK_MULTIPLY;
const unsigned numDiv = VK_DIVIDE;
const unsigned numAdd = VK_ADD;
const unsigned numSub = VK_SUBTRACT;
const unsigned numDot = VK_DECIMAL;
const unsigned numEnter = VK_RETURN;
const unsigned keyA = 'A';
const unsigned keyB = 'B';
const unsigned keyC = 'C';
const unsigned keyD = 'D';
const unsigned keyE = 'E';
const unsigned keyF = 'F';
const unsigned keyG = 'G';
const unsigned keyH = 'H';
const unsigned keyI = 'I';
const unsigned keyJ = 'J';
const unsigned keyK = 'K';
const unsigned keyL = 'L';
const unsigned keyM = 'M';
const unsigned keyN = 'N';
const unsigned keyO = 'O';
const unsigned keyP = 'P';
const unsigned keyQ = 'Q';
const unsigned keyR = 'R';
const unsigned keyS = 'S';
const unsigned keyT = 'T';
const unsigned keyU = 'U';
const unsigned keyV = 'V';
const unsigned keyW = 'W';
const unsigned keyX = 'X';
const unsigned keyY = 'Y';
const unsigned keyZ = 'Z';

const unsigned capsLockOn = CAPSLOCK_ON;
const unsigned numLockOn = NUMLOCK_ON;
const unsigned scrollLockOn = SCROLLLOCK_ON;
const unsigned leftShiftKeyDown = 0;
const unsigned rightShiftKeyDown = 0;
const unsigned shiftKeyDown = SHIFT_PRESSED;
const unsigned leftAltKeyDown = LEFT_ALT_PRESSED;
const unsigned rightAltKeyDown = RIGHT_ALT_PRESSED;
const unsigned altKeyDown = (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED);
const unsigned leftCtrlKeyDown = LEFT_CTRL_PRESSED;
const unsigned rightCtrlKeyDown = RIGHT_CTRL_PRESSED;
const unsigned ctrlKeyDown = (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);

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

    if( (*consolePtr)->stdinHandle != INVALID_HANDLE_VALUE ) {
      // Clear event queue
      FlushConsoleInputBuffer( (*consolePtr)->stdinHandle );

      // Restore OS input mode
      if( (*consolePtr)->callerInputMode ) {
        SetConsoleMode( (*consolePtr)->stdinHandle,
          (*consolePtr)->callerInputMode );
      }
    }

    // Restore OS output buffer
    if( (*consolePtr)->stdoutHandle != INVALID_HANDLE_VALUE ) {
      SetConsoleActiveScreenBuffer( (*consolePtr)->stdoutHandle );
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
  INPUT_RECORD event = {};
  KeyEvent keyEvent = {};

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
      memset( &keyEvent, 0, sizeof(keyEvent) );

      keyEvent.eventType = evtKeyUp;
      if( event.Event.KeyEvent.bKeyDown ) {
        keyEvent.eventType = evtKeyDown;
      }

      keyEvent.keyCode = event.Event.KeyEvent.wVirtualKeyCode;
      keyEvent.keyChar = event.Event.KeyEvent.uChar.AsciiChar;
      keyEvent.modifiers = event.Event.KeyEvent.dwControlKeyState;

      if( console->onKey && console->onKey(console, console->onKeyData, &keyEvent) ) {
        continue; // Developer processed event
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
