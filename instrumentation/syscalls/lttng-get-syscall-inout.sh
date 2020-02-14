#!/bin/bash
# SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)

# example usage:
# lttng-get-syscall-inout.sh table-syscall-inout.txt select 1

FILENAME=$1
SYSCALL_NAME=$2
ARG_NR=$3
TMPFILE=$(mktemp)

# Delete temp file on exit
trap 'rm -f "$TMPFILE"' EXIT


if [ x"${FILENAME}" = x"" ]; then
	echo "Error: Please specify input file name as first argument" >&2
	exit 1
fi

if [ x"${SYSCALL_NAME}" = x"" ]; then
	echo "Error: Please specify system call name as second argument" >&2
	exit 1
fi

if [[ x"${ARG_NR}" = x"" || ${ARG_NR} == 0 ]]; then
	echo "Error: Please specify argument number larger than 0 as third argument" >&2
	exit 1
fi

# Abort on error and undefined variable
set -eu

# Get the required syscall
grep "syscall ${SYSCALL_NAME} " "${FILENAME}" > "${TMPFILE}" || true

# Error out if we got more than one syscall
NR_MATCH=$(wc -l < "${TMPFILE}")
if [ "${NR_MATCH}" -gt 1 ]; then
	echo "Error: more than one system call match" >&2
	exit 1
fi

# Default to sc_inout for unknown syscalls
if [ "${NR_MATCH}" -eq 0 ]; then
	echo "Warning: no match for syscall '${SYSCALL_NAME}', set to 'inout'" >&2
	# no match, default to inout
	echo "sc_inout"
	exit 0
fi

# Get the number of argument
SC_ARGS=$(sed 's/^syscall \([^ ]*\) nbargs \([^ ]*\) .*/\2/g' "${TMPFILE}")

if [ "${ARG_NR}" -gt "${SC_ARGS}" ]; then
	echo "Error: argument number (${ARG_NR}) is larger than number of syscall arguments (${SC_ARGS})" >&2
	exit 1
fi

if [ "${ARG_NR}" == 1 ]; then
	SC_ARG_TYPE=$(sed 's/^syscall \([^ ]*\) nbargs \([^ ]*\) rw: (\([^,)]*\).*/\3/g' "${TMPFILE}")
fi

if [ "${ARG_NR}" == 2 ]; then
	SC_ARG_TYPE=$(sed 's/^syscall \([^ ]*\) nbargs \([^ ]*\) rw: ([^,]*, \([^,)]*\).*/\3/g' "${TMPFILE}")
fi

if [ "${ARG_NR}" == 3 ]; then
	SC_ARG_TYPE=$(sed 's/^syscall \([^ ]*\) nbargs \([^ ]*\) rw: ([^,]*, [^,]*, \([^,)]*\).*/\3/g' "${TMPFILE}")
fi

if [ "${ARG_NR}" == 4 ]; then
	SC_ARG_TYPE=$(sed 's/^syscall \([^ ]*\) nbargs \([^ ]*\) rw: ([^,]*, [^,]*, [^,]*, \([^,)]*\).*/\3/g' "${TMPFILE}")
fi

if [ "${ARG_NR}" == 5 ]; then
	SC_ARG_TYPE=$(sed 's/^syscall \([^ ]*\) nbargs \([^ ]*\) rw: ([^,]*, [^,]*, [^,]*, [^,]*, \([^,)]*\).*/\3/g' "${TMPFILE}")
fi

if [ "${ARG_NR}" == 6 ]; then
	SC_ARG_TYPE=$(sed 's/^syscall \([^ ]*\) nbargs \([^ ]*\) rw: ([^,]*, [^,]*, [^,]*, [^,]*, [^,]*, \([^,)]*\).*/\3/g' "${TMPFILE}")
fi


if [ "${SC_ARG_TYPE}" = "r" ]; then
	echo "sc_in"
fi
if [ "${SC_ARG_TYPE}" = "w" ]; then
	echo "sc_out"
fi
if [ "${SC_ARG_TYPE}" = "rw" ]; then
	echo "sc_inout"
fi

# EOF
