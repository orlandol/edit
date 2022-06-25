
#include <stdio.h>

#ifdef _WIN32
#include "windows.h"

#ifndef ENABLE_EXTENDED_FLAGS
#define ENABLE_EXTENDED_FLAGS 0x0080
#endif

#define keyEscape (VK_ESCAPE)
#endif

typedef struct KeyInfo {
  unsigned code;
  char ch;
  unsigned modifiers;
  unsigned count;
} KeyInfo;

unsigned Keypressed();
unsigned ReadKey( KeyInfo* key );
void RouteEvent();

HANDLE stdInput = NULL;

unsigned Keypressed() {
  DWORD eventsRead = 1;
  INPUT_RECORD event = {};
  KeyInfo keyInfo = {};

  if( (PeekConsoleInput(stdInput, &event, 1,
      &eventsRead) == 0) || (eventsRead == 0) ) {
    return 0;
  }

  if( event.EventType == KEY_EVENT ) {
    return 1;
  }

  return 0;
}

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

int main( int argc, char** argv ) {
  KeyInfo key = {};
  DWORD callerInputMode = 0;

  stdInput = GetStdHandle(STD_INPUT_HANDLE);

  GetConsoleMode( stdInput, &callerInputMode );

  SetConsoleMode(stdInput, ENABLE_EXTENDED_FLAGS |
      ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT |
      ENABLE_WINDOW_INPUT );

  while( key.code != keyEscape ) {
    if( ReadKey(&key) ) {
      if( key.ch ) {
        putchar( key.ch );
      }
    } else {
      RouteEvent();
    }
  }

  SetConsoleMode( stdInput, callerInputMode );

  return 0;
}
