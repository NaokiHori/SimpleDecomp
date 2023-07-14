Example: get number of points up to my position as a ``y1pencil`` when the total number of points in x direction is ``1024``:

.. code-block:: c

   const size_t glsize = 1024;
   size_t offset = 0;
   sdecomp.get_pencil_offset(
       info,
       SDECOMP_Y1PENCIL,
       SDECOMP_XDIR,
       glsize_x,
       &offset_x
   );

.. image:: /images/y1pencil_3d.png
   :align: center
   :width: 40%

Let us imagine that this function is called by the green process.
Then ``8`` is assigned to ``offset_x``, since the reddish process (having 8 grids inside) sits in front of the green one.

