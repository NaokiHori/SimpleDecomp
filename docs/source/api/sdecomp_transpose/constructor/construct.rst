Example: create a transpose plan to rotate ``x1pencil`` to ``y1pencil``, whose global array size is ``256 x 512 x 1024`` and each element is the type of ``double``:

.. code-block:: c

   #define NDIMS 3
   const size_t glsizes[NDIMS] = {256, 512, 1024};

   sdecomp_transpose_plan_t *plan = NULL;
   sdecomp.transpose.construct(
       info,
       SDECOMP_X1PENCIL,
       SDECOMP_Y1PENCIL,
       glsizes,
       sizeof(double),
       &plan
   );

.. note::

   If the arguments are invalid (e.g., trying to rotate from ``SDECOMP_X1PENCIL`` to ``SDECOMP_Z1PENCIL``, which is not applicable in this project), this function returns non-zero exit code.
   It is strongly recommended to check the returned plan to confirm the desired plan is really created.

