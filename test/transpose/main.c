#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <complex.h>
#include <mpi.h>
#include "sdecomp.h"

static int get_mysizes(
    const sdecomp_info_t * info,
    const sdecomp_pencil_t pencil,
    const size_t * glsizes,
    size_t * mysizes
){
  size_t ndims = 0;
  sdecomp.get_ndims(info, &ndims);
  for(size_t n = 0; n < ndims; n++){
    if(0 != sdecomp.get_pencil_mysize(info, pencil, n, glsizes[n], &mysizes[n])){
      return 1;
    }
  }
  return 0;
}

static int get_offsets(
    const sdecomp_info_t * info,
    const sdecomp_pencil_t pencil,
    const size_t * glsizes,
    size_t * offsets
){
  size_t ndims = 0;
  sdecomp.get_ndims(info, &ndims);
  for(size_t n = 0; n < ndims; n++){
    if(0 != sdecomp.get_pencil_offset(info, pencil, n, glsizes[n], &offsets[n])){
      return 1;
    }
  }
  return 0;
}

static int get_nitems(
    const size_t ndims,
    const size_t * mysizes,
    size_t * nitems
){
  *nitems = 1;
  for(size_t dim = 0; dim < ndims; dim++){
    const size_t mysize = mysizes[dim];
    if(SIZE_MAX / mysize < *nitems){
      return 1;
    }
    *nitems *= mysize;
  }
  return 0;
}

static int get_indices(
    const size_t ndims,
    const sdecomp_pencil_t pencil,
    const size_t * mysizes,
    size_t index,
    size_t * indices
){
  if(2 == ndims){
    // x1 pencil
    //   + j * x1pencil_mysizes[0]
    //   + i
    // y1 pencil
    //   + i * y1pencil_mysizes[0]
    //   + j
    if(SDECOMP_X1PENCIL == pencil){
      indices[1] = index / mysizes[0];
      indices[0] = index - mysizes[0] * indices[1];
      return 0;
    }else if(SDECOMP_Y1PENCIL == pencil){
      indices[0] = index / mysizes[1];
      indices[1] = index - mysizes[1] * indices[0];
      return 0;
    }else{
      return 1;
    }
  }else{
    // x pencils
    //   + k * x1pencil_mysizes[0] * x1pencil_mysizes[1]
    //   + j * x1pencil_mysizes[0]
    //   + i
    // y pencils
    //   + i * y1pencil_mysizes[1] * y1pencil_mysizes[2]
    //   + k * y1pencil_mysizes[1]
    //   + j
    // z pencils
    //   + j * z1pencil_mysizes[2] * z1pencil_mysizes[0]
    //   + i * z1pencil_mysizes[2]
    //   + k
    if(SDECOMP_X1PENCIL == pencil || SDECOMP_X2PENCIL == pencil){
      indices[2] = index / (mysizes[0] * mysizes[1]);
      index      = index - (mysizes[0] * mysizes[1]) * indices[2];
      indices[1] = index /  mysizes[0];
      indices[0] = index -  mysizes[0]               * indices[1];
      return 0;
    }else if(SDECOMP_Y1PENCIL == pencil || SDECOMP_Y2PENCIL == pencil){
      indices[0] = index / (mysizes[1] * mysizes[2]);
      index      = index - (mysizes[1] * mysizes[2]) * indices[0];
      indices[2] = index /  mysizes[1];
      indices[1] = index -  mysizes[1]               * indices[2];
      return 0;
    }else if(SDECOMP_Z1PENCIL == pencil || SDECOMP_Z2PENCIL == pencil){
      indices[1] = index / (mysizes[2] * mysizes[0]);
      index      = index - (mysizes[2] * mysizes[0]) * indices[1];
      indices[0] = index /  mysizes[2];
      indices[2] = index -  mysizes[2]               * indices[0];
      return 0;
    }else{
      return 1;
    }
  }
}

static uint_fast8_t answer(
    const size_t ndims,
    const size_t * glsizes,
    const size_t * offsets,
    const size_t * indices
){
  // it may overflow, which is an expected and a well-defined behaviour
  if(2 == ndims){
    return
      + (indices[1] + offsets[1]) * glsizes[0]
      + (indices[0] + offsets[0]);
  }else{
    return
      + (indices[2] + offsets[2]) * glsizes[1] * glsizes[0]
      + (indices[1] + offsets[1]) * glsizes[0]
      + (indices[0] + offsets[0]);
  }
}

static int init_bef_pencil(
    const size_t ndims,
    const size_t * glsizes,
    const sdecomp_pencil_t pencil_bef,
    const size_t bef_nitems,
    const size_t * bef_mysizes,
    const size_t * bef_offsets,
    const size_t size_of_element,
    uint_fast8_t * bef
){
  uint_fast8_t * values = calloc(1, size_of_element);
  for(size_t index = 0; index < bef_nitems; index++){
    size_t * indices = calloc(ndims, sizeof(size_t));
    if(0 != get_indices(ndims, pencil_bef, bef_mysizes, index, indices)){
      return 1;
    }
    const uint_fast8_t value = answer(ndims, glsizes, bef_offsets, indices);
    free(indices);
    for(size_t n = 0; n < size_of_element; n++){
      values[n] = value;
    }
    memcpy(
        bef + index * size_of_element,
        values,
        size_of_element
    );
  }
  free(values);
  return 0;
}

static int check_aft_pencil(
    const size_t ndims,
    const size_t * glsizes,
    const sdecomp_pencil_t pencil_aft,
    const size_t aft_nitems,
    const size_t * aft_mysizes,
    const size_t * aft_offsets,
    const size_t size_of_element,
    uint_fast8_t * aft,
    bool * success
){
  uint_fast8_t * values = calloc(1, size_of_element);
  for(size_t index = 0; index < aft_nitems; index++){
    size_t * indices = calloc(ndims, sizeof(size_t));
    if(0 != get_indices(ndims, pencil_aft, aft_mysizes, index, indices)){
      return 1;
    }
    const uint_fast8_t value = answer(ndims, glsizes, aft_offsets, indices);
    free(indices);
    for(size_t n = 0; n < size_of_element; n++){
      values[n] = value;
    }
    if(0 != memcmp(
      aft + index * size_of_element,
      values,
      size_of_element
    )){
      *success = false;
      break;
    }
  }
  free(values);
  return 0;
}

static int write_result(
    const sdecomp_info_t * info,
    const size_t * glsizes,
    const sdecomp_pencil_t pencil_bef,
    const sdecomp_pencil_t pencil_aft,
    const size_t size_of_element,
    const bool success
){
  size_t ndims = 0;
  sdecomp.get_ndims(info, &ndims);
  int nprocs = 0;
  int myrank = 0;
  sdecomp.get_comm_size(info, &nprocs);
  sdecomp.get_comm_rank(info, &myrank);
  if(0 == myrank){
    printf("size: ");
    for(size_t n = 0; n < ndims; n++){
      printf("%4zu%s", glsizes[n], ndims - 1 == n ? ", " : " x ");
    }
    printf("%4d procs, ", nprocs);
    printf("size of element: %2zu, ", size_of_element);
    printf("from %u to %u - ", pencil_bef, pencil_aft);
    printf("%s\n", success ? "PASSED" : "FAILED");
  }
  return 0;
}

static int kernel(
    const sdecomp_info_t * info,
    const size_t * glsizes,
    const sdecomp_pencil_t pencil_bef,
    const sdecomp_pencil_t pencil_aft,
    const size_t size_of_element
){
  size_t ndims = 0;
  sdecomp.get_ndims(info, &ndims);
  // prepare
  size_t * bef_mysizes = calloc(ndims, sizeof(size_t));
  size_t * bef_offsets = calloc(ndims, sizeof(size_t));
  size_t * aft_mysizes = calloc(ndims, sizeof(size_t));
  size_t * aft_offsets = calloc(ndims, sizeof(size_t));
  if(0 != get_mysizes(info, pencil_bef, glsizes, bef_mysizes)){
    return 1;
  }
  if(0 != get_offsets(info, pencil_bef, glsizes, bef_offsets)){
    return 1;
  }
  if(0 != get_mysizes(info, pencil_aft, glsizes, aft_mysizes)){
    return 1;
  }
  if(0 != get_offsets(info, pencil_aft, glsizes, aft_offsets)){
    return 1;
  }
  size_t bef_nitems = 0;
  size_t aft_nitems = 0;
  if(0 != get_nitems(ndims, bef_mysizes, &bef_nitems)){
    return 1;
  }
  if(0 != get_nitems(ndims, aft_mysizes, &aft_nitems)){
    return 1;
  }
  uint_fast8_t * bef = calloc(bef_nitems, size_of_element);
  uint_fast8_t * aft = calloc(aft_nitems, size_of_element);
  // set send buffer
  init_bef_pencil(ndims, glsizes, pencil_bef, bef_nitems, bef_mysizes, bef_offsets, size_of_element, bef);
  // transpose
  sdecomp_transpose_plan_t * plan = NULL;
  if(0 != sdecomp.transpose.construct(
      info,
      pencil_bef,
      pencil_aft,
      glsizes,
      size_of_element,
      &plan
  )){
    return 1;
  }
  if(0 != sdecomp.transpose.execute(plan, bef, aft)){
    return 1;
  }
  if(0 != sdecomp.transpose.destruct(plan)){
    return 1;
  }
  // check receive buffer
  bool success = true;
  check_aft_pencil(ndims, glsizes, pencil_aft, aft_nitems, aft_mysizes, aft_offsets, size_of_element, aft, &success);
  // clean up
  free(bef_mysizes);
  free(bef_offsets);
  free(aft_mysizes);
  free(aft_offsets);
  free(bef);
  free(aft);
  write_result(info, glsizes, pencil_bef, pencil_aft, size_of_element, success);
  return success ? 0 : 1;
}

static int test(
    const sdecomp_info_t * info,
    const size_t * glsizes,
    const size_t size_of_element
){
  int retval = 0;
  size_t ndims = 0;
  sdecomp.get_ndims(info, &ndims);
  // test all possible transpose cases
  if(2 == ndims){
    retval += kernel(info, glsizes, SDECOMP_X1PENCIL, SDECOMP_Y1PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_Y1PENCIL, SDECOMP_X1PENCIL, size_of_element);
  }else{
    retval += kernel(info, glsizes, SDECOMP_X1PENCIL, SDECOMP_Y1PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_Y1PENCIL, SDECOMP_Z1PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_Z1PENCIL, SDECOMP_X2PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_X2PENCIL, SDECOMP_Y2PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_Y2PENCIL, SDECOMP_Z2PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_Z2PENCIL, SDECOMP_X1PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_X1PENCIL, SDECOMP_Z2PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_Y1PENCIL, SDECOMP_X1PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_Z1PENCIL, SDECOMP_Y1PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_X2PENCIL, SDECOMP_Z1PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_Y2PENCIL, SDECOMP_X2PENCIL, size_of_element);
    retval += kernel(info, glsizes, SDECOMP_Z2PENCIL, SDECOMP_Y2PENCIL, size_of_element);
  }
  return retval;
}

static int get_domain_size(
    const char argv[],
    size_t * size
){
  *size = 0;
  // convert string to number (long)
  errno = 0;
  const long lsize = strtol(argv, NULL, 10);
  if(0 != errno){
    perror("argument is too large");
    return 1;
  }
  // check overflow etc.
  if(lsize <= 0){
    printf("positive integer is expected, obtain: %ld\n", lsize);
    return 1;
  }
  if(INT_MAX <= lsize){
    printf("integer smaller than %d is expected, obtain: %ld\n", INT_MAX, lsize);
    return 1;
  }
  *size = (size_t)(lsize);
  return 0;
}

int main(
    int argc,
    char * argv[]
){
  int retval = 0;
  MPI_Init(&argc, &argv);
  int nprocs = 0;
  int myrank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  if(3 != argc && 4 != argc){
    retval = 1;
    goto terminate;
  }
  // e.g. char, short, int, double, double complex
  const size_t size_of_elements[] = {1, 2, 4, 8, 16};
  const size_t ndims = argc - 1;
  // init domain decomposition
  size_t * dims = calloc(ndims, sizeof(size_t));
  bool * periods = calloc(ndims, sizeof(bool));
  sdecomp_info_t * info = NULL;
  if(0 != sdecomp.construct(MPI_COMM_WORLD, ndims, dims, periods, &info)){
    return 1;
  }
  free(dims);
  free(periods);
  // domain size
  size_t * glsizes = calloc(ndims, sizeof(size_t));
  for(size_t n = 0; n < ndims; n++){
    if(0 != get_domain_size(argv[n + 1], &glsizes[n])){
      return 1;
    }
  }
  // test rotations, for various element sizes, all forward and backward
  const size_t nitems = sizeof(size_of_elements) / sizeof(size_of_elements[0]);
  for(size_t n = 0; n < nitems; n++){
    const size_t size_of_element = size_of_elements[n];
    retval += test(info, glsizes, size_of_element);
  }
  free(glsizes);
  // clean-up domain decomposition
  if(0 != sdecomp.destruct(info)){
    return 1;
  }
  // clean-up
terminate:
  MPI_Finalize();
  return retval;
}

