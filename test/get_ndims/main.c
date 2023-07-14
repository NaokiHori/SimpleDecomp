#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
#include "sdecomp.h"

static int test(
    const size_t ndims
){
  sdecomp_info_t * info = NULL;
  size_t * dims = calloc(ndims, sizeof(size_t));
  bool * periods = calloc(ndims, sizeof(bool));
  for(size_t n = 0; n < ndims; n++){
    dims[n] = 0;
    periods[n] = false;
  }
  if(0 != sdecomp.construct(MPI_COMM_WORLD, ndims, dims, periods, &info)){
    return 1;
  }
  size_t ndims_ = 0;
  if(0 != sdecomp.get_ndims(info, &ndims_)){
    return 1;
  }
  if(ndims != ndims_){
    return 1;
  }
  free(dims);
  free(periods);
  return 0;
}

int main(
    void
){
  int retval = 0;
  MPI_Init(NULL, NULL);
  retval += test(2);
  retval += test(3);
  MPI_Finalize();
  return retval;
}

