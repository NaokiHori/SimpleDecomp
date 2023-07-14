#############
Simple Decomp
#############

This library utilises a pencil-like approach to decompose two- and three-dimensional structured meshes among multiple (``MPI``) processes:

   .. image:: /images/default_2d.png
      :align: center
      :width: 75%

Here, a two-dimensional domain depicted in the left image, with each grey cube representing an element of the array, undergoes decomposition into :math:`1 \times 6` chunks.
The right image illustrates the differently coloured chunks resulting from the decomposition.
Each coloured chunk is assigned to a distinct process, enabling the distribution of the overall workload.

   .. image:: /images/default_3d.png
      :align: center
      :width: 75%

Likewise, a three-dimensional domain displayed in the left image is partitioned into :math:`1 \times 3 \times 4` chunks.
These instances are referred to as ``x1pencils`` since each chunk resembles a pencil pointing in the :math:`x` direction.

It is possible to align the pencils differently, i.e.

   .. image:: /images/transpose_2d.png
      :align: left
      :width: 67%

for two-dimensional domains, or

   .. image:: /images/transpose_3d.png
      :align: left
      :width: 100%

for three-dimensional domains.

In these cases, each process encompasses all grid points in the :math:`y` (or :math:`z`) direction.
This can be advantageous when conducting operations that require access to all information along a particular axis, such as multi-dimensional Fast Fourier Transforms.

The primary emphasis of this library revolves around these types of pencil rotations.

.. toctree::
   :hidden:

   introduction
   api/main

