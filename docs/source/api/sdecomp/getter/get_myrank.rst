Example: get my location as a ``y1pencil`` in x direction:

.. code-block:: c

   const int myposition_1pencils_x = 0;
   sdecomp.get_myrank(
       info,
       SDECOMP_Y1PENCIL, // y1 pencil
       SDECOMP_XDIR,     // x direction
       &myposition_1pencils_x
   );

.. image:: /images/y1pencil_3d.png
   :align: center
   :width: 40%

Let us imagine that this function is called by the green process.
Then ``1`` is assigned to ``myposition_1pencils_x``, since the green process is in the second position (notice that the index in C starts from 0).

