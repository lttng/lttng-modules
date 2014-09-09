#!/bin/bash

# example usage:
# lttng-get-syscall-inout.sh table-syscall-inout.txt select 1

FILENAME=$1
SYSCALL_NAME=$2
ARG_NR=$3
TMPFILE=inout.tmp.1
TMPFILE2=inout.tmp.2

if [ x"${FILENAME}" = x"" ]; then
	echo "Error: Please specify input file name as first argument"
	exit 1
fi

if [ x"${SYSCALL_NAME}" = x"" ]; then
	echo "Error: Please specify system call name as second argument"
	exit 1
fi

if [[ x"${ARG_NR}" = x"" || ${ARG_NR} == 0 ]]; then
	echo "Error: Please specify argument number larger than 0 as 3rd argument"
	exit 1
fi

grep "syscall ${SYSCALL_NAME} " ${FILENAME} > ${TMPFILE}
perl -p -e 's/^syscall ([^ ]*) nbargs ([^ ]*) .*/'\
'$2/g' ${TMPFILE} > ${TMPFILE2}

NR_MATCH=$(wc -l ${TMPFILE} | perl -p -e 's/^([^ ])*.*/$1/g')
if [ ${NR_MATCH} -gt 1 ]; then
	echo "Error: more than one system call match"
	exit 1
fi

if [ ${NR_MATCH} -eq 0 ]; then
	# no match, default to inout
	echo "sc_inout"
	exit 0
fi

SC_ARGS=$(cat ${TMPFILE2})

if [ ${ARG_NR} -gt ${SC_ARGS} ]; then
	echo "Error: argument number (${ARG_NR}) is larger than number of syscall arguments (${SC_ARGS})"
	exit 1
fi

if [ ${ARG_NR} == 1 ]; then
	perl -p -e 's/^syscall ([^ ]*) nbargs ([^ ]*) rw: \(([^,\)]*).*/$3/g' ${TMPFILE} > ${TMPFILE2}
fi

if [ ${ARG_NR} == 2 ]; then
	perl -p -e 's/^syscall ([^ ]*) nbargs ([^ ]*) rw: \([^,]*, ([^,\)]*).*/$3/g' ${TMPFILE} > ${TMPFILE2}
fi

if [ ${ARG_NR} == 3 ]; then
	perl -p -e 's/^syscall ([^ ]*) nbargs ([^ ]*) rw: \([^,]*, [^,]*, ([^,\)]*).*/$3/g' ${TMPFILE} > ${TMPFILE2}
fi

if [ ${ARG_NR} == 4 ]; then
	perl -p -e 's/^syscall ([^ ]*) nbargs ([^ ]*) rw: \([^,]*, [^,]*, [^,]*, ([^,\)]*).*/$3/g' ${TMPFILE} > ${TMPFILE2}
fi

if [ ${ARG_NR} == 5 ]; then
	perl -p -e 's/^syscall ([^ ]*) nbargs ([^ ]*) rw: \([^,]*, [^,]*, [^,]*, [^,]*, ([^,\)]*).*/$3/g' ${TMPFILE} > ${TMPFILE2}
fi

if [ ${ARG_NR} == 6 ]; then
	perl -p -e 's/^syscall ([^ ]*) nbargs ([^ ]*) rw: \([^,]*, [^,]*, [^,]*, [^,]*, [^,]*, ([^,\)]*).*/$3/g' ${TMPFILE} > ${TMPFILE2}
fi

SC_ARG_TYPE=$(cat ${TMPFILE2})

if [ ${SC_ARG_TYPE} = "r" ]; then
	echo "sc_in"
fi
if [ ${SC_ARG_TYPE} = "w" ]; then
	echo "sc_out"
fi
if [ ${SC_ARG_TYPE} = "rw" ]; then
	echo "sc_inout"
fi

rm -f ${TMPFILE} ${TMPFILE2}
