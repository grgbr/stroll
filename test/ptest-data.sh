#!/bin/bash -e

ECHOE="/bin/echo -e"

log()
{
	$ECHOE "$*" >&2
}

error()
{
	$ECHOE "$arg0: $*" >&2
}
export -f error

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

	if ! python3 $ptest_data_script generate \
	                                --output "$path" \
	                                $nr \
	                                $order >/dev/null 2>&1; then
		error "failed to generate $path."
		return 1
	fi

	echo "$path" >&2
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
    -h | --help        this help message
_EOF
}

arg0="$(basename $0)"
export arg0

cmdln=$(getopt --options h --longoptions help --name "$arg0" -- "$@")
if [ $? -gt 0 ]; then
	usage
	exit 1
fi

eval set -- "$cmdln"
while true; do
	case "$1" in
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

if cpu=$(lscpu --online --parse=CPU | tail -1); then
	cpu=$((cpu + 1))
	log "Generating data in parallel over $cpu CPUs..."
else
	cpu=1
	log "Generating data..."
fi

nr=8
while [ $nr -le $((1024 * 1024)) ]; do
	for order in 0 5 10 25 45 50 55 75 90 95 100; do
		echo $ptest_data_base $nr $order
	done
	nr=$((2*nr))
done | \
nice --adjustment=19 \
     xargs -P $cpu -L 1 bash -c 'ptest_gen_data "$@"' --
