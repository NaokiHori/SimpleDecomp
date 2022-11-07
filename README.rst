#############
Simple Decomp
#############

|License|_ |CI|_ |Documentation|_ |LastCommit|_

.. |License| image:: https://img.shields.io/github/license/NaokiHori/SimpleDecomp
.. _License: https://opensource.org/licenses/MIT

.. |CI| image:: https://github.com/NaokiHori/SimpleDecomp/actions/workflows/ci.yml/badge.svg
.. _CI: https://github.com/NaokiHori/SimpleDecomp/actions/workflows/ci.yml

.. |Documentation| image:: https://github.com/NaokiHori/SimpleDecomp/actions/workflows/documentation.yml/badge.svg
.. _Documentation: https://github.com/NaokiHori/SimpleDecomp/actions/workflows/documentation.yml

.. |LastCommit| image:: https://img.shields.io/github/last-commit/NaokiHori/SimpleDecomp/main
.. _LastCommit: https://github.com/NaokiHori/SimpleDecomp/commits/main

Simple pencil-like domain decomposition for two- and three-dimensional structured meshes written in C.

.. image:: https://naokihori.github.io/SimpleDecomp/_images/default.png
   :width: 67%

.. image:: https://naokihori.github.io/SimpleDecomp/_images/default1.png
   :width: 67%

Here two- or three-dimensional domains are distributed among multiple MPI process.

Functions taking care of pencil rotations are also included:

.. image:: https://naokihori.github.io/SimpleDecomp/_images/transpose.png
   :width: 67%

.. image:: https://naokihori.github.io/SimpleDecomp/_images/transpose1.png
   :width: 100%

which might be useful to perform operations when one process needs to contain all data in one direction, e.g., multi-dimensional Fast Fourier Transforms.

**********************
Reinventing the wheel?
**********************

`Exactly <https://github.com/xcompact3d/2decomp-fft>`_.
(Slightly customised for my purpose though...)

Also some functions are missing for simplicity.

************
Installation
************

Copy source files under ``src/sdecomp`` and a corresponding header file ``include/sdecomp.h`` to your project, i.e.,

.. code-block:: text

   src
   ├── sdecomp
   │  ├── finalise.c
   │  ├── init.c
   │  ├── memory.c
   │  ├── pencil.c
   │  └── transpose.c
   ├── your_source_file_1.c
   └── your_source_file_2.c

.. code-block:: text

   include
   ├── sdecomp.h
   └── your_header_file.h

Now all functions are ready to be used.

Note that, ``src/main.c``, ``src/test_2d.c``, ``src/test_3d.c``, and ``include/test.h`` exist only for introductory purpose (stand-alone runs) and thus they are not necessary to use this library itself.

**********
Dependency
**********

* C compiler

* MPI

*****
Usage
*****

Please refer to `the documentation <https://naokihori.github.io/SimpleDecomp/index.html>`_.

***********
Application
***********

This library is used to solve incompressible Navier-Stokes equations efficiently (in particular as a part of Poisson solver) in `my project <https://github.com/NaokiHori/SimpleNavierStokesSolver>`_.

.. image:: https://github.com/NaokiHori/SimpleNavierStokesSolver/blob/main/docs/source/snapshot3d.png
   :width: 67%

***************
Acknowledgement
***************

This library is inspired by `2decomp-fft <https://github.com/xcompact3d/2decomp-fft>`_.

The schematics are drawn by using `PyVista <https://docs.pyvista.org>`_.

