#################################
Domain decomposition: ``sdecomp``
#################################

APIs to achieve the domain decomposition are listed in this page.

***********
Constructor
***********

=============
``construct``
=============

   Creating a structure ``sdecomp_info_t`` which contains all essential information to describe a decomposed domain and returns a pointer to it.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: constructor of sdecomp_info_t

   .. mydetails:: Details

      .. include:: constructor/construct.rst

**********
Destructor
**********

============
``destruct``
============

   Destructor of ``sdecomp_info_t``.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: destructor of sdecomp_info_t

   .. mydetails:: Details

      .. include:: destructor/destruct.rst

******
Getter
******

=============
``get_ndims``
=============

   Get the number of dimensions currently considered.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: getter, number of dimensions

   .. mydetails:: Details

      .. include:: getter/get_ndims.rst

=================
``get_comm_size``
=================

   Get the **total** number of processes in the default communicator.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: getter, number of total processes in comm_cart

   .. mydetails:: Details

      .. include:: getter/get_comm_size.rst

=================
``get_comm_rank``
=================

   Get the ID of my rank in the default communicator.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: getter, my ID in comm_cart

   .. mydetails:: Details

      .. include:: getter/get_comm_rank.rst

==============
``get_nprocs``
==============

   Get number of processes (number of pencils) in one direction.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: getter, number of processes in one dimension

   .. mydetails:: Details

      .. include:: getter/get_nprocs.rst

==============
``get_myrank``
==============

   Get the position of my pencil.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: getter, position of my process

   .. mydetails:: Details

      .. include:: getter/get_myrank.rst

==================
``get_neighbours``
==================

   Get ranks of my neighbours under the communicator ``get_comm_cart`` gives.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: getter, ranks of my neighbours

   .. mydetails:: Details

      .. include:: getter/get_neighbours.rst

=====================
``get_pencil_mysize``
=====================

   Get the size of my pencil.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: getter, local size of my pencil

   .. mydetails:: Details

      .. include:: getter/get_pencil_mysize.rst

=====================
``get_pencil_offset``
=====================

   Get the offset of my pencil.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: getter, offset of my pencil

   .. mydetails:: Details

      .. include:: getter/get_pencil_offset.rst

=================
``get_comm_cart``
=================

   Get the communicator holding the Cartesian topology of ``x1pencil``.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: getter, default communicator

   .. mydetails:: Details

      .. include:: getter/get_comm_cart.rst

