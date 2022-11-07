#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <mpi.h>
#include "test.h"
#include "sdecomp.h"


// test forward (clockwise) and backward (counter-clockwise) operations

#define SDECOMP_NDIMS 3

// TEST_MYTYPE:     datatype to be transposed, e.g., int
// TEST_MPI_MYTYPE: datatype to be transposed, e.g., MPI_INT
#define TEST_MYTYPE     int
#define TEST_MPI_MYTYPE MPI_INT

int test_fwrd_3d(const int itot, const int jtot, const int ktot){
  // init decomposition library
  const int dims[SDECOMP_NDIMS] = {0};
  const int periods[SDECOMP_NDIMS] = {0};
  sdecomp_t *sdecomp = sdecomp_init(MPI_COMM_WORLD, SDECOMP_NDIMS, dims, periods);
  // pack global array size which will be extensively used later
  const int gsizes[SDECOMP_NDIMS] = {itot, jtot, ktot};
  // local sizes (and offset only for x1) of pencils
  int x1pencil_mysizes[SDECOMP_NDIMS], y1pencil_mysizes[SDECOMP_NDIMS], z1pencil_mysizes[SDECOMP_NDIMS],
      x2pencil_mysizes[SDECOMP_NDIMS], y2pencil_mysizes[SDECOMP_NDIMS], z2pencil_mysizes[SDECOMP_NDIMS];
  int x1pencil_offsets[SDECOMP_NDIMS];
  for(int dim = 0; dim < SDECOMP_NDIMS; dim++){
    x1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_X1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    y1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Y1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    z1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Z1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    x2pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_X2PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    y2pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Y2PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    z2pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Z2PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    x1pencil_offsets[dim] = sdecomp_get_pencil_offset(sdecomp, SDECOMP_X1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
  }
  // allocate buffers storing pencils
  TEST_MYTYPE *x1pencil = sdecomp_calloc((size_t)x1pencil_mysizes[0] * (size_t)x1pencil_mysizes[1] * (size_t)x1pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *y1pencil = sdecomp_calloc((size_t)y1pencil_mysizes[0] * (size_t)y1pencil_mysizes[1] * (size_t)y1pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *z1pencil = sdecomp_calloc((size_t)z1pencil_mysizes[0] * (size_t)z1pencil_mysizes[1] * (size_t)z1pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *x2pencil = sdecomp_calloc((size_t)x2pencil_mysizes[0] * (size_t)x2pencil_mysizes[1] * (size_t)x2pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *y2pencil = sdecomp_calloc((size_t)y2pencil_mysizes[0] * (size_t)y2pencil_mysizes[1] * (size_t)y2pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *z2pencil = sdecomp_calloc((size_t)z2pencil_mysizes[0] * (size_t)z2pencil_mysizes[1] * (size_t)z2pencil_mysizes[2], sizeof(TEST_MYTYPE));
  // initialise x1pencil
  {
    const int isize = x1pencil_mysizes[0];
    const int jsize = x1pencil_mysizes[1];
    const int ksize = x1pencil_mysizes[2];
    const int ioffs = x1pencil_offsets[0];
    const int joffs = x1pencil_offsets[1];
    const int koffs = x1pencil_offsets[2];
    for(int k = 0; k < ksize; k++){
      for(int j = 0; j < jsize; j++){
        for(int i = 0; i < isize; i++){
          int index = k * jsize * isize + j * isize + i;
          TEST_MYTYPE value = (TEST_MYTYPE)((k + koffs) * jtot * itot + (j + joffs) * itot + (i + ioffs));
          x1pencil[index] = value;
        }
      }
    }
  }
  // convert x1pencil to y1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_fwrd_init(sdecomp, SDECOMP_X1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_x1_to_y1(sdecomp, plan);
    sdecomp_transpose_execute(plan, x1pencil, y1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert y1pencil to z1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_fwrd_init(sdecomp, SDECOMP_Y1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_y1_to_z1(sdecomp, plan);
    sdecomp_transpose_execute(plan, y1pencil, z1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert z1pencil to x2pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_fwrd_init(sdecomp, SDECOMP_Z1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_z1_to_x2(sdecomp, plan);
    sdecomp_transpose_execute(plan, z1pencil, x2pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert x2pencil to y2pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_fwrd_init(sdecomp, SDECOMP_X2PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_x2_to_y2(sdecomp, plan);
    sdecomp_transpose_execute(plan, x2pencil, y2pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert y2pencil to z2pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_fwrd_init(sdecomp, SDECOMP_Y2PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_y2_to_z2(sdecomp, plan);
    sdecomp_transpose_execute(plan, y2pencil, z2pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert z2pencil to x1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_fwrd_init(sdecomp, SDECOMP_Z2PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_z2_to_x1(sdecomp, plan);
    sdecomp_transpose_execute(plan, z2pencil, x1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // check result
  {
    const int isize = x1pencil_mysizes[0];
    const int jsize = x1pencil_mysizes[1];
    const int ksize = x1pencil_mysizes[2];
    const int ioffs = x1pencil_offsets[0];
    const int joffs = x1pencil_offsets[1];
    const int koffs = x1pencil_offsets[2];
    for(int k = 0; k < ksize; k++){
      for(int j = 0; j < jsize; j++){
        for(int i = 0; i < isize; i++){
          int index = k * jsize * isize + j * isize + i;
          TEST_MYTYPE value = (TEST_MYTYPE)((k + koffs) * jtot * itot + (j + joffs) * itot + (i + ioffs));
          assert(0 == memcmp(&x1pencil[index], &value, sizeof(TEST_MYTYPE)));
        }
      }
    }
  }
  // clean-up
  sdecomp_free(x1pencil);
  sdecomp_free(y1pencil);
  sdecomp_free(z1pencil);
  sdecomp_free(x2pencil);
  sdecomp_free(y2pencil);
  sdecomp_free(z2pencil);
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

int test_bwrd_3d(const int itot, const int jtot, const int ktot){
  // init decomposition library
  const int dims[SDECOMP_NDIMS] = {0};
  const int periods[SDECOMP_NDIMS] = {0};
  sdecomp_t *sdecomp = sdecomp_init(MPI_COMM_WORLD, SDECOMP_NDIMS, dims, periods);
  // pack global array size which will be extensively used later
  const int gsizes[SDECOMP_NDIMS] = {itot, jtot, ktot};
  // local sizes (and offset only for x1) of pencils
  int x1pencil_mysizes[SDECOMP_NDIMS], y1pencil_mysizes[SDECOMP_NDIMS], z1pencil_mysizes[SDECOMP_NDIMS],
      x2pencil_mysizes[SDECOMP_NDIMS], y2pencil_mysizes[SDECOMP_NDIMS], z2pencil_mysizes[SDECOMP_NDIMS];
  int x1pencil_offsets[SDECOMP_NDIMS];
  for(int dim = 0; dim < SDECOMP_NDIMS; dim++){
    x1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_X1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    y1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Y1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    z1pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Z1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    x2pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_X2PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    y2pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Y2PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    z2pencil_mysizes[dim] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Z2PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
    x1pencil_offsets[dim] = sdecomp_get_pencil_offset(sdecomp, SDECOMP_X1PENCIL, (sdecomp_dir_t)dim, gsizes[dim]);
  }
  // allocate buffers storing pencils
  TEST_MYTYPE *x1pencil = sdecomp_calloc((size_t)x1pencil_mysizes[0] * (size_t)x1pencil_mysizes[1] * (size_t)x1pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *y1pencil = sdecomp_calloc((size_t)y1pencil_mysizes[0] * (size_t)y1pencil_mysizes[1] * (size_t)y1pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *z1pencil = sdecomp_calloc((size_t)z1pencil_mysizes[0] * (size_t)z1pencil_mysizes[1] * (size_t)z1pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *x2pencil = sdecomp_calloc((size_t)x2pencil_mysizes[0] * (size_t)x2pencil_mysizes[1] * (size_t)x2pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *y2pencil = sdecomp_calloc((size_t)y2pencil_mysizes[0] * (size_t)y2pencil_mysizes[1] * (size_t)y2pencil_mysizes[2], sizeof(TEST_MYTYPE));
  TEST_MYTYPE *z2pencil = sdecomp_calloc((size_t)z2pencil_mysizes[0] * (size_t)z2pencil_mysizes[1] * (size_t)z2pencil_mysizes[2], sizeof(TEST_MYTYPE));
  // initialise x1pencil
  {
    const int isize = x1pencil_mysizes[0];
    const int jsize = x1pencil_mysizes[1];
    const int ksize = x1pencil_mysizes[2];
    const int ioffs = x1pencil_offsets[0];
    const int joffs = x1pencil_offsets[1];
    const int koffs = x1pencil_offsets[2];
    for(int k = 0; k < ksize; k++){
      for(int j = 0; j < jsize; j++){
        for(int i = 0; i < isize; i++){
          int index = k * jsize * isize + j * isize + i;
          TEST_MYTYPE value = (TEST_MYTYPE)((k + koffs) * jtot * itot + (j + joffs) * itot + (i + ioffs));
          x1pencil[index] = value;
        }
      }
    }
  }
  // convert x1pencil to z2pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_bwrd_init(sdecomp, SDECOMP_X1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_x1_to_z2(sdecomp, plan);
    sdecomp_transpose_execute(plan, x1pencil, z2pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert z2pencil to y2pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_bwrd_init(sdecomp, SDECOMP_Z2PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_z2_to_y2(sdecomp, plan);
    sdecomp_transpose_execute(plan, z2pencil, y2pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert y2pencil to x2pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_bwrd_init(sdecomp, SDECOMP_Y2PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_y2_to_x2(sdecomp, plan);
    sdecomp_transpose_execute(plan, y2pencil, x2pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert x2pencil to z1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_bwrd_init(sdecomp, SDECOMP_X2PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_x2_to_z1(sdecomp, plan);
    sdecomp_transpose_execute(plan, x2pencil, z1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert z1pencil to y1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_bwrd_init(sdecomp, SDECOMP_Z1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_z1_to_y1(sdecomp, plan);
    sdecomp_transpose_execute(plan, z1pencil, y1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // convert y1pencil to x1pencil
  {
    sdecomp_transpose_t *plan = sdecomp_transpose_bwrd_init(sdecomp, SDECOMP_Y1PENCIL, gsizes, sizeof(TEST_MYTYPE), TEST_MPI_MYTYPE);
    sdecomp_test_transpose_3d_y1_to_x1(sdecomp, plan);
    sdecomp_transpose_execute(plan, y1pencil, x1pencil);
    sdecomp_transpose_finalise(plan);
  }
  // check result
  {
    const int isize = x1pencil_mysizes[0];
    const int jsize = x1pencil_mysizes[1];
    const int ksize = x1pencil_mysizes[2];
    const int ioffs = x1pencil_offsets[0];
    const int joffs = x1pencil_offsets[1];
    const int koffs = x1pencil_offsets[2];
    for(int k = 0; k < ksize; k++){
      for(int j = 0; j < jsize; j++){
        for(int i = 0; i < isize; i++){
          int index = k * jsize * isize + j * isize + i;
          TEST_MYTYPE value = (TEST_MYTYPE)((k + koffs) * jtot * itot + (j + joffs) * itot + (i + ioffs));
          assert(0 == memcmp(&x1pencil[index], &value, sizeof(TEST_MYTYPE)));
        }
      }
    }
  }
  // clean-up
  sdecomp_free(x1pencil);
  sdecomp_free(y1pencil);
  sdecomp_free(z1pencil);
  sdecomp_free(x2pencil);
  sdecomp_free(y2pencil);
  sdecomp_free(z2pencil);
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

