#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <mpi.h>
#include "test.h"


int main(int argc, char *argv[]){
  /* decide global array size and exec tests */
  MPI_Init(&argc, &argv);
  // sanitise input
  // depending on 2D or 3D,
  //   argc == 3: __FILE__, litot, ljtot
  //   argc == 4: __FILE__, litot, ljtot, lktot
  assert(argc == 3 || argc == 4);
  if(argc == 3){
    // 2D
    const long litot = strtol(argv[1], NULL, 10);
    const long ljtot = strtol(argv[2], NULL, 10);
    // prepare to convet to integers
    assert(0 < litot && litot < INT_MAX);
    assert(0 < ljtot && ljtot < INT_MAX);
    // we got the domain size
    const int itot = (int)litot;
    const int jtot = (int)ljtot;
    // now it's time to test transposings
    test_fwrd_2d(itot, jtot);
    test_bwrd_2d(itot, jtot);
  }else{
    // 3D
    const long litot = strtol(argv[1], NULL, 10);
    const long ljtot = strtol(argv[2], NULL, 10);
    const long lktot = strtol(argv[3], NULL, 10);
    // prepare to convet to integers
    assert(0 < litot && litot < INT_MAX);
    assert(0 < ljtot && ljtot < INT_MAX);
    assert(0 < lktot && lktot < INT_MAX);
    // we got the domain size
    const int itot = (int)litot;
    const int jtot = (int)ljtot;
    const int ktot = (int)lktot;
    // now it's time to test transposings
    test_fwrd_3d(itot, jtot, ktot);
    test_bwrd_3d(itot, jtot, ktot);
  }
  // clean-up
  MPI_Finalize();
  return 0;
}

