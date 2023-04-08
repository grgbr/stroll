.. include:: _cdefs.rst

.. _breathe:              https://github.com/breathe-doc/breathe
.. _cmocka:               https://cmocka.org/
.. _ebuild:               https://github.com/grgbr/ebuild/
.. |eBuild|               replace:: `eBuild <ebuild_>`_
.. |eBuild User Guide|    replace:: :external+ebuild:doc:`eBuild User Guide <user>`
.. |eBuild Prerequisites| replace:: :external+ebuild:ref:`eBuild Prerequisites <sect-user-prerequisites>`
.. |Configure|            replace:: :external+ebuild:ref:`sect-user-configure`
.. |Build|                replace:: :external+ebuild:ref:`sect-user-build`
.. |Install|              replace:: :external+ebuild:ref:`sect-user-install`
.. |Staged install|       replace:: :external+ebuild:ref:`sect-user-staged-install`
.. |BUILDDIR|             replace:: :external+ebuild:ref:`var-builddir`
.. |PREFIX|               replace:: :external+ebuild:ref:`var-prefix`
.. |CROSS_COMPILE|        replace:: :external+ebuild:ref:`var-cross_compile`
.. |DESTDIR|              replace:: :external+ebuild:ref:`var-destdir`

Overview
========

This guide mainly focuses upon the construction process required to install
Stroll.

Stroll's build logic is based upon |eBuild|. In addition to the build process
description explained below, you may refer to the |eBuild User Guide|
for further detailed informations.

Prerequisites
=============

In addition to the standard |eBuild Prerequisites|, no particular packages are
required to build Stroll.

Optionally, you will need cmocka_ at build time and at runtime when unit testing
suite is enabled (see CONFIG_STROLL_UTEST_).

Optionally, you will need multiple packages installed to build the
documentation. In addition to packages listed into |eBuild Prerequisites|,
Stroll's documentation generation process requires breathe_.

Getting help
============

From Stroll source tree root, enter:

.. code-block:: console

   $ make help

Also note that a more detailed help message is available:

.. code-block:: console

   $ make help-full

Refer to :external+ebuild:ref:`eBuild help target <target-help>` and
:external+ebuild:ref:`eBuild help-full target <target-help-full>` for further
informations.

The :external+ebuild:ref:`eBuild Troubleshooting <sect-user-troubleshooting>`
section also contains valuable informations.

Build Workflow
==============

As mentioned earlier, Stroll's build logic is based on |eBuild|, a |GNU make|
based build system. To build and install Stroll, the typical workflow is:

#. Prepare and collect workflow requirements,
#. |Configure| the construction logic,
#. |Build| programs, libraries, documentation, etc.,
#. |Install| components, copying files previously built to
   system-wide directories

Alternatively, you may replace the last step mentioned above with a |Staged
Install|. You will find below a **quick starting guide** showing how to build
Stroll.

.. topic:: Preparation phase

The overall :external+ebuild:ref:`eBuild Workflow <sect-user-workflow>` is
customizable thanks to multiple :command:`make` variable settings. You should
adjust values according to your specific needs.

Most of the time, setting |BUILDDIR|, |PREFIX|, |CROSS_COMPILE| is enough.
You should also set the :envvar:`PATH` environment variable according to the set
of tools required by the build process.

Optionally, you may set ``EXTRA_CFLAGS`` and ``EXTRA_LDFLAGS`` variables to
give the compiler and linker additional flags respectively.

Refer to :external+ebuild:ref:`eBuild Tools <sect-user-tools>` and
:external+ebuild:ref:`eBuild Variables <sect-user-variables>` for further
informations.

.. _workflow-configure-phase:
   
.. topic:: Configure phase

To begin with, |Configure| the build process interactively by running the
:external+ebuild:ref:`eBuild menuconfig target <target-menuconfig>`:

.. code-block:: console

   $ make menuconfig BUILDDIR=$HOME/build/stroll

.. topic:: Build phase

Now, proceed to the |Build| phase and compile / link programs, libraries, etc.
by running the :external+ebuild:ref:`eBuild build target <target-build>`:

.. code-block:: console

   $ make build BUILDDIR=$HOME/build/stroll PREFIX=/usr
 
.. topic:: Install phase

Finally, |Install| programs, libraries, etc.: by running the
:external+ebuild:ref:`eBuild install target <target-install>`:

.. code-block:: console

   $ make install BUILDDIR=$HOME/build/stroll PREFIX=/usr
 
.. topic:: Alternative staged install phase

Alternatively, perform a |Staged install| by specifying the |DESTDIR| variable
instead:

.. code-block:: console

   $ make install BUILDDIR=$HOME/build/stroll PREFIX=/usr DESTDIR=$HOME/staging

.. topic:: Documentation generation

You may generate Stroll documentation by running the `doc` target like so:

.. code-block:: console

   $ make doc BUILDDIR=$HOME/build/stroll PREFIX=/usr

You may further install generated documentation by running the `install-doc`
target:

.. code-block:: console

   $ make install-doc BUILDDIR=$HOME/build/stroll PREFIX=/usr DESTDIR=$HOME/staging

Finally, you may find lots of usefull informations into the
:external+ebuild:ref:`Reference <sect-user-reference>` section of the |eBuild
User Guide|.

Build configuration
===================

At :ref:`Build configuration time <workflow-configure-phase>`, multiple build
options are available to customize final Stroll build. These are listed below.

.. _CONFIG_STROLL_ASSERT:
   
CONFIG_STROLL_ASSERT
--------------------

Build stroll library assertion framework exposing a programmatic assertion
API for use by client code.

.. _CONFIG_STROLL_ASSERT_API:

CONFIG_STROLL_ASSERT_API
------------------------

Build stroll library with high-level API assertions enabled to ensure that API
functions exposed by Stroll will perform full verification of arguments
consistency.

.. _CONFIG_STROLL_ASSERT_INTERN:
   
CONFIG_STROLL_ASSERT_INTERN
---------------------------

Build stroll library with internal assertions enabled to check for internal
Stroll behavioral consistency.

.. _CONFIG_STROLL_UTEST:
   
CONFIG_STROLL_UTEST
-------------------

Build stroll library unit test suite.

.. _CONFIG_STROLL_VALGRIND:
   
CONFIG_STROLL_VALGRIND
----------------------

Build stroll library unit test suite with Valgrind support enabled.

.. _CONFIG_STROLL_BOPS:
   
CONFIG_STROLL_BOPS
------------------

Build stroll library bit operations framework exposing functions to perform
bitwise operations upon words in memory.

.. _CONFIG_STROLL_BMAP:

CONFIG_STROLL_BMAP
------------------

Build stroll library with bitmap framework exposing functions to perform usual
operations over bitmaps stored memory words.

.. _CONFIG_STROLL_LVSTR:

CONFIG_STROLL_LVSTR
-------------------

Build stroll library with length-value string framework.
