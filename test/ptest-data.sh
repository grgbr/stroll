#!/bin/bash -e

ECHOE="/bin/echo -e"

log()
{
	if [ $quiet -eq 0 ]; then
		$ECHOE "$*" >&2
	fi
}
export -f log

error()
{
	$ECHOE "$arg0: $*" >&2
}
export -f error

ptest_parse_nr()
{
	local arg="$1"
	local val=$(echo "$arg" | sed -n "s/[0-9]*//gp")

	if [ -n "$val" ]; then
		error "invalid number of samples '$arg':" \
		      "integer expected.\n"
		return 1
	fi
	if [ $((arg)) -lt 8 ]; then
		error "invalid number of samples '$arg':" \
		      "integer >= 8 expected.\n"
		return 1
	fi

	echo $arg
}

ptest_parse_orders()
{
	local arg="$1"
	local val=$(echo "$arg" | sed -n "s/[0-9]*//gp")

	if [ -n "$val" ]; then
		error "invalid ordering ratio '$arg': integer expected.\n"
		return 1
	fi
	if [ $((arg)) -gt 100 ]; then
		error "invalid ordering ratio '$arg':" \
		      "integer <= 100 expected.\n"
		return 1
	fi

	echo $arg
}

ptest_data_path()
{
	local base="$1"
	local nr=$2
	local order=$3

	echo ${base}_${nr}_${order}.dat
}
export -f ptest_data_path

ptest_gen_data()
{
	local base="$1"
	local nr=$2
	local order=$3
	local path=$(ptest_data_path "$base" $nr $order)
	local out

	if ! out=$(${PYTHON3:-python3} \
	           $ptest_data_script generate --output "$path" \
	                                       $nr \
	                                       $order 2>&1); then

		$ECHOE "$out" >&2
		error "failed to generate $path."
		return 1
	fi

	log "$path"
}
export -f ptest_gen_data

usage()
{
	cat >&2 <<_EOF
Usage: $arg0 [OPTIONS] OUTPUT_DIRECTORY
Stroll performance test data generator.

With:
    OUTPUT_DIRECTORY  pathname to directory where to store generated data
Where OPTIONS:
    -q | --quiet       operate silently
    -n | --number      number of samples
    -r | --order-ratio ordering ratio
    -h | --help        this help message
_EOF
}

arg0="$(basename $0)"
export arg0

cmdln=$(getopt --options hn:qr: \
               --longoptions number:,order-ratio:,quiet,help \
               --name "$arg0" -- "$@")
if [ $? -gt 0 ]; then
	usage
	exit 1
fi

req_nr=
req_ord=
quiet=0
eval set -- "$cmdln"
while true; do
	case "$1" in
	-n|--number)
		if ! req_nr=$(ptest_parse_nr "$2"); then
			usage
			exit 1
		fi
		shift 2;;
	-q|--quiet)
		quiet=1
		shift;;
	-r|--order-ratio)
		if ! req_ord=$(ptest_parse_orders "$2"); then
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

if [ $# -ne 1 ]; then
	error "missing output directory.\n"
	usage
	exit 1
fi

outdir="$1"
if [ ! -d "$outdir" ] || [ ! -w "$outdir" ] || [ ! -x "$outdir" ]; then
	error "invalid output directory '$outdir'."
	exit 1
fi
ptest_data_base="$outdir/stroll_ptest"

ptest_data_script=$(dirname $(realpath --canonicalize-existing $0))/ptest-data.py
export ptest_data_script

if [ ! -r "$ptest_data_script" ]; then
	error "performance test data generator script 'ptest-data.py' not found."
	exit 1
fi

nr="8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576"
if [ -n "$req_nr" ]; then
	nr=$req_nr
fi

orders="0 5 25 45 50 55 75 95 100"
if [ -n "$req_ord" ]; then
	orders=$req_ord
fi

renice --priority +19 --pid $!

if [ -n "$req_nr" ] && [ -n "$req_ord" ]; then
	ptest_gen_data $ptest_data_base $nr $orders
	exit $?
fi

if cpu=$(lscpu --online --parse=CPU | tail -1); then
	cpu=$((cpu + 1))
	log "Generating data in parallel over $cpu CPUs..."
else
	cpu=1
	log "Generating data..."
fi

for n in $nr; do
	for o in $orders; do
		echo $ptest_data_base $n $o
	done
done | \
xargs -P $cpu -L 1 bash -c 'ptest_gen_data "$@"' --
