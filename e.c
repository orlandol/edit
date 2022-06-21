
#include <stdio.h>
#include <stdlib.h>

/** Console declarations **/

unsigned InitConsole();
void ReleaseConsole();

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
  atexit( Cleanup );

  if( InitConsole() == 0 ) {
    printf( "Error initializing console\n" );
    exit(1);
  }

  while( IsActive() ) {
    ExitApp(); // Temporary placeholder
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

HANDLE stdIn = INVALID_HANDLE_VALUE;
HANDLE stdOut = INVALID_HANDLE_VALUE;
HANDLE stdErr = INVALID_HANDLE_VALUE;

HANDLE outBuffer = INVALID_HANDLE_VALUE;

unsigned InitConsole() {
  stdIn = GetStdHandle(STD_INPUT_HANDLE);
  stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  stdErr = GetStdHandle(STD_ERROR_HANDLE);

  // Initialize handle values for proper cleanup
  if( (stdIn == INVALID_HANDLE_VALUE) ||
      (stdOut == INVALID_HANDLE_VALUE) ||
      (stdErr == INVALID_HANDLE_VALUE) ) {
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
  if( GetConsoleMode(stdIn, &callerInputMode) == 0 ) {
    goto ReturnError;
  }

  // Set Edit input mode
  if( SetConsoleMode(stdIn, ENABLE_EXTENDED_FLAGS |
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
  if( stdIn != INVALID_HANDLE_VALUE ) {
    FlushConsoleInputBuffer( stdIn );

    if( callerInputMode ) {
      SetConsoleMode( stdIn, callerInputMode );
    }
  }

  if( stdOut != INVALID_HANDLE_VALUE ) {
    SetConsoleActiveScreenBuffer( stdOut );
  }

  if( outBuffer != INVALID_HANDLE_VALUE ) {
    CloseHandle( outBuffer );
    outBuffer = INVALID_HANDLE_VALUE;
  }

  stdIn = INVALID_HANDLE_VALUE;
  stdOut = INVALID_HANDLE_VALUE;
  stdErr = INVALID_HANDLE_VALUE;
}

/** App implementation **/

unsigned IsActive() {
  if( appIsActive &&
      (outBuffer != INVALID_HANDLE_VALUE) &&
      (stdIn != INVALID_HANDLE_VALUE) ) {
    return appIsActive;
  }

  return 0;
}

void ExitApp() {
  if( appIsActive ) {
    appIsActive = 0;
  }
}
