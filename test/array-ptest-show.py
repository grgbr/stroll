#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

from typing import Iterator
import sys
import os
import argparse as argp
import io
import csv
import matplotlib
matplotlib.use('GTK3Agg')
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import math

# Skip the first 7 lines and comment lines starting with the '#' character.
# Remove duplicate spaces so that the CSV reader object may use space as the
# delimiter.
def array_ptest_xform_data(csvfile: io.TextIOWrapper) -> list[str]:
    return list(map(lambda line: ' '.join(line.split()),
                filter(lambda row: row[0]!='#' and row[0]!='-',
                       csvfile.readlines()[7:])))


def array_ptest_load(path: str) -> np.array:
    dt = np.dtype([('algo', object),
                   ('nr', int),
                   ('single', int),
                   ('order', int),
                   ('size', int),
                   ('nsec', int)])
    with open(path, newline='') as csvfile:
        reader = csv.DictReader(array_ptest_xform_data(csvfile), delimiter=' ')
        return np.array([tuple(row.values()) for row in reader], dtype = dt)


def array_ptest_select(results,
                       algos: np.array,
                       orders: np.array,
                       sizes: np.array,
                       nrs: np.array) -> np.array:
    return results[np.in1d(results['algo'], algos) &
                   np.in1d(results['order'], orders) &
                   np.in1d(results['size'], sizes) &
                   np.in1d(results['nr'], nrs)]


def array_ptest_print_1darray(array: np.ndarray, name: str, desc: str) -> None:
    print((name + desc + ', '.join(['{}'] * len(array))).format(*array))


def array_ptest_list(results,
                     algos: np.array,
                     singles: np.array,
                     orders: np.array,
                     sizes: np.array,
                     nrs: np.array) -> None:
    print('FIELD  DESCRIPTION           VALUES')
    array_ptest_print_1darray(algos,   'algo   ', 'algorithms      ')
    array_ptest_print_1darray(nrs,     'nr     ', '#Samples        ')
    array_ptest_print_1darray(singles, 'single ', 'Distinct ratios ')
    array_ptest_print_1darray(orders,  'order  ', 'Ordering ratios ')
    array_ptest_print_1darray(sizes,   'size   ', 'Sample sizes    ')


ARRAY_PTEST_PROPS = {
    'algo': {
        'label':  '  ALGORITHM       ',
        'format': '  {:16s}',
        'get':    (lambda r: r['algo'])
    },
    'nr': {
        'label':  '    #SAMPLES',
        'format': '  {:10d}',
        'get':    (lambda r: r['nr'])
    },
    'single': {
        'label':  ' DISTINCT(%)',
        'format': '         {:3d}',
        'get':    (lambda r: r['single'])
    },
    'order': {
        'label':  '  ORDER(%)',
        'format': '       {:3d}',
        'get':    (lambda r: r['order'])
    },
    'size': {
        'label':  '  SIZE(B)',
        'format': '     {:4d}',
        'get':    (lambda r: r['size'])
    },
    'nsec': {
        'label':  '     TIME(us)',
        'format': '  {:11.3f}',
        'get':    (lambda r: r['nsec'] / 1000)
    }
}


def array_ptest_print(results,
                      groupby: list[str],
                      algos: np.array,
                      singles: np.array,
                      orders: np.array,
                      sizes: np.array,
                      nrs: np.array) -> None:
    flds = groupby + [f for f in ARRAY_PTEST_PROPS.keys() if f not in groupby]
    fmt = ''.join([ARRAY_PTEST_PROPS[f]['format'] for f in flds])

    print(''.join([ARRAY_PTEST_PROPS[f]['label'] for f in flds])[2:])

    res = array_ptest_select(results, algos, orders, sizes, nrs)
    for r in np.sort(res, order = flds):
        print(fmt[2:].format(*[ARRAY_PTEST_PROPS[f]['get'](r) for f in flds]))


def array_ptest_setup_lin(axe: matplotlib.axes.Axes,
                          title: str or None,
                          x_label: str or None,
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


def array_ptest_setup_log(axe: matplotlib.axes.Axes,
                          title: str,
                          x_base: int,
                          x_label: str or None,
                          y_label: bool) -> None:
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


def array_ptest_finish_plot(axe: matplotlib.axes.Axes) -> None:
    axe.grid(which = 'major', linestyle = '-', alpha = 0.4)
    axe.grid(which = 'minor', linestyle = '--', alpha = 0.2)
    axe.legend(loc = 'upper left')


def array_ptest_plot_lin(axe: matplotlib.axes.Axes,
                         title: str or None,
                         plot_label: str,
                         x_label: str or None,
                         x: str,
                         y_label: bool,
                         results) -> None:
    array_ptest_setup_lin(axe, title, x_label, y_label)
    res = np.sort(results, order = x)
    axe.plot(res[x], res['nsec'] / 1000, label = plot_label, marker = 'o')
    array_ptest_finish_plot(axe)


def array_ptest_plot_log(axe: matplotlib.axes.Axes,
                         title: str or None,
                         plot_label: str,
                         x_base: int,
                         x_label: str or None,
                         x: str,
                         y_label: bool,
                         results) -> None:
    array_ptest_setup_log(axe, title, x_base, x_label, y_label)
    res = np.sort(results, order = x)
    axe.plot(res[x], res['nsec'] / 1000, label = plot_label, marker = 'o')
    array_ptest_finish_plot(axe)


def array_ptest_check_single_algo(algos: np.array) -> None:
    if algos.size != 1:
        raise Exception('one single algorithm expected')


def array_ptest_check_single_distinct(singles: np.array) -> None:
    if singles.size != 1:
        raise Exception('one single distinct value ratio expected')


def array_ptest_check_single_order(orders: np.array) -> None:
    if orders.size != 1:
        raise Exception('one single ordering ratio expected')


def array_ptest_check_single_size(sizes: np.array) -> None:
    if sizes.size != 1:
        raise Exception('one single sample size expected')


def array_ptest_check_single_nr(nrs: np.array) -> None:
    if nrs.size != 1:
        raise Exception('one single number of samples expected')


def array_ptest_plot_title(axe: matplotlib.axes.Axes, title: str) -> None:
        axe.get_figure().suptitle(title, fontweight = 'bold')


def array_ptest_plot_algos_funcof_singles(axe: matplotlib.axes.Axes,
                                          title: str or None,
                                          showx: bool,
                                          showy: bool,
                                          results,
                                          algos: np.array,
                                          singles: np.array,
                                          orders: np.array,
                                          sizes: np.array,
                                          nrs: np.array) -> None:
    array_ptest_check_single_order(orders)
    array_ptest_check_single_size(sizes)
    array_ptest_check_single_nr(nrs)

    if title is None:
        array_ptest_plot_title(axe,
                               'Algorithms as a function of '
                               'distinct value ratio')
        title = '#Samples: {} - ' \
                'Sample size: {} Bytes - ' \
                'Order: {}%'.format(nrs[0], sizes[0], orders[0])
    res = results[(results['nr'] == nrs[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['size'] == sizes[0])]
    for a in algos:
        array_ptest_plot_lin(axe,
                             title,
                             a,
                             'Distinct ratio' if showx else None,
                             'single',
                             showy,
                             res[res['algo'] == a])


def array_ptest_plot_algos_funcof_orders(axe: matplotlib.axes.Axes,
                                         title: str or None,
                                         showx: bool,
                                         showy: bool,
                                         results,
                                         algos: np.array,
                                         singles: np.array,
                                         orders: np.array,
                                         sizes: np.array,
                                         nrs: np.array) -> None:
    array_ptest_check_single_distinct(singles)
    array_ptest_check_single_size(sizes)
    array_ptest_check_single_nr(nrs)

    if title is None:
        array_ptest_plot_title(axe,
                               'Algorithms as a function of '
                               'order ratio')
        title = '#Samples: {} - ' \
                'Distinct: {}% - ' \
                'Sample size: {} Bytes'.format(nrs[0], singles[0], sizes[0])
    res = results[(results['nr'] == nrs[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['size'] == sizes[0])]
    for a in algos:
        array_ptest_plot_lin(axe,
                             title,
                             a,
                             'Order ratio' if showx else None,
                             'order',
                             showy,
                             res[res['algo'] == a])


def array_ptest_plot_algos_funcof_sizes(axe: matplotlib.axes.Axes,
                                        title: str or None,
                                        showx: bool,
                                        showy: bool,
                                        results,
                                        algos: np.array,
                                        singles: np.array,
                                        orders: np.array,
                                        sizes: np.array,
                                        nrs: np.array) -> None:
    array_ptest_check_single_distinct(singles)
    array_ptest_check_single_order(orders)
    array_ptest_check_single_nr(nrs)

    if title is None:
        array_ptest_plot_title(axe,
                               'Algorithms as a function of '
                               'Data sample size')
        title = '#Samples: {} - ' \
                'Distinct: {}% - ' \
                'Order: {}%'.format(nrs[0], singles[0], orders[0])
    res = results[(results['nr'] == nrs[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['order'] == orders[0])]
    for a in algos:
        array_ptest_plot_lin(axe,
                             title,
                             a,
                             'Sample size' if showx else None,
                             'size',
                             showy,
                             res[res['algo'] == a])


def array_ptest_plot_algos_funcof_nrs(axe: matplotlib.axes.Axes,
                                      title: str or None,
                                      showx: bool,
                                      showy: bool,
                                      results,
                                      algos: np.array,
                                      singles: np.array,
                                      orders: np.array,
                                      sizes: np.array,
                                      nrs: np.array) -> None:
    array_ptest_check_single_distinct(singles)
    array_ptest_check_single_order(orders)
    array_ptest_check_single_size(sizes)

    if title is None:
        array_ptest_plot_title(axe,
                               'Algorithms as a function of '
                               'number of data samples')
        title = 'Distinct: {}% - ' \
                'Order: {}% - ' \
                'Sample size: {} Bytes'.format(singles[0], orders[0], sizes[0])
    res = results[(results['single'] == singles[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['size'] == sizes[0])]
    for a in algos:
        array_ptest_plot_log(axe,
                             title,
                             a,
                             2,
                             '#Samples' if showx else None,
                             'nr',
                             showy,
                             res[res['algo'] == a])


def array_ptest_plot_singles_funcof_orders(axe: matplotlib.axes.Axes,
                                           title: str or None,
                                           showx: bool,
                                           showy: bool,
                                           results,
                                           algos: np.array,
                                           singles: np.array,
                                           orders: np.array,
                                           sizes: np.array,
                                           nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_size(sizes)
    array_ptest_check_single_nr(nrs)

    if title is None:
        array_ptest_plot_title(axe,
                               'Distinct ratios as a function of '
                               'order ratio')
        title = 'Algorithm: {} - ' \
                '#Samples: {} - ' \
                'Sample size: {} Bytes'.format(algos[0], nrs[0], sizes[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['size'] == sizes[0]) & \
                  (results['nr'] == nrs[0])]
    for s in singles:
        array_ptest_plot_lin(axe,
                             title,
                             '{}%'.format(s),
                             'Order ratio' if showx else None,
                             'order',
                             showy,
                             res[res['single'] == s])


def array_ptest_plot_singles_funcof_sizes(axe: matplotlib.axes.Axes,
                                          title: str or None,
                                          showx: bool,
                                          showy: bool,
                                          results,
                                          algos: np.array,
                                          singles: np.array,
                                          orders: np.array,
                                          sizes: np.array,
                                          nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_order(orders)
    array_ptest_check_single_nr(nrs)

    if title is None:
        array_ptest_plot_title(axe,
                               'Distinct ratios as a function of '
                               'Data sample size')
        title = 'Algorithm: {} - ' \
                '#Samples: {} - ' \
                'Order: {}%'.format(algos[0], nrs[0], orders[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['nr'] == nrs[0])]
    for s in singles:
        array_ptest_plot_lin(axe,
                             title,
                             '{}%'.format(s),
                             'Sample size' if showx else None,
                             'size',
                             showy,
                             res[res['single'] == s])


def array_ptest_plot_singles_funcof_nrs(axe: matplotlib.axes.Axes,
                                        title: str or None,
                                        showx: bool,
                                        showy: bool,
                                        results,
                                        algos: np.array,
                                        singles: np.array,
                                        orders: np.array,
                                        sizes: np.array,
                                        nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_order(orders)
    array_ptest_check_single_size(sizes)

    if title is None:
        array_ptest_plot_title(axe,
                               'Distinct ratios as a function of '
                               'number of data samples')
        title = 'Algorithm: {} - ' \
                'Order: {}% - ' \
                'Sample size: {} Bytes'.format(algos[0], orders[0], sizes[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['size'] == sizes[0])]
    for s in singles:
        array_ptest_plot_log(axe,
                             title,
                             '{}%'.format(s),
                             2,
                             '#Samples' if showx else None,
                             'nr',
                             showy,
                             res[res['single'] == s])


def array_ptest_plot_orders_funcof_singles(axe: matplotlib.axes.Axes,
                                           title: str or None,
                                           showx: bool,
                                           showy: bool,
                                           results,
                                           algos: np.array,
                                           singles: np.array,
                                           orders: np.array,
                                           sizes: np.array,
                                           nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_size(sizes)
    array_ptest_check_single_nr(nrs)

    if title is None:
        array_ptest_plot_title(axe,
                               'Order ratios as a function of '
                               'distinct value ratio')
        title = 'Algorithms: {} - ' \
                '#Samples: {} - ' \
                'Sample size: {} Bytes'.format(algos[0], nrs[0], sizes[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['nr'] == nrs[0]) & \
                  (results['size'] == sizes[0])]
    for o in orders:
        array_ptest_plot_lin(axe,
                             title,
                             '{}%'.format(o),
                             'Distinct ratio' if showx else None,
                             'single',
                             showy,
                             res[res['order'] == o])


def array_ptest_plot_orders_funcof_sizes(axe: matplotlib.axes.Axes,
                                         title: str or None,
                                         showx: bool,
                                         showy: bool,
                                         results,
                                         algos: np.array,
                                         singles: np.array,
                                         orders: np.array,
                                         sizes: np.array,
                                         nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_distinct(singles)
    array_ptest_check_single_nr(nrs)

    if title is None:
        array_ptest_plot_title(axe,
                               'Order ratios as a function of '
                               'Data sample size')
        title = 'Algorithm: {} - ' \
                '#Samples: {} - ' \
                'Distinct: {}%'.format(algos[0], nrs[0], singles[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['nr'] == nrs[0])]
    for o in orders:
        array_ptest_plot_lin(axe,
                             title,
                             '{}%'.format(o),
                             'Sample size' if showx else None,
                             'size',
                             showy,
                             res[res['order'] == o])


def array_ptest_plot_orders_funcof_nrs(axe: matplotlib.axes.Axes,
                                       title: str or None,
                                       showx: bool,
                                       showy: bool,
                                       results,
                                       algos: np.array,
                                       singles: np.array,
                                       orders: np.array,
                                       sizes: np.array,
                                       nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_distinct(singles)
    array_ptest_check_single_size(sizes)

    if title is None:
        array_ptest_plot_title(axe,
                               'Order ratios as a function of '
                               'number of data samples')
        title = 'Algorithm: {} - ' \
                'Distinct: {}% - ' \
                'Sample size: {} Bytes'.format(algos[0], singles[0], sizes[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['size'] == sizes[0])]
    for o in orders:
        array_ptest_plot_log(axe,
                             title,
                             '{}%'.format(o),
                             2,
                             '#Samples' if showx else None,
                             'nr',
                             showy,
                             res[res['order'] == o])



def array_ptest_plot_sizes_funcof_orders(axe: matplotlib.axes.Axes,
                                         title: str or None,
                                         showx: bool,
                                         showy: bool,
                                         results,
                                         algos: np.array,
                                         singles: np.array,
                                         orders: np.array,
                                         sizes: np.array,
                                         nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_distinct(singles)
    array_ptest_check_single_nr(nrs)

    if title is None:
        array_ptest_plot_title(axe,
                               'Sample sizes as a function of '
                               'order ratio')
        title = 'Algorithm: {} - ' \
                '#Samples: {} - ' \
                'Distinct: {}%'.format(algos[0], nrs[0], singles[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['nr'] == nrs[0])]
    for s in sizes:
        array_ptest_plot_lin(axe,
                             title,
                             '{} Bytes'.format(s),
                             'Order ratio' if showx else None,
                             'order',
                             showy,
                             res[res['size'] == s])

def array_ptest_plot_sizes_funcof_singles(axe: matplotlib.axes.Axes,
                                          title: str or None,
                                          showx: bool,
                                          showy: bool,
                                          results,
                                          algos: np.array,
                                          singles: np.array,
                                          orders: np.array,
                                          sizes: np.array,
                                          nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_order(orders)
    array_ptest_check_single_nr(nrs)

    if title is None:
        array_ptest_plot_title(axe,
                               'Sample sizes as a function of '
                               'distinct value ratio')
        title = 'Algorithms: {} - ' \
                'Order: {}% - ' \
                '#Samples: {}'.format(algos[0], orders[0], nrs[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['nr'] == nrs[0])]
    for s in sizes:
        array_ptest_plot_lin(axe,
                             title,
                             '{} Bytes'.format(s),
                             'Distinct ratio' if showx else None,
                             'single',
                             showy,
                             res[res['size'] == s])


def array_ptest_plot_sizes_funcof_nrs(axe: matplotlib.axes.Axes,
                                      title: str or None,
                                      showx: bool,
                                      showy: bool,
                                      results,
                                      algos: np.array,
                                      singles: np.array,
                                      orders: np.array,
                                      sizes: np.array,
                                      nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_distinct(singles)
    array_ptest_check_single_order(orders)

    if title is None:
        array_ptest_plot_title(axe,
                               'Sample sizes as a function of '
                               'number of data samples')
        title = 'Algorithm: {} - ' \
                'Distinct: {}% - ' \
                'Order: {}%'.format(algos[0], singles[0], orders[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['order'] == orders[0])]
    for s in sizes:
        array_ptest_plot_log(axe,
                             title,
                             '{} Bytes'.format(s),
                             2,
                             '#Samples' if showx else None,
                             'nr',
                             showy,
                             res[res['size'] == s])


def array_ptest_plot_nrs_funcof_orders(axe: matplotlib.axes.Axes,
                                       title: str or None,
                                       showx: bool,
                                       showy: bool,
                                       results,
                                       algos: np.array,
                                       singles: np.array,
                                       orders: np.array,
                                       sizes: np.array,
                                       nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_distinct(singles)
    array_ptest_check_single_size(sizes)

    if title is None:
        array_ptest_plot_title(axe,
                               '#Samples as a function of order ratio')
        title = 'Algorithm: {} - ' \
                'Distinct: {}% - ' \
                'Sample size: {} Bytes'.format(algos[0], singles[0], sizes[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['size'] == sizes[0])]
    for n in nrs:
        array_ptest_plot_lin(axe,
                             title,
                             n,
                             'Order ratio' if showx else None,
                             'order',
                             showy,
                             res[res['nr'] == n])



def array_ptest_plot_nrs_funcof_singles(axe: matplotlib.axes.Axes,
                                        title: str or None,
                                        showx: bool,
                                        showy: bool,
                                        results,
                                        algos: np.array,
                                        singles: np.array,
                                        orders: np.array,
                                        sizes: np.array,
                                        nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_order(orders)
    array_ptest_check_single_size(sizes)

    if title is None:
        array_ptest_plot_title(axe,
                               '#Samples as a function of '
                               'distinct value ratio')
        title = 'Algorithms: {} - ' \
                'Order: {}% - ' \
                'Sample size: {} Bytes'.format(algos[0], orders[0], sizes[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['size'] == sizes[0])]
    for n in nrs:
        array_ptest_plot_lin(axe,
                             title,
                             n,
                             'Distinct ratio' if showx else None,
                             'single',
                             showy,
                             res[res['nr'] == n])



def array_ptest_plot_nrs_funcof_sizes(axe: matplotlib.axes.Axes,
                                      title: str or None,
                                      showx: bool,
                                      showy: bool,
                                      results,
                                      algos: np.array,
                                      singles: np.array,
                                      orders: np.array,
                                      sizes: np.array,
                                      nrs: np.array) -> None:
    array_ptest_check_single_algo(algos)
    array_ptest_check_single_distinct(singles)
    array_ptest_check_single_order(orders)

    if title is None:
        array_ptest_plot_title(axe,
                               '#Samples as a function of '
                               'Data sample size')
        title = 'Algorithm: {} - ' \
                'Distinct: {}% - ' \
                'Order: {}%'.format(algos[0], singles[0], orders[0])
    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['order'] == orders[0])]
    for n in nrs:
        array_ptest_plot_lin(axe,
                             title,
                             n,
                             'Sample size' if showx else None,
                             'size',
                             showy,
                             res[res['nr'] == n])


ARRAY_PTEST_SINGLE_PLOTTERS = {
        'algo':  {
            'single': array_ptest_plot_algos_funcof_singles,
            'order':  array_ptest_plot_algos_funcof_orders,
            'size':   array_ptest_plot_algos_funcof_sizes,
            'nr':     array_ptest_plot_algos_funcof_nrs
        },
        'single':  {
            'order': array_ptest_plot_singles_funcof_orders,
            'size':  array_ptest_plot_singles_funcof_sizes,
            'nr':    array_ptest_plot_singles_funcof_nrs
        },
        'order':  {
            'single': array_ptest_plot_orders_funcof_singles,
            'size':   array_ptest_plot_orders_funcof_sizes,
            'nr':     array_ptest_plot_orders_funcof_nrs
        },
        'size':  {
            'order':  array_ptest_plot_sizes_funcof_orders,
            'single': array_ptest_plot_sizes_funcof_singles,
            'nr':     array_ptest_plot_sizes_funcof_nrs
        },
        'nr':  {
            'order':  array_ptest_plot_nrs_funcof_orders,
            'single': array_ptest_plot_nrs_funcof_singles,
            'size':   array_ptest_plot_nrs_funcof_sizes
        }
}


def array_ptest_single_plotter(funcof: list[str]) -> [[matplotlib.axes.Axes,
                                                       str or None,
                                                       bool,
                                                       bool,
                                                       np.array,
                                                       np.array,
                                                       np.array,
                                                       np.array,
                                                       np.array,
                                                       np.array], None]:
    plotter = ARRAY_PTEST_SINGLE_PLOTTERS
    for fld in funcof:
        if not isinstance(plotter, dict):
            raise Exception("'{}': unexpected extra plotter name"
                            " field".format(','.join(funcof)))
        assert fld in plotter
        plotter = plotter[fld]

    if isinstance(plotter, dict):
        raise Exception("'{}': plotter name field "
                        "missing".format(','.join(groupby)))

    return plotter


def array_ptest_show_single_plot(results,
                                 funcof: list[str],
                                 algos: np.array,
                                 singles: np.array,
                                 orders: np.array,
                                 sizes: np.array,
                                 nrs: np.array) -> None:
    plotter = array_ptest_single_plotter(funcof)

    fig, axe = plt.subplots(nrows = 1, ncols = 1, constrained_layout = True)
    plotter(axe, None, True, True, results, algos, singles, orders, sizes, nrs)
    plt.show()


def array_ptest_group_plots(title: str,
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
                if indx >= len(groups):
                    break
                ax = axes[r, c] if axes.ndim == 2 else axes[c]
                showx = r == (rows - 1)
                showy = c == 0
                yield (indx, ax, showx, showy)
                indx += 1
    else:
                yield (0, axes, True, True)


def array_ptest_plot_algo_group(results: np.array,
                                title: str,
                                plotter,
                                algos: np.array,
                                singles: np.array,
                                orders: np.array,
                                sizes: np.array,
                                nrs: np.array) -> None:
    for indx, axe, showx, showy in array_ptest_group_plots(title, algos):
            plotter(axe,
                    "Algorithm: {}".format(algos[indx]),
                    showx,
                    showy,
                    results,
                    algos[algos == algos[indx]],
                    singles,
                    orders,
                    sizes,
                    nrs)


def array_ptest_plot_distinct_group(results: np.array,
                                    title: str,
                                    plotter,
                                    algos: np.array,
                                    singles: np.array,
                                    orders: np.array,
                                    sizes: np.array,
                                    nrs: np.array) -> None:
    for indx, axe, showx, showy in array_ptest_group_plots(title, singles):
            plotter(axe,
                    "Distinct: {}%".format(singles[indx]),
                    showx,
                    showy,
                    results,
                    algos,
                    singles[singles == singles[indx]],
                    orders,
                    sizes,
                    nrs)


def array_ptest_plot_order_group(results: np.array,
                                 title: str,
                                 plotter,
                                 algos: np.array,
                                 singles: np.array,
                                 orders: np.array,
                                 sizes: np.array,
                                 nrs: np.array) -> None:
    for indx, axe, showx, showy in array_ptest_group_plots(title, orders):
            plotter(axe,
                    "Order: {}%".format(orders[indx]),
                    showx,
                    showy,
                    results,
                    algos,
                    singles,
                    orders[orders == orders[indx]],
                    sizes,
                    nrs)


def array_ptest_plot_size_group(results: np.array,
                                title: str,
                                plotter,
                                algos: np.array,
                                singles: np.array,
                                orders: np.array,
                                sizes: np.array,
                                nrs: np.array) -> None:
    for indx, axe, showx, showy in array_ptest_group_plots(title, sizes):
            plotter(axe,
                    "Size: {} Bytes".format(sizes[indx]),
                    showx,
                    showy,
                    results,
                    algos,
                    singles,
                    orders,
                    sizes[sizes == sizes[indx]],
                    nrs)


def array_ptest_plot_nr_group(results: np.array,
                              title: str,
                              plotter,
                              algos: np.array,
                              singles: np.array,
                              orders: np.array,
                              sizes: np.array,
                              nrs: np.array) -> None:
    for indx, axe, showx, showy in array_ptest_group_plots(title, nrs):
            plotter(axe,
                    "#Samples: {}".format(nrs[indx]),
                    showx,
                    showy,
                    results,
                    algos,
                    singles,
                    orders,
                    sizes,
                    nrs[nrs == nrs[indx]])


ARRAY_PTEST_GROUP_PLOTTERS = {
        'algo':   array_ptest_plot_algo_group,
        'single': array_ptest_plot_distinct_group,
        'order':  array_ptest_plot_order_group,
        'size':   array_ptest_plot_size_group,
        'nr':     array_ptest_plot_nr_group
}

def array_ptest_plots_title(group_field: str | None,
                            funcof_fields: list[str],
                            algos: np.array,
                            singles: np.array,
                            orders: np.array,
                            sizes: np.array,
                            nrs: np.array) -> str:
    if (group_field is not None) and (group_field in funcof_fields):
        raise Exception("'{}': "
                        "unexpected duplicate plot group field".format(group))

    sup = []
    for f in funcof_fields:
        if f == 'algo':
            sup.append('Algorithms')
        elif f == 'nr':
            sup.append('#Samples')
        elif f == 'single':
            sup.append('Distinct ratios')
        elif f == 'order':
            sup.append('Order ratios')
        elif f == 'size':
            sup.append('Sample sizes')
    sup = ' as a function of '.join(sup)

    sub = []
    flds = [ 'algo', 'nr', 'single', 'order', 'size' ]
    if group_field is not None:
        flds.remove(group_field)
    for f in funcof_fields:
        flds.remove(f)
    for f in flds:
        if f == 'algo':
            array_ptest_check_single_algo(algos)
            sub.append('Algorithm: {}'.format(algos[0]))
        elif f == 'nr':
            array_ptest_check_single_nr(nrs)
            sub.append('#Samples: {}'.format(nrs[0]))
        elif f == 'single':
            array_ptest_check_single_distinct(singles)
            sub.append('Distinct: {}%'.format(singles[0]))
        elif f == 'order':
            array_ptest_check_single_order(orders)
            sub.append('Order: {}%'.format(orders[0]))
        elif f == 'size':
            array_ptest_check_single_order(sizes)
            sub.append('Size: {} Bytes'.format(sizes[0]))
    sub = ' - '.join(sub)

    return sup + '\n' + sub


def array_ptest_show_plots_group(results,
                                 group: str,
                                 funcof: list[str],
                                 algos: np.array,
                                 singles: np.array,
                                 orders: np.array,
                                 sizes: np.array,
                                 nrs: np.array) -> None:
    group_plotter = ARRAY_PTEST_GROUP_PLOTTERS[group]
    funcof_plotter = array_ptest_single_plotter(funcof)
    title = array_ptest_plots_title(group,
                                    funcof,
                                    algos,
                                    singles,
                                    orders,
                                    sizes,
                                    nrs)
    group_plotter(results,
                  title,
                  funcof_plotter,
                  algos,
                  singles,
                  orders,
                  sizes,
                  nrs)
    plt.show()


def array_ptest_parse_str_list(arg: str) -> list[str]:
    return [a.strip() for a in arg.split(',')]


def array_ptest_parse_clause(arg: str, clause: str, allowed: dict) -> list[str]:
    lst = array_ptest_parse_str_list(arg)
    if len(set(lst)) != len(lst):
        raise argp.ArgumentError(
            None,
            "'" + f + "': unexpected " + clause + " clause duplicate field(s).")
    for f in lst:
        if f not in allowed:
            raise argp.ArgumentError(
                None,
                "'" + f + "': unexpected " + clause + " clause field.")
    return lst


def array_ptest_parse_print_groupby(arg: str) -> list[str]:
    return array_ptest_parse_clause(arg, 'groupby', ARRAY_PTEST_PROPS)


def array_ptest_parse_plot_groupby(arg: str) -> list[str]:
    if arg not in ARRAY_PTEST_GROUP_PLOTTERS.keys():
        raise argp.ArgumentError(None,
                                 "'" + f + "': unexpected plot group field.")
    return arg


def array_ptest_parse_plot_funcof(arg: str) -> list[str]:
    return array_ptest_parse_clause(arg, 'funcof', ARRAY_PTEST_SINGLE_PLOTTERS)


def array_ptest_parse_int_list(arg: str) -> list[int]:
    return [int(a.strip()) for a in arg.split(',')]


def array_ptest_init_prop(results: np.ndarray,
                          prop: str,
                          args: argp.Namespace) -> np.ndarray:
    keys = np.unique(results[prop])

    if hasattr(args, prop):
        a = getattr(args, prop)
        if a is not None:
            keys = keys[np.in1d(keys, a)]

    return np.sort(keys)


def main():
    opt_parser = argp.ArgumentParser(add_help = False)
    opt_parser.add_argument('results_path',
                            type = str,
                            metavar = 'RESULTS_PATH',
                            help = 'Pathname to file where test results are '
                                   'stored.')
    opt_parser.add_argument('-a', '--algos',
                            dest = 'algo',
                            type = array_ptest_parse_str_list,
                            metavar = 'ALGORITHMS',
                            help = 'Comma separated list of algorithms')
    opt_parser.add_argument('-i', '--single-ratios',
                            dest = 'single',
                            type = array_ptest_parse_int_list,
                            metavar = 'SINGLE_RATIOS',
                            help = 'Comma separated list of distinct value ratios')
    opt_parser.add_argument('-r', '--order-ratios',
                            dest = 'order',
                            type = array_ptest_parse_int_list,
                            metavar = 'ORDER_RATIOS',
                            help = 'Comma separated list of ordering ratios')
    opt_parser.add_argument('-s', '--sizes',
                            dest = 'size',
                            type = array_ptest_parse_int_list,
                            metavar = 'SAMPLE_SIZES',
                            help = 'Comma separated list of sample sizes')
    opt_parser.add_argument('-n', '--samples',
                            dest = 'nr',
                            type = array_ptest_parse_int_list,
                            metavar = 'SAMPLE_COUNTS',
                            help = 'Comma separated list of number of samples')

    main_parser = argp.ArgumentParser(description = 'Stroll performance test '
                                                    'visualization tool')
    cmd_parser = main_parser.add_subparsers(dest = 'cmd',
                                            required = True,
                                            metavar = 'COMMAND')
    list_parser = cmd_parser.add_parser('list',
                                        parents = [opt_parser],
                                        help = 'List test result fields')
    print_parser = cmd_parser.add_parser('print',
                                         parents = [opt_parser],
                                         help = 'Print test results')
    print_parser.add_argument('-g', '--groupby',
                              dest = 'groupby',
                              type = array_ptest_parse_print_groupby,
                              default = 'algo,nr,order,size,single',
                              metavar = 'RESULT_FIELDS',
                              help = 'Comma separated list of result fields to '
                                     'group results by')

    plot_parser = cmd_parser.add_parser('plot',
                                        parents = [opt_parser],
                                        help = 'Plot test results')
    plot_parser.add_argument('-f', '--funcof',
                             dest = 'funcof',
                             type = array_ptest_parse_plot_funcof,
                             default = 'algo,order',
                             metavar = 'RESULT_FIELDS',
                             help = 'Comma separated list of 2 result fields '
                                    'where the 1st is plotted as a function of '
                                    'the 2nd')
    plot_parser.add_argument('-g', '--groupby',
                             dest = 'groupby',
                             type = array_ptest_parse_plot_groupby,
                             default = None,
                             metavar = 'RESULT_FIELDS',
                             help = 'Comma separated list of result '
                                    'fields to plot results by')


    args = main_parser.parse_args()

    try:
        results = array_ptest_load(args.results_path)

        algos = array_ptest_init_prop(results, 'algo', args)
        if len(algos) == 0:
            raise Exception('Unexpected list of algorithm(s) specified')
        nrs = array_ptest_init_prop(results, 'nr', args)
        if len(nrs) == 0:
            raise Exception('Unexpected list of numbers of samples specified')
        singles = array_ptest_init_prop(results, 'single', args)
        if len(singles) == 0:
            raise Exception('Unexpected list of '
                            'distinct value ratio(s) specified')
        orders = array_ptest_init_prop(results, 'order', args)
        if len(orders) == 0:
            raise Exception('Unexpected list of ordering ratio(s) specified')
        sizes = array_ptest_init_prop(results, 'size', args)
        if len(sizes) == 0:
            raise Exception('Unexpected list of sample size(s) specified')

        if args.cmd == 'list':
            array_ptest_list(results, algos, singles, orders, sizes, nrs)
        elif args.cmd == 'print':
            array_ptest_print(results,
                              args.groupby,
                              algos,
                              singles,
                              orders,
                              sizes,
                              nrs)
        elif args.cmd == 'plot':
            if args.groupby is not None:
                array_ptest_show_plots_group(results,
                                             args.groupby,
                                             args.funcof,
                                             algos,
                                             singles,
                                             orders,
                                             sizes,
                                             nrs)
            else:
                array_ptest_show_single_plot(results,
                                             args.funcof,
                                             algos,
                                             singles,
                                             orders,
                                             sizes,
                                             nrs)
    except KeyboardInterrupt:
        print("{}: Interrupted!".format(os.path.basename(sys.argv[0])),
              file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        raise e
        print("{}: {}.".format(os.path.basename(sys.argv[0]), e),
              file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
