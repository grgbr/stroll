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

.. index:: common definitions, cdefs

Common definitions
==================

Stroll library exposes various C preprocessor macros used to implement Stroll
internals and meant for application development purposes. These are:

.. hlist::

   * Compile time assertions :

      * :c:macro:`compile_assert`
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

When compiled with the :c:macro:CONFIG_STROLL_ASSERT build configuration option
enabled, the Stroll library exposes the :c:macro:`stroll_assert` macro so that
developper may perform standard assertion checking.

.. index:: bit operations, bitops

Bit operations
==============

When compiled with the :c:macro:CONFIG_STROLL_BITOPS build configuration option
enabled, the Stroll library provides support for bit manipulation operations.
These are:

.. hlist::

   * Find First bit Set:

      * :c:func:`stroll_bops32_ffs`
      * :c:func:`stroll_bops64_ffs`
      * :c:func:`stroll_bops_ffs`

   * Find Last bit Set:

      * :c:func:`stroll_bops32_fls`
      * :c:func:`stroll_bops64_fls`
      * :c:func:`stroll_bops_fls`

   * Find number of set bits (:index:`Hammimg weight`):

      * :c:func:`stroll_bops32_hweight`
      * :c:func:`stroll_bops64_hweight`
      * :c:func:`stroll_bops_hweight`

Atomic operations
=================

Power of 2 operations
=====================

Bitmaps
=======

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

stroll_assert
*************

.. doxygendefine:: stroll_assert

stroll_bops_ffs
***************

.. doxygendefine:: stroll_bops_ffs

stroll_bops_fls
***************

.. doxygendefine:: stroll_bops_fls

stroll_bops_hweight
*******************

.. doxygendefine:: stroll_bops_hweight

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
