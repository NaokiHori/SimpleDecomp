Example: get number of points I am responsible for as a ``y1pencil`` when the total number of points in x direction is ``24``:

.. code-block:: c

   const size_t glsize_x = 24;
   size_t mysize_x = 0;
   sdecomp.get_pencil_mysize(
       info,
       SDECOMP_Y1PENCIL, // y1 pencil
       SDECOMP_XDIR,     // x direction
       glsize_x,
       &mysize_x
   );

.. image:: /images/y1pencil_3d.png
   :align: center
   :width: 40%

Let us imagine that this function is called by the green process.
Then ``8`` is assigned to ``mysize_x`` (this process is responsible for 8 grids in x direction).

