#!/bin/sh -e

SORT_PTEST_BIN="${BINDIR:-@@BINDIR@@}/stroll-sort-ptest"
SORT_PTEST_DATA="${DATADIR:-@@DATADIR@@}/stroll"
SORT_PTEST_COMMON="${LIBEXECDIR:-@@LIBEXECDIR@@}/stroll/ptest-common.sh"

if [ ! -r "$SORT_PTEST_COMMON" ]; then
	echo "$(basename $0): '$SORT_PTEST_COMMON'" \
	     "Stroll common performance test definitions not found !" >&2
	exit 1
fi

. $SORT_PTEST_COMMON

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

sort_ptest_run()
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

	if ! out=$($SORT_PTEST_BIN $prio "$path" "$algo" $size $loops); then
		return 1
	fi
	if ! echo -n "$out" | awk -F': *' "$ptest_parse_awk"; then
		return 1
	fi

	return 0
}

sort_algos='
array_bubble     4096
array_select     4096
array_insert     4096
slist_bubble     4096
slist_select     4096
slist_insert     4096
dlist_bubble     4096
dlist_select     4096
array_qsort   1048576
array_quick   1048576
array_3wquick 1048576
array_merge   1048576
array_fbheap  1048576
array_fwheap  1048576
slist_merge   1048576
'

sort_list_algos()
{
	local name
	local dummy

	echo "$sort_algos" | awk '{ printf(" %s", $1) }' | tail -c +2
}

sort_algo_exists()
{
	local name="$1"
	local algo
	local dummy

	while read algo dummy; do
		if [ "$algo" = "$name" ]; then
			return 0
		fi
	done <<-_EOF
	$sort_algos
	_EOF

	return 1
}

sort_algo_max_nr()
{
	local name="$1"
	local algo
	local max_nr
	local dummy

	while read algo max_nr dummy; do
		if [ "$algo" = "$name" ]; then
			echo $max_nr
		fi
	done <<-_EOF
	$sort_algos
	_EOF
}

algos=$(sort_list_algos)

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
		if ! sort_algo_exists "$a"; then
			error "unknown algorithm '$a'."
			exit 1
		fi
	done
	algos="$*"
fi

if [ ! -x "$SORT_PTEST_BIN" ]; then
	error "sorting performance test runner not found." >&2
	exit 1
fi

echo "Running Stroll sorting performance tests with following settings:" >$output
echo "#Samples:        $nr" >>$output
echo "Distinct ratios: $singles" >>$output
echo "Order ratios:    $orders" >>$output
echo "Data sizes:      $sizes" >>$output
echo "Algorithms:      $algos" >>$output
echo "#Loops:          $loops" >>$output

echo >>$output
echo "Algorithm          #Samples Distinct(%) Order(%) Size(B)   Mean(ns)" >>$output

for a in $algos; do
	# Restrict number of test data samples to 8192 since these may take
	# quite a long time to complete (up to several tenth of minutes...).
	maxnr=$(sort_algo_max_nr "$a")
	for n in $nr; do
		if [ $n -gt $maxnr ]; then
			continue
		fi
		for o in $orders; do
			for i in $singles; do
				path=$(sort_ptest_data_path \
				       "$ptest_data_base" \
				       "$n" \
				       "$i" \
				       "$o")
				if [ ! -r "$path" ]; then
					continue
				fi
				for s in $sizes; do
					if ! sort_ptest_run "$a" \
					                    "$path" \
					                    "$s" \
					                    "$prio" \
					                    "$loops" \
					                    >>$output;
					then
						exit 1
					fi
				done
			done
		done
	done
done
