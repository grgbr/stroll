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
    dt = np.dtype([('algo', np.object),
                   ('nr', np.int),
                   ('order', np.int),
                   ('size', np.int),
                   ('nsec', np.int)])
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


def array_ptest_plot_byalgo(results,
                            algo: str,
                            axe: matplotlib.axes.Axes,
                            showx: bool,
                            showy: bool) -> None:
    orders = np.unique(results['order'])
    
    axe.set_title('{} algorithm'.format(algo))
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
     
    for o in orders:
        array_ptest_plot(results, algo, o, 16, axe, '{}% ordering'.format(o))

    axe.grid(linestyle = '--', alpha = 0.4)
    axe.legend(loc = 'upper left')


def array_ptest_plot_byorder(results,
                             order: int,
                             axe: matplotlib.axes.Axes,
                             showx: bool,
                             showy: bool) -> None:
    algos = np.unique(results['algo'])
    
    axe.set_title('{}% ordering'.format(order))
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
    
    for a in algos:
        array_ptest_plot(results, a, order, 16, axe, a)

    axe.grid(linestyle = '--', alpha = 0.4)
    axe.legend(loc = 'upper left')


def array_ptest_plot_algos(results) -> None:
    algos = np.unique(results['algo'])
    cols = 2
    rows = int((len(algos) + cols - 1) / cols)
    fig, axes = plt.subplots(nrows = rows,
                             ncols = cols,
                             sharex = True,
                             sharey = True)
    indx = 0
    for r in range(0, rows):
        for c in range(0, cols):
            if indx >= len(algos):
                break
            showx = r == (rows - 1)
            showy = c == 0
            ax = axes[r, c] if axes.ndim == 2 else axes[c]
            array_ptest_plot_byalgo(results, algos[indx], ax, showx, showy)
            indx += 1


def array_ptest_plot_orders(results) -> None:
    orders = np.unique(results['order'])
    cols = 3
    rows = int((len(orders) + cols - 1) / cols)
    fig, axes = plt.subplots(nrows = rows,
                             ncols = cols,
                             sharex = True,
                             sharey = True)
    indx = 0
    for r in range(0, rows):
        for c in range(0, cols):
            if indx >= len(orders):
                break
            showx = r == (rows - 1)
            showy = c == 0
            ax = axes[r, c] if axes.ndim == 2 else axes[c]
            array_ptest_plot_byorder(results, orders[indx], ax, showx, showy)
            indx += 1

results = array_ptest_load(sys.argv[1])
array_ptest_plot_algos(results)
array_ptest_plot_orders(results)
plt.show()
