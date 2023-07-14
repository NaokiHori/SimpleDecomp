/*
 * Copyright 2022 Naoki Hori
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// https://github.com/NaokiHori/SimpleDecomp

#include "sdecomp.h"
#define SDECOMP_INTERNAL
#include "internal.h"

typedef enum {
  TYPE_NPROCS = 0,
  TYPE_MYRANK = 1
} type_t;

static sdecomp_dir_t check_table_2d(
    const sdecomp_pencil_t pencil,
    const sdecomp_dir_t dir
){
  const sdecomp_dir_t table_2d[2][2] = {
    {SDECOMP_XDIR, SDECOMP_YDIR},
    {SDECOMP_YDIR, SDECOMP_XDIR},
  };
  return table_2d[pencil][dir];
}

static sdecomp_dir_t check_table_3d(
    const sdecomp_pencil_t pencil,
    const sdecomp_dir_t dir
){
  const sdecomp_dir_t table_3d[6][3] = {
    {SDECOMP_XDIR, SDECOMP_YDIR, SDECOMP_ZDIR},
    {SDECOMP_YDIR, SDECOMP_XDIR, SDECOMP_ZDIR},
    {SDECOMP_YDIR, SDECOMP_ZDIR, SDECOMP_XDIR},
    {SDECOMP_XDIR, SDECOMP_ZDIR, SDECOMP_YDIR},
    {SDECOMP_ZDIR, SDECOMP_XDIR, SDECOMP_YDIR},
    {SDECOMP_ZDIR, SDECOMP_YDIR, SDECOMP_XDIR},
  };
  return table_3d[pencil][dir];
}

static int get_process_config(
    const char error_label[],
    const sdecomp_info_t * info,
    const sdecomp_pencil_t pencil,
    const sdecomp_dir_t dir,
    const type_t type,
    int * data
){
  // sanitise
  if(0 != sdecomp_internal_sanitise_pencil(error_label, info->ndims, pencil)) return 1;
  if(0 != sdecomp_internal_sanitise_dir   (error_label, info->ndims,    dir)) return 1;
  // fetch x1pencil
  int buffers[3][3] = {0};
  MPI_Cart_get(
      info->comm_cart,
      info->ndims,
      buffers[0],
      buffers[1],
      buffers[2]
  );
  // convert it to the desired pencil
  if(2 == info->ndims){
    *data = buffers[TYPE_NPROCS == type ? 0 : 2][check_table_2d(pencil, dir)];
  }else{
    *data = buffers[TYPE_NPROCS == type ? 0 : 2][check_table_3d(pencil, dir)];
  }
  return 0;
}

/**
 * @brief get number of processes in the given dimension
 * @param[in]  info   : struct containing information of process distribution
 * @param[in]  pencil : type of pencil (e.g., SDECOMP_X1PENCIL)
 * @param[in]  dir    : direction which I am interested in
 * @param[out] nprocs : (success) number of processes in the given dimension
 *                      (failure) undefined
 * @return            : (success) 0
 *                      (failure) non-zero value
 */
int sdecomp_internal_get_nprocs(
    const sdecomp_info_t * info,
    const sdecomp_pencil_t pencil,
    const sdecomp_dir_t dir,
    int * nprocs
){
  const char error_label[] = {"sdecomp.get_nprocs"};
  // NULL check
  if(0 != sdecomp_internal_sanitise_null(error_label,   "info",   info)) return 1;
  if(0 != sdecomp_internal_sanitise_null(error_label, "nprocs", nprocs)) return 1;
  // main
  if(0 != get_process_config(error_label, info, pencil, dir, TYPE_NPROCS, nprocs)) return 1;
  return 0;
}

/**
 * @brief get my process position in the whole domain
 * @param[in]  info   : struct containing information of process distribution
 * @param[in]  pencil : type of pencil (e.g., SDECOMP_X1PENCIL)
 * @param[in]  dir    : direction which I am interested in
 * @param[out] myrank : (success) my position in the given dimension
 *                      (failure) undefined
 * @return            : (success) 0
 *                      (failure) non-zero value
 */
int sdecomp_internal_get_myrank(
    const sdecomp_info_t * info,
    const sdecomp_pencil_t pencil,
    const sdecomp_dir_t dir,
    int * myrank
){
  const char error_label[] = {"sdecomp.get_myrank"};
  // NULL check
  if(0 != sdecomp_internal_sanitise_null(error_label,   "info",   info)) return 1;
  if(0 != sdecomp_internal_sanitise_null(error_label, "myrank", myrank)) return 1;
  // main
  if(0 != get_process_config(error_label, info, pencil, dir, TYPE_MYRANK, myrank)) return 1;
  return 0;
}

/**
 * @brief get neighbour process ranks
 * @param[in]  info       : struct containing information of process distribution
 * @param[in]  pencil     : type of pencil (e.g., SDECOMP_X1PENCIL)
 * @param[in]  dir        : direction which I am interested in
 * @param[out] neighbours : (success) neighbour ranks in sdecomp->comm_cart
 *                          (failure) undefined
 * @return                : (success) 0
 *                          (failure) non-zero value
 */
int sdecomp_internal_get_neighbours(
    const sdecomp_info_t * info,
    const sdecomp_pencil_t pencil,
    const sdecomp_dir_t dir,
    int neighbours[2]
){
  const char error_label[] = {"sdecomp.get_neighbours"};
  if(0 != sdecomp_internal_sanitise_null(error_label,       "info",       info)) return 1;
  if(0 != sdecomp_internal_sanitise_null(error_label, "neighbours", neighbours)) return 1;
  // reject all invalid pencils and directions to simplify the following conditions
  if(0 != sdecomp_internal_sanitise_pencil(error_label, info->ndims, pencil)) return 1;
  if(0 != sdecomp_internal_sanitise_dir   (error_label, info->ndims,    dir)) return 1;
  // initialise with "no neighbour"
  neighbours[0] = MPI_PROC_NULL;
  neighbours[1] = MPI_PROC_NULL;
  // check direction, I need direction w.r.t. comm_cart (x1 pencil)
  // NOTE: see figure to see the correspondence
  int direction = -1;
  if(2 == info->ndims){
    direction = check_table_2d(pencil, dir);
  }else{
    direction = check_table_3d(pencil, dir);
  }
  MPI_Comm comm_cart = MPI_COMM_NULL;
  sdecomp.get_comm_cart(info, &comm_cart);
  // always interested in one-process away
  const int disp = 1;
  MPI_Cart_shift(
      comm_cart,
      direction,
      disp,
      neighbours + 0,
      neighbours + 1
  );
  return 0;
}

static int get_pencil_config(
    const char error_label[],
    const sdecomp_info_t * info,
    const sdecomp_pencil_t pencil,
    const sdecomp_dir_t dir,
    const size_t glsize,
    int (* const kernel)(
      const char error_label[],
      const size_t glsize,
      const int nprocs,
      const int myrank,
      size_t * result
    ),
    size_t * result
){
  // wrapper function to get mysize / offset of pencil
  // sanitise
  if(0 != sdecomp_internal_sanitise_pencil(error_label, info->ndims, pencil)) return 1;
  if(0 != sdecomp_internal_sanitise_dir   (error_label, info->ndims,    dir)) return 1;
  // compute number of process and my location in the given direction
  int nprocs = 0;
  int myrank = 0;
  if(0 != sdecomp_internal_get_nprocs(info, pencil, dir, &nprocs)) return 1;
  if(0 != sdecomp_internal_get_myrank(info, pencil, dir, &myrank)) return 1;
  // call one of "number of grid calculator" or "offset calculator"
  if(0 != kernel(error_label, glsize, nprocs, myrank, result)) return 1;
  return 0;
}

/**
 * @brief get number of grid points of the given pencil in the given direction
 * @param[in]  info   : struct containing information of process distribution
 * @param[in]  pencil : type of pencil (e.g., SDECOMP_X1PENCIL)
 * @param[in]  dir    : direction which I am interested in
 * @param[in]  glsize : number of total grid points in the direction
 * @param[out] mysize : (success) number of local grid points in the direction
 *                      (failure) undefined
 * @return            : (success) 0
 *                      (failure) non-zero value
 */
int sdecomp_internal_get_pencil_mysize(
    const sdecomp_info_t * info,
    const sdecomp_pencil_t pencil,
    const sdecomp_dir_t dir,
    const size_t glsize,
    size_t * mysize
){
  const char error_label[] = {"sdecomp.get_pencil_mysize"};
  // NULL check
  if(0 != sdecomp_internal_sanitise_null(error_label,   "info",   info)) return 1;
  if(0 != sdecomp_internal_sanitise_null(error_label, "mysize", mysize)) return 1;
  // main
  if(0 != get_pencil_config(error_label, info, pencil, dir, glsize, sdecomp_internal_kernel_get_mysize, mysize)) return 1;
  return 0;
}

/**
 * @brief get offset of grid points of the given pencil in the given direction
 * @param[in]  info   : struct containing information of process distribution
 * @param[in]  pencil : type of pencil (e.g., SDECOMP_X1PENCIL)
 * @param[in]  dir    : direction which I am interested in
 * @param[in]  glsize : number of global grid points in the direction
 * @param[out] offset : (success) offset in the give direction
 *                      (failure) undefined
 * @return            : (success) 0
 *                      (failure) non-zero value
 */
int sdecomp_internal_get_pencil_offset(
    const sdecomp_info_t * info,
    const sdecomp_pencil_t pencil,
    const sdecomp_dir_t dir,
    const size_t glsize,
    size_t * offset
){
  const char error_label[] = {"sdecomp.get_pencil_mysize"};
  // NULL check
  if(0 != sdecomp_internal_sanitise_null(error_label,   "info",   info)) return 1;
  if(0 != sdecomp_internal_sanitise_null(error_label, "offset", offset)) return 1;
  // main
  if(0 != get_pencil_config(error_label, info, pencil, dir, glsize, sdecomp_internal_kernel_get_offset, offset)) return 1;
  return 0;
}

