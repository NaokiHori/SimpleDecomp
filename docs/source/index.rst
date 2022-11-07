#############
Simple Decomp
#############

Simple pencil-like domain decomposition for two- and three-dimensional structured meshes written in C.

.. image:: data/2d/default.png
   :width: 100%

Here, a two-dimensional domain shown in the left picture, in which each gray cube denotes one element of the array, is decomposed into `1 x 6` chunks (each coloured chunk in the right picture).

.. image:: data/3d/default.png
   :width: 100%

Similarly, a three-dimensional domain shown in the left picture, in which each gray cube denotes one element of the array, is decomposed into `1 x 3 x 4` chunks (each coloured chunk in the right picture).

Note that each coloured chunk is processed by different MPI process in order to distribute the total workload.

********************************
Initialisation and ``x1pencils``
********************************

In this library, this decomposition is achieved by

.. mydeclare:: /../../src/sdecomp/init.c
   :language: c
   :tag: sdecomp_init

For instance, a three-dimensional domain whose boundaries are (:math:`x`: periodic, :math:`y`: periodic, :math:`z`: non-periodic) can be decomposed by

.. code-block:: c

   const int ndims = 3;
   int *dims    = calloc(ndims, sizeof(int));
   int *periods = calloc(ndims, sizeof(int));
   dims[0] = 0; // all-zero: automatic decomposition
   dims[1] = 0; // all-zero: automatic decomposition
   dims[2] = 0; // all-zero: automatic decomposition
   periods[0] = 1; //     periodic
   periods[1] = 1; //     periodic
   periods[2] = 0; // non-periodic
   sdecomp_t *sdecomp = sdecomp_init(MPI_COMM_WORLD, ndims, dims, periods);
   free(periods);

``dims`` take number of processes in each dimension, whose caveat is as follows:

* I do not care the number of processes in each dimension.

   Initialise all with zero, which asks the library (``MPI_Cart_create``) to automatically choose the number of processes in each dimension.

* The number of processes is important to me

   Specify all properly so that you can manually decide the number of processes in each dimension.
   It is readily apparent that the products of ``dims`` should be identical to the total number of processes calling this initialiser (i.e., number of processes belonging to the communicator ``comm_default``).

   Note that ``dims[0]`` should be ``1``, since we do **not** decompose the domain in :math:`x` direction.
   Thus, for two-dimensional domain, a pair of ``dims[0] = 1`` and ``dims[1] = nprocs`` is the only possibility.
   For three-dimensional domain, there can be several answers as long as a relation ``dims[1] * dims[2] == nprocs`` is satisfied.

``sdecomp_init`` returns a pointer to ``sdecomp_t``, whose definition is

.. myliteralinclude:: /../../include/sdecomp.h
   :language: c
   :tag: definition of sdecomp_t

in which the number of spatial dimension ``ndims`` and an MPI communicator storing Cartesian topology ``comm_cart`` (i.e., how each process is mapped to the domain, neighbouring processes, periodic or not, etc.) are included.

Note that the domain is **not** decomposed in :math:`x` direction, and each process is responsible for part of the domain decomposed in :math:`y` (and :math:`z` for three-dimensional domain) dimensions.
I refer to these segments as ``pencil``, and hereafter the above default pencils are referred to as ``x1pencil``, since each segment looks like a pencil directing toward :math:`x` direction.
The reason why *1* follows after :math:`x` will be introduced in the next section.

*************************************
Other pencils and parallel transposes
*************************************

Again, in the above schematics, the domains are not decomposed in :math:`x` direction, while other dimensions are split into several segments.
One might need to change this configuration so that all grid points in :math:`y` direction (or :math:`z` direction) are contained by each process.

This would be the case when one needs to perform multi-dimensional FFTs for instance, since a conventional `Cooley-Tukey FFT algorithm <https://en.wikipedia.org/wiki/Cooley–Tukey_FFT_algorithm>`_ requires information from all nodes, and fetching missing information via MPI communication every time should be too slow.

The schematics are like

.. image:: data/2d/transpose.png
   :width: 67%

for the same two-dimensional domain given above, and

.. image:: data/3d/transpose.png
   :width: 100%

for the three-dimensional domain, which are referred to as *parallel transposes* and implemented in this library.

========================
Position of each process
========================

Before discussing the transition from one pencil to the other, we introduce a few utility functions.

Let's imagine I am the process coloured by marine-blue in the ``x1pencil`` of the three-dimensional case, and would like to know my position, namely, :math:`\left( 0, 3, 1 \right)`.
Although this can be achieved by ``MPI_Cart_get``, considering for the other pencils are a bit cumbersome.
So this library offers a wrapper function:

.. mydeclare:: /../../src/sdecomp/pencil.c
   :language: c
   :tag: sdecomp_get_myrank

For example,

.. code-block:: c

   const sdecomp_pencil_t pencil = SDECOMP_X1PENCIL;
   const int x1pencil_mypos_x = sdecomp_get_myrank(sdecomp, pencil, SDECOMP_XDIR);
   const int x1pencil_mypos_y = sdecomp_get_myrank(sdecomp, pencil, SDECOMP_YDIR);
   const int x1pencil_mypos_z = sdecomp_get_myrank(sdecomp, pencil, SDECOMP_ZDIR);

give the positions of the marine-blue process in `x1` configuration :math:`\left( 0, 3, 1 \right)`, while

.. code-block:: c

   const sdecomp_pencil_t pencil = SDECOMP_Y1PENCIL;
   const int y1pencil_mysize_x = sdecomp_get_myrank(sdecomp, pencil, SDECOMP_XDIR);
   const int y1pencil_mysize_y = sdecomp_get_myrank(sdecomp, pencil, SDECOMP_YDIR);
   const int y1pencil_mysize_z = sdecomp_get_myrank(sdecomp, pencil, SDECOMP_ZDIR);

give the positions of the marine-blue process in `y1` configuration :math:`\left( 1, 0, 3 \right)`.
One can easily obtain the positions for the other pencils in a similar manner.

A similar API

.. mydeclare:: /../../src/sdecomp/pencil.c
   :language: c
   :tag: sdecomp_get_nprocs

returns the number of processes of the given pencil (``pencil``) in the given dimension (``dim``), e.g.,

.. code-block:: c

   const sdecomp_pencil_t pencil = SDECOMP_X1PENCIL;
   const int x1pencil_nprocs_x = sdecomp_get_nprocs(sdecomp, pencil, SDECOMP_XDIR);
   const int x1pencil_nprocs_y = sdecomp_get_nprocs(sdecomp, pencil, SDECOMP_YDIR);
   const int x1pencil_nprocs_z = sdecomp_get_nprocs(sdecomp, pencil, SDECOMP_ZDIR);

returns ``1``, ``3``, and ``4``, respectively.

=====================
Number of grid points
=====================

Up to here, how each process is mapped to the domain is discussed.
The number of grid points in each dimension for each pencil are, on the other hand, very important from a practical point of view.
The sizes of pencils, i.e., **local** array sizes, can be obtained by

.. mydeclare:: /../../src/sdecomp/pencil.c
   :language: c
   :tag: sdecomp_get_pencil_mysize

For instance, if I am the process coloured by marine-blue in the above three-dimensional schematic again,

.. code-block:: c

   const int nx_global = 24;
   const int ny_global = 18;
   const int nz_global = 12;
   const sdecomp_pencil_t pencil = SDECOMP_X1PENCIL;
   const int x1pencil_mysize_x = sdecomp_get_pencil_mysize(sdecomp, pencil, SDECOMP_XDIR, nx_global);
   const int x1pencil_mysize_y = sdecomp_get_pencil_mysize(sdecomp, pencil, SDECOMP_YDIR, ny_global);
   const int x1pencil_mysize_z = sdecomp_get_pencil_mysize(sdecomp, pencil, SDECOMP_ZDIR, nz_global);

returns ``24``, ``6``, and ``3``, respectively.

Similarly, the offsets (starting indices in terms of the global coordinate) can be computed by

.. mydeclare:: /../../src/sdecomp/pencil.c
   :language: c
   :tag: sdecomp_get_pencil_offset

e.g.,

.. code-block:: c

   const int nx_global = 24;
   const int ny_global = 18;
   const int nz_global = 12;
   const sdecomp_pencil_t pencil = SDECOMP_X1PENCIL;
   const int x1pencil_offset_x = sdecomp_get_pencil_offset(sdecomp, pencil, SDECOMP_XDIR, nx_global);
   const int x1pencil_offset_y = sdecomp_get_pencil_offset(sdecomp, pencil, SDECOMP_YDIR, ny_global);
   const int x1pencil_offset_z = sdecomp_get_pencil_offset(sdecomp, pencil, SDECOMP_ZDIR, nz_global);

returns ``0``, ``6``, and ``9``, respectively.

.. warning::

   Obviously it is meaningless to assign processes to one direction more than the number of grid points in the direction (e.g., 30 processes in :math:`x` direction in the above example), whose behaviour is **not defined**.
   Please be so careful that this kind of situation does not occur **for all possible pencils**.

===================
Parallel transposes
===================

Functions to achieve the parallel transposes are implemented in this library, which requests three steps: initialisation, execution, and finalisation.

#. Initialisation

   One can initialise the transpose plan by

   .. mydeclare:: /../../src/sdecomp/transpose.c
      :language: c
      :tag: sdecomp_transpose_fwrd_init

   for forward transposes, while

   .. mydeclare:: /../../src/sdecomp/transpose.c
      :language: c
      :tag: sdecomp_transpose_bwrd_init

   for backward transposes, respectively.

   Here *forward* denotes the following transitions:

   * from ``x1pencil`` (0) to ``y1pencil`` (1)
   * from ``y1pencil`` (1) to ``z1pencil`` (2)
   * from ``z1pencil`` (2) to ``x2pencil`` (3)
   * from ``x2pencil`` (3) to ``y2pencil`` (4)
   * from ``y2pencil`` (4) to ``z2pencil`` (5)
   * from ``z2pencil`` (5) to ``x1pencil`` (0)

   while *backward* denotes the following transitions:

   * from ``x1pencil`` (0) to ``z2pencil`` (5)
   * from ``z2pencil`` (5) to ``y2pencil`` (4)
   * from ``y2pencil`` (4) to ``x2pencil`` (3)
   * from ``x2pencil`` (3) to ``z1pencil`` (2)
   * from ``z1pencil`` (2) to ``y1pencil`` (1)
   * from ``y1pencil`` (1) to ``x1pencil`` (0)

   For two-dimensional domain, there is no difference between *forward* and *backward*.

#. Execution

   The created plan in the first step can be used by

   .. mydeclare:: /../../src/sdecomp/transpose.c
      :language: c
      :tag: sdecomp_transpose_execute

   Here, the user should allocate ``sendbuf`` and ``recvbuf`` a priori.
   In order to know the sizes of the pencils before and after being transposed, use ``sdecomp_get_pencil_mysize``, e.g.,

   .. code-block:: c

      int x1_sizes[3], y1_sizes[3];
      x1_sizes[0] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_X1PENCIL, SDECOMP_XDIR, 24);
      x1_sizes[1] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_X1PENCIL, SDECOMP_YDIR, 18);
      x1_sizes[2] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_X1PENCIL, SDECOMP_ZDIR, 12);
      y1_sizes[0] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Y1PENCIL, SDECOMP_XDIR, 24);
      y1_sizes[1] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Y1PENCIL, SDECOMP_YDIR, 18);
      y1_sizes[2] = sdecomp_get_pencil_mysize(sdecomp, SDECOMP_Y1PENCIL, SDECOMP_ZDIR, 12);
      double * sendbuf = calloc(x1_sizes[0] * x1_sizes[1] * x2_sizes[2], sizeof(double));
      double * recvbuf = calloc(y1_sizes[0] * y1_sizes[1] * x2_sizes[2], sizeof(double));

   .. note::

      For the same input and output sizes, the same plan can be repeatedly used, i.e., the user do not have to re-generate the plan everytime.

   .. warning::

      In-place transposes are not allowed, i.e., ``sendbuf`` and ``recvbuf`` should not overlapped.

#. Finalisation

   The plan should be cleaned-up when it is no longer needed by

   .. mydeclare:: /../../src/sdecomp/transpose.c
      :language: c
      :tag: sdecomp_transpose_finalise

There are several things to be noted:

   #. Limited transposes

      Since there are 6 types of pencils, 30 functions are needed to cover all parallel matrix transposes.
      This library, however, contains only two functions, namely forward (0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 0 -> ... in the above picture) and backward (0 -> 5 -> 4 -> 3 -> 2 -> 1 -> 0 -> ... in the above picture) transposings.
      Other transposes, e.g., from ``x1pencil`` (0) to ``x2pencil`` (3) are not implemented since they are much more time consuming since all processes should participate in the communications.

   #. Memory orders

      By default (``x1pencil``), the memory is contiguous in x direction, which is followed by y direction, and the biggest stride is in z direction.
      When an operation is done, not only the direction of pencils but also the memory contiguity is changed.
      For instance, y is contiguous and x has the biggest stride for ``y1pencil`` (middle-top case in the above schematic), which is the minor difference compared to the reference library (as long as I know) but simplifies the algorithm quite a lot.

   #. Two pencil types in the same dimension

      As being discussed above, there are two types of ``xpencil`` for each dimension.
      The user should notice that a location might be taken care of by different process (in short; notice that they are totally different).

*****
Tests
*****

Since the implementations of parallel transposes are fairly complicated, I always wonder they are really implemented correctly.
This library offers simple test functions to check the correctness, which are also implemented in ``src/sdecomp/transpose.c``.

Let us consider a two-dimensional matrix, which are decomposed in ``x1pencil`` by two processes:

.. code-block:: text

   +---------------+
   | 12 13   14 15 |
   | 08 09   10 11 |
   +---------------+
   | 04 05   06 07 |
   | 00 01   02 03 |
   +---------------+

Here, the buffer of the process occupying the lower pencil leads

.. code-block:: text

   00 01 02 03 04 05 06 07.

What the parallel transpose does is to convert this to ``y1pencil``, namely

.. code-block:: text

   +-------+-------+
   | 12 13 | 14 15 |
   | 08 09 | 10 11 |
   |       |       |
   | 04 05 | 06 07 |
   | 00 01 | 02 03 |
   +-------+-------+

The buffer of the process occupying the left pencil leads

.. code-block:: text

   00 04 08 12 01 05 09 13.

The answer can be easily obtained by considering the sizes and offsets of each pencil, and thus they are compared by these test functions.

The corresponding test function (in this case we want to test a transpose from ``x1pencil`` to ``y1pencil``) can be used by

.. code-block:: c

    sdecomp_test_transpose_2d_x1_to_y1(sdecomp, plan);

Here ``sdecomp`` is a pointer to a struct ``sdecomp_t``, and ``plan`` is what is returned by the constructor of the transpose algorithm ``sdecomp_transpose_fwrd_init``.

All test cases covering the possible transposes are implemented as follows.

#. Two-dimensional transposes

   .. code-block:: c

       sdecomp_test_transpose_2d_x1_to_y1(sdecomp, plan);
       sdecomp_test_transpose_2d_y1_to_x1(sdecomp, plan);

#. Three-dimensional forward transposes

   .. code-block:: c

       sdecomp_test_transpose_3d_x1_to_y1(sdecomp, plan);
       sdecomp_test_transpose_3d_y1_to_z1(sdecomp, plan);
       sdecomp_test_transpose_3d_z1_to_x2(sdecomp, plan);
       sdecomp_test_transpose_3d_x2_to_y2(sdecomp, plan);
       sdecomp_test_transpose_3d_y2_to_z2(sdecomp, plan);
       sdecomp_test_transpose_3d_z2_to_x1(sdecomp, plan);

#. Three-dimensional backward transposes

   .. code-block:: c

       sdecomp_test_transpose_3d_x1_to_z2(sdecomp, plan);
       sdecomp_test_transpose_3d_z2_to_y2(sdecomp, plan);
       sdecomp_test_transpose_3d_y2_to_x2(sdecomp, plan);
       sdecomp_test_transpose_3d_x2_to_z1(sdecomp, plan);
       sdecomp_test_transpose_3d_z1_to_y1(sdecomp, plan);
       sdecomp_test_transpose_3d_y1_to_x1(sdecomp, plan);

Please refer to ``src/test_2d.c`` and ``src/test_3d.c`` (and ``src/main.c``), which are included just for introductory purpose for stand-alone tests, in which all these test functions are called, while both forward and backward transposes are used as examples.

