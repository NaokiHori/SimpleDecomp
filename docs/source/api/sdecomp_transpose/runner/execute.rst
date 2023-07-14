Example: transpose ``x1pencil`` to ``y1pencil``:

.. code-block:: c

   // global domain size
   const size_t glsizes[NDIMS] = {256, 512, 1024};
   // you should allocate x1pencil and y1pencil beforehand
   size_t mysizes_x1[NDIMS] = {0};
   for(sdecomp_dir_t dir = 0; dir < NDIMS; dir++){
      sdecomp.get_pencil_mysize(info, SDECOMP_X1PENCIL, dir, glsizes[dir], mysizes_x1 + dir),
   }
   size_t mysizes_y1[NDIMS] = {0};
   for(sdecomp_dir_t dir = 0; dir < NDIMS; dir++){
      sdecomp.get_pencil_mysize(info, SDECOMP_Y1PENCIL, dir, glsizes[dir], mysizes_y1 + dir),
   }
   double *x1pencil = calloc(mysizes_x1[0] * mysizes_x1[1] * mysizes_x1[2], sizeof(double));
   double *y1pencil = calloc(mysizes_y1[0] * mysizes_y1[1] * mysizes_y1[2], sizeof(double));

   // initialise x1pencil:
   //
   //    for(size_t k = 0; k < mysizes_x1[2]; k++){
   //        for(size_t j = 0; j < mysizes_x1[1]; j++){
   //            for(size_t i = 0; i < mysizes_x1[0]; i++){
   //                const size_t index =
   //                    + k * mysizes_x1[0] * mysizes_x1[1]
   //                    + j * mysizes_x1[0]
   //                    + i;
   //                x1pencil[index] = ...;
   //            }
   //        }
   //    }

   sdecomp.transpose.execute(
       info,
       x1pencil,
       y1pencil
   );

   // check y1pencil, now memory is contiguous in y direction
   //
   //    for(size_t i = 0; i < mysizes_y1[0]; i++){
   //        for(size_t k = 0; k < mysizes_y1[2]; k++){
   //            for(size_t j = 0; j < mysizes_y1[1]; j++){
   //                const size_t index =
   //                    + i * mysizes_y1[1] * mysizes_y1[2]
   //                    + k * mysizes_y1[1]
   //                    + j;
   //                ... = y1pencil[index];
   //            }
   //        }
   //    }

