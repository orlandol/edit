
#include <stdlib.h>
#include <stdio.h>

#include "edit.h"
#include "cui.h"

Console* console = NULL;

/** Edit event handler declarations **/

unsigned ConfirmClose( void* handlerData );

/*
 * void Cleanup()
 *
 * Releases allocated resources.
 *
 * Returns: Nothing
 *
 */
void Cleanup() {
  ResetKeyHandler( console );
  ResetMouseHandler( console );

  ReleaseConsole( &console );
}

/*
 * Main program
 */
int main( int argc, char** argv ) {
  atexit( Cleanup );

  console = InitConsole();
  if( console == NULL ) {
    printf( "Error initializing console\n" );
    exit(1);
  }

  SetCloseHandler( console, ConfirmClose, NULL );

  while( IsActive(console) ) {
    RouteEvents( console );
    UpdateUI( console );

    ExitApp( console ); // Temporary placeholder
  }

  Cleanup();

  return 0;
}

/** Edit event handler implementation **/

unsigned ConfirmClose( void* handlerData ) {
  return 1; // Temporary placeholder
}
