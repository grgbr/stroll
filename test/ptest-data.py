#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

import sys
import os
import argparse as argp
import random as rnd
import math
import statistics as stats
import struct
import io
import matplotlib
matplotlib.use('GTK3Agg')
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import matplotlib.colors as colors
import numpy as np

SAMPLES_MIN = 8

def generate(samples_nr: int, order_ratio: int, single_ratio: int) -> list[int]:
    assert samples_nr >= SAMPLES_MIN
    assert order_ratio >= 0 and order_ratio <= 100
    assert single_ratio >= 0 and single_ratio <= 100

    if (single_ratio < 100 and order_ratio < 50) or \
       (single_ratio == 0 and order_ratio != 100):
        raise Exception("Inconsistent distinct value and ordering ratios")

    snr = round(samples_nr * single_ratio / 100)
    if snr == 0:
        return [0] * samples_nr
    else:
        data = list(range(snr)) + rnd.choices(range(snr), k = samples_nr - snr)

    if order_ratio < 50:
        order_ratio = 100 - order_ratio
        data = list(reversed(data))
    else:
        data = sorted(data)

    if (order_ratio == 100):
        return data

    coef = -math.sqrt((200 * order_ratio) - 10000) + 100
    indices = rnd.sample(range(0, samples_nr),
                         max(int(samples_nr * coef / 100), 2))
    for i, j in zip(indices,indices[1:]):
        data[i], data[j] = data[j], data[i]

    return data


def eval_order(desc: dict[str,
                          int,
                          int,
                          int,
                          str,
                          list[int],
                          list[int],
                          list[int]]) -> None:
    assert len(desc['cmd']) > 0
    assert desc['endian'] in [ 'native', 'little', 'big' ]
    assert desc['order_ratio'] >= 0 and desc['order_ratio'] <= 100
    assert desc['single_ratio'] >= 0 and desc['single_ratio'] <= 100
    assert desc['samples_nr'] >= SAMPLES_MIN
    assert len(desc['data']) == desc['samples_nr']

    in_runs = []
    in_runs_index = -1
    rev_runs = []
    rev_runs_index = -1
    dup_runs = []
    dup_runs_index = -1
    for d in range(1, len(desc['data'])):
        if desc['data'][d] >= desc['data'][d - 1]:
            if in_runs_index < 0:
                in_runs_index = d
            if rev_runs_index >= 0:
                rev_runs.append(d - rev_runs_index)
                rev_runs_index = -1
        else:
            if in_runs_index >= 0:
                in_runs.append(d - in_runs_index)
                in_runs_index = -1
            if rev_runs_index < 0:
                rev_runs_index = d
        if desc['data'][d] == desc['data'][d - 1]:
            if dup_runs_index < 0:
                dup_runs_index = d
        else:
            if dup_runs_index >= 0:
                dup_runs.append(d - dup_runs_index)
                dup_runs_index = -1
    if in_runs_index >= 0:
        in_runs.append(d + 1 - in_runs_index)
    if rev_runs_index >= 0:
        rev_runs.append(d + 1 - rev_runs_index)
    if dup_runs_index >= 0:
        dup_runs.append(d + 1 - dup_runs_index)

    desc['in_runs'] = in_runs
    desc['rev_runs'] = rev_runs

    desc['single_nr'] = len(set(desc['data']))
    desc['dup_runs'] = dup_runs


def show_dup_runs(desc, count: int, stdio: io.TextIOWrapper) -> None:
    assert count >= SAMPLES_MIN

    runs = desc['dup_runs']
    dups_nr = count - desc['single_nr']
    dup_sum = sum(runs)

    print("Duplicates:")

    if dup_sum > 0:
        dup_bins = dict()
        for r in runs:
            if r in dup_bins.keys():
                dup_bins[r] += 1
            else:
                dup_bins[r] = 1
        dup_bins = dict(sorted(dup_bins.items()))

        print("    ratio:    {}/{} ({:.2f}%)\n"
              "    min run:  {}\n"
              "    avg run:  {:.1f}\n"
              "    med run:  {}\n"
              "    max run:  {}\n"
              "    run bins: {}".format(dups_nr,
                                        count,
                                        dups_nr * 100 / count,
                                        min(runs),
                                        dup_sum / len(runs),
                                        round(stats.median(runs)),
                                        max(runs),
                                        dup_bins),
              file = stdio)
    else:
        print("    ratio:    0/{} (0%)\n"
              "    min run:  0\n"
              "    avg run:  0\n"
              "    med run:  0\n"
              "    max run:  0\n"
              "    run bins: {{}}".format(count),
              file = stdio)


def show_order_runs(runs: list[int],
                    count: int,
                    title: str,
                    stdio: io.TextIOWrapper) -> None:
    assert count >= SAMPLES_MIN
    assert len(title) > 0

    order_sum = sum(runs)

    print("{}:".format(title), file = stdio)

    if order_sum > 0:
        order_bins = dict()
        for r in runs:
            if r in order_bins.keys():
                order_bins[r] += 1
            else:
                order_bins[r] = 1
        order_bins = dict(sorted(order_bins.items()))

        print("    ratio:    {}/{} ({:.2f}%)\n"
              "    min run:  {}\n"
              "    avg run:  {:.1f}\n"
              "    med run:  {}\n"
              "    max run:  {}\n"
              "    run bins: {}".format(order_sum,
                                        count - 1,
                                        order_sum * 100 / (count - 1),
                                        min(runs),
                                        order_sum / len(runs),
                                        stats.median(runs),
                                        max(runs),
                                        order_bins),
              file = stdio)
    else:
        print("    ratio:    0/{} (0%)\n"
              "    min run:  0\n"
              "    avg run:  0\n"
              "    med run:  0\n"
              "    max run:  0\n"
              "    run bins: {{}}".format(count - 1),
              file = stdio)


def show_data(desc: dict[str,
                         int,
                         int,
                         int,
                         str,
                         list[int],
                         list[int],
                         list[int]],
              stdio: io.TextIOWrapper) -> None:
    assert len(desc['cmd']) > 0
    assert desc['endian'] in [ 'native', 'little', 'big' ]
    assert desc['order_ratio'] >= 0 and desc['order_ratio'] <= 100
    assert desc['single_ratio'] >= 0 and desc['single_ratio'] <= 100
    assert desc['samples_nr'] >= SAMPLES_MIN
    assert len(desc['data']) == desc['samples_nr']
    assert len(desc['dup_runs']) >= 0
    assert len(desc['in_runs']) >= 0
    assert len(desc['rev_runs']) >= 0
    assert desc['single_nr'] >= 0 and desc['single_nr'] <= desc['samples_nr']

    cnt = len(desc['data'])

    print("Command line: '{}'\n"
          "Endianness:   {}\n"
          "#Samples:     {}\n"
          "#Distinct:    {}/{} ({:.2f}%)".format(desc['cmd'],
                                                 desc['endian'],
                                                 cnt,
                                                 desc['single_nr'],
                                                 cnt,
                                                 desc['single_nr'] * 100 / cnt),
          file = stdio)

    show_dup_runs(desc, cnt, stdio)
    show_order_runs(desc['in_runs'], cnt, 'In order', stdio)
    show_order_runs(desc['rev_runs'], cnt, 'Reverse order', stdio)


def struct_endian(endian: str) -> chr:
    assert endian in [ 'native', 'little', 'big' ]

    if endian == 'native':
        return '='
    elif endian == 'little':
        return '<'
    else:
        return '>'


def pack_str(string: str, endian: chr) -> bytes:
    length = len(string)
    assert length > 0 and length <= 0xffff
    assert endian in [ '=', '<', '>' ]

    return struct.pack("{}H{}s".format(endian, length),
                       length,
                       bytes(string, encoding='ascii'))


def pack_char(char: chr) -> bytes:
    return struct.pack("c", bytes(char, encoding='ascii'))


def pack_ushrt(ushrt: int, endian: chr) -> bytes:
    assert ushrt >= 0 and ushrt <= 0xffff
    assert endian in [ '=', '<', '>' ]

    return struct.pack("{}H".format(endian), ushrt)


def pack_uint(uint: int, endian: chr) -> bytes:
    assert uint >= 0 and uint <= 0xffffffff
    assert endian in [ '=', '<', '>' ]

    return struct.pack("{}I".format(endian), uint)


def pack_data(desc: dict[str,
                         int,
                         int,
                         int,
                         str,
                         list[int],
                         list[int],
                         list[int]]) -> bytes:
    assert len(desc['cmd']) > 0
    assert desc['endian'] in [ 'native', 'little', 'big' ]
    assert desc['order_ratio'] >= 0 and desc['order_ratio'] <= 100
    assert desc['single_ratio'] >= 0 and desc['single_ratio'] <= 100
    assert desc['samples_nr'] >= SAMPLES_MIN
    assert len(desc['data']) == desc['samples_nr']

    pack = bytes()
    e = struct_endian(desc['endian'])

    pack += pack_char(desc['endian'][0])
    pack += pack_ushrt(desc['order_ratio'], e)
    pack += pack_ushrt(desc['single_ratio'], e)
    pack += pack_uint(len(desc['data']), e)
    pack += pack_str(desc['cmd'], e)
    for d in desc['data']:
        pack += pack_uint(d, e)

    return pack


def unpack_char(data: bytes, off: int) -> tuple[chr, int]:
    assert off >= 0

    length = struct.calcsize('c')
    if length > (len(data) - off):
        raise Exception('Cannot unpack character: missing input data')

    char = struct.unpack_from('c', data, off)[0].decode(encoding = 'ascii')

    return char, length


def unpack_ushrt(data: bytes, off: int, endian: chr) -> tuple[int, int]:
    assert off >= 0
    assert endian in [ '=', '<', '>' ]

    fmt = "{}H".format(endian)
    length = struct.calcsize(fmt)
    if length > (len(data) - off):
        raise Exception('Cannot unpack unsigned short: missing input data')

    return struct.unpack_from(fmt, data, off)[0], length


def unpack_uint(data: bytes, off: int, endian: chr) -> tuple[int, int]:
    assert off >= 0
    assert endian in [ '=', '<', '>' ]

    fmt = "{}I".format(endian)
    length = struct.calcsize(fmt)
    if length > (len(data) - off):
        raise Exception('Cannot unpack unsigned integer: missing input data')

    return struct.unpack_from(fmt, data, off)[0], length


def unpack_str(data: bytes, off: int, endian: chr) -> tuple[str, int]:
    assert off >= 0
    assert endian in [ '=', '<', '>' ]

    length, consumed = unpack_ushrt(data, off, endian)
    if length <= 0:
        raise Exception('Cannot unpack string: unexpected length')

    fmt = "{}s".format(length)
    length = struct.calcsize(fmt) + consumed
    if length > (len(data) - off):
        raise Exception('Cannot unpack string: missing input data')

    string = struct.unpack_from(fmt, data, off + consumed)[0]
    return string.decode(encoding = 'ascii'), length


def unpack_data(desc: dict[str,
                           int,
                           int,
                           int,
                           str,
                           list[int],
                           list[int],
                           list[int]],
                data: bytes) -> None:
    if len(data) < 12:
        raise Exception("Invalid input data length")

    off = 0
    consumed = 0

    endian, consumed = unpack_char(data, off)
    if endian == 'n':
        desc['endian'] = 'native'
        endian = '='
    elif endian == 'l':
        desc['endian'] = 'little'
        endian = '<'
    elif endian == 'b':
        desc['endian'] = 'big'
        endian = '>'
    else:
        raise Exception("Unexpected input data endianness")

    off += consumed
    desc['order_ratio'], consumed = unpack_ushrt(data, off, endian)
    if desc['order_ratio'] < 0 or desc['order_ratio'] > 100:
        raise Exception("Unexpected input data ordering ratio "
                        "'{}'".format(desc['order_ratio']))

    off += consumed
    desc['single_ratio'], consumed = unpack_ushrt(data, off, endian)
    if desc['single_ratio'] < 0 or desc['single_ratio'] > 100:
        raise Exception("Unexpected input data distinct value ratio "
                        "'{}'".format(desc['single_ratio']))

    off += consumed
    desc['samples_nr'], consumed = unpack_uint(data, off, endian)
    if desc['samples_nr'] < SAMPLES_MIN:
        raise Exception("Unexpected input data number of samples "
                        "'{}'".format(desc['samples_nr']))

    off += consumed
    desc['cmd'], consumed = unpack_str(data, off, endian)

    if (off +
        consumed +
        struct.calcsize("{}{}I".format(endian, desc['samples_nr']))) != \
       len(data):
        raise Exception('Cannot unpack samples: invalid input data size')

    desc['data'] = [None] * desc['samples_nr']
    s = 0
    while s < desc['samples_nr']:
        off += consumed
        desc['data'][s], consumed = unpack_uint(data, off, endian)
        s += 1


def check_samples_nr(value):
    try:
        val = int(value)
        if val < SAMPLES_MIN:
            raise ValueError()
    except Exception:
        raise ValueError()
    return val


def check_ratio(value):
    try:
        val = int(value)
        if val < 0 or val > 100:
            raise ValueError()
    except Exception:
        raise ValueError()
    return val


def plot_data(desc: dict[str,
                         int,
                         int,
                         int,
                         str,
                         list[int],
                         list[int],
                         list[int]]) -> None:
    assert len(desc['cmd']) > 0
    assert desc['endian'] in [ 'native', 'little', 'big' ]
    assert desc['order_ratio'] >= 0 and desc['order_ratio'] <= 100
    assert desc['single_ratio'] >= 0 and desc['single_ratio'] <= 100
    assert desc['samples_nr'] >= SAMPLES_MIN
    assert len(desc['data']) == desc['samples_nr']
    assert len(desc['dup_runs']) >= 0
    assert len(desc['in_runs']) >= 0
    assert len(desc['rev_runs']) >= 0

    scat_axe = plt.subplot2grid((2, 2), (0, 0))
    scat_axe.set_title('Sample value dispersion')
    scat_axe.set_xlabel('#Samples')
    scat_axe.set_ylabel('Value')
    scat_axe.scatter(range(desc['samples_nr']), desc['data'], s = .5)

    if len(desc['dup_runs']) > 0:
        bmin = min(desc['dup_runs'])
        bmax = max(desc['dup_runs'])
        bstep =  max(int((bmax - bmin) / 10), 1)
        dup_hist, bins = np.histogram(desc['dup_runs'],
                                      range(bmin, bmax + (2 * bstep), bstep))
    else:
        bstep = 1
        dup_hist, bins = np.histogram([], [])
    dup_axe = plt.subplot2grid((2, 2), (1, 0))
    dup_axe.set_title('Duplicate run length distribution')
    dup_axe.set_xlabel('Run length')
    dup_axe.set_ylabel('#Run')
    dup_axe.set_xticks(bins)
    dup_axe.yaxis.set_major_locator(ticker.MaxNLocator(10))
    dup_axe.yaxis.set_minor_locator(ticker.MaxNLocator(50))
    dup_axe.grid(linestyle = '--', alpha = 0.4)
    dup_axe.bar(bins[:-1],
                dup_hist,
                width = bstep * 0.9,
                facecolor = colors.to_rgba('yellowgreen', alpha = 0.7),
                edgecolor = 'olivedrab')

    bmin = min(desc['in_runs'] + desc['rev_runs'])
    bmax = max(desc['in_runs'] + desc['rev_runs'])
    bstep =  max(int((bmax - bmin) / 10), 1)
    in_hist, bins = np.histogram(desc['in_runs'],
                                 range(bmin, bmax + (2 * bstep), bstep))
    rev_hist, _ = np.histogram(desc['rev_runs'], bins)

    in_axe = plt.subplot2grid((2, 2), (0, 1))
    in_axe.set_title('Ordering run length distribution')
    in_axe.set_xlabel('Run length')
    in_axe.set_ylabel('#Run')
    in_axe.set_xticks(bins)
    in_axe.yaxis.set_major_locator(ticker.MaxNLocator(10))
    in_axe.yaxis.set_minor_locator(ticker.MaxNLocator(50))
    in_axe.grid(linestyle = '--', alpha = 0.4)
    in_axe.bar(bins[:-1],
               in_hist,
               label = 'increasing',
               width = bstep * 0.9,
               facecolor = 'lightsteelblue',
               edgecolor = 'steelblue')
    in_axe.legend()

    rev_axe =  plt.subplot2grid((2, 2), (1, 1), sharex = in_axe)
    rev_axe.set_ylabel('#Run')
    rev_axe.set_xticks(bins)
    rev_axe.xaxis.tick_top()
    rev_axe.yaxis.set_major_locator(ticker.MaxNLocator(10))
    rev_axe.yaxis.set_minor_locator(ticker.MaxNLocator(50))
    rev_axe.invert_yaxis()
    rev_axe.grid(linestyle = '--', alpha = 0.4)
    rev_axe.bar(bins[:-1],
                rev_hist,
                label = 'decreasing',
                width = bstep * 0.9,
                facecolor = 'plum',
                edgecolor = 'orchid')
    rev_axe.legend()

    ttl = '#Samples {} - Order ratio {}% - Distinct value ratio {}%'
    plt.suptitle(ttl.format(desc['samples_nr'],
                            desc['order_ratio'],
                            desc['single_ratio']))
    plt.show()


def main():
    parser = argp.ArgumentParser(description = 'Stroll performance data set '
                                               'tool')

    subparser = parser.add_subparsers(dest = 'cmd',
                                      metavar = 'COMMAND',
                                      required = True)

    show_parser = subparser.add_parser('show',
                                       help = 'Display integer data set'
                                              ' properties')
    show_parser.add_argument('input',
                             type = str,
                             nargs = '?',
                             default = '-',
                             metavar = 'INPUT_PATH',
                             help = "Pathname to file where samples are stored "
                                    "(defaults to '-', i.e., stdin)")

    plot_parser = subparser.add_parser('plot',
                                       help = 'Plot integer data set'
                                              ' properties')
    plot_parser.add_argument('input',
                             type = str,
                             nargs = '?',
                             default = '-',
                             metavar = 'INPUT_PATH',
                             help = "Pathname to file where samples are stored "
                                    "(defaults to '-', i.e., stdin)")

    gen_parser = subparser.add_parser('generate',
                                      help = 'Generate integer data set')
    gen_parser.add_argument('-e', '--endian',
                            type = str,
                            choices = [ 'native', 'little', 'big' ],
                            default = 'native',
                            metavar = 'ENDIAN',
                            help = "Storage endianness (either 'native', "
                                   "'little' or 'big', defaults to 'native')")
    gen_parser.add_argument('-o', '--output',
                            type = str,
                            default = '-',
                            metavar = 'OUTPUT_PATH',
                            help = "Pathname to file where to store samples "
                                   "(defaults to '-', i.e., stdout)")
    gen_parser.add_argument('samples_nr',
                            type = check_samples_nr,
                            default = None,
                            metavar = 'SAMPLES_NR',
                            help = 'Number of samples to generate '
                                   '(SAMPLES_NR >= {})'.format(SAMPLES_MIN))
    gen_parser.add_argument('order_ratio',
                            type = check_ratio,
                            default = None,
                            metavar = 'ORDER_RATIO',
                            help = 'Samples ordering ratio '
                                   '(0 <= ORDER_RATIO <= 100)')
    gen_parser.add_argument('single_ratio',
                            type = check_ratio,
                            default = None,
                            metavar = 'SINGLE_RATIO',
                            help = 'Distinct sample values ratio '
                                   '(0 <= SINGLE_RATIO <= 100)')

    args = parser.parse_args()

    try:
        desc = {}
        if args.cmd == 'generate':
            rnd.seed(0, version = 2)

            desc['cmd'] = os.path.basename(sys.argv[0]) + \
                          ' ' + \
                          ' '.join(sys.argv[1:])
            desc['samples_nr'] = args.samples_nr
            desc['order_ratio'] = args.order_ratio
            desc['single_ratio'] = args.single_ratio
            desc['endian'] = args.endian
            data = generate(args.samples_nr,
                            args.order_ratio,
                            args.single_ratio)
            desc['data'] = data

            if args.output == '-':
                with os.fdopen(sys.stdout.fileno(), 'wb', closefd=False) as f:
                    f.write(pack_data(desc))
                    f.flush()
            else:
                with open(args.output, 'wb') as f:
                    f.write(pack_data(desc))
            eval_order(desc)
            show_data(desc, sys.stderr)
        elif args.cmd == 'show':
            if args.input == '-':
                with os.fdopen(sys.stdin.fileno(), 'rb', closefd=False) as f:
                    unpack_data(desc, f.read())
            else:
                with open(args.input, 'rb') as f:
                    unpack_data(desc, f.read())
            eval_order(desc)
            show_data(desc, sys.stdout)
        elif args.cmd == 'plot':
            if args.input == '-':
                with os.fdopen(sys.stdin.fileno(), 'rb', closefd=False) as f:
                    unpack_data(desc, f.read())
            else:
                with open(args.input, 'rb') as f:
                    unpack_data(desc, f.read())
            eval_order(desc)
            plot_data(desc)
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
