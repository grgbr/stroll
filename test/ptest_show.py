#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

from typing import Iterator, Callable, Optional, cast, Union
import argparse as argp
import _io
import csv
import math
import numpy as np
import matplotlib
matplotlib.use('GTK3Agg')
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

# Skip the first 8 lines and comment lines starting with the '#' character.
# Remove duplicate spaces so that the CSV reader object may use space as the
# delimiter.
def ptest_xform_data(csvfile: _io.TextIOWrapper) -> list[str]:
    return list(map(lambda line: ' '.join(line.split()),
                filter(lambda row: row[0]!='#' and row[0]!='-',
                       csvfile.readlines()[8:])))


def ptest_load(path: str, dtype: np.dtype) -> np.array:
    with open(path, newline='') as csvfile:
        reader = csv.DictReader(ptest_xform_data(csvfile), delimiter=' ')
        return np.array([tuple(row.values()) for row in reader], dtype = dtype)


def ptest_print_1darray(array: np.array, name: str, desc: str) -> None:
    print((name + desc + ', '.join(['{}'] * len(array))).format(*array))


def ptest_print(results: np.array,
                groupby: list[str],
                properties: list[dict]) -> None:
    flds = groupby + [f for f in properties.keys() if f not in groupby]
    fmt = ''.join([str(properties[f]['format']) for f in flds])

    print(''.join([str(properties[f]['label']) for f in flds])[2:])

    for r in np.sort(results, order = flds):
        vals = []
        for f in flds:
            get = cast(Callable[[np.void], Union[str, int, float]],
                       properties[f]['get'])
            vals.append(get(r))
        print(fmt[2:].format(*vals))


def ptest_setup_lin(axe: matplotlib.axes.Axes,
                    title: Optional[str],
                    x_label: Optional[str],
                    y_label: bool) -> None:
    if title is not None:
        axe.set_title(title)
    if x_label is not None:
        axe.set_xlabel(x_label)
    axe.set_xscale('linear')
    axe.xaxis.set_major_locator(ticker.AutoLocator())
    axe.xaxis.set_minor_locator(ticker.AutoMinorLocator())
    if y_label is not None:
        axe.set_ylabel('Time [uSec]')
    axe.set_yscale('linear')
    axe.yaxis.set_major_locator(ticker.AutoLocator())
    axe.yaxis.set_minor_locator(ticker.AutoMinorLocator())
    axe.tick_params(which = 'both',
                    bottom = True,
                    labelbottom = x_label is not None,
                    top = True,
                    labeltop = False,
                    left = True,
                    labelleft = y_label is not None,
                    right = True,
                    labelright = False,
                    direction = 'in')


def ptest_setup_log(axe: matplotlib.axes.Axes,
                    title: Optional[str],
                    x_base: int,
                    x_label: Optional[str],
                    y_label: bool) -> None:
    if title is not None:
        axe.set_title(title)
    if x_label is not None:
        axe.set_xlabel(x_label)
    axe.set_xscale('log', base = x_base)
    if y_label is not None:
        axe.set_ylabel('Time [uSec]')
    axe.set_yscale('log')
    axe.yaxis.set_major_locator(ticker.LogLocator())
    axe.yaxis.set_minor_locator(ticker.LogLocator(subs = 'auto'))
    axe.tick_params(which = 'both',
                    bottom = True,
                    labelbottom = x_label is not None,
                    top = True,
                    labeltop = False,
                    left = True,
                    labelleft = y_label is not None,
                    right = True,
                    labelright = False,
                    direction = 'in')


def ptest_finish_plot(axe: matplotlib.axes.Axes) -> None:
    axe.grid(which = 'major', linestyle = '-', alpha = 0.4, visible = True)
    axe.grid(which = 'minor', linestyle = '--', alpha = 0.2, visible = True)
    axe.legend(loc = 'upper left')


def ptest_plot_lin(axe: matplotlib.axes.Axes,
                   title: Optional[str],
                   plot_label: str,
                   x_label: Optional[str],
                   x: str,
                   y_label: bool,
                   results) -> None:
    ptest_setup_lin(axe, title, x_label, y_label)
    res = np.sort(results, order = x)
    axe.plot(res[x], res['nsec'] / 1000, label = plot_label, marker = 'o')
    ptest_finish_plot(axe)


def ptest_plot_log(axe: matplotlib.axes.Axes,
                   title: Optional[str],
                   plot_label: str,
                   x_base: int,
                   x_label: Optional[str],
                   x: str,
                   y_label: bool,
                   results) -> None:
    ptest_setup_log(axe, title, x_base, x_label, y_label)
    res = np.sort(results, order = x)
    axe.plot(res[x], res['nsec'] / 1000, label = plot_label, marker = 'o')
    ptest_finish_plot(axe)


def ptest_group_plots(title: str,
                      groups: list[str]) -> Iterator[tuple[int,
                                                           matplotlib.axes.Axes,
                                                           bool,
                                                           bool]]:
    rows = round(math.sqrt(len(groups)))
    cols = math.ceil(len(groups) / rows)
    fig, axes = plt.subplots(nrows = rows,
                             ncols = cols,
                             sharex = True,
                             sharey = True,
                             constrained_layout = True)
    fig.suptitle(title, fontweight = 'bold')

    if (rows + cols) > 2:
        indx = 0
        for r in range(0, rows):
            for c in range(0, cols):
                ax = axes[r, c] if axes.ndim == 2 else axes[c]
                if indx >= len(groups):
                    ax.set_axis_off()
                    continue
                showx = r == (rows - 1)
                showy = c == 0
                yield (indx, ax, showx, showy)
                indx += 1
    else:
                yield (0, axes, True, True)


def ptest_init_prop(results: np.array,
                    prop: str,
                    args: argp.Namespace) -> np.array:
    keys = np.unique(results[prop])

    if hasattr(args, prop):
        a = getattr(args, prop)
        if a is not None:
            keys = keys[np.in1d(keys, a)]

    return np.sort(keys)


def ptest_init_algos(results: np.array, args: argp.Namespace) -> np.array:
    algos = ptest_init_prop(results, 'algo', args)
    if len(algos) == 0:
        raise Exception('Unexpected list of algorithm(s) specified')
    
    return algos
    
def ptest_init_nrs(results: np.array, args: argp.Namespace) -> np.array:
    nrs = ptest_init_prop(results, 'nr', args)
    if len(nrs) == 0:
        raise Exception('Unexpected list of numbers of samples specified')
        
    return nrs
    

def ptest_init_singles(results: np.array, args: argp.Namespace) -> np.array:
    singles = ptest_init_prop(results, 'single', args)
    if len(singles) == 0:
        raise Exception('Unexpected list of '
                        'distinct value ratio(s) specified')
    
    return singles


def ptest_init_orders(results: np.array, args: argp.Namespace) -> np.array:
    orders = ptest_init_prop(results, 'order', args)
    if len(orders) == 0:
        raise Exception('Unexpected list of ordering ratio(s) specified')
    
    return orders


def ptest_init_sizes(results: np.array, args: argp.Namespace) -> np.array:
    sizes = ptest_init_prop(results, 'size', args)
    if len(sizes) == 0:
        raise Exception('Unexpected list of sample size(s) specified')

    return sizes

def ptest_parse_int_list(arg: str) -> list[int]:
    return [int(a.strip()) for a in arg.split(',')]


def ptest_parse_str_list(arg: str) -> list[str]:
    return [a.strip() for a in arg.split(',')]


def ptest_parse_clause(arg: str, clause: str, allowed: dict) -> list[str]:
    lst = ptest_parse_str_list(arg)
    if len(set(lst)) != len(lst):
        raise argp.ArgumentError(
            None,
            "'" + arg + "': unexpected " + clause + " duplicate field(s).")
    for f in lst:
        if f not in allowed:
            raise argp.ArgumentError(
                None,
                "'" + f + "': unexpected " + clause + " field.")
    return lst


def ptest_check_single_algo(algos: np.array) -> None:
    if algos.size != 1:
        raise Exception('one single algorithm expected')


def ptest_check_single_distinct(singles: np.array) -> None:
    if singles.size != 1:
        raise Exception('one single distinct value ratio expected')


def ptest_check_single_order(orders: np.array) -> None:
    if orders.size != 1:
        raise Exception('one single ordering ratio expected')


def ptest_check_single_size(sizes: np.array) -> None:
    if sizes.size != 1:
        raise Exception('one single sample size expected')


def ptest_check_single_nr(nrs: np.array) -> None:
    if nrs.size != 1:
        raise Exception('one single number of samples expected')
