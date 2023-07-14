Example: get number of ``y1pencils`` (number of processes) in x direction:

.. code-block:: c

   const int ny1pencils_x = 0;
   sdecomp.get_nprocs(
       info,
       SDECOMP_Y1PENCIL, // y1 pencil
       SDECOMP_XDIR,     // x direction
       &ny1pencils_x
   );

.. image:: /images/y1pencil_3d.png
   :align: center
   :width: 40%

In the case of the image, ``3`` is assigned to ``ny1pencils_x`` (see three pencils exist in x direction).

