#!/bin/bash
# SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)

# example usage:
# lttng-get-syscall-inout.sh table-syscall-inout.txt select 1

ARCH_NAME=$1
SYSCALL_NAME=$2
ARG_NR=$3
TMPFILE=$(mktemp)
GENERIC_INOUT_DESCRIPTION_FILE="$(dirname "$0")/table-syscall-inout.txt"

# Delete temp file on exit
trap 'rm -f "$TMPFILE"' EXIT

if [ x"${GENERIC_INOUT_DESCRIPTION_FILE}" = x"" ]; then
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

# Search for the in/out description of a syscall. This function attempts to find
# a matching description in the per-architecture description override file (if it exists)
# and falls back to the generic description file otherwise.
#
# Returns 0 if a description was found and written to the output file, 1 otherwise.
function write_inout_description ()
{
	local arch_name=$1
	local syscall_name=$2
	local output_file=$3
	local description_files=("$(dirname "$0")/table-syscall-inout-${arch_name}-override.txt" "$GENERIC_INOUT_DESCRIPTION_FILE")
	local found=0

	for file in ${description_files[@]}; do
		if [ ! -f "$file" ]; then
			continue
		fi

		# Look for the syscall's in/out description
		grep "syscall ${syscall_name} " "${file}" > "${output_file}" || true

		# Error out if we got more than one syscall
		local match_count=$(wc -l < "${output_file}")
		if [ "${match_count}" -gt 1 ]; then
			# Fatal error; invalid description file
			echo "Error: more than one system call match" >&2
			exit 1
		elif [ "${match_count}" -eq 1 ]; then
			# Description found
			return 0
		fi
	done

	return 1
}

# Abort on error and undefined variable
set -eu


# Default to sc_inout for unknown syscalls
if ! write_inout_description "$ARCH_NAME" "$SYSCALL_NAME" "$TMPFILE"; then
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
