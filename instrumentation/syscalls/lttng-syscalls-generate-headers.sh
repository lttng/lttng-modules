#!/bin/sh

# Generate system call probe description macros from syscall metadata dump file.
# The resulting header will be written in the headers subdirectory, in a file name
# based on the name of the input file. 
#
# example usage:
#
# lttng-syscalls-generate-headers.sh <type> <input_dir> <input_filename_in_dir> <bitness>
# lttng-syscalls-generate-headers.sh integers 3.0.4 x86-64-syscalls-3.0.4 64
# lttng-syscalls-generate-headers.sh pointers 3.0.4 x86-64-syscalls-3.0.4 64

CLASS=$1
INPUTDIR=$2
INPUTFILE=$3
BITNESS=$4
INPUT=${INPUTDIR}/${INPUTFILE}
SRCFILE=gen.tmp.0
TMPFILE=gen.tmp.1
TMPFILE2=gen.tmp.2
TMPFILE3=gen.tmp.3
HEADER=headers/${INPUTFILE}_${CLASS}.h

if [ x"$INPUTDIR" = x"" ]; then
	echo "Error: Please specify input directory as second argument"
	exit 1
fi

if [ x"$INPUTFILE" = x"" ]; then
	echo "Error: Please specify input file as third argument"
	exit 1
fi

if [ x"$BITNESS" != x"32" ] && [ x"$BITNESS" != x"64" ]; then
	echo "Error: Please specify bitness as fourth argument (\"32\" or \"64\")"
	exit 1
fi

cp ${INPUT} ${SRCFILE}

#Cleanup
perl -p -e 's/^\[.*\] //g' ${SRCFILE} > ${TMPFILE}
mv ${TMPFILE} ${SRCFILE}

perl -p -e 's/^syscall sys_([^ ]*)/syscall $1/g' ${SRCFILE} > ${TMPFILE}
mv ${TMPFILE} ${SRCFILE}

#Filter

if [ "$CLASS" = integers ]; then
	#select integers and no-args.
	CLASSCAP=INTEGERS
	grep -v "\\*\|cap_user_header_t" ${SRCFILE} > ${TMPFILE}
	mv ${TMPFILE} ${SRCFILE}
fi


if [ "$CLASS" = pointers ]; then
	#select system calls using pointers.
	CLASSCAP=POINTERS
	grep "\\*\|cap_#user_header_t" ${SRCFILE} > ${TMPFILE}
	mv ${TMPFILE} ${SRCFILE}
fi

if [ x"$CLASSCAP" = x"" ]; then
	echo "Error: Please specify \"integers\" or \"pointers\" as first argument"
	rm -f ${SRCFILE}
	exit 1
fi

echo "/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */" > ${HEADER}

echo \
"#ifndef CREATE_SYSCALL_TABLE

#if !defined(_TRACE_SYSCALLS_${CLASSCAP}_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_${CLASSCAP}_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/syscalls.h>
#include \"${INPUTFILE}_${CLASS}_override.h\"
#include \"syscalls_${CLASS}_override.h\"
" >> ${HEADER}

if [ "$CLASS" = integers ]; then

NRARGS=0

printf \
'#ifdef SC_ENTER\n'\
	>> ${HEADER}

printf \
'SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(syscalls_noargs,\n'\
'	TP_FIELDS()\n'\
')\n'\
	>> ${HEADER}

grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}
perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) '\
'types: \(([^)]*)\) '\
'args: \(([^)]*)\)/'\
'#ifndef OVERRIDE_'"${BITNESS}"'_$1\n'\
'SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, $1)\n'\
'#endif/g'\
	${TMPFILE} >> ${HEADER}

printf \
'#else /* #ifdef SC_ENTER */\n'\
	>> ${HEADER}

grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}
perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) '\
'types: \(([^)]*)\) '\
'args: \(([^)]*)\)/'\
'#ifndef OVERRIDE_'"${BITNESS}"'_$1\n'\
'SC_LTTNG_TRACEPOINT_EVENT($1,\n'\
'	TP_PROTO(sc_exit(long ret)),\n'\
'	TP_ARGS(sc_exit(ret)),\n'\
'	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))\n'\
')\n'\
'#endif/g'\
	${TMPFILE} >> ${HEADER}

printf \
'#endif /* else #ifdef SC_ENTER */\n'\
	>> ${HEADER}

fi


# types: 4
# args   5

NRARGS=1
grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}

while read LINE; do
	echo "${LINE}" > ${TMPFILE2}
	perl -p -e 's/^syscall ([^ ]*) .*/$1/g' ${TMPFILE2} > ${TMPFILE3}
	SC_NAME=$(cat ${TMPFILE3})
	ARG1=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 1)
	echo Syscall: ${SC_NAME} ${ARG1}
	perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) '\
'types: \(([^)]*)\) '\
'args: \(([^)]*)\)/'\
'#ifndef OVERRIDE_'"${BITNESS}"'_$1\n'\
'SC_LTTNG_TRACEPOINT_EVENT($1,\n'\
'	TP_PROTO(sc_exit(long ret,) $4 $5),\n'\
'	TP_ARGS(sc_exit(ret,) $5),\n'\
'	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) '"${ARG1}"'(ctf_integer($4, $5, $5)))\n'\
')\n'\
'#endif/g'\
		${TMPFILE2} >> ${HEADER}
done < ${TMPFILE}

# types: 4 5
# args   6 7

NRARGS=2
grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}

while read LINE; do
	echo "${LINE}" > ${TMPFILE2}
	perl -p -e 's/^syscall ([^ ]*) .*/$1/g' ${TMPFILE2} > ${TMPFILE3}
	SC_NAME=$(cat ${TMPFILE3})
	ARG1=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 1)
	ARG2=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 2)
	echo Syscall: ${SC_NAME} ${ARG1} ${ARG2}
	perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) '\
'types: \(([^,]*), ([^)]*)\) '\
'args: \(([^,]*), ([^)]*)\)/'\
'#ifndef OVERRIDE_'"${BITNESS}"'_$1\n'\
'SC_LTTNG_TRACEPOINT_EVENT($1,\n'\
'	TP_PROTO(sc_exit(long ret,) $4 $6, $5 $7),\n'\
'	TP_ARGS(sc_exit(ret,) $6, $7),\n'\
'	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) '"${ARG1}"'(ctf_integer($4, $6, $6)) '"${ARG2}"'(ctf_integer($5, $7, $7)))\n'\
')\n'\
'#endif/g'\
		${TMPFILE2} >> ${HEADER}
done < ${TMPFILE}

# types: 4 5 6
# args   7 8 9

NRARGS=3
grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}

while read LINE; do
	echo "${LINE}" > ${TMPFILE2}
	perl -p -e 's/^syscall ([^ ]*) .*/$1/g' ${TMPFILE2} > ${TMPFILE3}
	SC_NAME=$(cat ${TMPFILE3})
	ARG1=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 1)
	ARG2=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 2)
	ARG3=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 3)
	echo Syscall: ${SC_NAME} ${ARG1} ${ARG2} ${ARG3}
	perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) '\
'types: \(([^,]*), ([^,]*), ([^)]*)\) '\
'args: \(([^,]*), ([^,]*), ([^)]*)\)/'\
'#ifndef OVERRIDE_'"${BITNESS}"'_$1\n'\
'SC_LTTNG_TRACEPOINT_EVENT($1,\n'\
'	TP_PROTO(sc_exit(long ret,) $4 $7, $5 $8, $6 $9),\n'\
'	TP_ARGS(sc_exit(ret,) $7, $8, $9),\n'\
'	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) '"${ARG1}"'(ctf_integer($4, $7, $7)) '"${ARG2}"'(ctf_integer($5, $8, $8)) '"${ARG3}"'(ctf_integer($6, $9, $9)))\n'\
')\n'\
'#endif/g'\
		${TMPFILE2} >> ${HEADER}
done < ${TMPFILE}


# types: 4 5  6  7
# args   8 9 10 11

NRARGS=4
grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}

while read LINE; do
	echo "${LINE}" > ${TMPFILE2}
	perl -p -e 's/^syscall ([^ ]*) .*/$1/g' ${TMPFILE2} > ${TMPFILE3}
	SC_NAME=$(cat ${TMPFILE3})
	ARG1=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 1)
	ARG2=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 2)
	ARG3=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 3)
	ARG4=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 4)
	echo Syscall: ${SC_NAME} ${ARG1} ${ARG2} ${ARG3} ${ARG4}
	perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) '\
'types: \(([^,]*), ([^,]*), ([^,]*), ([^)]*)\) '\
'args: \(([^,]*), ([^,]*), ([^,]*), ([^)]*)\)/'\
'#ifndef OVERRIDE_'"${BITNESS}"'_$1\n'\
'SC_LTTNG_TRACEPOINT_EVENT($1,\n'\
'	TP_PROTO(sc_exit(long ret,) $4 $8, $5 $9, $6 $10, $7 $11),\n'\
'	TP_ARGS(sc_exit(ret,) $8, $9, $10, $11),\n'\
'	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) '"${ARG1}"'(ctf_integer($4, $8, $8)) '"${ARG2}"'(ctf_integer($5, $9, $9)) '"${ARG3}"'(ctf_integer($6, $10, $10)) '"${ARG4}"'(ctf_integer($7, $11, $11)))\n'\
')\n'\
'#endif/g'\
		${TMPFILE2} >> ${HEADER}
done < ${TMPFILE}

# types: 4  5  6  7  8
# args   9 10 11 12 13

NRARGS=5
grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}

while read LINE; do
	echo "${LINE}" > ${TMPFILE2}
	perl -p -e 's/^syscall ([^ ]*) .*/$1/g' ${TMPFILE2} > ${TMPFILE3}
	SC_NAME=$(cat ${TMPFILE3})
	ARG1=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 1)
	ARG2=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 2)
	ARG3=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 3)
	ARG4=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 4)
	ARG5=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 5)
	echo Syscall: ${SC_NAME} ${ARG1} ${ARG2} ${ARG3} ${ARG4} ${ARG5}
	perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) '\
'types: \(([^,]*), ([^,]*), ([^,]*), ([^,]*), ([^)]*)\) '\
'args: \(([^,]*), ([^,]*), ([^,]*), ([^,]*), ([^)]*)\)/'\
'#ifndef OVERRIDE_'"${BITNESS}"'_$1\n'\
'SC_LTTNG_TRACEPOINT_EVENT($1,\n'\
'	TP_PROTO(sc_exit(long ret,) $4 $9, $5 $10, $6 $11, $7 $12, $8 $13),\n'\
'	TP_ARGS(sc_exit(ret,) $9, $10, $11, $12, $13),\n'\
'	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) '"${ARG1}"'(ctf_integer($4, $9, $9)) '"${ARG2}"'(ctf_integer($5, $10, $10)) '"${ARG3}"'(ctf_integer($6, $11, $11)) '"${ARG4}"'(ctf_integer($7, $12, $12)) '"${ARG5}"'(ctf_integer($8, $13, $13)))\n'\
')\n'\
'#endif/g'\
	${TMPFILE2} >> ${HEADER}
done < ${TMPFILE}


# types: 4   5  6  7  8  9
# args   10 11 12 13 14 15

NRARGS=6
grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}

while read LINE; do
	echo "${LINE}" > ${TMPFILE2}
	perl -p -e 's/^syscall ([^ ]*) .*/$1/g' ${TMPFILE2} > ${TMPFILE3}
	SC_NAME=$(cat ${TMPFILE3})
	ARG1=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 1)
	ARG2=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 2)
	ARG3=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 3)
	ARG4=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 4)
	ARG5=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 5)
	ARG6=$(./lttng-get-syscall-inout.sh table-syscall-inout.txt ${SC_NAME} 6)
	echo Syscall: ${SC_NAME} ${ARG1} ${ARG2} ${ARG3} ${ARG4} ${ARG5} ${ARG6}
	perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) '\
'types: \(([^,]*), ([^,]*), ([^,]*), ([^,]*), ([^,]*), ([^\)]*)\) '\
'args: \(([^,]*), ([^,]*), ([^,]*), ([^,]*), ([^,]*), ([^\)]*)\)/'\
'#ifndef OVERRIDE_'"${BITNESS}"'_$1\n'\
'SC_LTTNG_TRACEPOINT_EVENT($1,\n'\
'	TP_PROTO(sc_exit(long ret,) $4 $10, $5 $11, $6 $12, $7 $13, $8 $14, $9 $15),\n'\
'	TP_ARGS(sc_exit(ret,) $10, $11, $12, $13, $14, $15),\n'\
'	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) '"${ARG1}"'(ctf_integer($4, $10, $10)) '"${ARG2}"'(ctf_integer($5, $11, $11)) '"${ARG3}"'(ctf_integer($6, $12, $12)) '"${ARG4}"'(ctf_integer($7, $13, $13)) '"${ARG5}"'(ctf_integer($8, $14, $14)) '"${ARG6}"'(ctf_integer($9, $15, $15)))\n'\
')\n'\
'#endif/g'\
	${TMPFILE2} >> ${HEADER}
done < ${TMPFILE}

# Macro for tracing syscall table

echo \
"
#endif /*  _TRACE_SYSCALLS_${CLASSCAP}_H */

/* This part must be outside protection */
#include <probes/define_trace.h>

#else /* CREATE_SYSCALL_TABLE */

#include \"${INPUTFILE}_${CLASS}_override.h\"
#include \"syscalls_${CLASS}_override.h\"
" >> ${HEADER}

NRARGS=0

if [ "$CLASS" = integers ]; then
#noargs

printf \
'#ifdef SC_ENTER\n'\
	>> ${HEADER}

grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}
perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) .*$/'\
'#ifndef OVERRIDE_TABLE_'"${BITNESS}"'_$1\n'\
'TRACE_SYSCALL_TABLE\(syscalls_noargs, $1, $2, $3\)\n'\
'#endif/g'\
	${TMPFILE} >> ${HEADER}

printf \
'#else /* #ifdef SC_ENTER */\n'\
	>> ${HEADER}

grep "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}
perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) .*$/'\
'#ifndef OVERRIDE_TABLE_'"${BITNESS}"'_$1\n'\
'TRACE_SYSCALL_TABLE($1, $1, $2, $3)\n'\
'#endif/g'\
	${TMPFILE} >> ${HEADER}

printf \
'#endif /* else #ifdef SC_ENTER */\n'\
	>> ${HEADER}

fi

#others.
grep -v "^syscall [^ ]* nr [^ ]* nbargs ${NRARGS} " ${SRCFILE} > ${TMPFILE}
perl -p -e 's/^syscall ([^ ]*) nr ([^ ]*) nbargs ([^ ]*) .*$/'\
'#ifndef OVERRIDE_TABLE_'"${BITNESS}"'_$1\n'\
'TRACE_SYSCALL_TABLE($1, $1, $2, $3)\n'\
'#endif/g'\
	${TMPFILE} >> ${HEADER}

echo -n \
"
#endif /* CREATE_SYSCALL_TABLE */
" >> ${HEADER}

#fields names: ...char * type with *name* or *file* or *path* or *root*
# or *put_old* or *type*
cp -f ${HEADER} ${TMPFILE}
rm -f ${HEADER}
perl -p -e 's/ctf_integer\(([^,)]*char \*), ([^\)]*)(name|file|path|root|put_old|type)([^\)]*)\)/ctf_user_string($2$3$4)/g'\
	${TMPFILE} >> ${HEADER}

#prettify addresses heuristics.
#field names with addr or ptr
cp -f ${HEADER} ${TMPFILE}
rm -f ${HEADER}
perl -p -e 's/ctf_integer\(([^,)]*), ([^,)]*addr|[^,)]*ptr)([^),]*)\)/ctf_integer_hex($1, $2$3, $2$3)/g'\
	${TMPFILE} >> ${HEADER}

#field types ending with '*'
cp -f ${HEADER} ${TMPFILE}
rm -f ${HEADER}
perl -p -e 's/ctf_integer\(([^,)]*\*), ([^),]*)\)/ctf_integer_hex($1, $2, $2)/g'\
	${TMPFILE} >> ${HEADER}

rm -f ${INPUTFILE}.tmp
rm -f ${TMPFILE3}
rm -f ${TMPFILE2}
rm -f ${TMPFILE}
rm -f ${SRCFILE}
