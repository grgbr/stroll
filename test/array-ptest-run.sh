#!/bin/sh -e

ARRAY_PTEST_BIN="${BINDIR:-@@BINDIR@@}/stroll-array-ptest"
ARRAY_PTEST_DATA="${DATADIR:-@@DATADIR@@}/stroll"

ECHOE="/bin/echo -e"

log()
{
	$ECHOE "$*" >&2
}

error()
{
	$ECHOE "$arg0: $*" >&2
}

array_ptest_parse_nr()
{
	local args="$1"
	local a

	if ! args=$($ECHOE -n "$args" | tr -s ',' ' ' | sort -u -n); then
		return 1
	fi

	for a in $args; do
		local val=$(echo "$a" | sed -n "s/[0-9]*//gp")

		if [ -n "$val" ]; then
			error "invalid number of samples '$a':" \
			      "integer expected.\n"
			return 1
		fi
		if [ $((a)) -lt 8 ]; then
			error "invalid number of samples '$a':" \
			      "integer >= 8 expected.\n"
			return 1
		fi
	done

	echo $args
}

array_ptest_parse_orders()
{
	local args="$1"
	local a

	if ! args=$($ECHOE -n "$args" | tr -s ',' ' ' | sort -u -n); then
		return 1
	fi

	for a in $args; do
		local val=$(echo "$a" | sed -n "s/[0-9]*//gp")

		if [ -n "$val" ]; then
			error "invalid ordering ratio '$a': integer expected.\n"
			return 1
		fi
		if [ $((a)) -gt 100 ]; then
			error "invalid ordering ratio '$a':" \
			      "positive integer <= 100 expected.\n"
			return 1
		fi
	done

	echo $args
}

array_ptest_parse_singles()
{
	local args="$1"
	local a

	if ! args=$($ECHOE -n "$args" | tr -s ',' ' ' | sort -u -n); then
		return 1
	fi

	for a in $args; do
		local val=$(echo "$a" | sed -n "s/[0-9]*//gp")

		if [ -n "$val" ]; then
			error "invalid distinct value ratio '$a': " \
			      "integer expected.\n"
			return 1
		fi
		if [ $((a)) -gt 100 ]; then
			error "invalid distinct value ratio '$a':" \
			      "positive integer <= 100 expected.\n"
			return 1
		fi
	done

	echo $args
}

array_ptest_parse_prio()
{
	local arg="$1"
	local val

	val=$(echo "$arg" | sed -n "s/[0-9]*//gp")
	if [ -n "$val" ]; then
		error "invalid scheduling priority '$arg': integer expected.\n"
		return 1
	fi
	if [ $((arg)) -lt 1 ] || [ $((arg)) -gt 99 ]; then
		error "invalid scheduling priority '$arg':" \
		      "1 <= integer <= 99 expected.\n"
		return 1
	fi

	echo $arg
}

array_ptest_parse_sizes()
{
	local args="$1"

	if ! args=$($ECHOE -n "$args" | tr -s ',' ' ' | sort -u -n); then
		return 1
	fi

	for a in $args; do
		local val=$(echo "$a" | sed -n "s/[0-9]*//gp")

		if [ -n "$val" ]; then
			error "invalid data size '$a': integer expected.\n"
			return 1
		fi
		if [ $((a % 4)) -ne 0 ]; then
			error "invalid data size '$a': " \
			      "multiple of 4 expected.\n"
			return 1
		fi
	done

	echo $args
}

array_ptest_probe_nr()
{
	local expr="s@${ptest_data_base}_n\([0-9]\+\)_s[0-9]\+_o[0-9]\+\.dat@\1\n@gp"

	echo -n $@ | sed --silent $expr | sort -b -u -n | xargs
}

array_ptest_probe_orders()
{
	local expr="s@${ptest_data_base}_n[0-9]\+_s[0-9]\+_o\([0-9]\+\)\.dat@\1\n@gp"

	echo -n $@ | sed --silent $expr | sort -b -u -n | xargs
}

array_ptest_probe_singles()
{
	local expr="s@${ptest_data_base}_n[0-9]\+_s\([0-9]\+\)_o[0-9]\+\.dat@\1\n@gp"

	echo -n $@ | sed --silent $expr | sort -b -u -n | xargs
}

stroll_array_ptest_data_path()
{
	local base="$1"
	local nr=$2
	local single=$3
	local order=$4

	echo ${base}_n${nr}_s${single}_o${order}.dat
}

ptest_parse_awk='
/^Algorithm/      { algo=$2 }
/^#Samples/       { nr=$2 }
/^Distinct ratio/ { single=$2 }
/^Order ratio/    { order=$2 }
/^Data size/      { size=$2 }
/^Mean/           { mean=$2 }
END               {
	printf("%-8s %10u         %3u      %3u    %4u %10u\n", algo, nr, single, order, size, mean)
}
'

stroll_array_ptest_run()
{
	local algo="$1"
	local path="$2"
	local size=$3
	local prio=""

	if [ $4 -gt 0 ]; then
		prio="--prio $4"
	fi

	$ARRAY_PTEST_BIN $prio "$path" "$algo" $size 10000 | \
	awk -F': *' "$ptest_parse_awk"
}

usage()
{
	cat >&2 <<_EOF
Usage: $arg0 [OPTIONS] [ALGORITHM...]
Run Stroll performance tests.

With:
    ALGORITHM  space separated list of sorting algorithms
Where OPTIONS:
    -d | --datadir        pathname to input test data directory
    -i | --distinct-ratio comma separated list of distinct value ratios
    -l | --list           show available optional settings
    -n | --number         comma separated list of number of samples
    -o | --output         pathname to output result file
    -r | --order-ratio    comma separated list of ordering ratios
    -s | --size           comma separated list of sample sizes
    -h | --help           this help message
_EOF
}

arg0="$(basename $0)"

cmdln=$(getopt --options d:ln:i:o:p:r:s:h \
               --longoptions datadir:,list,number:,output:,prio:,distinct-ratio:,order-ratio:,size:,help \
               --name "$arg0" \
               -- "$@")
if [ $? -gt 0 ]; then
	usage
	exit 1
fi

show=0
prio=0
req_nr=
req_ord=
req_sngl=
req_sz=
output="/dev/stdout"
ptest_data_base="$ARRAY_PTEST_DATA/stroll_ptest"
eval set -- "$cmdln"
while true; do
	case "$1" in
	-d|--datadir)
		ptest_data_base="$2/stroll_ptest"
		shift 2;;
	-l|--list)
		show=1; shift;;
	-n|--number)
		if ! req_nr=$(array_ptest_parse_nr "$2"); then
			usage
			exit 1
		fi
		shift 2;;
	-o|--output)
		if [ "$2" != "-" ]; then
			output="$2"
		fi
		shift 2;;
	-p|--prio)
		if ! prio=$(array_ptest_parse_prio "$2"); then
			usage
			exit 1
		fi
		shift 2;;
	-r|--order-ratio)
		if ! req_ord=$(array_ptest_parse_orders "$2"); then
			usage
			exit 1
		fi
		shift 2;;
	-i|--distinct-ratio)
		if ! req_sngl=$(array_ptest_parse_singles "$2"); then
			usage
			exit 1
		fi
		shift 2;;
	-s|--size)
		if ! req_sz=$(array_ptest_parse_sizes "$2"); then
			usage
			exit 1
		fi
		shift 2;;
	-h|--help)
		usage
		exit 0;;
	--)
		shift;
		break;;
	*)
		break;;
	esac
done

if [ ! -d $(dirname "$ptest_data_base") ] || \
   [ ! -x $(dirname "$ptest_data_base") ]; then
	error "invalid test data directory '$(dirname $ptest_data_base)'."
	exit 1
fi

array_ptest_data_files="${ptest_data_base}_*"
nr=$(array_ptest_probe_nr $array_ptest_data_files)
orders=$(array_ptest_probe_orders $array_ptest_data_files)
singles=$(array_ptest_probe_singles $array_ptest_data_files)
sizes="4 8 16 32 64 128 256"
simple_algos="bubble select insert"
algos="qsort quick 3wquick merge $simple_algos"

if [ -z "$nr" ] || [ -z "$singles" ] || [ -z "$orders" ]; then
	error "no data file found under test data directory" \
	      "'$(dirname $ptest_data_base)'."
	exit 1
fi

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

if [ -n "$req_nr" ]; then
	for n in $req_nr; do
		if ! echo "$nr" | grep -w -q $n; then
			error "invalid number of samples '$n':" \
			      "no matching sample file."
			exit 1
		fi
	done
	nr=$req_nr
fi

if [ -n "$req_sngl" ]; then
	for s in $req_sngl; do
		if ! echo "$singles" | grep -w -q $s; then
			error "invalid ordering ratio '$s':" \
			      "no matching sample file."
			exit 1
		fi
	done
	singles=$req_sngl
fi

if [ -n "$req_ord" ]; then
	for o in $req_ord; do
		if ! echo "$orders" | grep -w -q $o; then
			error "invalid ordering ratio '$o':" \
			      "no matching sample file."
			exit 1
		fi
	done
	orders=$req_ord
fi

if [ -n "$req_sz" ]; then
	sizes=$req_sz
fi

if [ ! -x "$ARRAY_PTEST_BIN" ]; then
	error "performance test runner not found." >&2
	exit 1
fi

echo "Running Stroll array performance tests with following settings:" >$output
echo "#Samples:        $nr" >>$output
echo "Distinct ratios: $singles" >>$output
echo "Order ratios:    $orders" >>$output
echo "Data sizes:      $sizes" >>$output
echo "Algorithms:      $algos" >>$output

echo >>$output
echo "Algorithm  #Samples Distinct(%) Order(%) Size(B)   Mean(ns)" >>$output

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
					stroll_array_ptest_run "$a" "$path" $s $prio >>$output
				done
			done
		done
	done
done
