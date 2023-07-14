#######
sdecomp
#######

This directory contains the implementation of ``Simple Decomp`` library.
Normally you do not have to touch anything here.
If you are interested in the details, each ``C`` source plays the following role.

#. ``get.c``

   Some getter functions, which are too complicated to put in ``main.c``, are implemented.

#. ``kernel.c``

   A central algorithm to decide the grid decomposition is implemented.

#. ``main.c``

   ``sdecomp`` is defined and all function pointers are assigned.

#. ``memory.c``

   General-purpose memory manager.

#. ``sanitise.c``

   Argument sanitisers.

#. ``transpose/``

   Pencil rotations.

