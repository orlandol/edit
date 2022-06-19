#ifndef CUI_H
#define CUI_H

/** Abstract console declarations **/

typedef struct SysConsole Console;

Console* InitConsole();
void ReleaseConsole( Console** consolePtr );

/** Event handler declarations **/

/// Keyboard event declarations ///

typedef struct KeyCodeType {
  unsigned keyEscape;
  unsigned keySpace;
  unsigned keyReturn;
  unsigned keyBackspace;
  unsigned keyTab;
  unsigned keyLeftShift;
  unsigned keyRightShift;
  unsigned keyShift;
  unsigned keyLeftCtrl;
  unsigned keyRightCtrl;
  unsigned keyCtrl;
  unsigned keyMenu;
  unsigned keyPause;

  unsigned keyInsert;
  unsigned keyDelete;

  unsigned keyCapsLock;
  unsigned keyScrollLock;
  unsigned keyNumLock;

  unsigned keyF1;
  unsigned keyF2;
  unsigned keyF3;
  unsigned keyF4;
  unsigned keyF5;
  unsigned keyF6;
  unsigned keyF7;
  unsigned keyF8;
  unsigned keyF9;
  unsigned keyF10;
  unsigned keyF11;
  unsigned keyF12;

  unsigned keyPageUp;
  unsigned keyPageDown;
  unsigned keyHome;
  unsigned keyEnd;
  unsigned keyLeft;
  unsigned keyUp;
  unsigned keyRight;
  unsigned keyDown;
  unsigned keyPrintScreen;

  unsigned keyLeftWin;
  unsigned keyRightWin;

  unsigned keyTilde;
  unsigned keyUnderscore;

  unsigned key0;
  unsigned key1;
  unsigned key2;
  unsigned key3;
  unsigned key4;
  unsigned key5;
  unsigned key6;
  unsigned key7;
  unsigned key8;
  unsigned key9;

  unsigned num0;
  unsigned num1;
  unsigned num2;
  unsigned num3;
  unsigned num4;
  unsigned num5;
  unsigned num6;
  unsigned num7;
  unsigned num8;
  unsigned num9;

  unsigned numMul;
  unsigned numDiv;
  unsigned numAdd;
  unsigned numSub;
  unsigned numDot;
  unsigned numEnter;

  unsigned keyA;
  unsigned keyB;
  unsigned keyC;
  unsigned keyD;
  unsigned keyE;
  unsigned keyF;
  unsigned keyG;
  unsigned keyH;
  unsigned keyI;
  unsigned keyJ;
  unsigned keyK;
  unsigned keyL;
  unsigned keyM;
  unsigned keyN;
  unsigned keyO;
  unsigned keyP;
  unsigned keyQ;
  unsigned keyR;
  unsigned keyS;
  unsigned keyT;
  unsigned keyU;
  unsigned keyV;
  unsigned keyW;
  unsigned keyX;
  unsigned keyY;
  unsigned keyZ;
} KeyCodeType;

extern const KeyCodeType KeyCode;

enum ControlKeyState {
  capsLockOn = (1 << 0),
  numLockOn = (1 << 1),
  scrollLockOn = (1 << 2),

  leftShiftKeyDown = (1 << 3),
  rightShiftKeyDown = (1 << 4),
  shiftKeyDown = (leftShiftKeyDown | rightShiftKeyDown),

  leftAltKeyDown = (1 << 5),
  rightAltKeyDown = (1 << 6),
  altKeyDown = (leftAltKeyDown | rightAltKeyDown),

  leftCtrlKeyDown = (1 << 7),
  rightCtrlKeyDown = (1 << 8),
  ctrlKeyDown = (leftCtrlKeyDown | rightCtrlKeyDown)
};

typedef struct KeyEvent {
  unsigned keyCode;
  char keyChar;
  unsigned repeatCount;
  unsigned controlKeyState;
} KeyEvent;

typedef unsigned (*KeyHandler)( void* handlerData );

unsigned SetKeyHandler( Console* console,
  KeyHandler onKey, void* handlerData );
void ResetKeyHandler( Console* console );

/// Mouse event declarations ///
typedef unsigned (*MouseHandler)( void* handlerData );

unsigned SetMouseHandler( Console* console,
  MouseHandler onMouse, void* handlerData );
void ResetMouseHandler( Console* console );

/// Close event handler ///
typedef unsigned (*CloseHandler)( void* handlerData );

unsigned SetCloseHandler( Console* console,
  CloseHandler onClose, void* handlerData );
void ResetCloseHandler( Console* console );

/** App declarations **/

/// App main loop declarations ///
unsigned IsActive( Console* console );
void RouteEvents( Console* console );
void UpdateUI( Console* console );

/// App state control declarations ///
void ExitApp( Console* console );

#endif
