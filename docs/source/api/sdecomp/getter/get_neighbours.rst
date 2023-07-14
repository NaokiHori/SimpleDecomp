Example: get neighbour ranks of ``y1pencil`` in x direction:

.. code-block:: c

   const int neighbour_ranks[2] = {
      MPI_PROC_NULL,
      MPI_PROC_NULL
   };
   sdecomp.get_neighbours(
       info,
       SDECOMP_Y1PENCIL, // y1 pencil
       SDECOMP_XDIR,     // x direction
       neighbour_ranks
   );

.. image:: /images/y1pencil_3d.png
   :align: center
   :width: 40%

Let us imagine that this function is called by the green process.
Then the ranks of the reddish and bluish processes are assigned to ``neighbour_ranks[0]`` and ``neighbour_ranks[1]``, respectively.

.. note::

   ``MPI_PROC_NULL`` is assigned to ``neighbour_ranks`` if there is no neighbour there.
   This is the case when the constructor ``sdecomp.construct`` is called with the no-periodic condition in the direction.

   Also my rank is assigned when there is only one process in the direction and the direction is periodic.

