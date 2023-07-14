######################################
Pencil rotation: ``sdecomp.transpose``
######################################

APIs to achieve the pencil rotations are listed in this page.

***********
Constructor
***********

=============
``construct``
=============

   Creating a structure ``sdecomp_transpose_plan_t`` which contains all essential information to achieve pencil rotations and returns a pointer to it.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: constructor of sdecomp_transpose_plan_t

   .. mydetails:: Details

      .. include:: constructor/construct.rst

**********
Destructor
**********

============
``destruct``
============

   Destructing a plan created by ``sdecomp.transpose.construct``.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: destructor of sdecomp_transpose_plan_t

   .. mydetails:: Details

      .. include:: destructor/destruct.rst

******
Runner
******

===========
``execute``
===========

   Executing pencil rotations based on the plan created by ``sdecomp.transpose.construct``.

   .. myliteralinclude:: /../../include/sdecomp.h
      :language: c
      :tag: transpose runner

   .. mydetails:: Details

      .. include:: runner/execute.rst

