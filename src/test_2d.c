#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <mpi.h>
#include "test.h"
#include "sdecomp.h"


// test forward and backward transposings

#define SDECOMP_NDIMS 2

// TEST_MYTYPE:     datatype to be transposed, e.g., double
// TEST_MPI_MYTYPE: datatype to be transposed, e.g., MPI_DOUBLE
#define TEST_MYTYPE     double
#define TEST_MPI_MYTYPE MPI_DOUBLE

int test_fwrd_2d(const int itot, const int jtot){
  // init decomposition library
  const int dims[SDECOMP_NDIMS] = {0};
  const int periods[SDECOMP_NDIMS] = {0};
  sdecomp_t *sdecomp = sdecomp_init(MPI_COMM_WORLD, SDECOMP_NDIMS, dims, periods);
  // pack global array size which will be extensively used later
  const int gsizes[SDECOMP_NDIMS] = {itot, jtot};
  // local sizes (and offset only for x1) of pencils
  int x1pencil_mysizes[SDECOMP_NDIMS], y1pencil_mysizes[SDECOMP_NDIMS];
  int x1pencil_offsets[SDECOMP_NDIMS];
  for(int dim = 0; dim < SDECOMP_NDIMS; dim++){
    x1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_X1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    y1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Y1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    x1pencil_offsets[dim] = sdecomp_get_pencil_offset(sdecomp, SDECOMP_X1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
  }
  // allocate buffers storing pencils
  TEST_MYTYPE *x1pencil = sdecomp_calloc((size_t)x1pencil_mysizes[0] * (size_t)x1pencil_mysizes[1], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *y1pencil = sdecomp_calloc((size_t)y1pencil_mysizes[0] * (size_t)y1pencil_mysizes[1], sizeof(TEST_MYTYPE));
  // initialise x1pencil
  {
    const int isize = x1pencil_mysizes[0];
    const int jsize = x1pencil_mysizes[1];
    const int ioffs = x1pencil_offsets[0];
    const int joffs = x1pencil_offsets[1];
    for(int j = 0; j < jsize; j++){
      for(int i = 0; i < isize; i++){
        int index = j * isize + i;
        TEST_MYTYPE value = (TEST_MYTYPE)((j + joffs) * itot + (i + ioffs));
        x1pencil[index] = value;
      }
    }
  }
  // convert x1pencil to y1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_fwrd_init(sdecomp, SDECOMP_X1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_2d_x1_to_y1(sdecomp, plan);
    sdecomp_transpose_execute(plan, x1pencil, y1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert y1pencil to x1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_fwrd_init(sdecomp, SDECOMP_Y1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_2d_y1_to_x1(sdecomp, plan);
    sdecomp_transpose_execute(plan, y1pencil, x1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // check result
  {
    const int isize = x1pencil_mysizes[0];
    const int jsize = x1pencil_mysizes[1];
    const int ioffs = x1pencil_offsets[0];
    const int joffs = x1pencil_offsets[1];
    for(int j = 0; j < jsize; j++){
      for(int i = 0; i < isize; i++){
        int index = j * isize + i;
        TEST_MYTYPE value = (TEST_MYTYPE)((j + joffs) * itot + (i + ioffs));
        assert(0 == memcmp(&x1pencil[index], &value, sizeof(TEST_MYTYPE)));
      }
    }
  }
  // clean-up
  sdecomp_free(x1pencil);
  sdecomp_free(y1pencil);
  sdecomp_finalise(sdecomp);
  // report success
  {
    int myrank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    if(myrank == 0){
      printf("%s success\n", __func__);
    }
  }
  return 0;
}

int test_bwrd_2d(const int itot, const int jtot){
  // init decomposition library
  const int dims[SDECOMP_NDIMS] = {0};
  const int periods[SDECOMP_NDIMS] = {0};
  sdecomp_t *sdecomp = sdecomp_init(MPI_COMM_WORLD, SDECOMP_NDIMS, dims, periods);
  // pack global array size which will be extensively used later
  const int gsizes[SDECOMP_NDIMS] = {itot, jtot};
  // local sizes (and offset only for x1) of pencils
  int x1pencil_mysizes[SDECOMP_NDIMS], y1pencil_mysizes[SDECOMP_NDIMS];
  int x1pencil_offsets[SDECOMP_NDIMS];
  for(int dim = 0; dim < SDECOMP_NDIMS; dim++){
    x1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_X1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    y1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Y1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    x1pencil_offsets[dim] = sdecomp_get_pencil_offset(sdecomp, SDECOMP_X1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
  }
  // allocate buffers storing pencils
  TEST_MYTYPE *x1pencil = sdecomp_calloc((size_t)x1pencil_mysizes[0] * (size_t)x1pencil_mysizes[1], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *y1pencil = sdecomp_calloc((size_t)y1pencil_mysizes[0] * (size_t)y1pencil_mysizes[1], sizeof(TEST_MYTYPE));
  // initialise x1pencil
  {
    const int isize = x1pencil_mysizes[0];
    const int jsize = x1pencil_mysizes[1];
    const int ioffs = x1pencil_offsets[0];
    const int joffs = x1pencil_offsets[1];
    for(int j = 0; j < jsize; j++){
      for(int i = 0; i < isize; i++){
        int index = j * isize + i;
        TEST_MYTYPE value = (TEST_MYTYPE)((j + joffs) * itot + (i + ioffs));
        x1pencil[index] = value;
      }
    }
  }
  // convert x1pencil to y1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_bwrd_init(sdecomp, SDECOMP_X1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_2d_x1_to_y1(sdecomp, plan);
    sdecomp_transpose_execute(plan, x1pencil, y1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert y1pencil to x1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_bwrd_init(sdecomp, SDECOMP_Y1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_2d_y1_to_x1(sdecomp, plan);
    sdecomp_transpose_execute(plan, y1pencil, x1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // check result
  {
    const int isize = x1pencil_mysizes[0];
    const int jsize = x1pencil_mysizes[1];
    const int ioffs = x1pencil_offsets[0];
    const int joffs = x1pencil_offsets[1];
    for(int j = 0; j < jsize; j++){
      for(int i = 0; i < isize; i++){
        int index = j * isize + i;
        TEST_MYTYPE value = (TEST_MYTYPE)((j + joffs) * itot + (i + ioffs));
        assert(0 == memcmp(&x1pencil[index], &value, sizeof(double)));
      }
    }
  }
  // clean-up
  sdecomp_free(x1pencil);
  sdecomp_free(y1pencil);
  sdecomp_finalise(sdecomp);
  // report success
  {
    int myrank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    if(myrank == 0){
      printf("%s success\n", __func__);
    }
  }
  return 0;
}

#undef TEST_MYTYPE
#undef TEST_MPI_MYTYPE

#undef SDECOMP_NDIMS

