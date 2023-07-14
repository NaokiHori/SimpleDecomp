#. ``const MPI_Comm comm_default``

   ``MPI`` communicator which includes all processes which participate in this decomposition.

   In most cases this should be ``MPI_COMM_WORLD``.

#. ``const size_t ndims``

   Number of spatial dimensions of the domain you want to decompose.
   This should be ``2`` or ``3`` for two- / three-dimensional domains, respectively.

#. ``const size_t *dims``

   Number of processes in each dimension.
   Obviously the size (length) of this variable should be equivalent to the number of dimensions ``ndims``, i.e.

   .. code-block:: c

      #define NDIMS 2
      size_t dims[NDIMS] = {0};

   or

   .. code-block:: c

      #define NDIMS 3
      size_t dims[NDIMS] = {0};

   You can specify how the domain is decomposed by the multiple processes using this parameter.

   * When you do not care the number of processes in each dimension:

      Initialise all with zero, which asks ``MPI_Dims_create`` to automatically choose the number of processes in each dimension.

   * When the number of processes in each dimension is important to you:

      You can manually decide the number of processes in each dimension.
      However, you need to specify all values properly.
      First, it is apparent that the products of ``dims`` (e.g. ``dims[0] * dims[1]`` when ``ndims`` is 2) should be equal to the total number of processes calling this initialiser (i.e., number of processes which belong to the communicator ``comm_default``).

      Second, ``dims[0]`` should be ``1``, since we do **not** decompose the domain in :math:`x` direction (remember that we are dealing with pencils, not with cubes).
      Thus, for two-dimensional domains, ``dims[2] = {1, nprocs}`` is the only possible choice.
      For three-dimensional domains, there are several answers in general, as long as ``dims[1] * dims[2] == nprocs`` is satisfied, which is up to you.

#. ``periods``

   Periodicities in each dimension.
   In the above example, the boundaries are periodic (``1``) in :math:`x` and :math:`y` directions, while :math:`z` direction is not (``0``), e.g. wall-bounded.

Example: automatic decomposition of a three-dimensional domain where y and z directions are periodic:

.. code-block:: c

   #define NDIMS 3
   const size_t dims[NDIMS] = {0, 0, 0};
   const bool periods[NDIMS] = {false, true, true};

   sdecomp_info_t *info = NULL;
   const int retval = sdecomp.construct(
       MPI_COMM_WORLD,
       NDIMS,
       dims,
       periods,
       &info
   );
   if(0 != retval){
      // failed to create info, error handling
   }

Example: decomposition of a three-dimensional domain with 16384 processes, where both y and z directions are split into 128 pencils:

.. code-block:: c

   #define NDIMS 3
   const size_t dims[NDIMS] = {1, 128, 128};
   const bool periods[NDIMS] = {false, true, true};

   // check total number of processes in MPI_COMM_WORLD,
   //   which should be equal to dims[0] * dims[1] * dims[2]
   int nprocs = 0;
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
   assert(dims[0] * dims[1] * dims[2] == nprocs);

   sdecomp_info_t *info = NULL;
   const int retval = sdecomp.construct(
       MPI_COMM_WORLD,
       NDIMS,
       dims,
       periods,
       &info
   );
   if(0 != retval){
      // failed to create info, error handling
   }

