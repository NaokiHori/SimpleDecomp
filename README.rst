#############
Simple Decomp
#############

|CI| |Documentation|

.. |CI| image:: https://github.com/NaokiHori/SimpleDecomp/actions/workflows/ci.yml/badge.svg
.. _CI: https://github.com/NaokiHori/SimpleDecomp/actions/workflows/ci.yml

.. |Documentation| image:: https://github.com/NaokiHori/SimpleDecomp/actions/workflows/documentation.yml/badge.svg
.. _Documentation: https://github.com/NaokiHori/SimpleDecomp/actions/workflows/documentation.yml

|License| |LastCommit| |CodeFactor|

.. |License| image:: https://img.shields.io/github/license/NaokiHori/SimpleDecomp
.. _License: https://opensource.org/license/MIT

.. |LastCommit| image:: https://img.shields.io/github/last-commit/NaokiHori/SimpleDecomp/main
.. _LastCommit: https://github.com/NaokiHori/SimpleDecomp/commits/main

.. |CodeFactor| image:: https://www.codefactor.io/repository/github/naokihori/simpledecomp/badge/main
.. _CodeFactor: https://www.codefactor.io/repository/github/naokihori/simpledecomp/overview/main

This library decomposes two- and three-dimensional structured meshes between multiple processes in a pencil-like manner:

.. image:: https://naokihori.github.io/SimpleDecomp/_images/default_2d.png
   :width: 67%

.. image:: https://naokihori.github.io/SimpleDecomp/_images/default_3d.png
   :width: 67%

The main objective is to perform so-called parallel matrix transpose (rotating ``pencils``):

.. image:: https://naokihori.github.io/SimpleDecomp/_images/transpose_2d.png
   :width: 67%

.. image:: https://naokihori.github.io/SimpleDecomp/_images/transpose_3d.png
   :width: 100%

which are useful for performing operations where a process needs to contain all data in one direction, e.g. multi-dimensional `Fast Fourier Transforms <https://en.wikipedia.org/wiki/Fast_Fourier_transform>`_ or `Thomas algorithm <https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm>`_.

********
Features
********

* ``C99``-compatible.

* Domain decompositions and pencil rotations for arbitrary data types using a single function.

* Memory efficient: no internal buffers to store intermediate data.

* Matrix transposition (row-major and column-major orders are interchanged) while rotating the pencils to improve the cache efficiency of the successive operations (e.g. Fast Fourier Transforms, Thomas algorithm).

* Transparent (i.e. hackable) and well-documented implementation.

* `Well-tested <https://github.com/NaokiHori/SimpleDecomp/blob/main/.github/workflows/ci.yml>`_ for various data types, domain sizes and number of processes.

**********
Dependency
**********

* `C compiler <https://gcc.gnu.org>`_

* `MPI <https://www.open-mpi.org>`_

***********
Quick start
***********

Checkout this project

.. code-block:: console

   git clone https://github.com/NaokiHori/SimpleDecomp
   cd SimpleDecomp

and execute

.. code-block:: console

   make all
   mpirun -2 --oversubscribe ./a.out

which runs a small program to get started, which is elaborated below.

To understand the concept easily, I consider a two-dimensional array which is split into two sub-arrays (i.e. two processes) and is contained by each process:

.. code-block:: text

  +-------------+
  | 16 17 18 19 |
  | 12 13 14 15 |
  | 08 09 10 11 |
  +-------------+
  | 04 05 06 07 |
  | 00 01 02 03 |
  +-------------+

where the lower (from 00 to 07) and the upper (from 08 to 19) parts are stored by the rank 0 and 1, respectively.
Notice that the matrix is stored in the row-major order, i.e.

.. code-block:: text

   00 01 02 03 04 05 06 07

in the buffer of the rank 0, and

.. code-block:: text

   08 09 10 11 12 13 14 15 16 17 18 19

in the buffer of the rank 1.

They are called ``x pencils`` in this project.

After the pencils are rotated, I obtain

.. code-block:: text

  +-------+-------+
  | 16 17 | 18 19 |
  | 12 13 | 14 15 |
  | 08 09 | 10 11 |
  | 04 05 | 06 07 |
  | 00 01 | 02 03 |
  +-------+-------+

where the left and the right parts are stored by the rank 0 and 1, respectively.
Notice that the matrix is now stored in the column-major order, i.e.:

.. code-block:: text

   00 04 08 12 16 01 05 09 13 17

in the buffer of the rank 0, while

.. code-block:: text

   02 06 10 14 18 03 07 11 15 19

in the buffer of the rank 1.

They are named as ``y pencils`` in this project.

Check `src/main.c <https://github.com/NaokiHori/SimpleDecomp/blob/main/src/main.c>`_ to see how the matrix is initialised and the library APIs are called.

*****
Usage
*****

Please refer to `the documentation <https://naokihori.github.io/SimpleDecomp>`_ for practical usages and all available APIs.

***********
Application
***********

I use this library to solve the incompressible Navier-Stokes equations efficiently on clusters.

#. Finite-difference method:

   .. image:: https://raw.githubusercontent.com/NaokiHori/NaokiHori/main/assets/SimpleNSSolver.svg
      :target: https://github.com/NaokiHori/SimpleNSSolver
      :width: 50%

#. Spectral method:

   .. image:: https://raw.githubusercontent.com/NaokiHori/NaokiHori/main/assets/SpectralNSSolver1.svg
      :target: https://github.com/NaokiHori/SpectralNSSolver1
      :width: 50%

**********************
Reinventing the wheel?
**********************

Although the concept is similar to `2decomp-fft <https://github.com/xcompact3d/2decomp-fft>`_, the pencil rotations are largely tailored to my purposes and the implementation is simplified.

I also recognise many other nice libraries which aim to do the same thing (`in Juila <https://github.com/jipolanco/PencilArrays.jl>`_ or `in Fortran <https://github.com/GongZheng-Justin/ParaTC>`_, I am pretty sure there are more).

***************
Acknowledgement
***************

This library is inspired by `2decomp-fft <https://github.com/xcompact3d/2decomp-fft>`_.

