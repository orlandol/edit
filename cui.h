#ifndef CUI_H
#define CUI_H

/** Abstract console declarations **/

typedef struct SysConsole Console;

Console* InitConsole();
void ReleaseConsole( Console** consolePtr );

/** Event handler declarations **/

enum EventType {
  evtKeyDown = 1,
  evtKeyUp,
  evtMouseMove,
  evtMouseUp,
  evtMouseDown
};

/// Keyboard event declarations ///

/// Key code declarations ///
extern const unsigned keyEscape;
extern const unsigned keySpace;
extern const unsigned keyReturn;
extern const unsigned keyBackspace;
extern const unsigned keyTab;
extern const unsigned keyLeftShift;
extern const unsigned keyRightShift;
extern const unsigned keyShift;
extern const unsigned keyLeftCtrl;
extern const unsigned keyRightCtrl;
extern const unsigned keyCtrl;
extern const unsigned keyLeftMenu;
extern const unsigned keyRightMenu;
extern const unsigned keyMenu;
extern const unsigned keyPause;
extern const unsigned keyInsert;
extern const unsigned keyDelete;
extern const unsigned keyCapsLock;
extern const unsigned keyScrollLock;
extern const unsigned keyNumLock;
extern const unsigned keyF1;
extern const unsigned keyF2;
extern const unsigned keyF3;
extern const unsigned keyF4;
extern const unsigned keyF5;
extern const unsigned keyF6;
extern const unsigned keyF7;
extern const unsigned keyF8;
extern const unsigned keyF9;
extern const unsigned keyF10;
extern const unsigned keyF11;
extern const unsigned keyF12;
extern const unsigned keyF13;
extern const unsigned keyF14;
extern const unsigned keyF15;
extern const unsigned keyF16;
extern const unsigned keyF17;
extern const unsigned keyF18;
extern const unsigned keyF19;
extern const unsigned keyF20;
extern const unsigned keyF21;
extern const unsigned keyF22;
extern const unsigned keyF23;
extern const unsigned keyF24;
extern const unsigned keyPageUp;
extern const unsigned keyPageDown;
extern const unsigned keyHome;
extern const unsigned keyEnd;
extern const unsigned keyLeft;
extern const unsigned keyUp;
extern const unsigned keyRight;
extern const unsigned keyDown;
extern const unsigned keyPrintScreen;
extern const unsigned keyLeftWin;
extern const unsigned keyRightWin;
extern const unsigned keyWin;
extern const unsigned key0;
extern const unsigned key1;
extern const unsigned key2;
extern const unsigned key3;
extern const unsigned key4;
extern const unsigned key5;
extern const unsigned key6;
extern const unsigned key7;
extern const unsigned key8;
extern const unsigned key9;
extern const unsigned num0;
extern const unsigned num1;
extern const unsigned num2;
extern const unsigned num3;
extern const unsigned num4;
extern const unsigned num5;
extern const unsigned num6;
extern const unsigned num7;
extern const unsigned num8;
extern const unsigned num9;
extern const unsigned numMul;
extern const unsigned numDiv;
extern const unsigned numAdd;
extern const unsigned numSub;
extern const unsigned numDot;
extern const unsigned numEnter;
extern const unsigned keyA;
extern const unsigned keyB;
extern const unsigned keyC;
extern const unsigned keyD;
extern const unsigned keyE;
extern const unsigned keyF;
extern const unsigned keyG;
extern const unsigned keyH;
extern const unsigned keyI;
extern const unsigned keyJ;
extern const unsigned keyK;
extern const unsigned keyL;
extern const unsigned keyM;
extern const unsigned keyN;
extern const unsigned keyO;
extern const unsigned keyP;
extern const unsigned keyQ;
extern const unsigned keyR;
extern const unsigned keyS;
extern const unsigned keyT;
extern const unsigned keyU;
extern const unsigned keyV;
extern const unsigned keyW;
extern const unsigned keyX;
extern const unsigned keyY;
extern const unsigned keyZ;

/// Modifier declarations ///
extern const unsigned capsLockOn;
extern const unsigned numLockOn;
extern const unsigned scrollLockOn;
extern const unsigned leftShiftKeyDown;
extern const unsigned rightShiftKeyDown;
extern const unsigned shiftKeyDown;
extern const unsigned leftAltKeyDown;
extern const unsigned rightAltKeyDown;
extern const unsigned altKeyDown;
extern const unsigned leftCtrlKeyDown;
extern const unsigned rightCtrlKeyDown;
extern const unsigned ctrlKeyDown;

typedef struct KeyEvent {
  unsigned eventType;
  unsigned keyCode;
  char keyChar;
  unsigned repeatCount;
  unsigned modifiers;
} KeyEvent;

typedef unsigned (*KeyHandler)( Console *console, void* handlerData, KeyEvent* keyEvent );

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
