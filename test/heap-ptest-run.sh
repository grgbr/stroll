#!/bin/sh -e

HEAP_PTEST_BIN="${BINDIR:-@@BINDIR@@}/stroll-heap-ptest"
PTEST_DATA="${DATADIR:-@@DATADIR@@}/stroll"
PTEST_COMMON="${LIBEXECDIR:-@@LIBEXECDIR@@}/stroll/ptest-common.sh"

if [ ! -r "$PTEST_COMMON" ]; then
	echo "$(basename $0): '$PTEST_COMMON'" \
	     "Stroll common performance test definitions not found !" >&2
	exit 1
fi

. $PTEST_COMMON


ptest_parse_awk='
BEGIN               {
	op=""; imean=-1; emean=-1; hmean=-1; rmean=-1; pmean=-1; dmean=-1
}
/^Algorithm/        { algo=$2 }
/^#Samples/         { nr=$2 }
/^Distinct ratio/   { single=$2 }
/^Order ratio/      { order=$2 }
/^Data size/        { size=$2 }
/^heapify/          { op="heapify" }
/^insert/           { op="insert" }
/^extract/          { op="extract" }
/^remove/           { op="remove" }
/^promote/          { op="promote" }
/^demote/           { op="demote" }
/^[[:blank:]]+Mean/ {
	if (op == "heapify")
		hmean=$2
	else if (op == "insert")
		imean=$2
	else if (op == "extract")
		emean=$2
	else if (op == "remove")
		rmean=$2
	else if (op == "promote")
		pmean=$2
	else if (op == "demote")
		dmean=$2
}
END                 {
	if (hmean > 0)
		printf("%-8s %10u         %3u      %3u    %4u heapify   %10u\n", algo, nr, single, order, size, hmean)
	if (imean > 0)
		printf("%-8s %10u         %3u      %3u    %4u insert    %10u\n", algo, nr, single, order, size, imean)
	if (emean > 0)
		printf("%-8s %10u         %3u      %3u    %4u extract   %10u\n", algo, nr, single, order, size, emean)
	if (rmean > 0)
		printf("%-8s %10u         %3u      %3u    %4u remove    %10u\n", algo, nr, single, order, size, rmean)
	if (pmean > 0)
		printf("%-8s %10u         %3u      %3u    %4u promote   %10u\n", algo, nr, single, order, size, pmean)
	if (dmean > 0)
		printf("%-8s %10u         %3u      %3u    %4u demote    %10u\n", algo, nr, single, order, size, dmean)
}
'

heap_ptest_run()
{
	local algo="$1"
	local path="$2"
	local size=$3
	local prio=""
	local loops=$5
	local out

	if [ $4 -gt 0 ]; then
		prio="--prio $4"
	fi

	if ! out=$($HEAP_PTEST_BIN $prio "$path" "$algo" $size $loops); then
		return 1
	fi
	if ! echo -n "$out" | awk -F': *' "$ptest_parse_awk"; then
		return 1
	fi

	return 0
}

algos="fbheap fwheap hprheap drpheap dprheap pprheap dbnheap"

if [ $show -eq 1 ]; then
	echo "#Samples:        $nr"
	echo "Distinct ratios: $singles"
	echo "Order ratios:    $orders"
	echo "Data sizes:      $sizes"
	echo "Algorithms:      $algos"
	exit 0
fi

if [ $# -ge 1 ]; then
	for a in $*; do
		if ! echo "$algos" | grep -w -q $a; then
			error "unknown algorithm '$a'."
			exit 1
		fi
	done
	algos="$*"
fi

if [ ! -x "$HEAP_PTEST_BIN" ]; then
	error "performance test runner not found." >&2
	exit 1
fi

echo "Running Stroll heap performance tests with following settings:" >$output
echo "#Samples:        $nr" >>$output
echo "Distinct ratios: $singles" >>$output
echo "Order ratios:    $orders" >>$output
echo "Data sizes:      $sizes" >>$output
echo "Algorithms:      $algos" >>$output
echo "#Loops:          $loops" >>$output

echo >>$output
echo "Algorithm  #Samples Distinct(%) Order(%) Size(B) Operation   Mean(ns)" >>$output

#set -o pipefail
for a in $algos; do
	for n in $nr; do
		for o in $orders; do
			for i in $singles; do
				path=$(ptest_data_path \
				       "$ptest_data_base" \
				       $n \
				       $i \
				       $o)
				if [ ! -r "$path" ]; then
					continue
				fi
				for s in $sizes; do
					heap_ptest_run "$a" \
						       "$path" \
						       "$s" \
						       "$prio" \
						       "$loops" \
						       >>$output
				done
			done
		done
	done
done
