#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

from typing import Callable, Optional, cast
import sys
import os
import argparse as argp
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

from ptest_show import ptest_load, \
                       ptest_print, \
                       ptest_plot_lin, \
                       ptest_plot_log, \
                       ptest_group_plots, \
                       ptest_print_1darray, \
                       ptest_init_algos, \
                       ptest_init_nrs, \
                       ptest_init_singles, \
                       ptest_init_orders, \
                       ptest_init_sizes, \
                       ptest_parse_int_list, \
                       ptest_parse_str_list, \
                       ptest_parse_clause, \
                       ptest_check_single_algo, \
                       ptest_check_single_distinct, \
                       ptest_check_single_order, \
                       ptest_check_single_size, \
                       ptest_check_single_nr


def array_ptest_load(path: str) -> np.array:
    return ptest_load(path, np.dtype([('algo', object),
                                      ('nr', int),
                                      ('single', int),
                                      ('order', int),
                                      ('size', int),
                                      ('nsec', int)]))


def array_ptest_select(results: np.array,
                       algos: np.array,
                       orders: np.array,
                       sizes: np.array,
                       nrs: np.array) -> np.array:
    return results[np.in1d(results['algo'], algos) &
                   np.in1d(results['order'], orders) &
                   np.in1d(results['size'], sizes) &
                   np.in1d(results['nr'], nrs)]


def array_ptest_list(results,
                     algos: np.array,
                     singles: np.array,
                     orders: np.array,
                     sizes: np.array,
                     nrs: np.array) -> None:
    print('FIELD  DESCRIPTION           VALUES')
    ptest_print_1darray(algos,   'algo   ', 'algorithms      ')
    ptest_print_1darray(nrs,     'nr     ', '#Samples        ')
    ptest_print_1darray(singles, 'single ', 'Distinct ratios ')
    ptest_print_1darray(orders,  'order  ', 'Ordering ratios ')
    ptest_print_1darray(sizes,   'size   ', 'Sample sizes    ')


ARRAY_PTEST_PROPS = {
    'algo': {
        'label':  '  ALGORITHM       ',
        'format': '  {:16s}',
        'get':    lambda r: r['algo']
    },
    'nr': {
        'label':  '    #SAMPLES',
        'format': '  {:10d}',
        'get':    lambda r: r['nr']
    },
    'single': {
        'label':  ' DISTINCT(%)',
        'format': '         {:3d}',
        'get':    lambda r: r['single']
    },
    'order': {
        'label':  '  ORDER(%)',
        'format': '       {:3d}',
        'get':    lambda r: r['order']
    },
    'size': {
        'label':  '  SIZE(B)',
        'format': '     {:4d}',
        'get':    lambda r: r['size']
    },
    'nsec': {
        'label':  '     TIME(us)',
        'format': '  {:11.3f}',
        'get':    lambda r: r['nsec'] / 1000
    }
}


def array_ptest_print(results: np.array,
                      groupby: list[str],
                      algos: np.array,
                      singles: np.array,
                      orders: np.array,
                      sizes: np.array,
                      nrs: np.array) -> None:
    ptest_print(array_ptest_select(results, algos, orders, sizes, nrs),
                groupby,
                ARRAY_PTEST_PROPS)


def array_ptest_plot_algos_funcof_singles(axe: matplotlib.axes.Axes,
                                          title: Optional[str],
                                          showx: bool,
                                          showy: bool,
                                          results,
                                          algos: np.array,
                                          singles: np.array,
                                          orders: np.array,
                                          sizes: np.array,
                                          nrs: np.array) -> None:
    ptest_check_single_order(orders)
    ptest_check_single_size(sizes)
    ptest_check_single_nr(nrs)

    res = results[(results['nr'] == nrs[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['size'] == sizes[0])]
    for a in algos:
        ptest_plot_lin(axe,
                       title,
                       a,
                       'Distinct ratio' if showx else None,
                       'single',
                       showy,
                       res[res['algo'] == a])


def array_ptest_plot_algos_funcof_orders(axe: matplotlib.axes.Axes,
                                         title: Optional[str],
                                         showx: bool,
                                         showy: bool,
                                         results,
                                         algos: np.array,
                                         singles: np.array,
                                         orders: np.array,
                                         sizes: np.array,
                                         nrs: np.array) -> None:
    ptest_check_single_distinct(singles)
    ptest_check_single_size(sizes)
    ptest_check_single_nr(nrs)

    res = results[(results['nr'] == nrs[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['size'] == sizes[0])]
    for a in algos:
        ptest_plot_lin(axe,
                       title,
                       a,
                       'Order ratio' if showx else None,
                       'order',
                       showy,
                       res[res['algo'] == a])


def array_ptest_plot_algos_funcof_sizes(axe: matplotlib.axes.Axes,
                                        title: Optional[str],
                                        showx: bool,
                                        showy: bool,
                                        results,
                                        algos: np.array,
                                        singles: np.array,
                                        orders: np.array,
                                        sizes: np.array,
                                        nrs: np.array) -> None:
    ptest_check_single_distinct(singles)
    ptest_check_single_order(orders)
    ptest_check_single_nr(nrs)

    res = results[(results['nr'] == nrs[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['order'] == orders[0])]
    for a in algos:
        ptest_plot_lin(axe,
                       title,
                       a,
                       'Sample size' if showx else None,
                       'size',
                       showy,
                       res[res['algo'] == a])


def array_ptest_plot_algos_funcof_nrs(axe: matplotlib.axes.Axes,
                                      title: Optional[str],
                                      showx: bool,
                                      showy: bool,
                                      results,
                                      algos: np.array,
                                      singles: np.array,
                                      orders: np.array,
                                      sizes: np.array,
                                      nrs: np.array) -> None:
    ptest_check_single_distinct(singles)
    ptest_check_single_order(orders)
    ptest_check_single_size(sizes)

    res = results[(results['single'] == singles[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['size'] == sizes[0])]
    for a in algos:
        ptest_plot_log(axe,
                       title,
                       a,
                       2,
                       '#Samples' if showx else None,
                       'nr',
                       showy,
                       res[res['algo'] == a])


def array_ptest_plot_singles_funcof_orders(axe: matplotlib.axes.Axes,
                                           title: Optional[str],
                                           showx: bool,
                                           showy: bool,
                                           results,
                                           algos: np.array,
                                           singles: np.array,
                                           orders: np.array,
                                           sizes: np.array,
                                           nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_size(sizes)
    ptest_check_single_nr(nrs)

    res = results[(results['algo'] == algos[0]) & \
                  (results['size'] == sizes[0]) & \
                  (results['nr'] == nrs[0])]
    for s in singles:
        ptest_plot_lin(axe,
                       title,
                       '{}%'.format(s),
                       'Order ratio' if showx else None,
                       'order',
                       showy,
                       res[res['single'] == s])


def array_ptest_plot_singles_funcof_sizes(axe: matplotlib.axes.Axes,
                                          title: Optional[str],
                                          showx: bool,
                                          showy: bool,
                                          results,
                                          algos: np.array,
                                          singles: np.array,
                                          orders: np.array,
                                          sizes: np.array,
                                          nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_order(orders)
    ptest_check_single_nr(nrs)

    res = results[(results['algo'] == algos[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['nr'] == nrs[0])]
    for s in singles:
        ptest_plot_lin(axe,
                       title,
                       '{}%'.format(s),
                       'Sample size' if showx else None,
                       'size',
                       showy,
                       res[res['single'] == s])


def array_ptest_plot_singles_funcof_nrs(axe: matplotlib.axes.Axes,
                                        title: Optional[str],
                                        showx: bool,
                                        showy: bool,
                                        results,
                                        algos: np.array,
                                        singles: np.array,
                                        orders: np.array,
                                        sizes: np.array,
                                        nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_order(orders)
    ptest_check_single_size(sizes)

    res = results[(results['algo'] == algos[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['size'] == sizes[0])]
    for s in singles:
        ptest_plot_log(axe,
                       title,
                       '{}%'.format(s),
                       2,
                       '#Samples' if showx else None,
                       'nr',
                       showy,
                       res[res['single'] == s])


def array_ptest_plot_orders_funcof_singles(axe: matplotlib.axes.Axes,
                                           title: Optional[str],
                                           showx: bool,
                                           showy: bool,
                                           results,
                                           algos: np.array,
                                           singles: np.array,
                                           orders: np.array,
                                           sizes: np.array,
                                           nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_size(sizes)
    ptest_check_single_nr(nrs)

    res = results[(results['algo'] == algos[0]) & \
                  (results['nr'] == nrs[0]) & \
                  (results['size'] == sizes[0])]
    for o in orders:
        ptest_plot_lin(axe,
                       title,
                       '{}%'.format(o),
                       'Distinct ratio' if showx else None,
                       'single',
                       showy,
                       res[res['order'] == o])


def array_ptest_plot_orders_funcof_sizes(axe: matplotlib.axes.Axes,
                                         title: Optional[str],
                                         showx: bool,
                                         showy: bool,
                                         results,
                                         algos: np.array,
                                         singles: np.array,
                                         orders: np.array,
                                         sizes: np.array,
                                         nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_distinct(singles)
    ptest_check_single_nr(nrs)

    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['nr'] == nrs[0])]
    for o in orders:
        ptest_plot_lin(axe,
                       title,
                       '{}%'.format(o),
                       'Sample size' if showx else None,
                       'size',
                       showy,
                       res[res['order'] == o])


def array_ptest_plot_orders_funcof_nrs(axe: matplotlib.axes.Axes,
                                       title: Optional[str],
                                       showx: bool,
                                       showy: bool,
                                       results,
                                       algos: np.array,
                                       singles: np.array,
                                       orders: np.array,
                                       sizes: np.array,
                                       nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_distinct(singles)
    ptest_check_single_size(sizes)

    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['size'] == sizes[0])]
    for o in orders:
        ptest_plot_log(axe,
                       title,
                       '{}%'.format(o),
                       2,
                       '#Samples' if showx else None,
                       'nr',
                       showy,
                       res[res['order'] == o])



def array_ptest_plot_sizes_funcof_orders(axe: matplotlib.axes.Axes,
                                         title: Optional[str],
                                         showx: bool,
                                         showy: bool,
                                         results,
                                         algos: np.array,
                                         singles: np.array,
                                         orders: np.array,
                                         sizes: np.array,
                                         nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_distinct(singles)
    ptest_check_single_nr(nrs)

    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['nr'] == nrs[0])]
    for s in sizes:
        ptest_plot_lin(axe,
                       title,
                       '{} Bytes'.format(s),
                       'Order ratio' if showx else None,
                       'order',
                       showy,
                       res[res['size'] == s])

def array_ptest_plot_sizes_funcof_singles(axe: matplotlib.axes.Axes,
                                          title: Optional[str],
                                          showx: bool,
                                          showy: bool,
                                          results,
                                          algos: np.array,
                                          singles: np.array,
                                          orders: np.array,
                                          sizes: np.array,
                                          nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_order(orders)
    ptest_check_single_nr(nrs)

    res = results[(results['algo'] == algos[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['nr'] == nrs[0])]
    for s in sizes:
        ptest_plot_lin(axe,
                       title,
                       '{} Bytes'.format(s),
                       'Distinct ratio' if showx else None,
                       'single',
                       showy,
                       res[res['size'] == s])


def array_ptest_plot_sizes_funcof_nrs(axe: matplotlib.axes.Axes,
                                      title: Optional[str],
                                      showx: bool,
                                      showy: bool,
                                      results,
                                      algos: np.array,
                                      singles: np.array,
                                      orders: np.array,
                                      sizes: np.array,
                                      nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_distinct(singles)
    ptest_check_single_order(orders)

    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['order'] == orders[0])]
    for s in sizes:
        ptest_plot_log(axe,
                       title,
                       '{} Bytes'.format(s),
                       2,
                       '#Samples' if showx else None,
                       'nr',
                       showy,
                       res[res['size'] == s])


def array_ptest_plot_nrs_funcof_orders(axe: matplotlib.axes.Axes,
                                       title: Optional[str],
                                       showx: bool,
                                       showy: bool,
                                       results,
                                       algos: np.array,
                                       singles: np.array,
                                       orders: np.array,
                                       sizes: np.array,
                                       nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_distinct(singles)
    ptest_check_single_size(sizes)

    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['size'] == sizes[0])]
    for n in nrs:
        ptest_plot_lin(axe,
                       title,
                       n,
                       'Order ratio' if showx else None,
                       'order',
                       showy,
                       res[res['nr'] == n])



def array_ptest_plot_nrs_funcof_singles(axe: matplotlib.axes.Axes,
                                        title: Optional[str],
                                        showx: bool,
                                        showy: bool,
                                        results,
                                        algos: np.array,
                                        singles: np.array,
                                        orders: np.array,
                                        sizes: np.array,
                                        nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_order(orders)
    ptest_check_single_size(sizes)

    res = results[(results['algo'] == algos[0]) & \
                  (results['order'] == orders[0]) & \
                  (results['size'] == sizes[0])]
    for n in nrs:
        ptest_plot_lin(axe,
                       title,
                       n,
                       'Distinct ratio' if showx else None,
                       'single',
                       showy,
                       res[res['nr'] == n])



def array_ptest_plot_nrs_funcof_sizes(axe: matplotlib.axes.Axes,
                                      title: Optional[str],
                                      showx: bool,
                                      showy: bool,
                                      results,
                                      algos: np.array,
                                      singles: np.array,
                                      orders: np.array,
                                      sizes: np.array,
                                      nrs: np.array) -> None:
    ptest_check_single_algo(algos)
    ptest_check_single_distinct(singles)
    ptest_check_single_order(orders)

    res = results[(results['algo'] == algos[0]) & \
                  (results['single'] == singles[0]) & \
                  (results['order'] == orders[0])]
    for n in nrs:
        ptest_plot_lin(axe,
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


def array_ptest_single_plotter(funcof: list[str]) -> Callable[[matplotlib.axes.Axes,
                                                               Optional[str],
                                                               bool,
                                                               bool,
                                                               np.array,
                                                               np.array,
                                                               np.array,
                                                               np.array,
                                                               np.array,
                                                               np.array],
                                                              None]:
    plotter: object = ARRAY_PTEST_SINGLE_PLOTTERS
    for fld in funcof:
        if not (isinstance(plotter, dict) and (fld in plotter)):
            raise Exception("'{}': unexpected extra funcof plotter name"
                            " field".format(fld))
        assert fld in plotter
        plotter = plotter[fld]

    if isinstance(plotter, dict):
        raise Exception("'{}': funcof plotter name field "
                        "missing".format(','.join(funcof)))

    return cast(Callable[[matplotlib.axes.Axes,
                          Optional[str],
                          bool,
                          bool,
                          np.array,
                          np.array,
                          np.array,
                          np.array,
                          np.array,
                          np.array],
                         None],
                plotter)


def array_ptest_plots_title(group_field: Optional[str],
                            funcof_fields: list[str],
                            algos: np.array,
                            singles: np.array,
                            orders: np.array,
                            sizes: np.array,
                            nrs: np.array) -> str:
    if (group_field is not None) and (group_field in funcof_fields):
        raise Exception("'{}': unexpected duplicate plot group"
                        " field".format(group_field))

    lst = []
    for f in funcof_fields:
        if f == 'algo':
            lst.append('Algorithms')
        elif f == 'nr':
            lst.append('#Samples')
        elif f == 'single':
            lst.append('Distinct ratios')
        elif f == 'order':
            lst.append('Order ratios')
        elif f == 'size':
            lst.append('Sample sizes')
    sup = ' as a function of '.join(lst)

    lst = []
    flds = [ 'algo', 'nr', 'single', 'order', 'size' ]
    if group_field is not None:
        flds.remove(group_field)
    for f in funcof_fields:
        flds.remove(f)
    for f in flds:
        if f == 'algo':
            ptest_check_single_algo(algos)
            lst.append('Algorithm: {}'.format(algos[0]))
        elif f == 'nr':
            ptest_check_single_nr(nrs)
            lst.append('#Samples: {}'.format(nrs[0]))
        elif f == 'single':
            ptest_check_single_distinct(singles)
            lst.append('Distinct: {}%'.format(singles[0]))
        elif f == 'order':
            ptest_check_single_order(orders)
            lst.append('Order: {}%'.format(orders[0]))
        elif f == 'size':
            ptest_check_single_size(sizes)
            lst.append('Size: {} Bytes'.format(sizes[0]))
    sub = ' - '.join(lst)

    return sup + '\n' + sub


def array_ptest_show_single_plot(results,
                                 funcof: list[str],
                                 algos: np.array,
                                 singles: np.array,
                                 orders: np.array,
                                 sizes: np.array,
                                 nrs: np.array) -> None:
    plotter = array_ptest_single_plotter(funcof)

    fig, axe = plt.subplots(nrows = 1, ncols = 1, constrained_layout = True)
    title = array_ptest_plots_title(None,
                                    funcof,
                                    algos,
                                    singles,
                                    orders,
                                    sizes,
                                    nrs)
    fig.suptitle(title, fontweight = 'bold')
    plotter(axe, None, True, True, results, algos, singles, orders, sizes, nrs)
    plt.show()


def array_ptest_plot_algo_group(results: np.array,
                                title: str,
                                plotter,
                                algos: np.array,
                                singles: np.array,
                                orders: np.array,
                                sizes: np.array,
                                nrs: np.array) -> None:
    for indx, axe, showx, showy in ptest_group_plots(title, algos):
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
    for indx, axe, showx, showy in ptest_group_plots(title, singles):
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
    for indx, axe, showx, showy in ptest_group_plots(title, orders):
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
    for indx, axe, showx, showy in ptest_group_plots(title, sizes):
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
    for indx, axe, showx, showy in ptest_group_plots(title, nrs):
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


def array_ptest_parse_print_groupby(arg: str) -> list[str]:
    return ptest_parse_clause(arg, 'groupby', ARRAY_PTEST_PROPS)


def array_ptest_parse_plot_groupby(arg: str) -> str:
    if arg not in ARRAY_PTEST_GROUP_PLOTTERS.keys():
        raise argp.ArgumentError(None,
                                 "'" + arg + "': unexpected plot group field.")
    return arg


def array_ptest_parse_plot_funcof(arg: str) -> list[str]:
    return ptest_parse_clause(arg, 'funcof', ARRAY_PTEST_SINGLE_PLOTTERS)


def main():
    opt_parser = argp.ArgumentParser(add_help = False)
    opt_parser.add_argument('results_path',
                            type = str,
                            metavar = 'RESULTS_PATH',
                            help = 'Pathname to file where test results are '
                                   'stored.')
    opt_parser.add_argument('-a', '--algos',
                            dest = 'algo',
                            type = ptest_parse_str_list,
                            metavar = 'ALGORITHMS',
                            help = 'Comma separated list of algorithms')
    opt_parser.add_argument('-i', '--single-ratios',
                            dest = 'single',
                            type = ptest_parse_int_list,
                            metavar = 'SINGLE_RATIOS',
                            help = 'Comma separated list of distinct value ratios')
    opt_parser.add_argument('-r', '--order-ratios',
                            dest = 'order',
                            type = ptest_parse_int_list,
                            metavar = 'ORDER_RATIOS',
                            help = 'Comma separated list of ordering ratios')
    opt_parser.add_argument('-s', '--sizes',
                            dest = 'size',
                            type = ptest_parse_int_list,
                            metavar = 'SAMPLE_SIZES',
                            help = 'Comma separated list of sample sizes')
    opt_parser.add_argument('-n', '--samples',
                            dest = 'nr',
                            type = ptest_parse_int_list,
                            metavar = 'SAMPLE_COUNTS',
                            help = 'Comma separated list of number of samples')

    main_parser = argp.ArgumentParser(description = 'Stroll array performance '
                                                    'test visualization tool')
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

        algos = ptest_init_algos(results, args)
        nrs = ptest_init_nrs(results, args)
        singles = ptest_init_singles(results, args)
        orders = ptest_init_orders(results, args)
        sizes = ptest_init_sizes(results, args)

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
        print("{}: {}.".format(os.path.basename(sys.argv[0]), e),
              file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
