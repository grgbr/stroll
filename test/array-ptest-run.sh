#!/bin/sh -e

ARRAY_PTEST_BIN="${BINDIR:-@@BINDIR@@}/stroll-array-ptest"
SLIST_PTEST_BIN="${BINDIR:-@@BINDIR@@}/stroll-slist-ptest"
ARRAY_PTEST_DATA="${DATADIR:-@@DATADIR@@}/stroll"
ARRAY_PTEST_COMMON="${LIBEXECDIR:-@@LIBEXECDIR@@}/stroll/ptest-common.sh"

if [ ! -r "$ARRAY_PTEST_COMMON" ]; then
	echo "$(basename $0): '$ARRAY_PTEST_COMMON'" \
	     "Stroll common performance test definitions not found !" >&2
	exit 1
fi

. $ARRAY_PTEST_COMMON

ptest_parse_awk='
/^Algorithm/      { algo=$2 }
/^#Samples/       { nr=$2 }
/^Distinct ratio/ { single=$2 }
/^Order ratio/    { order=$2 }
/^Data size/      { size=$2 }
/^Mean/           { mean=$2 }
END               {
	printf("%-16s %10u         %3u      %3u    %4u %10u\n", algo, nr, single, order, size, mean)
}
'

stroll_array_ptest_run()
{
	local algo="$1"
	local path="$2"
	local size=$3
	local prio=""
	local loops=$5
	local kind=$(echo "$algo" | cut -f1 -d'_')
	local bin
	local out

	if [ $4 -gt 0 ]; then
		prio="--prio $4"
	fi

	if [ "$kind" = "array" ]; then
		bin="$ARRAY_PTEST_BIN"
	elif [ "$kind" = "slist" ]; then
		bin="$SLIST_PTEST_BIN"
	else
		return 1
	fi

	if ! out=$($bin $prio "$path" "$algo" $size $loops); then
		return 1
	fi
	if ! echo -n "$out" | awk -F': *' "$ptest_parse_awk"; then
		return 1
	fi

	return 0
}

simple_algos=\
"array_bubble array_select array_insert "\
"slist_bubble slist_select slist_insert"

algos=\
"array_qsort array_quick array_3wquick array_merge array_fbheap array_fwheap "\
"slist_merge "\
"$simple_algos"

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

if [ ! -x "$ARRAY_PTEST_BIN" ]; then
	error "array performance test runner not found." >&2
	exit 1
fi
if [ ! -x "$SLIST_PTEST_BIN" ]; then
	error "singly linked list performance test runner not found." >&2
	exit 1
fi

echo "Running Stroll array performance tests with following settings:" >$output
echo "#Samples:        $nr" >>$output
echo "Distinct ratios: $singles" >>$output
echo "Order ratios:    $orders" >>$output
echo "Data sizes:      $sizes" >>$output
echo "Algorithms:      $algos" >>$output
echo "#Loops:          $loops" >>$output

echo >>$output
echo "Algorithm          #Samples Distinct(%) Order(%) Size(B)   Mean(ns)" >>$output

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
					if ! stroll_array_ptest_run "$a" "$path" $s $prio $loops >>$output; then
						exit 1
					fi
				done
			done
		done
	done
done
