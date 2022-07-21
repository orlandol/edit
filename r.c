
#include <stdio.h>

#ifdef _WIN32
#include "windows.h"

#ifndef ENABLE_EXTENDED_FLAGS
#define ENABLE_EXTENDED_FLAGS 0x0080
#endif

#define keyEscape (VK_ESCAPE)
#endif

enum EventType {
  evtNone,
  evtKeyUp,
  evtKeyDown
};

typedef struct KeyInfo {
  unsigned code;
  char ch;
  unsigned modifiers;
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

  if( (ReadConsoleInput(stdInput, &event, 1, &eventsRead) == 0) ) {
    return 0;
  }

  if( event.EventType == KEY_EVENT ) {
    /// TODO: Call OnKeyDown or OnKeyUp based on bKeyDown

    keyInfo.code = event.Event.KeyEvent.wVirtualKeyCode;
    keyInfo.ch = event.Event.KeyEvent.uChar.AsciiChar;
    keyInfo.modifiers = event.Event.KeyEvent.dwControlKeyState;

    *key = keyInfo;
    return 1;
  }

  return 0;
}

KeyInfo key = {};

void RouteEvent() {
  DWORD eventsRead = 1;
  INPUT_RECORD event = {};

  if( (ReadConsoleInput(stdInput, &event, 1, &eventsRead) == 0) ) {
    return;
  }

  switch( event.EventType ) {
  case MOUSE_EVENT:
    printf( "[MOUSE_EVENT: ]\n" );
    break;

  case KEY_EVENT:
    key.code = event.Event.KeyEvent.wVirtualKeyCode;
    key.ch = event.Event.KeyEvent.uChar.AsciiChar;
    key.modifiers = event.Event.KeyEvent.dwControlKeyState;

    if( event.Event.KeyEvent.bKeyDown ) {
      if( key.ch == '\r' ) {
        printf( "\n" );
      } else {
        putchar( event.Event.KeyEvent.uChar.AsciiChar );
      }
    }
    break;

  default:
    printf( "[Other Event: ]\n" );
  }
}

int main( int argc, char** argv ) {
  DWORD callerInputMode = 0;

  stdInput = GetStdHandle(STD_INPUT_HANDLE);

  GetConsoleMode( stdInput, &callerInputMode );

  SetConsoleMode(stdInput, ENABLE_EXTENDED_FLAGS |
      ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT |
      ENABLE_WINDOW_INPUT );

  while( key.code != keyEscape ) {
    RouteEvent();

    /// TODO: Draw UI
  }

  SetConsoleMode( stdInput, callerInputMode );

  return 0;
}
