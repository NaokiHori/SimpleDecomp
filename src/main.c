#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
#include "sdecomp.h"

// consider a two-dimensional domain as an example
#define NDIMS 2

static int show_pencil(
    const sdecomp_info_t * info,
    const bool is_x1,
    const size_t mysizes[NDIMS],
    const size_t * pencil
){
  const char fname[] = {"sdecomp.log"};
  // get total number of processes and my rank
  int nprocs = 0, myrank = 0;
  sdecomp.get_comm_size(info, &nprocs);
  sdecomp.get_comm_rank(info, &myrank);
  // get communicator
  MPI_Comm comm_cart = MPI_COMM_NULL;
  sdecomp.get_comm_cart(info, &comm_cart);
  if(0 == myrank){
    FILE * fp = fopen(fname, "a");
    fprintf(fp, "======= %s pencil =======\n", is_x1 ? "x1" : "y1");
    fclose(fp);
  }
  MPI_Barrier(comm_cart);
  for(int rank = 0; rank < nprocs; rank++){
    if(myrank == rank){
      FILE * fp = fopen(fname, "a");
      if(is_x1){
        // x1 pencil
        for(size_t index = 0, j = 0; j < mysizes[1]; j++){
          fprintf(fp, "rank %2d | ", rank);
          for(size_t i = 0; i < mysizes[0]; i++){
            fprintf(fp, "%3zu%c", pencil[index++], mysizes[0] - 1 == i ? '\n' : ' ');
          }
        }
      }else{
        // y1 pencil, transposed
        for(size_t index = 0, j = 0; j < mysizes[0]; j++){
          fprintf(fp, "rank %2d | ", rank);
          for(size_t i = 0; i < mysizes[1]; i++){
            fprintf(fp, "%3zu%c", pencil[index++], mysizes[1] - 1 == i ? '\n' : ' ');
          }
        }
      }
      fclose(fp);
    }
    MPI_Barrier(comm_cart);
  }
  MPI_Barrier(comm_cart);
  return 0;
}

static int example(
    void
){
  // initialise library (create x1 pencils)
  // decompose domain into 1 x 2 pencils
  const size_t dims[NDIMS] = {1, 2};
  // non-periodic
  const bool periods[NDIMS] = {false, false};
  // call initialiser
  sdecomp_info_t * info = NULL;
  if(0 != sdecomp.construct(MPI_COMM_WORLD, NDIMS, dims, periods, &info)){
    return 1;
  }
  // global array size: 5 (number of rows) x 4 (number of columns)
  const size_t glsizes[NDIMS] = {4, 5};
  // get x1 pencil size and offset w.r.t. the global array
  size_t x1_mysizes[NDIMS] = {0};
  size_t x1_offsets[NDIMS] = {0};
  sdecomp.get_pencil_mysize(info, SDECOMP_X1PENCIL, SDECOMP_XDIR, glsizes[0], &x1_mysizes[0]);
  sdecomp.get_pencil_mysize(info, SDECOMP_X1PENCIL, SDECOMP_YDIR, glsizes[1], &x1_mysizes[1]);
  sdecomp.get_pencil_offset(info, SDECOMP_X1PENCIL, SDECOMP_XDIR, glsizes[0], &x1_offsets[0]);
  sdecomp.get_pencil_offset(info, SDECOMP_X1PENCIL, SDECOMP_YDIR, glsizes[1], &x1_offsets[1]);
  // allocate and initialise x1 pencil
  size_t * x1_pencil = calloc(x1_mysizes[0] * x1_mysizes[1], sizeof(size_t));
  for(size_t index = 0, myj = 0; myj < x1_mysizes[1]; myj++){
    const size_t glj = myj + x1_offsets[1];
    for(size_t myi = 0; myi < x1_mysizes[0]; myi++){
      const size_t gli = myi + x1_offsets[0];
      x1_pencil[index++] = glj * glsizes[0] + gli;
    }
  }
  // print x1 pencil
  show_pencil(info, true, x1_mysizes, x1_pencil);
  // get y1 pencil size and offset w.r.t. the global array
  size_t y1_mysizes[NDIMS] = {0};
  size_t y1_offsets[NDIMS] = {0};
  sdecomp.get_pencil_mysize(info, SDECOMP_Y1PENCIL, SDECOMP_XDIR, glsizes[0], &y1_mysizes[0]);
  sdecomp.get_pencil_mysize(info, SDECOMP_Y1PENCIL, SDECOMP_YDIR, glsizes[1], &y1_mysizes[1]);
  sdecomp.get_pencil_offset(info, SDECOMP_Y1PENCIL, SDECOMP_XDIR, glsizes[0], &y1_offsets[0]);
  sdecomp.get_pencil_offset(info, SDECOMP_Y1PENCIL, SDECOMP_YDIR, glsizes[1], &y1_offsets[1]);
  // allocate y1 pencil
  size_t * y1_pencil = calloc(y1_mysizes[0] * y1_mysizes[1], sizeof(size_t));
  // rotate pencil, from x1 to y1
  sdecomp_transpose_plan_t * plan = NULL;
  sdecomp.transpose.construct(info, SDECOMP_X1PENCIL, SDECOMP_Y1PENCIL, glsizes, sizeof(size_t), &plan);
  sdecomp.transpose.execute(plan, x1_pencil, y1_pencil);
  // print y1 pencil
  show_pencil(info, false, y1_mysizes, y1_pencil);
  // clean-up buffers
  free(x1_pencil);
  free(y1_pencil);
  // clean-up library
  if(0 != sdecomp.transpose.destruct(plan)){
    return 1;
  }
  if(0 != sdecomp.destruct(info)){
    return 1;
  }
  return 0;
}

int main(
    void
){
  MPI_Init(NULL, NULL);
  int nprocs = 0;
  int myrank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  if(2 != nprocs){
    if(0 == myrank){
      printf("please try with 2 processes\n");
    }
    goto terminate;
  }
  example();
terminate:
  MPI_Finalize();
  return 0;
}

