This function calls ``MPI_Comm_rank``, which returns the rank of the calling process in the default communicator ``comm_default`` (an argument of ``sdecomp.construct``).

Example:

.. code-block:: c

   int myrank = 0;
   sdecomp.get_comm_rank(
       info,
       &myrank
   );

