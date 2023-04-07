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

      * :c:macro:`STROLL_CONCAT`
      * :c:macro:`STROLL_STRING`
      * :c:macro:`STROLL_UNIQ`
      * :c:macro:`compile_assert`
      * :c:macro:`compile_choose`
      * :c:macro:`compile_eval`

   * Various

      * :c:macro:`array_nr`
      * :c:macro:`stroll_abs`
      * :c:macro:`stroll_min`
      * :c:macro:`stroll_max`

   * Attribute wrappers :

      * :c:macro:`__align`
      * :c:macro:`__const`
      * :c:macro:`__ctor`
      * :c:macro:`__dtor`
      * :c:macro:`__export_public`
      * :c:macro:`__export_protect`
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

When compiled with the :c:macro:`CONFIG_STROLL_BOPS` build configuration
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

.. index:: bitmaps, bmap

Bitmaps
=======

When compiled with the :c:macro:`CONFIG_STROLL_BMAP` build configuration
option enabled, the Stroll library provides support for bitmap operations.
These are:

.. hlist::

   * Initialization:

      * :c:macro:`STROLL_BMAP_INIT_CLEAR`
      * :c:macro:`STROLL_BMAP_INIT_SET`
      * :c:macro:`STROLL_BMAP32_INIT_CLEAR`
      * :c:macro:`STROLL_BMAP32_INIT_SET`
      * :c:macro:`STROLL_BMAP64_INIT_CLEAR`
      * :c:macro:`STROLL_BMAP64_INIT_SET`
      * :c:func:`stroll_bmap_setup_clear`
      * :c:func:`stroll_bmap32_setup_clear`
      * :c:func:`stroll_bmap64_setup_clear`
      * :c:func:`stroll_bmap_setup_set`
      * :c:func:`stroll_bmap32_setup_set`
      * :c:func:`stroll_bmap64_setup_set`

   * Iteration:

      * :c:macro:`stroll_bmap_foreach_clear`
      * :c:macro:`stroll_bmap32_foreach_clear`
      * :c:macro:`stroll_bmap64_foreach_clear`
      * :c:macro:`stroll_bmap_foreach_set`
      * :c:macro:`stroll_bmap32_foreach_set`
      * :c:macro:`stroll_bmap64_foreach_set`

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

   * Clear bit(s):

      * :c:func:`stroll_bmap_clear`
      * :c:func:`stroll_bmap_clear_mask`
      * :c:func:`stroll_bmap_clear_range`
      * :c:func:`stroll_bmap_clear_all`
      * :c:func:`stroll_bmap32_clear`
      * :c:func:`stroll_bmap32_clear_mask`
      * :c:func:`stroll_bmap32_clear_range`
      * :c:func:`stroll_bmap32_clear_all`
      * :c:func:`stroll_bmap64_clear`
      * :c:func:`stroll_bmap64_clear_mask`
      * :c:func:`stroll_bmap64_clear_range`
      * :c:func:`stroll_bmap64_clear_all`

   * Toggle bit(s):

      * :c:func:`stroll_bmap_toggle`
      * :c:func:`stroll_bmap_toggle_mask`
      * :c:func:`stroll_bmap_toggle_range`
      * :c:func:`stroll_bmap_toggle_all`
      * :c:func:`stroll_bmap32_toggle`
      * :c:func:`stroll_bmap32_toggle_mask`
      * :c:func:`stroll_bmap32_toggle_range`
      * :c:func:`stroll_bmap32_toggle_all`
      * :c:func:`stroll_bmap64_toggle`
      * :c:func:`stroll_bmap64_toggle_mask`
      * :c:func:`stroll_bmap64_toggle_range`
      * :c:func:`stroll_bmap64_toggle_all`

Length-Value Strings
====================

When compiled with the :c:macro:`CONFIG_STROLL_LVSTR` build configuration option
enabled, the Stroll library provides support for :c:struct:`stroll_lvstr`
length-value strings.

This framework may ease the management of C strings life-cycle. In addition,
it caches the length of string registered into it to mitigate client code string
length computation overhead.

The following manipulations are available:

.. hlist::

   * Accessors:

      * :c:macro:`STROLL_LVSTR_LEN_MAX`
      * :c:func:`stroll_lvstr_cstr`
      * :c:func:`stroll_lvstr_len`

   * C string registration:

      * :c:func:`stroll_lvstr_cede`
      * :c:func:`stroll_lvstr_dup`
      * :c:func:`stroll_lvstr_lend`
      * :c:func:`stroll_lvstr_ncede`
      * :c:func:`stroll_lvstr_ndup`
      * :c:func:`stroll_lvstr_nlend`

   * Static initialization:

      * :c:macro:`STROLL_LVSTR_INIT`
      * :c:macro:`STROLL_LVSTR_INIT_NLEND`
      * :c:macro:`STROLL_LVSTR_INIT_NCEDE`

   * Initialization:

      * :c:func:`stroll_lvstr_init`
      * :c:func:`stroll_lvstr_init_cede`
      * :c:func:`stroll_lvstr_init_dup`
      * :c:func:`stroll_lvstr_init_lend`
      * :c:func:`stroll_lvstr_init_ncede`
      * :c:func:`stroll_lvstr_init_ndup`
      * :c:func:`stroll_lvstr_init_nlend`

   * Finalization:

      * :c:func:`stroll_lvstr_fini`

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
   
__export_public
***************

.. doxygendefine:: __export_public

__export_protect
****************

.. doxygendefine:: __export_protect

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

STROLL_BMAP_INIT_CLEAR
**********************

.. doxygendefine:: STROLL_BMAP_INIT_CLEAR

STROLL_BMAP_INIT_SET
********************

.. doxygendefine:: STROLL_BMAP_INIT_SET

STROLL_BMAP32_INIT_CLEAR
************************

.. doxygendefine:: STROLL_BMAP32_INIT_CLEAR

STROLL_BMAP32_INIT_SET
**********************

.. doxygendefine:: STROLL_BMAP32_INIT_SET

STROLL_BMAP64_INIT_CLEAR
************************

.. doxygendefine:: STROLL_BMAP64_INIT_CLEAR

STROLL_BMAP64_INIT_SET
**********************

.. doxygendefine:: STROLL_BMAP64_INIT_SET

STROLL_CONCAT
*************

.. doxygendefine:: STROLL_CONCAT

STROLL_LVSTR_INIT
*****************

.. doxygendefine:: STROLL_LVSTR_INIT

STROLL_LVSTR_INIT_NLEND
***********************

.. doxygendefine:: STROLL_LVSTR_INIT_NLEND

STROLL_LVSTR_INIT_NCEDE
***********************

.. doxygendefine:: STROLL_LVSTR_INIT_NCEDE

STROLL_LVSTR_LEN_MAX
********************

.. doxygendefine:: STROLL_LVSTR_LEN_MAX

STROLL_STRING
*************

.. doxygendefine:: STROLL_STRING

STROLL_UNIQ
***********

.. doxygendefine:: STROLL_UNIQ

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

stroll_abs
**********

.. doxygendefine:: stroll_abs

stroll_assert
*************

.. doxygendefine:: stroll_assert

stroll_bmap_foreach_clear
*************************

.. doxygendefine:: stroll_bmap_foreach_clear

stroll_bmap_foreach_set
***********************

.. doxygendefine:: stroll_bmap_foreach_set

stroll_bmap32_foreach_clear
***************************

.. doxygendefine:: stroll_bmap32_foreach_clear

stroll_bmap32_foreach_set
*************************

.. doxygendefine:: stroll_bmap32_foreach_set

stroll_bmap64_foreach_clear
***************************

.. doxygendefine:: stroll_bmap64_foreach_clear

stroll_bmap64_foreach_set
*************************

.. doxygendefine:: stroll_bmap64_foreach_set

stroll_min
**********

.. doxygendefine:: stroll_min

stroll_max
**********

.. doxygendefine:: stroll_max

Structures
----------

stroll_lvstr
************

.. doxygenstruct:: stroll_lvstr

Functions
---------

stroll_bmap_and
***************

.. doxygenfunction:: stroll_bmap_and

stroll_bmap_and_range
*********************

.. doxygenfunction:: stroll_bmap_and_range

stroll_bmap_clear
*****************

.. doxygenfunction:: stroll_bmap_clear


stroll_bmap_clear_mask
**********************

.. doxygenfunction:: stroll_bmap_clear_mask

stroll_bmap_clear_range
***********************

.. doxygenfunction:: stroll_bmap_clear_range

stroll_bmap_clear_all
*********************

.. doxygenfunction:: stroll_bmap_clear_all

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

stroll_bmap_setup_clear
***********************

.. doxygenfunction:: stroll_bmap_setup_clear

stroll_bmap_setup_set
*********************

.. doxygenfunction:: stroll_bmap_setup_set

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

stroll_bmap_toggle
******************

.. doxygenfunction:: stroll_bmap_toggle

stroll_bmap_toggle_mask
***********************

.. doxygenfunction:: stroll_bmap_toggle_mask

stroll_bmap_toggle_range
************************

.. doxygenfunction:: stroll_bmap_toggle_range

stroll_bmap_toggle_all
**********************

.. doxygenfunction:: stroll_bmap_toggle_all

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

stroll_bmap32_clear
*******************

.. doxygenfunction:: stroll_bmap32_clear


stroll_bmap32_clear_mask
************************

.. doxygenfunction:: stroll_bmap32_clear_mask

stroll_bmap32_clear_range
*************************

.. doxygenfunction:: stroll_bmap32_clear_range

stroll_bmap32_clear_all
***********************

.. doxygenfunction:: stroll_bmap32_clear_all

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

stroll_bmap32_setup_clear
*************************

.. doxygenfunction:: stroll_bmap32_setup_clear

stroll_bmap32_setup_set
***********************

.. doxygenfunction:: stroll_bmap32_setup_set

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

stroll_bmap32_toggle
********************

.. doxygenfunction:: stroll_bmap32_toggle

stroll_bmap32_toggle_mask
*************************

.. doxygenfunction:: stroll_bmap32_toggle_mask

stroll_bmap32_toggle_range
**************************

.. doxygenfunction:: stroll_bmap32_toggle_range

stroll_bmap32_toggle_all
************************

.. doxygenfunction:: stroll_bmap32_toggle_all

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

stroll_bmap64_clear
*******************

.. doxygenfunction:: stroll_bmap64_clear


stroll_bmap64_clear_mask
************************

.. doxygenfunction:: stroll_bmap64_clear_mask

stroll_bmap64_clear_range
*************************

.. doxygenfunction:: stroll_bmap64_clear_range

stroll_bmap64_clear_all
***********************

.. doxygenfunction:: stroll_bmap64_clear_all

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

stroll_bmap64_setup_clear
*************************

.. doxygenfunction:: stroll_bmap64_setup_clear

stroll_bmap64_setup_set
***********************

.. doxygenfunction:: stroll_bmap64_setup_set

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

stroll_bmap64_toggle
********************

.. doxygenfunction:: stroll_bmap64_toggle

stroll_bmap64_toggle_mask
*************************

.. doxygenfunction:: stroll_bmap64_toggle_mask

stroll_bmap64_toggle_range
**************************

.. doxygenfunction:: stroll_bmap64_toggle_range

stroll_bmap64_toggle_all
************************

.. doxygenfunction:: stroll_bmap64_toggle_all

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

stroll_lvstr_cede
*****************

.. doxygenfunction:: stroll_lvstr_cede

stroll_lvstr_cstr
*****************

.. doxygenfunction:: stroll_lvstr_cstr

stroll_lvstr_dup
****************

.. doxygenfunction:: stroll_lvstr_dup

stroll_lvstr_fini
*****************

.. doxygenfunction:: stroll_lvstr_fini

stroll_lvstr_init
*****************

.. doxygenfunction:: stroll_lvstr_init

stroll_lvstr_init_cede
**********************

.. doxygenfunction:: stroll_lvstr_init_cede

stroll_lvstr_init_dup
*********************

.. doxygenfunction:: stroll_lvstr_init_dup

stroll_lvstr_init_lend
**********************

.. doxygenfunction:: stroll_lvstr_init_lend

stroll_lvstr_init_ncede
***********************

.. doxygenfunction:: stroll_lvstr_init_ncede

stroll_lvstr_init_ndup
**********************

.. doxygenfunction:: stroll_lvstr_init_ndup

stroll_lvstr_init_nlend
***********************

.. doxygenfunction:: stroll_lvstr_init_nlend

stroll_lvstr_len
****************

.. doxygenfunction:: stroll_lvstr_len

stroll_lvstr_lend
*****************

.. doxygenfunction:: stroll_lvstr_lend

stroll_lvstr_ncede
******************

.. doxygenfunction:: stroll_lvstr_ncede

stroll_lvstr_ndup
*****************

.. doxygenfunction:: stroll_lvstr_ndup

stroll_lvstr_nlend
******************

.. doxygenfunction:: stroll_lvstr_nlend
