This function retrieves ``ndims`` which was passed to ``sdecomp.construct`` when creating ``sdecomp_info_t *info``.

Example:

.. code-block:: c

   size_t ndims = 0;
   sdecomp.get_ndims(
       info,
       &ndims
   );

2 or 3 should be assigned to ``ndims`` in this case, depending on your initialisation.

