
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

  Cleanup();

  return 0;
}
