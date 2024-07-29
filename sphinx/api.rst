.. SPDX-License-Identifier: GPL-3.0-only
   
   This file is part of Stroll.
   Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>

.. include:: _cdefs.rst

Overview
========

What follows here provides a thorough description of how to use Stroll_'s
library.

Basically, Stroll_ library is a basic C framework that provides definitions
usefull to carry out common C/C++ applicative tasks.
The library is implemented to run on GNU Linux / glibc platforms only (although
porting to alternate C library such as `musl libc <https://www.musl-libc.org/>`_
should not be much of a hassle).

Stroll_ library API is organized around the following functional areas which
you can refer to for further details :

* `Common definitions`_,
* Assertions_,
* `Bit operations`_,
* `Power of 2 operations`_,
* `Bitmaps`_,
* `Fixed sized bitmaps`_,
* `Length-value strings`_,
* `Searching arrays`_,
* `Sorting arrays`_,
* `Linked lists`_.

Stroll_ sources are distributed under the :ref:`GNU Lesser General Public
License <lgpl>` whereas documentation manuals are distributed under the
:ref:`GNU General Public License <gpl>`.

.. index:: build configuration, configuration macros

Build configuration
===================

At :ref:`Build configuration time <workflow-configure-phase>`, multiple build
options are available to customize final Stroll_ build. From client code, you
may eventually refer to the corresponding C macros listed below:

* :c:macro:`CONFIG_STROLL_ARRAY_3WQUICK_SORT`
* :c:macro:`CONFIG_STROLL_ARRAY_3WQUICK_SORT_BENTLEY_MCILROY`
* :c:macro:`CONFIG_STROLL_ARRAY_3WQUICK_SORT_DIJKSTRA`
* :c:macro:`CONFIG_STROLL_ARRAY_3WQUICK_SORT_INSERT_THRESHOLD`
* :c:macro:`CONFIG_STROLL_ARRAY_BISECT_SEARCH`
* :c:macro:`CONFIG_STROLL_ARRAY_BUBBLE_SORT`
* :c:macro:`CONFIG_STROLL_ARRAY_INSERT_SORT`
* :c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT_FULL_RUNS`
* :c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT_RUNS_BYBLOCK`
* :c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT_INSERT_THRESHOLD`
* :c:macro:`CONFIG_STROLL_ARRAY_QUICK_SORT`
* :c:macro:`CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD`
* :c:macro:`CONFIG_STROLL_ARRAY_SELECT_SORT`
* :c:macro:`CONFIG_STROLL_ASSERT`
* :c:macro:`CONFIG_STROLL_ASSERT_API`
* :c:macro:`CONFIG_STROLL_ASSERT_INTERN`
* :c:macro:`CONFIG_STROLL_BOPS`
* :c:macro:`CONFIG_STROLL_BMAP`
* :c:macro:`CONFIG_STROLL_DLIST`
* :c:macro:`CONFIG_STROLL_DLIST_BUBBLE_SORT`
* :c:macro:`CONFIG_STROLL_DLIST_INSERT_SORT`
* :c:macro:`CONFIG_STROLL_DLIST_MERGE_SORT`
* :c:macro:`CONFIG_STROLL_DLIST_MERGE_SORT_INSERT_THRESHOLD`
* :c:macro:`CONFIG_STROLL_DLIST_SELECT_SORT`
* :c:macro:`CONFIG_STROLL_FBMAP`
* :c:macro:`CONFIG_STROLL_LVSTR`
* :c:macro:`CONFIG_STROLL_POW2`
* :c:macro:`CONFIG_STROLL_SLIST`
* :c:macro:`CONFIG_STROLL_SLIST_BUBBLE_SORT`
* :c:macro:`CONFIG_STROLL_SLIST_INSERT_SORT`
* :c:macro:`CONFIG_STROLL_SLIST_MERGE_SORT`
* :c:macro:`CONFIG_STROLL_SLIST_MERGE_SORT_INSERT_THRESHOLD`
* :c:macro:`CONFIG_STROLL_SLIST_SELECT_SORT`
* :c:macro:`CONFIG_STROLL_UTEST`
* :c:macro:`CONFIG_STROLL_VALGRIND`

.. index:: common definitions, cdefs

Common definitions
==================

Stroll_ library exposes various C preprocessor macros used to implement Stroll_
internals and meant for application development purposes. These are:

.. hlist::

   * Compile time logic :

      * :c:macro:`STROLL_CONCAT`
      * :c:macro:`STROLL_CONST_ABS`
      * :c:macro:`STROLL_CONST_MAX`
      * :c:macro:`STROLL_CONST_MIN`
      * :c:macro:`STROLL_STRING`
      * :c:macro:`STROLL_UNIQ`
      * :c:macro:`compile_assert`
      * :c:macro:`compile_choose`
      * :c:macro:`compile_eval`

   * Branch prediction and prefetching :

      * :c:macro:`STROLL_PREFETCH_ACCESS_RO`
      * :c:macro:`STROLL_PREFETCH_ACCESS_RW`
      * :c:macro:`STROLL_PREFETCH_LOCALITY_TMP`
      * :c:macro:`STROLL_PREFETCH_LOCALITY_LOW`
      * :c:macro:`STROLL_PREFETCH_LOCALITY_FAIR`
      * :c:macro:`STROLL_PREFETCH_LOCALITY_HIGH`
      * :c:macro:`stroll_likely`
      * :c:macro:`stroll_prefetch`
      * :c:macro:`stroll_unlikely`

   * Various

      * :c:macro:`stroll_array_nr`
      * :c:macro:`stroll_abs`
      * :c:macro:`stroll_min`
      * :c:macro:`stroll_max`

   * Attribute wrappers :

      * :c:macro:`__align`
      * :c:macro:`__const`
      * :c:macro:`__ctor`
      * :c:macro:`__dtor`
      * :c:macro:`__export_hidden`
      * :c:macro:`__export_intern`
      * :c:macro:`__export_protect`
      * :c:macro:`__export_public`
      * :c:macro:`__leaf`
      * :c:macro:`__nonull`
      * :c:macro:`__noreturn`
      * :c:macro:`__nothrow`
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
option enabled, the Stroll_ library exposes the :c:macro:`stroll_assert` macro so
that developper may perform standard assertion checking.

.. index:: bit operations, bitops, bops

Bit operations
==============

When compiled with the :c:macro:`CONFIG_STROLL_BOPS` build configuration
option enabled, the Stroll_ library provides support for bit manipulation
operations. These are:

.. hlist::

   * Find First bit Set:

      * :c:func:`stroll_bops_ffs`
      * :c:func:`stroll_bops_ffs32`
      * :c:func:`stroll_bops_ffs64`
      * :c:func:`stroll_bops_ffsul`

   * Find Last bit Set:

      * :c:func:`stroll_bops_fls`
      * :c:func:`stroll_bops_fls32`
      * :c:func:`stroll_bops_fls64`
      * :c:func:`stroll_bops_flsul`

   * Find First bit Cleared:

      * :c:func:`stroll_bops_ffc`
      * :c:func:`stroll_bops_ffc32`
      * :c:func:`stroll_bops_ffc64`
      * :c:func:`stroll_bops_ffcul`

   * Find number of set bits (:index:`Hammimg weight`):

      * :c:func:`stroll_bops_hweight`
      * :c:func:`stroll_bops_hweight32`
      * :c:func:`stroll_bops_hweight64`
      * :c:func:`stroll_bops_hweightul`

.. index:: power of 2 operations, pow2

Power of 2 operations
=====================

When compiled with the :c:macro:`CONFIG_STROLL_POW2` build configuration
option enabled, the Stroll_ library provides support for basic power of 2
operations. These are:

.. hlist::

   * Find closest lower power of 2:

      * :c:func:`stroll_pow2_low`
      * :c:func:`stroll_pow2_low32`
      * :c:func:`stroll_pow2_low64`
      * :c:func:`stroll_pow2_lowul`

   * Find closest upper power of 2:

      * :c:func:`stroll_pow2_up`
      * :c:func:`stroll_pow2_up32`
      * :c:func:`stroll_pow2_up64`
      * :c:func:`stroll_pow2_upul`

.. index:: bitmaps, bmap

Bitmaps
=======

When compiled with the :c:macro:`CONFIG_STROLL_BMAP` build configuration
option enabled, the Stroll_ library provides support for bitmap operations.
These are:

.. hlist::

   * Initialization:

      * :c:macro:`STROLL_BMAP_INIT_CLEAR`
      * :c:macro:`STROLL_BMAP_INIT_CLEAR32`
      * :c:macro:`STROLL_BMAP_INIT_CLEAR64`
      * :c:macro:`STROLL_BMAP_INIT_CLEARUL`
      * :c:macro:`STROLL_BMAP_INIT_SET`
      * :c:macro:`STROLL_BMAP_INIT_SET32`
      * :c:macro:`STROLL_BMAP_INIT_SET64`
      * :c:macro:`STROLL_BMAP_INIT_SETUL`
      * :c:func:`stroll_bmap_setup_clear`
      * :c:func:`stroll_bmap_setup_clear32`
      * :c:func:`stroll_bmap_setup_clear64`
      * :c:func:`stroll_bmap_setup_clearul`
      * :c:func:`stroll_bmap_setup_set`
      * :c:func:`stroll_bmap_setup_set32`
      * :c:func:`stroll_bmap_setup_set64`
      * :c:func:`stroll_bmap_setup_setul`

   * Iteration:

      * :c:macro:`stroll_bmap_foreach_clear`
      * :c:macro:`stroll_bmap_foreach_clear32`
      * :c:macro:`stroll_bmap_foreach_clear64`
      * :c:macro:`stroll_bmap_foreach_clearul`
      * :c:macro:`stroll_bmap_foreach_set`
      * :c:macro:`stroll_bmap_foreach_set32`
      * :c:macro:`stroll_bmap_foreach_set64`
      * :c:macro:`stroll_bmap_foreach_setul`

   * Compute masks:

      * :c:func:`stroll_bmap_mask`
      * :c:func:`stroll_bmap_mask32`
      * :c:func:`stroll_bmap_mask64`
      * :c:func:`stroll_bmap_maskul`

   * Compute number of bits set (:index:`Hammimg weight`):

      * :c:func:`stroll_bmap_hweight`
      * :c:func:`stroll_bmap_hweight32`
      * :c:func:`stroll_bmap_hweight64`
      * :c:func:`stroll_bmap_hweightul`

   * Perform bitwise AND operation:

      * :c:func:`stroll_bmap_and`
      * :c:func:`stroll_bmap_and32`
      * :c:func:`stroll_bmap_and64`
      * :c:func:`stroll_bmap_andul`
      * :c:func:`stroll_bmap_and_range`
      * :c:func:`stroll_bmap_and_range32`
      * :c:func:`stroll_bmap_and_range64`
      * :c:func:`stroll_bmap_and_rangeul`

   * Perform bitwise OR operation:

      * :c:func:`stroll_bmap_or`
      * :c:func:`stroll_bmap_or32`
      * :c:func:`stroll_bmap_or64`
      * :c:func:`stroll_bmap_orul`
      * :c:func:`stroll_bmap_or_range`
      * :c:func:`stroll_bmap_or_range32`
      * :c:func:`stroll_bmap_or_range64`
      * :c:func:`stroll_bmap_or_rangeul`

   * Perform bitwise XOR operation:

      * :c:func:`stroll_bmap_xor`
      * :c:func:`stroll_bmap_xor32`
      * :c:func:`stroll_bmap_xor64`
      * :c:func:`stroll_bmap_xorul`
      * :c:func:`stroll_bmap_xor_range`
      * :c:func:`stroll_bmap_xor_range32`
      * :c:func:`stroll_bmap_xor_range64`
      * :c:func:`stroll_bmap_xor_rangeul`

   * Test set bit(s):

      * :c:func:`stroll_bmap_test`
      * :c:func:`stroll_bmap_test32`
      * :c:func:`stroll_bmap_test64`
      * :c:func:`stroll_bmap_testul`
      * :c:func:`stroll_bmap_test_all`
      * :c:func:`stroll_bmap_test_all32`
      * :c:func:`stroll_bmap_test_all64`
      * :c:func:`stroll_bmap_test_allul`
      * :c:func:`stroll_bmap_test_mask`
      * :c:func:`stroll_bmap_test_mask32`
      * :c:func:`stroll_bmap_test_mask64`
      * :c:func:`stroll_bmap_test_maskul`
      * :c:func:`stroll_bmap_test_range`
      * :c:func:`stroll_bmap_test_range32`
      * :c:func:`stroll_bmap_test_range64`
      * :c:func:`stroll_bmap_test_rangeul`

   * Set bit(s):

      * :c:func:`stroll_bmap_set`
      * :c:func:`stroll_bmap_set32`
      * :c:func:`stroll_bmap_set64`
      * :c:func:`stroll_bmap_setul`
      * :c:func:`stroll_bmap_set_all`
      * :c:func:`stroll_bmap_set_all32`
      * :c:func:`stroll_bmap_set_all64`
      * :c:func:`stroll_bmap_set_allul`
      * :c:func:`stroll_bmap_set_mask`
      * :c:func:`stroll_bmap_set_mask32`
      * :c:func:`stroll_bmap_set_mask64`
      * :c:func:`stroll_bmap_set_maskul`
      * :c:func:`stroll_bmap_set_range`
      * :c:func:`stroll_bmap_set_range32`
      * :c:func:`stroll_bmap_set_range64`
      * :c:func:`stroll_bmap_set_rangeul`

   * Clear bit(s):

      * :c:func:`stroll_bmap_clear`
      * :c:func:`stroll_bmap_clear32`
      * :c:func:`stroll_bmap_clear64`
      * :c:func:`stroll_bmap_clearul`
      * :c:func:`stroll_bmap_clear_all`
      * :c:func:`stroll_bmap_clear_all32`
      * :c:func:`stroll_bmap_clear_all64`
      * :c:func:`stroll_bmap_clear_allul`
      * :c:func:`stroll_bmap_clear_mask`
      * :c:func:`stroll_bmap_clear_mask32`
      * :c:func:`stroll_bmap_clear_mask64`
      * :c:func:`stroll_bmap_clear_maskul`
      * :c:func:`stroll_bmap_clear_range`
      * :c:func:`stroll_bmap_clear_range32`
      * :c:func:`stroll_bmap_clear_range64`
      * :c:func:`stroll_bmap_clear_rangeul`

   * Toggle bit(s):

      * :c:func:`stroll_bmap_toggle`
      * :c:func:`stroll_bmap_toggle32`
      * :c:func:`stroll_bmap_toggle64`
      * :c:func:`stroll_bmap_toggleul`
      * :c:func:`stroll_bmap_toggle_all`
      * :c:func:`stroll_bmap_toggle_all32`
      * :c:func:`stroll_bmap_toggle_all64`
      * :c:func:`stroll_bmap_toggle_allul`
      * :c:func:`stroll_bmap_toggle_mask`
      * :c:func:`stroll_bmap_toggle_mask32`
      * :c:func:`stroll_bmap_toggle_mask64`
      * :c:func:`stroll_bmap_toggle_maskul`
      * :c:func:`stroll_bmap_toggle_range`
      * :c:func:`stroll_bmap_toggle_range32`
      * :c:func:`stroll_bmap_toggle_range64`
      * :c:func:`stroll_bmap_toggle_rangeul`

.. index:: bitmaps, bmap, fixed sized bitmaps, fbmap

Fixed sized bitmaps
===================

When compiled with the :c:macro:`CONFIG_STROLL_FBMAP` build configuration
option enabled, the Stroll_ library provides support for manipulating
:c:struct:`stroll_fbmap` bitmaps stored into memory area of arbitrary fixed
sizes. These are:

.. hlist::

   * Initialization:

      * :c:func:`stroll_fbmap_fini`
      * :c:func:`stroll_fbmap_init_clear`
      * :c:func:`stroll_fbmap_init_dup`
      * :c:func:`stroll_fbmap_init_set`

   * Modify bit:

      * :c:func:`stroll_fbmap_clear`
      * :c:func:`stroll_fbmap_clear_all`
      * :c:func:`stroll_fbmap_set`
      * :c:func:`stroll_fbmap_set_all`
      * :c:func:`stroll_fbmap_toggle`
      * :c:func:`stroll_fbmap_toggle_all`

   * Test bit:

      * :c:func:`stroll_fbmap_test`
      * :c:func:`stroll_fbmap_test_range`
      * :c:func:`stroll_fbmap_test_all`

   * Iteration:

      * :c:struct:`stroll_fbmap_iter`
      * :c:macro:`stroll_fbmap_foreach_range_set()`
      * :c:macro:`stroll_fbmap_foreach_set()`
      * :c:macro:`stroll_fbmap_foreach_range_clear()`
      * :c:macro:`stroll_fbmap_foreach_clear()`

   * Compute number of bits set (:index:`Hammimg weight`):

      * :c:func:`stroll_fbmap_hweight`

   * Various:

      * :c:func:`stroll_fbmap_nr`

.. _sect-api-lvstr:

.. index:: length-value string, lvstr

Length-Value Strings
====================

When compiled with the :c:macro:`CONFIG_STROLL_LVSTR` build configuration option
enabled, the Stroll_ library provides support for :c:struct:`stroll_lvstr`
length-value strings.

This framework ease the management of C strings life-cycle. In addition,
it caches the length of string registered into it to mitigate client code string
length computation overhead.

The following manipulations are available:

.. hlist::

   * Static initialization:

      * :c:macro:`STROLL_LVSTR_INIT`
      * :c:macro:`STROLL_LVSTR_INIT_LEND`
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

   * C string registration:

      * :c:func:`stroll_lvstr_cede`
      * :c:func:`stroll_lvstr_drop`
      * :c:func:`stroll_lvstr_dup`
      * :c:func:`stroll_lvstr_lend`
      * :c:func:`stroll_lvstr_ncede`
      * :c:func:`stroll_lvstr_ndup`
      * :c:func:`stroll_lvstr_nlend`

   * Accessors:

      * :c:macro:`STROLL_LVSTR_LEN_MAX`
      * :c:func:`stroll_lvstr_cstr`
      * :c:func:`stroll_lvstr_len`

   * Finalization:

      * :c:func:`stroll_lvstr_fini`

Array operations
================

.. index:: search, find, bisection search, dichotomic search

Searching arrays
----------------

When compiled with the :c:macro:`CONFIG_STROLL_ARRAY_BISECT_SEARCH` build
configuration option enabled, the Stroll_ library provides support for searching
pre-sorted arrays thanks to :c:func:`stroll_array_bisect_search`.

.. index:: sort

Sorting arrays
--------------

The Stroll_ library provides support for sorting arrays thanks to multiple
algorithms.

The table below presents available implementations with their related
:term:`sorting properties` to help you to select the right algorithm according
to your applicative requirements.

.. table:: Array sorting algorithm properties

    +------------------------+---------------------------------------------------------------------------------------------------------------------------+
    |                        | Algorithms                                                                                                                |
    | Properties             +------------------------+---------------------+---------------------+------------------+------------------+----------------+
    |                        | `Array merge           | `Array quick        | `Array 3-way quick  | `Array insertion | `Array selection | `Array bubble  |
    |                        | sort`_                 | sort`_              | sort`_              | sort`_           | sort`_           | sort`_         |
    +========================+========================+=====================+=====================+==================+==================+================+
    | |adaptive|             | no                     | duplicate /         | no                  | yes              | no               | yes            |
    |                        |                        | in-order inputs     |                     |                  |                  |                |
    +------------------------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    | |online|               | no                     | no                  | no                  | yes              | no               | no             |
    +------------------------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    | |stable|               | yes                    | no                  | no                  | yes              | no               | yes            |
    +------------------------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    | |recursive|            | yes                    | yes                 | yes                 | no               | no               | no             |
    +------------------------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    | |in-place|             | no                     | yes                 | yes                 | yes              | yes              | yes            |
    +------------------------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    | **allocation**         | on stack & heap        | on stack            | on stack            | none             | none             | none           |
    +--------------+---------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    |              | worst   | :math:`O(\frac{n}{2})` | :math:`O(log(n))`   | :math:`O(log(n))`   | :math:`O(1)`     | :math:`O(1)`     | :math:`O(1)`   |
    |              +---------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    | **space**    | average | :math:`O(\frac{n}{2})` | :math:`O(log(n))`   | :math:`O(log(n))`   | :math:`O(1)`     | :math:`O(1)`     | :math:`O(1)`   |
    | |complexity| +---------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    |              | best    | :math:`O(\frac{n}{2})` | :math:`O(log(n))`   | :math:`O(log(n))`   | :math:`O(1)`     | :math:`O(1)`     | :math:`O(1)`   |
    +--------------+---------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    |              | worst   | :math:`O(n log(n))`    | :math:`O(n^2)`      | :math:`O(n^2)`      | :math:`O(n^2)`   | :math:`O(n^2)`   | :math:`O(n^2)` |
    |              +---------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    | **time**     | average | :math:`O(n log(n))`    | :math:`O(n log(n))` | :math:`O(n log(n))` | :math:`O(n^2)`   | :math:`O(n^2)`   | :math:`O(n^2)` |
    | |complexity| +---------+------------------------+---------------------+---------------------+------------------+------------------+----------------+
    |              | best    | :math:`O(n log(n))`    | :math:`O(n log(n))` | :math:`O(n log(n))` | :math:`O(n)`     | :math:`O(n^2)`   | :math:`O(n)`   |
    +--------------+---------+------------------------+---------------------+---------------------+------------------+------------------+----------------+

.. index:: sort;array merge sort, merge sort;array, array;merge sort

Array merge sort
****************

When compiled with the :c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT` build
configuration option enabled, the Stroll_ library provides support for
`merge`_ sort algorithm thanks to :c:func:`stroll_array_merge_sort`.

Algorithm is implemented according to a top-down recursive implementation that
shows better runtime performances than its iterative bottom-up counterpart. It
includes the following usual optimizations:

* :math:`O(\frac{n}{2})` auxiliary space requirement ;
* limit number of copy operations during merging ;
* partially bypass merging operation when input is already in (reverse-)order
  (see :c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT_FULL_RUNS` and 
  :c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT_RUNS_BYBLOCK`) ;
* switch to `array insertion sort`_ when the number of partition elements is
  less than :c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT_INSERT_THRESHOLD` to limit
  recursion depth.

You may customize `array merge sort`_ switch to `array insertion sort`_ at
Stroll_ building time thanks to the
:c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT_INSERT_THRESHOLD` build configuration
macro.
For each partition, when the number of elements left to sort is below this
threshold, `array merge sort`_ will switch to `array insertion sort`_ to
minimize the number of element swap operations and recursion depth.

The :c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT_FULL_RUNS` build configuration
macro may be enabled to give merge sort the ability to detect fully ordered
partitions in order to skip the usual partitions / sub-arrays merging process at
the expense of 2 additional array element comparisons. This may improve
performances for partially / fully ordered data sets.

The :c:macro:`CONFIG_STROLL_ARRAY_MERGE_SORT_RUNS_BYBLOCK` build configuration
macro may be enabled to give merge sort the ability to try to merge partitions /
sub-arrays by blocks of consecutive ordered elements as opposed to the
traditional single element based merging process. This allows to replace
multiple primary element copy operations by one single copy of a block of
ordered primary elements. This may improve performances for partially / fully
ordered data sets.

.. note::

   * efficient, general-purpose sorting algorithm ;
   * is |stable| but not |in-place| ;
   * most of the time, exhibits slightly better performances than `quick sort`_,
     especially for (reverse-)ordered inputs and/or larger data sets ;
   * at the expense of additional memory requirements ;
   * refer to `Sorting arrays`_ for more informations related to algorithm
     selection.

.. index:: sort;array quick sort,
           quick sort;array,
           array;quick sort

Array quick sort
****************

When compiled with the :c:macro:`CONFIG_STROLL_ARRAY_QUICK_SORT` build
configuration option enabled, the Stroll_ library provides support for
`Quick`_ sort algorithm thanks to :c:func:`stroll_array_quick_sort`.

Algorithm is implemented according to a recursive implementation that includes
the following usual optimizations:

* recursion auxiliary stack space limited to :math:`O(log(n))` thanks to
  recursive tail call elimination ;
* *partition* elements according to the Hoare_ scheme ;
* choose *pivot* according to the median-of-three_ strategy to prevent from
  :math:`O(n^2)` degradation for already sorted inputs ;
* when the number of partition elements is less than
  :c:macro:`CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD`, stop recursion and
  run a final `array insertion sort`_ pass.

You may customize `array quick sort`_ switch to `array insertion sort`_ at
Stroll_ building time thanks to the
:c:macro:`CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD` build configuration
macro.
For each partition, when the number of elements left to sort is below this
threshold, `array quick sort`_ will switch to `array insertion sort`_ to
minimize the number of element swap operations and recursion depth.

.. note::

   * efficient, general-purpose sorting algorithm ;
   * not |stable| but |in-place| ;
   * exhibits poor performance for inputs containing small number of distinct
     keys (prefer `array 3-way quick sort`_ instead) ;
   * most of the time, exhibits slightly worse performances than
     `array merge sort`_, especially for (reverse-)ordered inputs and/or larger
     data sets ;
   * refer to `Sorting arrays`_ for more informations related to algorithm
     selection.

.. index:: sort;array 3-way quick sort,
           3-way quick sort;array,
           array;3-way quick sort

Array 3-way quick sort
**********************

When compiled with the :c:macro:`CONFIG_STROLL_ARRAY_3WQUICK_SORT` build
configuration option enabled, the Stroll_ library provides support for
`3-way quick`_ sort algorithm thanks to :c:func:`stroll_array_3wquick_sort`.

Algorithm is implemented according to a recursive implementation that includes
the following usual optimizations:

* recursion auxiliary stack space limited to :math:`O(log(n))` thanks to
  recursive tail call elimination ;
* choose *pivot* according to the median-of-three_ strategy to prevent from
  :math:`O(n^2)` degradation for already sorted inputs ;
* when the number of partition elements is less than
  :c:macro:`CONFIG_STROLL_ARRAY_3WQUICK_SORT_INSERT_THRESHOLD`, stop recursion
  and run a final `array insertion sort`_ pass.

Elements are partitioned into 3 sub-arrays according to one of the following
scheme that can be configured at building time:

* `Dijkstra`_ 3-way partitioning (thanks to the
  :c:macro:`CONFIG_STROLL_ARRAY_3WQUICK_SORT_DIJKSTRA` macro) ;
* or `Bentley-McIlroy`_ 3-way partitioning (thanks to the
  :c:macro:`CONFIG_STROLL_ARRAY_3WQUICK_SORT_BENTLEY_MCILROY` macro).

`Dijkstra`_'s scheme is implemented for *reference only*. Use
`Bentley-McIlroy`_'s scheme instead unless you really know what you are doing.

You may customize `array 3-way quick sort`_ switch to `array insertion sort`_ at
Stroll_ building time thanks to the
:c:macro:`CONFIG_STROLL_ARRAY_3WQUICK_SORT_INSERT_THRESHOLD` build configuration
macro.
For each partition, when the number of elements left to sort is below this
threshold, `array 3-way quick sort`_ will switch to `array insertion sort`_ to
minimize the number of element swap operations and recursion depth.

.. note::

   * general-purpose sorting algorithm efficient for inputs containing **many**
     duplicates;
   * not |stable| but |in-place| ;
   * exhibits worse performance than standard `array quick sort`_ for inputs
     containing moderate to high number of distinct key values ;
   * refer to `Sorting arrays`_ for more informations related to algorithm
     selection.

.. index:: sort;array insertion sort,
           insertion sort;array,
           array;insertion sort

Array insertion sort
********************

When compiled with the :c:macro:`CONFIG_STROLL_ARRAY_INSERT_SORT` build
configuration option enabled, the Stroll_ library provides support for
`Insertion`_ sort algorithm thanks to :c:func:`stroll_array_insert_sort`.

2 alternate functions may be used to add elements to an input array that is
known (for sure) to be already sorted. This allows to optimize situations where
sorting a *continuous stream of input elements*. These are:

* :c:func:`stroll_array_insert_inpsort_elem` for |in-place| |online| sorting,
* and :c:func:`stroll_array_insert_oopsort_elem`, for |out-of-place| |online|
  sorting.

.. note::

   * simple implementation ;
   * limited number of items swaps ;
   * very efficient on small and pre-sorted data sets ;
   * poor efficiency over large data sets ;
   * refer to `Sorting arrays`_ for more informations related to algorithm
     selection.

.. index:: sort;array selection sort,
           selection sort;array,
           array;selection sort

Array selection sort
********************

When compiled with the :c:macro:`CONFIG_STROLL_ARRAY_SELECT_SORT` build
configuration option enabled, the Stroll_ library provides support for
`Selection`_ sort algorithm thanks to :c:func:`stroll_array_select_sort`.

.. note::

   * poor to extremely low efficiency even over small data sets.

.. warning::

   Implemented for reference only: **DO NOT USE IT**. Refer to
   `Sorting arrays`_ for more informations related to algorithm selection.

.. index:: sort;array bubble sort,
           bubble sort;array,
           array;bubble sort

Array bubble sort
*****************

When compiled with the :c:macro:`CONFIG_STROLL_ARRAY_BUBBLE_SORT` build
configuration option enabled, the Stroll_ library provides support for
`Bubble`_ sort algorithm thanks to :c:func:`stroll_array_bubble_sort`.

.. note::

   * high number of items swaps ;
   * decent efficiency with pre-sorted data sets ;
   * poor to extremely low efficiency even over small data sets.

.. warning::

   Implemented for reference only: **DO NOT USE IT**. Refer to
   `Sorting arrays`_ for more informations related to algorithm selection.

.. index:: linked list, list

Linked lists
============

Stroll_ provides support for 2 different kinds of :term:`linked list`: 

* `singly linked lists`_,
* and `doubly linked lists`_.

.. index:: singly linked list, linked list;singly, list;singly

Singly linked lists
-------------------

When compiled with the :c:macro:`CONFIG_STROLL_SLIST` build configuration option
enabled, the Stroll_ library provides support for :c:struct:`stroll_slist`
singly linked lists where each :c:struct:`stroll_slist_node` node contains a
single reference to the next one in the sequence.

The following API is available:

.. hlist::

   * Initialization:

      * :c:macro:`STROLL_SLIST_INIT()`
      * :c:macro:`STROLL_SLIST_NODE_INIT`
      * :c:func:`stroll_slist_init()`

   * Accessing / iterating over nodes:

      * :c:func:`stroll_slist_first()`
      * :c:macro:`stroll_slist_first_entry()`
      * :c:macro:`stroll_slist_foreach_node()`
      * :c:macro:`stroll_slist_foreach_entry()`
      * :c:func:`stroll_slist_head()`
      * :c:func:`stroll_slist_last()`
      * :c:macro:`stroll_slist_last_entry()`
      * :c:func:`stroll_slist_next()`
      * :c:macro:`stroll_slist_next_entry()`

   * Modifying:

      * :c:func:`stroll_slist_append()`
      * :c:func:`stroll_slist_dqueue_front()`
      * :c:func:`stroll_slist_embed()`
      * :c:func:`stroll_slist_move()`
      * :c:func:`stroll_slist_nqueue_back()`
      * :c:func:`stroll_slist_nqueue_front()`
      * :c:func:`stroll_slist_remove()`
      * :c:func:`stroll_slist_splice()`
      * :c:func:`stroll_slist_withdraw()`

   * Various:

      * :c:func:`stroll_slist_empty()`
      * :c:macro:`stroll_slist_entry()`

.. index:: doubly linked list, linked list;doubly, list;doubly

Doubly linked lists
-------------------

When compiled with the :c:macro:`CONFIG_STROLL_DLIST` build configuration option
enabled, the Stroll_ library provides support for doubly linked lists where each
:c:struct:`stroll_dlist_node` node contains 2 links respectively referencing
previous and next nodes in the sequence.

The following API is available:

.. hlist::

   * Initialization:

      * :c:macro:`STROLL_DLIST_INIT()`
      * :c:func:`stroll_dlist_init()`

   * Accessing / iterating over nodes:

      * :c:macro:`stroll_dlist_foreach_entry()`
      * :c:macro:`stroll_dlist_foreach_entry_safe()`
      * :c:macro:`stroll_dlist_foreach_node()`
      * :c:macro:`stroll_dlist_foreach_node_safe()`
      * :c:macro:`stroll_dlist_foreach_prev_entry()`
      * :c:macro:`stroll_dlist_foreach_prev_entry_safe()`
      * :c:macro:`stroll_dlist_foreach_prev_node()`
      * :c:macro:`stroll_dlist_foreach_prev_node_safe()`
      * :c:func:`stroll_dlist_next()`
      * :c:macro:`stroll_dlist_next_entry()`
      * :c:func:`stroll_dlist_prev()`
      * :c:macro:`stroll_dlist_prev_entry()`

   * Modifying:

      * :c:func:`stroll_dlist_append()`
      * :c:func:`stroll_dlist_dqueue_back()`
      * :c:func:`stroll_dlist_dqueue_back_init()`
      * :c:func:`stroll_dlist_dqueue_front()`
      * :c:func:`stroll_dlist_dqueue_front_init()`
      * :c:func:`stroll_dlist_embed_after()`
      * :c:func:`stroll_dlist_embed_before()`
      * :c:func:`stroll_dlist_insert()`
      * :c:func:`stroll_dlist_move_after()`
      * :c:func:`stroll_dlist_move_before()`
      * :c:func:`stroll_dlist_nqueue_back()`
      * :c:func:`stroll_dlist_nqueue_front()`
      * :c:func:`stroll_dlist_remove()`
      * :c:func:`stroll_dlist_remove_init()`
      * :c:func:`stroll_dlist_replace()`
      * :c:func:`stroll_dlist_replace_init()`
      * :c:func:`stroll_dlist_splice_after()`
      * :c:func:`stroll_dlist_splice_before()`
      * :c:func:`stroll_dlist_withdraw()`

   * Various:

      * :c:func:`stroll_dlist_empty()`
      * :c:macro:`stroll_dlist_entry()`

Sorting lists
-------------

The Stroll_ library provides support for sorting :term:`linked lists` thanks to
multiple algorithms.

The table below presents available implementations with their related
:term:`sorting properties` to help you to select the right algorithm according
to your applicative requirements.

.. table:: Linked list sorting algorithm properties

    +------------------------+-------------------------------------------------------------------------------+
    |                        | Algorithms                                                                    |
    | Properties             +------------------------+------------------+------------------+----------------+
    |                        | `List merge            | `List insertion  | `List selection  | `List bubble   |
    |                        | sort`_                 | sort`_           | sort`_           | sort`_         |
    +========================+========================+==================+==================+================+
    | |adaptive|             | sort of                | yes              | no               | yes            |
    |                        |                        |                  |                  |                |
    +------------------------+------------------------+------------------+------------------+----------------+
    | |online|               | no                     | yes              | no               | no             |
    +------------------------+------------------------+------------------+------------------+----------------+
    | |stable|               | yes                    | yes              | yes              | yes            |
    +------------------------+------------------------+------------------+------------------+----------------+
    | |recursive|            | no                     | no               | no               | no             |
    +------------------------+------------------------+------------------+------------------+----------------+
    | |in-place|             | no                     | yes              | yes              | yes            |
    +------------------------+------------------------+------------------+------------------+----------------+
    | **allocation**         | on stack               | none             | none             | none           |
    +--------------+---------+------------------------+------------------+------------------+----------------+
    |              | worst   | :math:`O(log(n))`      | :math:`O(1)`     | :math:`O(1)`     | :math:`O(1)`   |
    |              +---------+------------------------+------------------+------------------+----------------+
    | **space**    | average | :math:`O(log(n))`      | :math:`O(1)`     | :math:`O(1)`     | :math:`O(1)`   |
    | |complexity| +---------+------------------------+------------------+------------------+----------------+
    |              | best    | :math:`O(log(n))`      | :math:`O(1)`     | :math:`O(1)`     | :math:`O(1)`   |
    +--------------+---------+------------------------+------------------+------------------+----------------+
    |              | worst   | :math:`O(1)`           | :math:`O(n^2)`   | :math:`O(n^2)`   | :math:`O(n^2)` |
    |              +---------+------------------------+------------------+------------------+----------------+
    | **time**     | average | :math:`O(n log(n))`    | :math:`O(n^2)`   | :math:`O(n^2)`   | :math:`O(n^2)` |
    | |complexity| +---------+------------------------+------------------+------------------+----------------+
    |              | best    | :math:`O(n)`           | :math:`O(n)`     | :math:`O(n^2)`   | :math:`O(n)`   |
    +--------------+---------+------------------------+------------------+------------------+----------------+

.. index:: sort;list merge sort,
           merge sort;list,
           list;merge sort

List merge sort
***************

When compiled with the :c:macro:`CONFIG_STROLL_SLIST_MERGE_SORT` build
configuration option enabled, the Stroll_ library provides support for
`Merge`_ sort algorithm over `singly linked lists`_ thanks to:

* :c:func:`stroll_slist_merge_presort`,
* :c:func:`stroll_slist_merge_sort`

In addition, when compiled with the :c:macro:`CONFIG_STROLL_DLIST_MERGE_SORT`
build configuration option enabled, the Stroll_ library provides support for
`Merge`_ sort algorithm over `doubly linked lists`_ thanks to:

* :c:func:`stroll_dlist_merge_presort`,
* :c:func:`stroll_dlist_merge_sort`

Algorithm is implemented according to an iterative bottom-up implementation
based upon an original idea attributed to Jon McCarthy and described
`here <http://richardhartersworld.com/cri/2007/schoen.html>`_.

The approach tries to avoid excessive scanning of the lists during sublist
splitting phases by using an additional :math:`O(log(n))` bounded auxiliary
space to store sublist heads.

The algorithm starts by using the `merge`_ sort’s divide and conquer strategy.
To limit the number of merging passes, it switches to `list insertion sort`_
when the number of nodes left to sort is below the
:c:macro:`CONFIG_STROLL_SLIST_MERGE_SORT_INSERT_THRESHOLD` or
:c:macro:`CONFIG_STROLL_DLIST_MERGE_SORT_INSERT_THRESHOLD` thresholds which may
be configured at Stroll_ building time.

.. note::

   * efficient, general-purpose sorting algorithm ;
   * is |stable| but not |in-place| ;
   * exhibits much better performances than `list insertion sort`_, except for
     very small data sets ;
   * refer to `Sorting lists`_ for more informations related to algorithm
     selection.

.. index:: sort;list insertion sort,
           insertion sort;list,
           list;insertion sort

List insertion sort
*******************

When compiled with the :c:macro:`CONFIG_STROLL_SLIST_INSERT_SORT` build
configuration option enabled, the Stroll_ library provides support for
`Insertion`_ sort algorithm over `singly linked lists`_ thanks to
:c:func:`stroll_slist_insert_sort`.

In addition, when compiled with the :c:macro:`CONFIG_STROLL_DLIST_INSERT_SORT`
build configuration option enabled, the Stroll_ library provides support for
`Insertion`_ sort algorithm over `doubly linked lists`_ thanks to
:c:func:`stroll_dlist_insert_sort`.

.. note::

   * simple implementation ;
   * limited number of items swaps ;
   * very efficient on small and pre-sorted data sets ;
   * poor efficiency over large data sets ;
   * refer to `Sorting lists`_ for more informations related to algorithm
     selection.

.. index:: sort;list selection sort,
           selection sort;list,
           list;selection sort

List selection sort
*******************

When compiled with the :c:macro:`CONFIG_STROLL_SLIST_SELECT_SORT` build
configuration option enabled, the Stroll_ library provides support for
`Selection`_ sort algorithm over `singly linked lists`_ thanks to
:c:func:`stroll_slist_select_sort`.

In addition, when compiled with the :c:macro:`CONFIG_STROLL_DLIST_SELECT_SORT`
build configuration option enabled, the Stroll_ library provides support for
`Selection`_ sort algorithm over `doubly linked lists`_ thanks to
:c:func:`stroll_dlist_select_sort`.

.. note::

   * poor to extremely low efficiency even over small data sets.

.. warning::

   Implemented for reference only: **DO NOT USE IT**. Refer to
   `Sorting lists`_ for more informations related to algorithm selection.

.. index:: sort;list bubble sort,
           bubble sort;list,
           list;bubble sort

List bubble sort
****************

When compiled with the :c:macro:`CONFIG_STROLL_SLIST_BUBBLE_SORT` build
configuration option enabled, the Stroll_ library provides support for
`Bubble`_ sort algorithm over `singly linked lists`_ thanks to
:c:func:`stroll_slist_bubble_sort`.

In addition, when compiled with the :c:macro:`CONFIG_STROLL_DLIST_BUBBLE_SORT`
build configuration option enabled, the Stroll_ library provides support for
`Bubble`_ sort algorithm over `doubly linked lists`_ thanks to
:c:func:`stroll_dlist_bubble_sort`.

.. note::

   * high number of items swaps ;
   * decent efficiency with pre-sorted data sets ;
   * poor to extremely low efficiency even over small data sets.

.. warning::

   Implemented for reference only: **DO NOT USE IT**. Refer to
   `Sorting lists`_ for more informations related to algorithm selection.


.. index:: heap

Heaps
=====

Stroll_ provides support for several kinds of :term:`heap`: 

Algorithm selection rationales:

Insert:
* hprheap
* pprheap, dprheap
* rpheap
* dbnheap

Extract:
hprheap
pprheap
dprheap
rpheap
dbnheap

Remove:  in order  / out of order
dprheap
            rpheap / pprheap
           pprheap / rpheap
dbnheap

Promote:
rpheap
dprheap
pprheap
dbnheap

Demote:
dprheap
pprheap
rpheap
dbnheap

Conclusion:

Insert/Extract only: hprheap
Promote intensive (Dijkstra): rpheap
Promote, demote, remove intensive: dprheap
All round usage: dprheap (better promote/demote/remove), then pprheap (better extract and smaller footprint)

.. todo:: complete documentation

.. index:: API reference, reference

Reference
=========

Configuration macros
--------------------

CONFIG_STROLL_ASSERT
********************

.. doxygendefine:: CONFIG_STROLL_ASSERT

CONFIG_STROLL_ARRAY_3WQUICK_SORT
********************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_3WQUICK_SORT

CONFIG_STROLL_ARRAY_3WQUICK_SORT_BENTLEY_MCILROY
************************************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_3WQUICK_SORT_BENTLEY_MCILROY

CONFIG_STROLL_ARRAY_3WQUICK_SORT_DIJKSTRA
*****************************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_3WQUICK_SORT_DIJKSTRA

CONFIG_STROLL_ARRAY_3WQUICK_SORT_INSERT_THRESHOLD
*************************************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_3WQUICK_SORT_INSERT_THRESHOLD

CONFIG_STROLL_ARRAY_BISECT_SEARCH
*********************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_BISECT_SEARCH

CONFIG_STROLL_ARRAY_BUBBLE_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_BUBBLE_SORT

CONFIG_STROLL_ARRAY_INSERT_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_INSERT_SORT

CONFIG_STROLL_ARRAY_MERGE_SORT_FULL_RUNS
****************************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_MERGE_SORT_FULL_RUNS

CONFIG_STROLL_ARRAY_MERGE_SORT_INSERT_THRESHOLD
***********************************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_MERGE_SORT_INSERT_THRESHOLD

CONFIG_STROLL_ARRAY_MERGE_SORT_RUNS_BYBLOCK
*******************************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_MERGE_SORT_RUNS_BYBLOCK

CONFIG_STROLL_ARRAY_QUICK_SORT
******************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_QUICK_SORT

CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD
***********************************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD

CONFIG_STROLL_ARRAY_SELECT_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_ARRAY_SELECT_SORT

CONFIG_STROLL_ASSERT_API
************************

.. doxygendefine:: CONFIG_STROLL_ASSERT_API

CONFIG_STROLL_ASSERT_INTERN
***************************

.. doxygendefine:: CONFIG_STROLL_ASSERT_INTERN

CONFIG_STROLL_BOPS
******************

.. doxygendefine:: CONFIG_STROLL_BOPS

CONFIG_STROLL_BMAP
******************

.. doxygendefine:: CONFIG_STROLL_BMAP

CONFIG_STROLL_DLIST
*******************

.. doxygendefine:: CONFIG_STROLL_DLIST

CONFIG_STROLL_DLIST_BUBBLE_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_DLIST_BUBBLE_SORT

CONFIG_STROLL_DLIST_INSERT_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_DLIST_INSERT_SORT

CONFIG_STROLL_DLIST_MERGE_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_DLIST_MERGE_SORT

CONFIG_STROLL_DLIST_MERGE_SORT_INSERT_THRESHOLD
***********************************************

.. doxygendefine:: CONFIG_STROLL_DLIST_MERGE_SORT_INSERT_THRESHOLD

CONFIG_STROLL_DLIST_SELECT_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_DLIST_SELECT_SORT

CONFIG_STROLL_FBMAP
*******************

.. doxygendefine:: CONFIG_STROLL_FBMAP

CONFIG_STROLL_LVSTR
*******************

.. doxygendefine:: CONFIG_STROLL_LVSTR

CONFIG_STROLL_POW2
******************

.. doxygendefine:: CONFIG_STROLL_POW2

.. _CONFIG_STROLL_UTEST:

CONFIG_STROLL_SLIST
*******************

.. doxygendefine:: CONFIG_STROLL_SLIST

CONFIG_STROLL_SLIST_BUBBLE_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_SLIST_BUBBLE_SORT

CONFIG_STROLL_SLIST_INSERT_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_SLIST_INSERT_SORT

CONFIG_STROLL_SLIST_MERGE_SORT
******************************

.. doxygendefine:: CONFIG_STROLL_SLIST_MERGE_SORT

CONFIG_STROLL_SLIST_MERGE_SORT_INSERT_THRESHOLD
***********************************************

.. doxygendefine:: CONFIG_STROLL_SLIST_MERGE_SORT_INSERT_THRESHOLD

CONFIG_STROLL_SLIST_SELECT_SORT
*******************************

.. doxygendefine:: CONFIG_STROLL_SLIST_SELECT_SORT

CONFIG_STROLL_UTEST
*******************

.. doxygendefine:: CONFIG_STROLL_UTEST

CONFIG_STROLL_VALGRIND
**********************

.. doxygendefine:: CONFIG_STROLL_VALGRIND

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
   
__export_hidden
***************

.. doxygendefine:: __export_hidden

__export_intern
***************

.. doxygendefine:: __export_intern

__export_protect
****************

.. doxygendefine:: __export_protect

__export_public
***************

.. doxygendefine:: __export_public

__leaf
******

.. doxygendefine:: __leaf

__nonull
********

.. doxygendefine:: __nonull

__noreturn
**********

.. doxygendefine:: __noreturn

__nothrow
*********

.. doxygendefine:: __nothrow

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

STROLL_BMAP_INIT_CLEAR32
************************

.. doxygendefine:: STROLL_BMAP_INIT_CLEAR32

STROLL_BMAP_INIT_CLEAR64
************************

.. doxygendefine:: STROLL_BMAP_INIT_CLEAR64

STROLL_BMAP_INIT_CLEARUL
************************

.. doxygendefine:: STROLL_BMAP_INIT_CLEARUL

STROLL_BMAP_INIT_SET
********************

.. doxygendefine:: STROLL_BMAP_INIT_SET

STROLL_BMAP_INIT_SET32
**********************

.. doxygendefine:: STROLL_BMAP_INIT_SET32

STROLL_BMAP_INIT_SET64
**********************

.. doxygendefine:: STROLL_BMAP_INIT_SET64

STROLL_BMAP_INIT_SETUL
**********************

.. doxygendefine:: STROLL_BMAP_INIT_SETUL

STROLL_CONCAT
*************

.. doxygendefine:: STROLL_CONCAT

STROLL_CONST_ABS
****************

.. doxygendefine:: STROLL_CONST_ABS

STROLL_CONST_MAX
****************

.. doxygendefine:: STROLL_CONST_MAX

STROLL_CONST_MIN
****************

.. doxygendefine:: STROLL_CONST_MIN

STROLL_DLIST_INIT
*****************

.. doxygendefine:: STROLL_DLIST_INIT

STROLL_LVSTR_INIT
*****************

.. doxygendefine:: STROLL_LVSTR_INIT

STROLL_LVSTR_INIT_LEND
**********************

.. doxygendefine:: STROLL_LVSTR_INIT_LEND

STROLL_LVSTR_INIT_NLEND
***********************

.. doxygendefine:: STROLL_LVSTR_INIT_NLEND

STROLL_LVSTR_INIT_NCEDE
***********************

.. doxygendefine:: STROLL_LVSTR_INIT_NCEDE

STROLL_LVSTR_LEN_MAX
********************

.. doxygendefine:: STROLL_LVSTR_LEN_MAX

STROLL_PREFETCH_ACCESS_RO
*************************

.. doxygendefine:: STROLL_PREFETCH_ACCESS_RO

STROLL_PREFETCH_ACCESS_RW
*************************

.. doxygendefine:: STROLL_PREFETCH_ACCESS_RW

STROLL_PREFETCH_LOCALITY_FAIR
*****************************

.. doxygendefine:: STROLL_PREFETCH_LOCALITY_FAIR

STROLL_PREFETCH_LOCALITY_HIGH
*****************************

.. doxygendefine:: STROLL_PREFETCH_LOCALITY_HIGH

STROLL_PREFETCH_LOCALITY_LOW
****************************

.. doxygendefine:: STROLL_PREFETCH_LOCALITY_LOW

STROLL_PREFETCH_LOCALITY_TMP
****************************

.. doxygendefine:: STROLL_PREFETCH_LOCALITY_TMP

STROLL_SLIST_INIT
*****************

.. doxygendefine:: STROLL_SLIST_INIT

STROLL_SLIST_NODE_INIT
**********************

.. doxygendefine:: STROLL_SLIST_NODE_INIT

STROLL_STRING
*************

.. doxygendefine:: STROLL_STRING

STROLL_UNIQ
***********

.. doxygendefine:: STROLL_UNIQ

stroll_array_nr
***************

.. doxygendefine:: stroll_array_nr

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

stroll_bmap_foreach_clear32
***************************

.. doxygendefine:: stroll_bmap_foreach_clear32

stroll_bmap_foreach_clear64
***************************

.. doxygendefine:: stroll_bmap_foreach_clear64

stroll_bmap_foreach_clearul
***************************

.. doxygendefine:: stroll_bmap_foreach_clearul

stroll_bmap_foreach_set
***********************

.. doxygendefine:: stroll_bmap_foreach_set

stroll_bmap_foreach_set32
*************************

.. doxygendefine:: stroll_bmap_foreach_set32

stroll_bmap_foreach_set64
*************************

.. doxygendefine:: stroll_bmap_foreach_set64

stroll_bmap_foreach_setul
*************************

.. doxygendefine:: stroll_bmap_foreach_setul

stroll_fbmap_foreach_clear()
****************************

.. doxygendefine:: stroll_fbmap_foreach_clear

stroll_dlist_entry
******************

.. doxygendefine:: stroll_dlist_entry

stroll_dlist_foreach_entry
**************************

.. doxygendefine:: stroll_dlist_foreach_entry

stroll_dlist_foreach_entry_safe
*******************************

.. doxygendefine:: stroll_dlist_foreach_entry_safe

stroll_dlist_foreach_node
*************************

.. doxygendefine:: stroll_dlist_foreach_node

stroll_dlist_foreach_node_safe
******************************

.. doxygendefine:: stroll_dlist_foreach_node_safe

stroll_dlist_foreach_prev_entry
*******************************

.. doxygendefine:: stroll_dlist_foreach_prev_entry

stroll_dlist_foreach_prev_entry_safe
************************************

.. doxygendefine:: stroll_dlist_foreach_prev_entry_safe

stroll_dlist_foreach_prev_node
******************************

.. doxygendefine:: stroll_dlist_foreach_prev_node

stroll_dlist_foreach_prev_node_safe
***********************************

.. doxygendefine:: stroll_dlist_foreach_prev_node_safe

stroll_dlist_next_entry
***********************

.. doxygendefine:: stroll_dlist_next_entry

stroll_dlist_prev_entry
***********************

.. doxygendefine:: stroll_dlist_prev_entry

stroll_fbmap_foreach_range_clear
********************************

.. doxygendefine:: stroll_fbmap_foreach_range_clear

stroll_fbmap_foreach_range_set
******************************

.. doxygendefine:: stroll_fbmap_foreach_range_set

stroll_fbmap_foreach_set
************************

.. doxygendefine:: stroll_fbmap_foreach_set

stroll_likely
*************

.. doxygendefine:: stroll_likely

stroll_min
**********

.. doxygendefine:: stroll_min

stroll_max
**********

.. doxygendefine:: stroll_max

stroll_prefetch
***************

.. doxygendefine:: stroll_prefetch

stroll_slist_entry
******************

.. doxygendefine:: stroll_slist_entry

stroll_slist_first_entry
************************

.. doxygendefine:: stroll_slist_first_entry

stroll_slist_foreach_node
*************************

.. doxygendefine:: stroll_slist_foreach_node

stroll_slist_foreach_entry
**************************

.. doxygendefine:: stroll_slist_foreach_entry

stroll_slist_last_entry
***********************

.. doxygendefine:: stroll_slist_last_entry

stroll_slist_next_entry
***********************

.. doxygendefine:: stroll_slist_next_entry

stroll_unlikely
***************

.. doxygendefine:: stroll_unlikely

Typedefs
--------

stroll_array_cmp_fn
*******************

.. doxygentypedef:: stroll_array_cmp_fn

stroll_slist_cmp_fn
*******************

.. doxygentypedef:: stroll_slist_cmp_fn

Structures
----------

stroll_dlist_node
*****************

.. doxygenstruct:: stroll_dlist_node

stroll_fbmap
************

.. doxygenstruct:: stroll_fbmap

stroll_fbmap_iter
*****************

.. doxygenstruct:: stroll_fbmap_iter

stroll_lvstr
************

.. doxygenstruct:: stroll_lvstr

stroll_slist
************

.. doxygenstruct:: stroll_slist

stroll_slist_node
*****************

.. doxygenstruct:: stroll_slist_node

Functions
---------

stroll_array_3wquick_sort
*************************

.. doxygenfunction:: stroll_array_3wquick_sort

stroll_array_bisect_search
**************************

.. doxygenfunction:: stroll_array_bisect_search

stroll_array_bubble_sort
************************

.. doxygenfunction:: stroll_array_bubble_sort

stroll_array_insert_inpsort_elem
********************************

.. doxygenfunction:: stroll_array_insert_inpsort_elem

stroll_array_insert_oopsort_elem
********************************

.. doxygenfunction:: stroll_array_insert_oopsort_elem

stroll_array_insert_sort
************************

.. doxygenfunction:: stroll_array_insert_sort

stroll_array_merge_sort
***********************

.. doxygenfunction:: stroll_array_merge_sort

stroll_array_quick_sort
***********************

.. doxygenfunction:: stroll_array_quick_sort

stroll_array_select_sort
************************

.. doxygenfunction:: stroll_array_select_sort

stroll_bmap_and
***************

.. doxygenfunction:: stroll_bmap_and

stroll_bmap_and32
*****************

.. doxygenfunction:: stroll_bmap_and32

stroll_bmap_and64
*****************

.. doxygenfunction:: stroll_bmap_and64

stroll_bmap_andul
*****************

.. doxygenfunction:: stroll_bmap_andul

stroll_bmap_and_range
*********************

.. doxygenfunction:: stroll_bmap_and_range

stroll_bmap_and_range32
***********************

.. doxygenfunction:: stroll_bmap_and_range32

stroll_bmap_and_range64
***********************

.. doxygenfunction:: stroll_bmap_and_range64

stroll_bmap_and_rangeul
***********************

.. doxygenfunction:: stroll_bmap_and_rangeul

stroll_bmap_clear
*****************

.. doxygenfunction:: stroll_bmap_clear

stroll_bmap_clear32
*******************

.. doxygenfunction:: stroll_bmap_clear32

stroll_bmap_clear64
*******************

.. doxygenfunction:: stroll_bmap_clear64

stroll_bmap_clearul
*******************

.. doxygenfunction:: stroll_bmap_clearul

stroll_bmap_clear_all
*********************

.. doxygenfunction:: stroll_bmap_clear_all

stroll_bmap_clear_all32
***********************

.. doxygenfunction:: stroll_bmap_clear_all32

stroll_bmap_clear_all64
***********************

.. doxygenfunction:: stroll_bmap_clear_all64

stroll_bmap_clear_allul
***********************

.. doxygenfunction:: stroll_bmap_clear_allul

stroll_bmap_clear_mask
**********************

.. doxygenfunction:: stroll_bmap_clear_mask

stroll_bmap_clear_mask32
************************

.. doxygenfunction:: stroll_bmap_clear_mask32

stroll_bmap_clear_mask64
************************

.. doxygenfunction:: stroll_bmap_clear_mask64

stroll_bmap_clear_maskul
************************

.. doxygenfunction:: stroll_bmap_clear_maskul

stroll_bmap_clear_range
***********************

.. doxygenfunction:: stroll_bmap_clear_range

stroll_bmap_clear_range32
*************************

.. doxygenfunction:: stroll_bmap_clear_range32

stroll_bmap_clear_range64
*************************

.. doxygenfunction:: stroll_bmap_clear_range64

stroll_bmap_clear_rangeul
*************************

.. doxygenfunction:: stroll_bmap_clear_rangeul

stroll_bmap_hweight
*******************

.. doxygenfunction:: stroll_bmap_hweight

stroll_bmap_hweight32
*********************

.. doxygenfunction:: stroll_bmap_hweight32

stroll_bmap_hweight64
*********************

.. doxygenfunction:: stroll_bmap_hweight64

stroll_bmap_hweightul
*********************

.. doxygenfunction:: stroll_bmap_hweightul

stroll_bmap_mask
****************

.. doxygenfunction:: stroll_bmap_mask

stroll_bmap_mask32
******************

.. doxygenfunction:: stroll_bmap_mask32

stroll_bmap_mask64
******************

.. doxygenfunction:: stroll_bmap_mask64

stroll_bmap_maskul
******************

.. doxygenfunction:: stroll_bmap_maskul

stroll_bmap_or
**************

.. doxygenfunction:: stroll_bmap_or

stroll_bmap_or32
****************

.. doxygenfunction:: stroll_bmap_or32

stroll_bmap_or64
****************

.. doxygenfunction:: stroll_bmap_or64

stroll_bmap_orul
****************

.. doxygenfunction:: stroll_bmap_orul

stroll_bmap_or_range
********************

.. doxygenfunction:: stroll_bmap_or_range

stroll_bmap_or_range32
**********************

.. doxygenfunction:: stroll_bmap_or_range32

stroll_bmap_or_range64
**********************

.. doxygenfunction:: stroll_bmap_or_range64

stroll_bmap_or_rangeul
**********************

.. doxygenfunction:: stroll_bmap_or_rangeul

stroll_bmap_set
***************

.. doxygenfunction:: stroll_bmap_set

stroll_bmap_set32
*****************

.. doxygenfunction:: stroll_bmap_set32

stroll_bmap_set64
*****************

.. doxygenfunction:: stroll_bmap_set64

stroll_bmap_setul
*****************

.. doxygenfunction:: stroll_bmap_setul

stroll_bmap_set_all
*******************

.. doxygenfunction:: stroll_bmap_set_all

stroll_bmap_set_all32
*********************

.. doxygenfunction:: stroll_bmap_set_all32

stroll_bmap_set_all64
*********************

.. doxygenfunction:: stroll_bmap_set_all64

stroll_bmap_set_allul
*********************

.. doxygenfunction:: stroll_bmap_set_allul

stroll_bmap_set_mask
********************

.. doxygenfunction:: stroll_bmap_set_mask

stroll_bmap_set_mask32
**********************

.. doxygenfunction:: stroll_bmap_set_mask32

stroll_bmap_set_mask64
**********************

.. doxygenfunction:: stroll_bmap_set_mask64

stroll_bmap_set_maskul
**********************

.. doxygenfunction:: stroll_bmap_set_maskul

stroll_bmap_set_range
*********************

.. doxygenfunction:: stroll_bmap_set_range

stroll_bmap_set_range32
***********************

.. doxygenfunction:: stroll_bmap_set_range32

stroll_bmap_set_range64
***********************

.. doxygenfunction:: stroll_bmap_set_range64

stroll_bmap_set_rangeul
***********************

.. doxygenfunction:: stroll_bmap_set_rangeul

stroll_bmap_setup_clear
***********************

.. doxygenfunction:: stroll_bmap_setup_clear

stroll_bmap_setup_clear32
*************************

.. doxygenfunction:: stroll_bmap_setup_clear32

stroll_bmap_setup_clear64
*************************

.. doxygenfunction:: stroll_bmap_setup_clear64

stroll_bmap_setup_clearul
*************************

.. doxygenfunction:: stroll_bmap_setup_clearul

stroll_bmap_setup_set
*********************

.. doxygenfunction:: stroll_bmap_setup_set

stroll_bmap_test
****************

.. doxygenfunction:: stroll_bmap_test

stroll_bmap_test32
******************

.. doxygenfunction:: stroll_bmap_test32

stroll_bmap_test64
******************

.. doxygenfunction:: stroll_bmap_test64

stroll_bmap_testul
******************

.. doxygenfunction:: stroll_bmap_testul

stroll_bmap_test_all
********************

.. doxygenfunction:: stroll_bmap_test_all

stroll_bmap_test_all32
**********************

.. doxygenfunction:: stroll_bmap_test_all32

stroll_bmap_test_all64
**********************

.. doxygenfunction:: stroll_bmap_test_all64

stroll_bmap_test_allul
**********************

.. doxygenfunction:: stroll_bmap_test_allul

stroll_bmap_test_mask
*********************

.. doxygenfunction:: stroll_bmap_test_mask

stroll_bmap_test_mask32
***********************

.. doxygenfunction:: stroll_bmap_test_mask32

stroll_bmap_test_mask64
***********************

.. doxygenfunction:: stroll_bmap_test_mask64

stroll_bmap_test_maskul
***********************

.. doxygenfunction:: stroll_bmap_test_maskul

stroll_bmap_test_range
**********************

.. doxygenfunction:: stroll_bmap_test_range

stroll_bmap_test_range32
************************

.. doxygenfunction:: stroll_bmap_test_range32

stroll_bmap_test_range64
************************

.. doxygenfunction:: stroll_bmap_test_range64

stroll_bmap_test_rangeul
************************

.. doxygenfunction:: stroll_bmap_test_rangeul

stroll_bmap_toggle
******************

.. doxygenfunction:: stroll_bmap_toggle

stroll_bmap_toggle32
********************

.. doxygenfunction:: stroll_bmap_toggle32

stroll_bmap_toggle64
********************

.. doxygenfunction:: stroll_bmap_toggle64

stroll_bmap_toggleul
********************

.. doxygenfunction:: stroll_bmap_toggleul

stroll_bmap_toggle_all
**********************

.. doxygenfunction:: stroll_bmap_toggle_all

stroll_bmap_toggle_all32
************************

.. doxygenfunction:: stroll_bmap_toggle_all32

stroll_bmap_toggle_all64
************************

.. doxygenfunction:: stroll_bmap_toggle_all64

stroll_bmap_toggle_allul
************************

.. doxygenfunction:: stroll_bmap_toggle_allul

stroll_bmap_toggle_mask
***********************

.. doxygenfunction:: stroll_bmap_toggle_mask

stroll_bmap_toggle_mask32
*************************

.. doxygenfunction:: stroll_bmap_toggle_mask32

stroll_bmap_toggle_mask64
*************************

.. doxygenfunction:: stroll_bmap_toggle_mask64

stroll_bmap_toggle_maskul
*************************

.. doxygenfunction:: stroll_bmap_toggle_maskul

stroll_bmap_toggle_range
************************

.. doxygenfunction:: stroll_bmap_toggle_range

stroll_bmap_toggle_range32
**************************

.. doxygenfunction:: stroll_bmap_toggle_range32

stroll_bmap_toggle_range64
**************************

.. doxygenfunction:: stroll_bmap_toggle_range64

stroll_bmap_toggle_rangeul
**************************

.. doxygenfunction:: stroll_bmap_toggle_rangeul

stroll_bmap_xor
***************

.. doxygenfunction:: stroll_bmap_xor

stroll_bmap_xor32
*****************

.. doxygenfunction:: stroll_bmap_xor32

stroll_bmap_xor64
*****************

.. doxygenfunction:: stroll_bmap_xor64

stroll_bmap_xorul
*****************

.. doxygenfunction:: stroll_bmap_xorul

stroll_bmap_xor_range
*********************

.. doxygenfunction:: stroll_bmap_xor_range

stroll_bmap_xor_range32
***********************

.. doxygenfunction:: stroll_bmap_xor_range32

stroll_bmap_xor_range64
***********************

.. doxygenfunction:: stroll_bmap_xor_range64

stroll_bmap_xor_rangeul
***********************

.. doxygenfunction:: stroll_bmap_xor_rangeul

stroll_bmap_setup_set32
***********************

.. doxygenfunction:: stroll_bmap_setup_set32

stroll_bmap_setup_set64
***********************

.. doxygenfunction:: stroll_bmap_setup_set64

stroll_bmap_setup_setul
***********************

.. doxygenfunction:: stroll_bmap_setup_setul

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

stroll_bops_ffc32
*****************

.. doxygenfunction:: stroll_bops_ffc32

stroll_bops_ffc64
*****************

.. doxygenfunction:: stroll_bops_ffc64

stroll_bops_ffcul
*****************

.. doxygenfunction:: stroll_bops_ffcul

stroll_bops_ffs32
*****************

.. doxygenfunction:: stroll_bops_ffs32

stroll_bops_ffs64
*****************

.. doxygenfunction:: stroll_bops_ffs64

stroll_bops_ffsul
*****************

.. doxygenfunction:: stroll_bops_ffsul

stroll_bops_fls32
*****************

.. doxygenfunction:: stroll_bops_fls32

stroll_bops_fls64
*****************

.. doxygenfunction:: stroll_bops_fls64

stroll_bops_flsul
*****************

.. doxygenfunction:: stroll_bops_flsul

stroll_bops_hweight32
*********************

.. doxygenfunction:: stroll_bops_hweight32

stroll_bops_hweight64
*********************

.. doxygenfunction:: stroll_bops_hweight64

stroll_bops_hweightul
*********************

.. doxygenfunction:: stroll_bops_hweightul

stroll_dlist_append
*******************

.. doxygenfunction:: stroll_dlist_append

stroll_dlist_bubble_sort
************************

.. doxygenfunction:: stroll_dlist_bubble_sort

stroll_dlist_dqueue_back
************************

.. doxygenfunction:: stroll_dlist_dqueue_back

stroll_dlist_dqueue_back_init
*****************************

.. doxygenfunction:: stroll_dlist_dqueue_back_init

stroll_dlist_dqueue_front
*************************

.. doxygenfunction:: stroll_dlist_dqueue_front

stroll_dlist_dqueue_front_init
******************************

.. doxygenfunction:: stroll_dlist_dqueue_front_init

stroll_dlist_embed_after
************************

.. doxygenfunction:: stroll_dlist_embed_after

stroll_dlist_embed_before
*************************

.. doxygenfunction:: stroll_dlist_embed_before

stroll_dlist_empty
******************

.. doxygenfunction:: stroll_dlist_empty

stroll_dlist_init
*****************

.. doxygenfunction:: stroll_dlist_init

stroll_dlist_insert
*******************

.. doxygenfunction:: stroll_dlist_insert

stroll_dlist_insert_sort
************************

.. doxygenfunction:: stroll_dlist_insert_sort

stroll_dlist_merge_sort
***********************

.. doxygenfunction:: stroll_dlist_merge_sort

stroll_dlist_merge_presort
**************************

.. doxygenfunction:: stroll_dlist_merge_presort

stroll_dlist_move_after
***********************

.. doxygenfunction:: stroll_dlist_move_after

stroll_dlist_move_before
************************

.. doxygenfunction:: stroll_dlist_move_before

stroll_dlist_next
*****************

.. doxygenfunction:: stroll_dlist_next

stroll_dlist_nqueue_back
************************

.. doxygenfunction:: stroll_dlist_nqueue_back

stroll_dlist_nqueue_front
*************************

.. doxygenfunction:: stroll_dlist_nqueue_front

stroll_dlist_prev
*****************

.. doxygenfunction:: stroll_dlist_prev

stroll_dlist_remove
*******************

.. doxygenfunction:: stroll_dlist_remove

stroll_dlist_remove_init
************************

.. doxygenfunction:: stroll_dlist_remove_init

stroll_dlist_replace
********************
.. doxygenfunction:: stroll_dlist_replace

stroll_dlist_replace_init
*************************
.. doxygenfunction:: stroll_dlist_replace_init

stroll_dlist_select_sort
************************

.. doxygenfunction:: stroll_dlist_select_sort

stroll_dlist_splice_after
*************************

.. doxygenfunction:: stroll_dlist_splice_after

stroll_dlist_splice_before
**************************

.. doxygenfunction:: stroll_dlist_splice_before

stroll_dlist_withdraw
*********************

.. doxygenfunction:: stroll_dlist_withdraw

stroll_fbmap_clear
******************

.. doxygenfunction:: stroll_fbmap_clear

stroll_fbmap_clear_all
**********************

.. doxygenfunction:: stroll_fbmap_clear_all

stroll_fbmap_fini
*****************

.. doxygenfunction:: stroll_fbmap_fini

stroll_fbmap_hweight
********************

.. doxygenfunction:: stroll_fbmap_hweight

stroll_fbmap_init_clear
***********************

.. doxygenfunction:: stroll_fbmap_init_clear

stroll_fbmap_init_dup
*********************

.. doxygenfunction:: stroll_fbmap_init_dup

stroll_fbmap_init_set
*********************

.. doxygenfunction:: stroll_fbmap_init_set

stroll_fbmap_nr
***************

.. doxygenfunction:: stroll_fbmap_nr

stroll_fbmap_set
****************

.. doxygenfunction:: stroll_fbmap_set

stroll_fbmap_set_all
********************

.. doxygenfunction:: stroll_fbmap_set_all

stroll_fbmap_test
*****************

.. doxygenfunction:: stroll_fbmap_test

stroll_fbmap_test_range
***********************

.. doxygenfunction:: stroll_fbmap_test_range

stroll_fbmap_test_all
*********************

.. doxygenfunction:: stroll_fbmap_test_all

stroll_fbmap_toggle
*******************

.. doxygenfunction:: stroll_fbmap_toggle

stroll_fbmap_toggle_all
***********************

.. doxygenfunction:: stroll_fbmap_toggle_all

stroll_lvstr_cede
*****************

.. doxygenfunction:: stroll_lvstr_cede

stroll_lvstr_cstr
*****************

.. doxygenfunction:: stroll_lvstr_cstr

.. _sect-api-stroll_lvstr_drop:

stroll_lvstr_drop
*****************

.. doxygenfunction:: stroll_lvstr_drop

stroll_lvstr_dup
****************

.. doxygenfunction:: stroll_lvstr_dup

.. _sect-api-stroll_lvstr_fini:

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

stroll_pow2_low
***************

.. doxygenfunction:: stroll_pow2_low

stroll_pow2_low32
*****************

.. doxygenfunction:: stroll_pow2_low32

stroll_pow2_low64
*******************

.. doxygenfunction:: stroll_pow2_low64

stroll_pow2_lowul
*****************

.. doxygenfunction:: stroll_pow2_lowul

stroll_pow2_up
**************

.. doxygenfunction:: stroll_pow2_up

stroll_pow2_up32
****************

.. doxygenfunction:: stroll_pow2_up32

stroll_pow2_up64
****************

.. doxygenfunction:: stroll_pow2_up64

stroll_pow2_upul
****************

.. doxygenfunction:: stroll_pow2_upul

stroll_slist_append
*******************

.. doxygenfunction:: stroll_slist_append

stroll_slist_bubble_sort
*************************

.. doxygenfunction:: stroll_slist_bubble_sort

stroll_slist_dqueue_front
*************************

.. doxygenfunction:: stroll_slist_dqueue_front

stroll_slist_embed
******************

.. doxygenfunction:: stroll_slist_embed

stroll_slist_empty
******************

.. doxygenfunction:: stroll_slist_empty

stroll_slist_first
******************

.. doxygenfunction:: stroll_slist_first

stroll_slist_head
*****************

.. doxygenfunction:: stroll_slist_head

stroll_slist_init
*****************

.. doxygenfunction:: stroll_slist_init

stroll_slist_insert_sort
************************

.. doxygenfunction:: stroll_slist_insert_sort

stroll_slist_last
*****************

.. doxygenfunction:: stroll_slist_last

stroll_slist_merge_sort
***********************

.. doxygenfunction:: stroll_slist_merge_sort

stroll_slist_merge_presort
**************************

.. doxygenfunction:: stroll_slist_merge_presort

stroll_slist_move
*****************

.. doxygenfunction:: stroll_slist_move

stroll_slist_next
*****************

.. doxygenfunction:: stroll_slist_next

stroll_slist_nqueue_back
************************

.. doxygenfunction:: stroll_slist_nqueue_back

stroll_slist_nqueue_front
*************************

.. doxygenfunction:: stroll_slist_nqueue_front

stroll_slist_remove
*******************

.. doxygenfunction:: stroll_slist_remove

stroll_slist_select_sort
************************

.. doxygenfunction:: stroll_slist_select_sort

stroll_slist_splice
*******************

.. doxygenfunction:: stroll_slist_splice

stroll_slist_withdraw
*********************

.. doxygenfunction:: stroll_slist_withdraw
