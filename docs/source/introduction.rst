############
Introduction
############

.. _usage:

*****
Usage
*****

In this section, I will explain how you can incorporate this library into your project.
While it is technically feasible to install the library (see below), I recommend taking the simplest approach by copying the source files located in ``src/sdecomp`` and the corresponding header file, ``include/sdecomp.h``, directly into your project directory:

.. code-block:: text

   src
   ├── your_source_file_1.c
   ├── your_source_file_2.c
   └── sdecomp
       ├── construct.c
       ├── destruct.c
       ├── get
       │  ├── main.c
       │  └── process_and_pencil.c
       ├── internal.h
       ├── kernel.c
       ├── main.c
       ├── memory.c
       ├── sanitise.c
       └── transpose
          ├── 2d.c
          ├── 3d.c
          ├── internal.h
          ├── main.c
          └── memory.c

.. code-block:: text

   include
   ├── your_header_file.h
   └── sdecomp.h

A typical ``Makefile`` would be as follows:

.. literalinclude:: /../../Makefile
   :language: makefile

Include ``include/sdecomp.h`` in your source to use the APIs:

.. code-block::

   #include "sdecomp.h"

   int main(void){
      ...
      return 0;
   }

.. mydetails:: (Optional) Installation

   While I suggest using the aforementioned approach for simplicity, you also have the option to build and install this library on your machine using the ``install.sh`` script to link this library externally.

   First, checkout `the repository <https://github.com/NaokiHori/SimpleDecomp>`_.

   To install,

   .. code-block:: console

      $ bash install.sh install

   To uninstall,

   .. code-block:: console

      $ bash install.sh uninstall

   By default, a dynamic library and a header file are installed under ``${HOME}/.local``.

   Remember to set paths and link it properly, e.g. ``-I${HOME}/.local/include`` (or set ``C_INCLUDE_PATH``) and ``-L${HOME}/.local/lib`` (or set ``LD_LIBRARY_PATH``) with ``-lsdecomp``.

***********
Error check
***********

In all :ref:`APIs <api>`, the final parameter serves as a container for the output, while the return value indicates the occurrence of an error (``0`` indicates success, any other value signifies failure).
Although error checks are excluded for the sake of simplicity in the following examples, it is essential for the user to consistently verify it:

.. code-block:: c

   int retval = sdecomp.xxx(...);
   if(0 != retval){
      goto err_hndl;
   }

I have not provided any specifications regarding the contents of the output parameter in the event of API failure.
Consequently, using the parameter in such cases results in undefined behavior.

**********
Data types
**********

In ``MPI``, it is customary for arguments to be integers, even when only non-negative numbers are applicable (such as the number of processes).
However, in this library, an effort has been made to minimise the need for input sanitisation by accepting ``size_t`` or similar unsigned integer types in the APIs.
It is important to exercise caution and avoid passing an ``int *`` where a ``size_t *`` is expected, as this would be incorrect if ``sizeof(int)`` and ``sizeof(size_t)`` are not the same.

*******
Caveats
*******

#. Limited rotations

   For three-dimensional domains, there are `6` types of pencils, resulting in a total of :math:`6 \times 5 = 30` patterns to encompass all possible rotations.
   However, in this library, the focus is narrowed down to only considering the clockwise

   .. math::

      0 \rightarrow 1 \rightarrow 2 \rightarrow 3 \rightarrow 4 \rightarrow 5 \rightarrow 0

   and the counter-clockwise

   .. math::

      0 \rightarrow 5 \rightarrow 4 \rightarrow 3 \rightarrow 2 \rightarrow 1 \rightarrow 0

   rotations (seee below).

   .. image:: /images/transpose_3d.png
      :width: 50%

#. Memory orders

   By default (``x1pencil``), the memory is contiguous in the :math:`x` direction, followed by the :math:`y` direction, with the largest stride occurring in the :math:`z` direction.
   When the pencils are rotated, both the direction of the pencils and the memory order are altered as follows.

   .. list-table:: Memory order (2D)
      :widths: 10 30 30
      :header-rows: 1

      * - Pencil
        - Contiguous
        - Sparse
      * - ``x1``, ``x2``
        - ``x``
        - ``y``
      * - ``y1``, ``y2``
        - ``y``
        - ``x``

   .. list-table:: Memory order (3D)
      :widths: 10 30 30 30
      :header-rows: 1

      * - Pencil
        - Contiguous
        - Intermediate
        - Sparse
      * - ``x1``, ``x2``
        - ``x``
        - ``y``
        - ``z``
      * - ``y1``, ``y2``
        - ``y``
        - ``z``
        - ``x``
      * - ``z1``, ``z2``
        - ``z``
        - ``x``
        - ``y``

   The purpose is to enhance the cache efficiency for operations like FFTs in the pencil direction.
   Also this rearrangement simplifies the implementation.

#. Two pencil types in the same dimension

   As shown above, there are two types of ``xpencils``, ``ypencils``, and ``zpencils``.
   Notice the difference between ``x1pencil`` and ``x2pencil``.

#. Reserved words

   Achieving perfect encapsulation in C can be challenging.
   Functions in this library are implemented in separate files to enhance maintainability, and as a result the internal functions are visible to the linker.
   To prevent name collisions and ensure consistency, it is advised not to define variables that begin with ``sdecomp`` or ``SDECOMP`` in your implementation.

