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

# Skip the first 6 lines and comment lines starting with the '#' character.
# Remove duplicate spaces so that the CSV reader object may use space as the
# delimiter.
def array_ptest_xform_data(csvfile: io.TextIOWrapper) -> list[str]:
    return list(map(lambda line: ' '.join(line.split()),
                filter(lambda row: row[0]!='#' and row[0]!='-',
                       csvfile.readlines()[6:])))


def array_ptest_load(path: str) -> np.ndarray:
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
        array_ptest_plot(results, algo, order, s, axe, "{} Bytes".format(s))
    array_ptest_finish_plot(axe)


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
    fig.suptitle(title)

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


def array_ptest_plot_algo_byorder(results,
                                  algos: list[str],
                                  orders: list[int],
                                  sizes: list[int]) -> None:
    for a in algos:
        for indx, \
            axe, \
            showx, \
            showy in array_ptest_group_plots("{} algorithm".format(a), orders):
                array_ptest_plot_sizes(results,
                                       a,
                                       orders[indx],
                                       sizes,
                                       "{}% order".format(orders[indx]),
                                       axe,
                                       showx,
                                       showy)


def array_ptest_plot_order_byalgo(results,
                                  algos: list[str],
                                  orders: list[int],
                                  sizes: list[int]) -> None:
    for o in orders:
        for indx, \
            axe, \
            showx, \
            showy in array_ptest_group_plots("{}% order".format(o), algos):
                array_ptest_plot_sizes(results,
                                       algos[indx],
                                       o,
                                       sizes,
                                       algos[indx],
                                       axe,
                                       showx,
                                       showy)


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
                                  algos: list[str],
                                  orders: list[int],
                                  sizes: list[int]) -> None:
    for o in orders:
        for indx, \
            axe, \
            showx, \
            showy in array_ptest_group_plots("{}% order".format(o), sizes):
                array_ptest_plot_algos(results,
                                       o,
                                       sizes[indx],
                                       algos,
                                       "{} Bytes".format(sizes[indx]),
                                       axe,
                                       showx,
                                       showy)


def array_ptest_plot_size_byorder(results,
                                  algos: list[str],
                                  orders: list[int],
                                  sizes: list[int]) -> None:
    for s in sizes:
        for indx, \
            axe, \
            showx, \
            showy in array_ptest_group_plots("{} Bytes".format(s), orders):
                array_ptest_plot_algos(results,
                                       orders[indx],
                                       s,
                                       algos,
                                       "{}% order".format(orders[indx]),
                                       axe,
                                       showx,
                                       showy)


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
        array_ptest_plot(results, algo, o, size, axe, "{}% order".format(o))
    array_ptest_finish_plot(axe)


def array_ptest_plot_algo_bysize(results,
                                 algos: list[str],
                                 orders: list[int],
                                 sizes: list[int]) -> None:
    for a in algos:
        for indx, \
            axe, \
            showx, \
            showy in array_ptest_group_plots("{} algorithm".format(a), sizes):
                array_ptest_plot_orders(results,
                                        a,
                                        sizes[indx],
                                        orders,
                                        "{} Bytes".format(sizes[indx]),
                                        axe,
                                        showx,
                                        showy)


def array_ptest_plot_size_byalgo(results,
                                 algos: list[str],
                                 orders: list[int],
                                 sizes: list[int]) -> None:
    for s in sizes:
        for indx, \
            axe, \
            showx, \
            showy in array_ptest_group_plots("{} Bytes".format(s), algos):
                array_ptest_plot_orders(results,
                                        algos[indx],
                                        s,
                                        orders,
                                        algos[indx],
                                        axe,
                                        showx,
                                        showy)


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
                     orders: np.array,
                     sizes: np.array,
                     nrs: np.array) -> None:
    print('FIELD  DESCRIPTION      VALUES')
    array_ptest_print_1darray(algos,  'algo   ', 'algorithms       ')
    array_ptest_print_1darray(nrs,    'nr     ', '#Samples:        ')
    array_ptest_print_1darray(orders, 'order  ', 'Ordering ratios  ')
    array_ptest_print_1darray(sizes,  'size   ', 'Sample sizes:    ')


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
                      orders: np.array,
                      sizes: np.array,
                      nrs: np.array) -> None:
    flds = groupby + [f for f in ARRAY_PTEST_PROPS.keys() if f not in groupby]
    fmt = ''.join([ARRAY_PTEST_PROPS[f]['format'] for f in flds])

    print(''.join([ARRAY_PTEST_PROPS[f]['label'] for f in flds])[2:])

    res = array_ptest_select(results, algos, orders, sizes, nrs)
    for r in np.sort(res, order = flds):
        print(fmt[2:].format(*[ARRAY_PTEST_PROPS[f]['get'](r) for f in flds]))


ARRAY_PTEST_PLOTTERS = {
        'algo':  {
            'order': array_ptest_plot_algo_byorder,
            'size':  array_ptest_plot_algo_bysize
        },
        'order': {
            'algo':  array_ptest_plot_order_byalgo,
            'size':  array_ptest_plot_order_bysize
        },
        'size': {
            'algo':  array_ptest_plot_size_byorder,
            'order': array_ptest_plot_size_byalgo
        }
}


def array_ptest_show_plots(results,
                           groupby: list[str],
                           algos: np.array,
                           orders: np.array,
                           sizes: np.array,
                           nrs: np.array) -> None:
    plotter = ARRAY_PTEST_PLOTTERS
    for fld in groupby:
        if not isinstance(plotter, dict):
            raise Exception("'{}': unexpected extra groupby clause"
                            " field".format(','.join(groupby)))
        assert fld in plotter
        plotter = plotter[fld]

    if isinstance(plotter, dict):
        raise Exception("'{}': groupby clause field "
                        "missing".format(','.join(groupby)))

    plotter(results, algos, orders, sizes)
    plt.show()


def array_ptest_parse_str_list(arg: str) -> list[str]:
    return [a.strip() for a in arg.split(',')]


def array_ptest_parse_groupby(arg: str, allowed: dict) -> list[str]:
    lst = array_ptest_parse_str_list(arg)
    if len(set(lst)) != len(lst):
        raise argp.ArgumentError(
            None,
            "'" + f + "': unexpected groupby clause duplicate field(s).")
    for f in lst:
        if f not in allowed:
            raise argp.ArgumentError(
                None,
                "'" + f + "': unexpected groupby clause field.")
    return lst


def array_ptest_parse_print_groupby(arg: str) -> list[str]:
    return array_ptest_parse_groupby(arg, ARRAY_PTEST_PROPS)


def array_ptest_parse_plot_groupby(arg: str) -> list[str]:
    return array_ptest_parse_groupby(arg, ARRAY_PTEST_PLOTTERS)


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
                              default = 'algo,nr,order,size',
                              metavar = 'RESULT_FIELDS',
                              help = 'Comma separated list of result fields to '
                                     'group results by')

    plot_parser = cmd_parser.add_parser('plot',
                                        parents = [opt_parser],
                                        help = 'Plot test results')
    plot_parser.add_argument('-g', '--groupby',
                             dest = 'groupby',
                             type = array_ptest_parse_plot_groupby,
                             default = 'size,algo',
                             metavar = 'RESULT_FIELDS',
                             help = 'Comma separated list of result fields to '
                                    'group results by')


    args = main_parser.parse_args()

    try:
        results = array_ptest_load(args.results_path)

        algos = array_ptest_init_prop(results, 'algo', args)
        if len(algos) == 0:
            raise Exception('Unexpected list of algorithm(s) specified')
        orders = array_ptest_init_prop(results, 'order', args)
        if len(orders) == 0:
            raise Exception('Unexpected list of ordering ratio(s) specified')
        sizes = array_ptest_init_prop(results, 'size', args)
        if len(sizes) == 0:
            raise Exception('Unexpected list of sample size(s) specified')
        nrs = array_ptest_init_prop(results, 'nr', args)
        if len(nrs) == 0:
            raise Exception('Unexpected list of numbers of samples specified')

        if args.cmd == 'list':
            array_ptest_list(results, algos, orders, sizes, nrs)
        elif args.cmd == 'print':
            array_ptest_print(results, args.groupby, algos, orders, sizes, nrs)
        elif args.cmd == 'plot':
            array_ptest_show_plots(results,
                                   args.groupby,
                                   algos,
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
