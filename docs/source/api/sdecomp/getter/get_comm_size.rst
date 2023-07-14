This function calls ``MPI_Comm_size``, which returns the number of processes belong to the default communicator ``comm_default`` (an argument of ``sdecomp.construct``).

Example:

.. code-block:: c

   int nprocs = 0;
   sdecomp.get_comm_size(
       info,
       &nprocs
   );

