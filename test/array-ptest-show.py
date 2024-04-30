#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

import sys
import io
import csv
import matplotlib
matplotlib.use('GTK3Agg')
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np

# Skip the first 6 lines and comment lines starting with the '#' character.
# Remove duplicate spaces so that the CSV reader object may use space as the
# delimiter.
def array_ptest_xform_data(csvfile: io.TextIOWrapper) -> list[str]:
    return list(map(lambda line: ' '.join(line.split()),
                filter(lambda row: row[0]!='#' and row[0]!='-',
                       csvfile.readlines()[6:])))


def array_ptest_load(path: str) -> list[list()]:
    dt = np.dtype([('algo', object),
                   ('nr', int),
                   ('order', int),
                   ('size', int),
                   ('nsec', int)])
    with open(path, newline='') as csvfile:
        reader = csv.DictReader(array_ptest_xform_data(csvfile), delimiter=' ')
        return np.array([tuple(row.values()) for row in reader], dtype = dt)


def array_ptest_plot(results,
                     algo: str,
                     order: int,
                     size: int,
                     axe: matplotlib.axes.Axes,
                     label: str) -> None:
    res = results[(results['order'] == order) & \
                  (results['size'] == size) & \
                  (results['algo'] == algo)]
    res = np.sort(res, order = 'nr')
    axe.plot(res['nr'],
             res['nsec'] / 1000,
             label = label,
             marker = 'o')


def array_ptest_setup_plot(title: str,
                           axe: matplotlib.axes.Axes,
                           showx: bool,
                           showy: bool) -> None:
    axe.set_title(title)
    if showx:
        axe.set_xlabel('#Samples')
    axe.set_xscale('log', base = 2)
    if showy:
        axe.set_ylabel('Time [uSec]')
    axe.set_yscale('log')
    axe.yaxis.set_major_locator(ticker.LogLocator())
    axe.yaxis.set_minor_locator(ticker.LogLocator(subs = 'auto'))
    axe.tick_params(which = 'both',
                    bottom = True,
                    labelbottom = showx,
                    top = True,
                    labeltop = False,
                    left = True,
                    labelleft = showy,
                    right = True,
                    labelright = False,
                    direction = 'in')


def array_ptest_finish_plot(axe: matplotlib.axes.Axes) -> None:
    axe.grid(linestyle = '--', alpha = 0.4)
    axe.legend(loc = 'upper left')


def array_ptest_plot_sizes(results,
                           algo: str,
                           order: int,
                           sizes: list[int],
                           title: str,
                           axe: matplotlib.axes.Axes,
                           showx: bool,
                           showy: bool) -> None:

    array_ptest_setup_plot(title, axe, showx, showy)
    for s in sizes:
        array_ptest_plot(results,
                         algo,
                         order,
                         s,
                         axe,
                         "{} Bytes".format(s))
    array_ptest_finish_plot(axe)


def array_ptest_plot_algo_byorder(results,
                                  algo: str,
                                  orders: list[int] | None = None,
                                  sizes: list[int] | None = None) -> None:
    if orders is None:
        orders = np.unique(results['order'])
    if sizes is None:
        sizes = np.unique(results['size'])

    cols = 3
    rows = int((len(orders) + cols - 1) / cols)
    fig, axes = plt.subplots(nrows = rows,
                             ncols = cols,
                             sharex = True,
                             sharey = True)
    fig.suptitle("{} algorithm".format(algo))
    indx = 0
    for r in range(0, rows):
        for c in range(0, cols):
            if indx >= len(orders):
                break
            showx = r == (rows - 1)
            showy = c == 0
            ax = axes[r, c] if axes.ndim == 2 else axes[c]
            array_ptest_plot_sizes(results,
                                   algo,
                                   orders[indx],
                                   sizes,
                                   "{}% order".format(orders[indx]),
                                   ax,
                                   showx,
                                   showy)
            indx += 1


def array_ptest_plot_order_byalgo(results,
                                  order: int,
                                  algos: list[str] | None = None,
                                  sizes: list[int] | None = None) -> None:
    if algos is None:
        algos = np.unique(results['algo'])
    if sizes is None:
        sizes = np.unique(results['size'])

    cols = 3
    rows = int((len(algos) + cols - 1) / cols)
    fig, axes = plt.subplots(nrows = rows,
                             ncols = cols,
                             sharex = True,
                             sharey = True)
    fig.suptitle("{}% order".format(order))
    indx = 0
    for r in range(0, rows):
        for c in range(0, cols):
            if indx >= len(algos):
                break
            showx = r == (rows - 1)
            showy = c == 0
            ax = axes[r, c] if axes.ndim == 2 else axes[c]
            array_ptest_plot_sizes(results,
                                   algos[indx],
                                   order,
                                   sizes,
                                   algos[indx],
                                   ax,
                                   showx,
                                   showy)
            indx += 1


def array_ptest_plot_algos(results,
                           order: int,
                           size: int,
                           algos: list[str],
                           title: str,
                           axe: matplotlib.axes.Axes,
                           showx: bool,
                           showy: bool) -> None:
    array_ptest_setup_plot(title, axe, showx, showy)
    for a in algos:
        array_ptest_plot(results, a, order, size, axe, a)
    array_ptest_finish_plot(axe)


def array_ptest_plot_order_bysize(results,
                                  order: int,
                                  algos: list[str] | None = None,
                                  sizes: list[int] | None = None) -> None:
    if algos is None:
        algos = np.unique(results['algo'])
    if sizes is None:
        sizes = np.unique(results['size'])

    cols = 3
    rows = int((len(sizes) + cols - 1) / cols)
    fig, axes = plt.subplots(nrows = rows,
                             ncols = cols,
                             sharex = True,
                             sharey = True)
    fig.suptitle("{}% order".format(order))
    indx = 0
    for r in range(0, rows):
        for c in range(0, cols):
            if indx >= len(sizes):
                break
            showx = r == (rows - 1)
            showy = c == 0
            ax = axes[r, c] if axes.ndim == 2 else axes[c]
            array_ptest_plot_algos(results,
                                   order,
                                   sizes[indx],
                                   algos,
                                   "{} Bytes".format(sizes[indx]),
                                   ax,
                                   showx,
                                   showy)
            indx += 1


def array_ptest_plot_size_byorder(results,
                                  size: int,
                                  algos: list[str] | None = None,
                                  orders: list[int] | None = None) -> None:
    if algos is None:
        algos = np.unique(results['algo'])
    if orders is None:
        orders = np.unique(results['order'])

    cols = 3
    rows = int((len(orders) + cols - 1) / cols)
    fig, axes = plt.subplots(nrows = rows,
                             ncols = cols,
                             sharex = True,
                             sharey = True)
    fig.suptitle("{} Bytes".format(size))
    indx = 0
    for r in range(0, rows):
        for c in range(0, cols):
            if indx >= len(orders):
                break
            showx = r == (rows - 1)
            showy = c == 0
            ax = axes[r, c] if axes.ndim == 2 else axes[c]
            array_ptest_plot_algos(results,
                                   orders[indx],
                                   size,
                                   algos,
                                   "{}% order".format(orders[indx]),
                                   ax,
                                   showx,
                                   showy)
            indx += 1


def array_ptest_plot_orders(results,
                            algo: str,
                            size: int,
                            orders: list[int],
                            title: str,
                            axe: matplotlib.axes.Axes,
                            showx: bool,
                            showy: bool) -> None:

    array_ptest_setup_plot(title, axe, showx, showy)
    for o in orders:
        array_ptest_plot(results,
                         algo,
                         o,
                         size,
                         axe,
                         "{}% order".format(o))
    array_ptest_finish_plot(axe)


def array_ptest_plot_algo_bysize(results,
                                 algo: str,
                                 orders: list[int] | None = None,
                                 sizes: list[int] | None = None) -> None:
    if orders is None:
        orders = np.unique(results['order'])
    if sizes is None:
        sizes = np.unique(results['size'])

    cols = 3
    rows = int((len(sizes) + cols - 1) / cols)
    fig, axes = plt.subplots(nrows = rows,
                             ncols = cols,
                             sharex = True,
                             sharey = True)
    fig.suptitle("{} algorithm".format(algo))
    indx = 0
    for r in range(0, rows):
        for c in range(0, cols):
            if indx >= len(sizes):
                break
            showx = r == (rows - 1)
            showy = c == 0
            ax = axes[r, c] if axes.ndim == 2 else axes[c]
            array_ptest_plot_orders(results,
                                    algo,
                                    sizes[indx],
                                    orders,
                                    "{} Bytes".format(sizes[indx]),
                                    ax,
                                    showx,
                                    showy)
            indx += 1


def array_ptest_plot_size_byalgo(results,
                                 size: int,
                                 algos: list[str] | None = None,
                                 orders: list[int] | None = None) -> None:
    if algos is None:
        algos = np.unique(results['algo'])
    if orders is None:
        orders = np.unique(results['order'])

    cols = 3
    rows = int((len(algos) + cols - 1) / cols)
    fig, axes = plt.subplots(nrows = rows,
                             ncols = cols,
                             sharex = True,
                             sharey = True)
    fig.suptitle("{} Bytes".format(size))
    indx = 0
    for r in range(0, rows):
        for c in range(0, cols):
            if indx >= len(algos):
                break
            showx = r == (rows - 1)
            showy = c == 0
            ax = axes[r, c] if axes.ndim == 2 else axes[c]
            array_ptest_plot_orders(results,
                                    algos[indx],
                                    size,
                                    orders,
                                    algos[indx],
                                    ax,
                                    showx,
                                    showy)
            indx += 1


results = array_ptest_load(sys.argv[1])

#array_ptest_plot_algo_byorder(results, 'merge')
#array_ptest_plot_algo_bysize(results, 'quick')

#array_ptest_plot_order_byalgo(results, 0)
#array_ptest_plot_order_bysize(results, 25)

#array_ptest_plot_size_byorder(results, 8)
array_ptest_plot_size_byalgo(results, 64)

plt.show()
