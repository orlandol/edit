
#include <stdlib.h>
#include <stdio.h>

#include "edit.h"

Console* console = NULL;

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

  while( IsActive(console) ) {
    RouteEvents( console );
    UpdateUI( console );
  }

  Cleanup();

  return 0;
}
