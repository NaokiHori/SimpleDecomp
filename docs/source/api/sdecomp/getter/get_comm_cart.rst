This is useful to call ``MPI_Cart_xxx`` directly, or to perform advanced operations.

Example:

.. code-block:: c

   MPI_Comm comm_cart = MPI_COMM_NULL;
   sdecomp.get_comm_cart(
       info,
       &comm_cart
   );

