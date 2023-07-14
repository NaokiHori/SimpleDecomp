#################
sdecomp/transpose
#################

This directory contains the implementation of ``Simple Decomp`` library, in particular functions which handle pencil rotations.
Normally you do not have to touch anything here.
If you are interested in the details, each ``C`` source plays the following role.

#. ``2d.c``, ``3d.c``

   Two- and three-dimensional pencil rotation constructors ``sdecomp.transpose.construct`` are implemented.

#. ``main.c``

   ``sdecomp.transpose`` is defined and all function pointers are assigned.
   Wrappers ``sdecomp.transpose.construct``, ``sdecomp.transpose.destruct`` and ``sdecomp.transpose.execute`` are defined, whose arguments are passed to the corresponding internal functions implemented in the other places.

