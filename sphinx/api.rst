Overview
========

What follows here provides a thorough description of how to use Stroll's
library.

Basically, Stroll library is a basic C framework that provides definitions
usefull to carry out common C/C++ applicative tasks.
The library is implemented to run on GNU Linux / glibc platforms only (although
porting to alternate C library such as `musl libc <https://www.musl-libc.org/>`_
should not be much of a hassle).

Stroll library API is organized around the following functional areas which
you can refer to for further details :

* `Common definitions`_,
* Assertions_,
* `Bit operations`_,
* `Atomic operations`_,
* `Power of 2 operations`_,
* `Bitmaps`_,
* `Doubly linked lists`_,
* `Singly linked lists`_,
* `Length-value strings`_.
  

Build configuration
===================

.. todo::

   Document build configuration macros from Kconfig


.. index:: common definitions, cdefs

Common definitions
==================

Stroll library exposes various C preprocessor macros used to implement Stroll
internals and meant for application development purposes. These are:

.. hlist::

   * Compile time logic :

      * :c:macro:`compile_assert`
      * :c:macro:`compile_choose`
      * :c:macro:`compile_eval`

   * Various

      * :c:macro:`array_nr`

   * Attribute wrappers :

      * :c:macro:`__align`
      * :c:macro:`__const`
      * :c:macro:`__ctor`
      * :c:macro:`__dtor`
      * :c:macro:`__leaf`
      * :c:macro:`__nonull`
      * :c:macro:`__noreturn`
      * :c:macro:`__packed`
      * :c:macro:`__printf`
      * :c:macro:`__pure`
      * :c:macro:`__returns_nonull`
      * :c:macro:`__unused`
      * :c:macro:`__warn_result`

.. index:: assertions

Assertions
==========

When compiled with the :c:macro:`CONFIG_STROLL_ASSERT` build configuration
option enabled, the Stroll library exposes the :c:macro:`stroll_assert` macro so
that developper may perform standard assertion checking.

.. index:: bit operations, bitops

Bit operations
==============

When compiled with the :c:macro:`CONFIG_STROLL_BITOPS` build configuration
option enabled, the Stroll library provides support for bit manipulation
operations. These are:

.. hlist::

   * Find First bit Set:

      * :c:func:`stroll_bops_ffs`
      * :c:func:`stroll_bops32_ffs`
      * :c:func:`stroll_bops64_ffs`

   * Find Last bit Set:

      * :c:func:`stroll_bops_fls`
      * :c:func:`stroll_bops32_fls`
      * :c:func:`stroll_bops64_fls`

   * Find First bit Cleared:

      * :c:func:`stroll_bops_ffc`
      * :c:func:`stroll_bops32_ffc`
      * :c:func:`stroll_bops64_ffc`

   * Find number of set bits (:index:`Hammimg weight`):

      * :c:func:`stroll_bops_hweight`
      * :c:func:`stroll_bops32_hweight`
      * :c:func:`stroll_bops64_hweight`

Atomic operations
=================

Power of 2 operations
=====================

.. index:: bitmaps, bmap
   
Bitmaps
=======

When compiled with the :c:macro:`CONFIG_STROLL_BITMAP` build configuration
option enabled, the Stroll library provides support for bitmap operations.
These are:

.. hlist::

   * Compute masks:

      * :c:func:`stroll_bmap_mask`
      * :c:func:`stroll_bmap32_mask`
      * :c:func:`stroll_bmap64_mask`

   * Compute number of bits set (:index:`Hammimg weight`):

      * :c:func:`stroll_bmap_hweight`
      * :c:func:`stroll_bmap32_hweight`
      * :c:func:`stroll_bmap64_hweight`

   * Perform bitwise AND operation:

      * :c:func:`stroll_bmap_and`
      * :c:func:`stroll_bmap_and_range`
      * :c:func:`stroll_bmap32_and`
      * :c:func:`stroll_bmap32_and_range`
      * :c:func:`stroll_bmap64_and`
      * :c:func:`stroll_bmap64_and_range`

   * Perform bitwise OR operation:

      * :c:func:`stroll_bmap_or`
      * :c:func:`stroll_bmap_or_range`
      * :c:func:`stroll_bmap32_or`
      * :c:func:`stroll_bmap32_or_range`
      * :c:func:`stroll_bmap64_or`
      * :c:func:`stroll_bmap64_or_range`

   * Perform bitwise XOR operation:

      * :c:func:`stroll_bmap_xor`
      * :c:func:`stroll_bmap_xor_range`
      * :c:func:`stroll_bmap32_xor`
      * :c:func:`stroll_bmap32_xor_range`
      * :c:func:`stroll_bmap64_xor`
      * :c:func:`stroll_bmap64_xor_range`

   * Test set bit(s):

      * :c:func:`stroll_bmap_test`
      * :c:func:`stroll_bmap_test_all`
      * :c:func:`stroll_bmap_test_mask`
      * :c:func:`stroll_bmap_test_range`
      * :c:func:`stroll_bmap32_test`
      * :c:func:`stroll_bmap32_test_all`
      * :c:func:`stroll_bmap32_test_mask`
      * :c:func:`stroll_bmap32_test_range`
      * :c:func:`stroll_bmap64_test`
      * :c:func:`stroll_bmap64_test_all`
      * :c:func:`stroll_bmap64_test_mask`
      * :c:func:`stroll_bmap64_test_range`

   * Set bit(s):

      * :c:func:`stroll_bmap_set`
      * :c:func:`stroll_bmap_set_mask`
      * :c:func:`stroll_bmap_set_range`
      * :c:func:`stroll_bmap_set_all`
      * :c:func:`stroll_bmap32_set`
      * :c:func:`stroll_bmap32_set_mask`
      * :c:func:`stroll_bmap32_set_range`
      * :c:func:`stroll_bmap32_set_all`
      * :c:func:`stroll_bmap64_set`
      * :c:func:`stroll_bmap64_set_mask`
      * :c:func:`stroll_bmap64_set_range`
      * :c:func:`stroll_bmap64_set_all`


Doubly linked lists
===================

Singly linked lists
===================

Length-value strings
====================

Reference
=========

Macros
------

__align
*******

.. doxygendefine:: __align

__const
*******

.. doxygendefine:: __const

__ctor
******

.. doxygendefine:: __ctor

__dtor
******

.. doxygendefine:: __dtor

__leaf
******

.. doxygendefine:: __leaf

__nonull
********

.. doxygendefine:: __nonull

__noreturn
**********

.. doxygendefine:: __noreturn

__packed
********

.. doxygendefine:: __packed

__printf
********

.. doxygendefine:: __printf

__pure
******

.. doxygendefine:: __pure

__returns_nonull
****************

.. doxygendefine:: __returns_nonull

__unused
********

.. doxygendefine:: __unused

__warn_result
*************

.. doxygendefine:: __warn_result

array_nr
********

.. doxygendefine:: array_nr

compile_eval
************

.. doxygendefine:: compile_eval

compile_assert
**************

.. doxygendefine:: compile_assert

compile_choose
**************

.. doxygendefine:: compile_choose

stroll_assert
*************

.. doxygendefine:: stroll_assert

Enumerations
------------

enbox_entry_type
****************

.. doxygenenum:: enbox_entry_type

Structures
----------

enbox_bind_entry
****************

.. doxygenstruct:: enbox_bind_entry

Functions
---------

stroll_bmap_and
***************

.. doxygenfunction:: stroll_bmap_and

stroll_bmap_and_range
*********************

.. doxygenfunction:: stroll_bmap_and_range

stroll_bmap_hweight
*******************

.. doxygenfunction:: stroll_bmap_hweight

stroll_bmap_mask
****************
   
.. doxygenfunction:: stroll_bmap_mask

stroll_bmap_or
**************

.. doxygenfunction:: stroll_bmap_or

stroll_bmap_or_range
********************

.. doxygenfunction:: stroll_bmap_or_range

stroll_bmap_set
***************

.. doxygenfunction:: stroll_bmap_set

stroll_bmap_set_mask
********************

.. doxygenfunction:: stroll_bmap_set_mask

stroll_bmap_set_range
*********************

.. doxygenfunction:: stroll_bmap_set_range

stroll_bmap_set_all
*******************

.. doxygenfunction:: stroll_bmap_set_all

stroll_bmap_test
****************

.. doxygenfunction:: stroll_bmap_test

stroll_bmap_test_all
********************

.. doxygenfunction:: stroll_bmap_test_all

stroll_bmap_test_mask
*********************

.. doxygenfunction:: stroll_bmap_test_mask

stroll_bmap_test_range
**********************

.. doxygenfunction:: stroll_bmap_test_range

stroll_bmap_xor
***************

.. doxygenfunction:: stroll_bmap_xor

stroll_bmap_xor_range
*********************

.. doxygenfunction:: stroll_bmap_xor_range

stroll_bmap32_and
*****************

.. doxygenfunction:: stroll_bmap32_and

stroll_bmap32_and_range
***********************

.. doxygenfunction:: stroll_bmap32_and_range

stroll_bmap32_hweight
*********************

.. doxygenfunction:: stroll_bmap32_hweight

stroll_bmap32_mask
******************

.. doxygenfunction:: stroll_bmap32_mask
   
stroll_bmap32_or
****************

.. doxygenfunction:: stroll_bmap32_or

stroll_bmap32_or_range
**********************

.. doxygenfunction:: stroll_bmap32_or_range

stroll_bmap32_set
***************

.. doxygenfunction:: stroll_bmap32_set

stroll_bmap32_set_mask
********************

.. doxygenfunction:: stroll_bmap32_set_mask

stroll_bmap32_set_range
*********************

.. doxygenfunction:: stroll_bmap32_set_range

stroll_bmap32_set_all
*******************

.. doxygenfunction:: stroll_bmap32_set_all

stroll_bmap32_test
******************

.. doxygenfunction:: stroll_bmap32_test

stroll_bmap32_test_all
**********************

.. doxygenfunction:: stroll_bmap32_test_all

stroll_bmap32_test_mask
***********************

.. doxygenfunction:: stroll_bmap32_test_mask

stroll_bmap32_test_range
************************

.. doxygenfunction:: stroll_bmap32_test_range

stroll_bmap32_xor
*****************

.. doxygenfunction:: stroll_bmap32_xor

stroll_bmap32_xor_range
***********************

.. doxygenfunction:: stroll_bmap32_xor_range

stroll_bmap64_and
*****************

.. doxygenfunction:: stroll_bmap64_and

stroll_bmap64_and_range
***********************

.. doxygenfunction:: stroll_bmap64_and_range

stroll_bmap64_hweight
*********************

.. doxygenfunction:: stroll_bmap64_hweight

stroll_bmap64_mask
******************
   
.. doxygenfunction:: stroll_bmap64_mask

stroll_bmap64_or
****************

.. doxygenfunction:: stroll_bmap64_or

stroll_bmap64_or_range
**********************

.. doxygenfunction:: stroll_bmap64_or_range

stroll_bmap64_set
***************

.. doxygenfunction:: stroll_bmap64_set

stroll_bmap64_set_mask
********************

.. doxygenfunction:: stroll_bmap64_set_mask

stroll_bmap64_set_range
*********************

.. doxygenfunction:: stroll_bmap64_set_range

stroll_bmap64_set_all
*******************

.. doxygenfunction:: stroll_bmap64_set_all

stroll_bmap64_test
******************

.. doxygenfunction:: stroll_bmap64_test

stroll_bmap64_test_all
**********************

.. doxygenfunction:: stroll_bmap64_test_all

stroll_bmap64_test_mask
***********************

.. doxygenfunction:: stroll_bmap64_test_mask

stroll_bmap64_test_range
************************

.. doxygenfunction:: stroll_bmap64_test_range

stroll_bmap64_xor
*****************

.. doxygenfunction:: stroll_bmap64_xor

stroll_bmap64_xor_range
***********************

.. doxygenfunction:: stroll_bmap64_xor_range

stroll_bops_ffc
***************

.. doxygenfunction:: stroll_bops_ffc

stroll_bops_ffs
***************

.. doxygenfunction:: stroll_bops_ffs

stroll_bops_fls
***************

.. doxygenfunction:: stroll_bops_fls

stroll_bops_hweight
*******************

.. doxygenfunction:: stroll_bops_hweight

stroll_bops32_ffc
*****************

.. doxygenfunction:: stroll_bops32_ffc

stroll_bops64_ffc
*****************

.. doxygenfunction:: stroll_bops64_ffc

stroll_bops32_ffs
*****************

.. doxygenfunction:: stroll_bops32_ffs

stroll_bops64_ffs
*****************

.. doxygenfunction:: stroll_bops64_ffs

stroll_bops32_fls
*****************

.. doxygenfunction:: stroll_bops32_fls

stroll_bops64_fls
*****************

.. doxygenfunction:: stroll_bops64_fls

stroll_bops32_hweight
*********************

.. doxygenfunction:: stroll_bops32_hweight

stroll_bops64_hweight
*********************

.. doxygenfunction:: stroll_bops64_hweight
