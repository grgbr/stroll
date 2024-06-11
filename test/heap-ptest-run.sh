#!/bin/sh -e

HEAP_PTEST_BIN="${BINDIR:-@@BINDIR@@}/stroll-heap-ptest"
ARRAY_PTEST_DATA="${DATADIR:-@@DATADIR@@}/stroll"
ARRAY_PTEST_COMMON="${LIBEXECDIR:-@@LIBEXECDIR@@}/stroll/ptest-common.sh"

if [ ! -r "$ARRAY_PTEST_COMMON" ]; then
	echo "$(basename $0): '$ARRAY_PTEST_COMMON'" \
	     "Stroll common performance test definitions not found !" >&2
	exit 1
fi

. $ARRAY_PTEST_COMMON


ptest_parse_awk='
BEGIN               { op=""; imean=-1; emean=-1; hmean=-1 }
/^Algorithm/        { algo=$2 }
/^#Samples/         { nr=$2 }
/^Distinct ratio/   { single=$2 }
/^Order ratio/      { order=$2 }
/^Data size/        { size=$2 }
/^Heapify/          { op="heapify" }
/^Insert/           { op="insert" }
/^Extract/          { op="extract" }
/^[[:blank:]]+Mean/ {
	if (op == "heapify")
		hmean=$2
	else if (op == "insert")
		imean=$2
	else if (op == "extract")
		emean=$2
}
END                 {
	if (hmean > 0)
		printf("%-8s %10u         %3u      %3u    %4u Heapify   %10u\n", algo, nr, single, order, size, hmean)
	if (imean > 0)
		printf("%-8s %10u         %3u      %3u    %4u Insert    %10u\n", algo, nr, single, order, size, imean)
	if (emean > 0)
		printf("%-8s %10u         %3u      %3u    %4u Extract   %10u\n", algo, nr, single, order, size, emean)
}
'

stroll_array_ptest_run()
{
	local algo="$1"
	local path="$2"
	local size=$3
	local prio=""
	local loops=$5

	if [ $4 -gt 0 ]; then
		prio="--prio $4"
	fi

	$HEAP_PTEST_BIN $prio "$path" "$algo" $size $loops | \
	awk -F': *' "$ptest_parse_awk"
}

algos="fbheap"

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
	# When not explicitly required, restrict number of test data samples to
	# 8192 since these may take quite a long time to complete (up to several
	# tenth of minutes...).
	maxnr=$(echo $nr | awk '{print $NF}')
	if [ -z "$req_nr" ]; then
		if echo "$simple_algos" | grep -qw "$a"; then
			maxnr=4096
		fi
	fi
	for n in $nr; do
		if [ $n -gt $maxnr ]; then
			continue
		fi
		for o in $orders; do
			for i in $singles; do
				path=$(stroll_array_ptest_data_path \
				       "$ptest_data_base" \
				       $n \
				       $i \
				       $o)
				if [ ! -r "$path" ]; then
					continue
				fi
				for s in $sizes; do
					stroll_array_ptest_run "$a" "$path" $s $prio $loops >>$output
				done
			done
		done
	done
done
