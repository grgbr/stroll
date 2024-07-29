ECHOE="/bin/echo -e"

error()
{
	$ECHOE "$arg0: $*" >&2
}

ptest_parse_loops()
{
	local arg="$1"
	local val

	val=$(echo "$arg" | sed -n "s/[0-9]*//gp")
	if [ -n "$val" ]; then
		error "invalid number of testing loops '$arg':" \
		      "integer expected.\n"
		return 1
	fi
	if [ $((arg)) -lt 1 ]; then
		error "invalid number of testing loops '$arg':" \
		      "integer >= 1.\n"
		return 1
	fi

	echo $arg
}

ptest_parse_nr()
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

ptest_parse_orders()
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

ptest_parse_singles()
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

ptest_parse_prio()
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

ptest_parse_sizes()
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

ptest_probe_nr()
{
	local expr="s@${ptest_data_base}_n\([0-9]\+\)_s[0-9]\+_o[0-9]\+\.dat@\1\n@gp"

	echo -n $@ | sed --silent $expr | sort -b -u -n | xargs
}

ptest_probe_orders()
{
	local expr="s@${ptest_data_base}_n[0-9]\+_s[0-9]\+_o\([0-9]\+\)\.dat@\1\n@gp"

	echo -n $@ | sed --silent $expr | sort -b -u -n | xargs
}

ptest_probe_singles()
{
	local expr="s@${ptest_data_base}_n[0-9]\+_s\([0-9]\+\)_o[0-9]\+\.dat@\1\n@gp"

	echo -n $@ | sed --silent $expr | sort -b -u -n | xargs
}

ptest_data_path()
{
	local base="$1"
	local nr=$2
	local single=$3
	local order=$4

	echo ${base}_n${nr}_s${single}_o${order}.dat
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
    -c | --loops          number of testing loops (defaults to 10000)
    -i | --distinct-ratio comma separated list of distinct value ratios
    -l | --list           show available performance data related optional settings
    -n | --number         comma separated list of number of samples
    -o | --output         pathname to output result file
    -p | --prio           SCHED_FIFO scheduling priority (defaults to none)
    -r | --order-ratio    comma separated list of ordering ratios
    -s | --size           comma separated list of sample sizes
    -h | --help           this help message
_EOF
}

arg0="$(basename $0)"

cmdln=$(getopt --options d:c:ln:i:o:p:r:s:h \
               --longoptions datadir:,list,loops:,number:,output:,prio:,distinct-ratio:,order-ratio:,size:,help \
               --name "$arg0" \
               -- "$@")
if [ $? -gt 0 ]; then
	usage
	exit 1
fi

show=0
prio=0
loops=10000
req_nr=
req_ord=
req_sngl=
req_sz=
output="/dev/stdout"
ptest_data_base="$PTEST_DATA/stroll_ptest"
eval set -- "$cmdln"
while true; do
	case "$1" in
	-d|--datadir)
		ptest_data_base="$2/stroll_ptest"
		shift 2;;
	-c|--loops)
		if ! loops=$(ptest_parse_loops "$2"); then
			usage
			exit 1
		fi
		shift 2;;
	-l|--list)
		show=1; shift;;
	-n|--number)
		if ! req_nr=$(ptest_parse_nr "$2"); then
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
		if ! prio=$(ptest_parse_prio "$2"); then
			usage
			exit 1
		fi
		if ! chrt -f $prio /bin/true; then
			error "cannot setup scheduling policy."
			exit 1
		fi
		shift 2;;
	-r|--order-ratio)
		if ! req_ord=$(ptest_parse_orders "$2"); then
			usage
			exit 1
		fi
		shift 2;;
	-i|--distinct-ratio)
		if ! req_sngl=$(ptest_parse_singles "$2"); then
			usage
			exit 1
		fi
		shift 2;;
	-s|--size)
		if ! req_sz=$(ptest_parse_sizes "$2"); then
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

ptest_data_files="${ptest_data_base}_*"
nr=$(ptest_probe_nr $ptest_data_files)
orders=$(ptest_probe_orders $ptest_data_files)
singles=$(ptest_probe_singles $ptest_data_files)
sizes="4 8 16 32 64 128 256"

if [ -z "$nr" ] || [ -z "$singles" ] || [ -z "$orders" ]; then
	error "no data file found under test data directory" \
	      "'$(dirname $ptest_data_base)'."
	exit 1
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
			error "invalid distinct value ratio '$s':" \
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
