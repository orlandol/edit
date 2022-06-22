
#include <stdio.h>
#include <stdlib.h>

/** Console declarations **/

unsigned InitConsole();
void ReleaseConsole();

void ConPutChar( char ch );

/** Event declarations **/

#ifdef _WIN32
#include "windows.h"

#define keyEscape (VK_ESCAPE)
#endif

typedef struct KeyInfo {
  unsigned code;
  char ch;
  unsigned modifiers;
} KeyInfo;

unsigned ReadKey( KeyInfo* key );
void RouteEvent();

/** App declarations **/

/// App main loop declarations ///
unsigned IsActive();

/// App state control declarations ///
void ExitApp();

/** Main program **/

void Cleanup() {
  ReleaseConsole();
}

int main( int argc, char** argv ) {
  KeyInfo key = {};

  atexit( Cleanup );

  if( InitConsole() == 0 ) {
    printf( "Error initializing console\n" );
    exit(1);
  }

  ConPutChar( '!' );
  while( IsActive() ) {

    if( ReadKey(&key) ) {
      switch( key.code ) {
      case keyEscape:
        ExitApp();
        break;

      default:
        if( key.ch ) {
          ConPutChar( key.ch );
        }
      }
    } else {
      RouteEvent();
    }
  }

  ReleaseConsole();

  return 0;
}

/** System declarations **/

#include "windows.h"

#ifndef ENABLE_EXTENDED_FLAGS
#define ENABLE_EXTENDED_FLAGS 0x0080
#endif

/** Console implementation **/

unsigned appIsActive = 0;

DWORD callerInputMode = 0;

HANDLE stdInput = INVALID_HANDLE_VALUE;
HANDLE stdOutput = INVALID_HANDLE_VALUE;
HANDLE stdError = INVALID_HANDLE_VALUE;

HANDLE outBuffer = INVALID_HANDLE_VALUE;

unsigned InitConsole() {
  stdInput = GetStdHandle(STD_INPUT_HANDLE);
  stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  stdError = GetStdHandle(STD_ERROR_HANDLE);

  // Initialize handle values for proper cleanup
  if( (stdInput == INVALID_HANDLE_VALUE) ||
      (stdOutput == INVALID_HANDLE_VALUE) ||
      (stdError == INVALID_HANDLE_VALUE) ) {
    goto ReturnError;
  }

  // Alloc and set Edit output buffer
  outBuffer = CreateConsoleScreenBuffer(
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    CONSOLE_TEXTMODE_BUFFER,
    NULL);
  if( outBuffer == INVALID_HANDLE_VALUE ) {
    goto ReturnError;
  }

  if( SetConsoleActiveScreenBuffer(outBuffer) == 0 ) {
    goto ReturnError;
  }

  // Save OS input mode
  if( GetConsoleMode(stdInput, &callerInputMode) == 0 ) {
    goto ReturnError;
  }

  // Set Edit input mode
  if( SetConsoleMode(stdInput, ENABLE_EXTENDED_FLAGS |
      ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT |
      ENABLE_WINDOW_INPUT) == 0 ) {
    goto ReturnError;
  }

  appIsActive = 1;

  return 1;

ReturnError:
  ReleaseConsole();
  return 0;
}

void ReleaseConsole() {
  if( stdInput != INVALID_HANDLE_VALUE ) {
    FlushConsoleInputBuffer( stdInput );

    if( callerInputMode ) {
      SetConsoleMode( stdInput, callerInputMode );
    }
  }

  if( stdOutput != INVALID_HANDLE_VALUE ) {
    SetConsoleActiveScreenBuffer( stdOutput );
  }

  if( outBuffer != INVALID_HANDLE_VALUE ) {
    CloseHandle( outBuffer );
    outBuffer = INVALID_HANDLE_VALUE;
  }

  stdInput = INVALID_HANDLE_VALUE;
  stdOutput = INVALID_HANDLE_VALUE;
  stdError = INVALID_HANDLE_VALUE;
}

void ConPutChar( char ch ) {
  DWORD charsWritten = 0;

  if( ch && (stdOutput != INVALID_HANDLE_VALUE) ) {
    WriteConsole( outBuffer, &ch, 1, &charsWritten, NULL );
  }
}

/** Event implementations **/

unsigned ReadKey( KeyInfo* key ) {
  DWORD eventsRead = 1;
  INPUT_RECORD event = {};
  KeyInfo keyInfo = {};

  if( key == NULL ) { return 0; }

  if( (PeekConsoleInput(stdInput, &event, 1,
      &eventsRead) == 0) || (eventsRead == 0) ) {
    return 0;
  }

  if( event.EventType == KEY_EVENT ) {
    if( (ReadConsoleInput(stdInput, &event, 1, &eventsRead) == 0) ||
        (event.Event.KeyEvent.bKeyDown == 0) ) {
      return 0;
    }

    keyInfo.code = event.Event.KeyEvent.wVirtualKeyCode;
    keyInfo.ch = event.Event.KeyEvent.uChar.AsciiChar;
    keyInfo.modifiers = event.Event.KeyEvent.dwControlKeyState;

    *key = keyInfo;
    return 1;
  }

  return 0;
}

void RouteEvent() {
  DWORD eventsRead = 1;
  INPUT_RECORD event = {};

  if( (PeekConsoleInput(stdInput, &event, 1,
      &eventsRead) == 0) || (eventsRead == 0) ) {
    return;
  }

  if( (ReadConsoleInput(stdInput, &event, 1, &eventsRead) == 0) ) {
    return;
  }
}

/** App implementation **/

unsigned IsActive() {
  if( appIsActive &&
      (outBuffer != INVALID_HANDLE_VALUE) &&
      (stdInput != INVALID_HANDLE_VALUE) ) {
    return 1;
  }

  return 0;
}

void ExitApp() {
  if( appIsActive ) {
    appIsActive = 0;
  }
}
