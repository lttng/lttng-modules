#!/bin/sh

# First argument is the path to the kernel headers.
KPATH=$1

if [ ! -f "${KPATH}/include/generated/autoconf.h" ]; then
	echo 0
	exit 0
fi

# Check if we are building against a Suse kernel
SUSE_KERNEL="$(sed -rn 's/^#define CONFIG_SUSE_KERNEL (.*)/\1/p' "${KPATH}/include/generated/autoconf.h")"

if [ "$SUSE_KERNEL" != "1" ]; then
	echo 0
	exit 0
fi


if [ ! -f "${KPATH}/include/generated/utsrelease.h" ]; then
	echo 0
	exit 0
fi

SLE_RELEASE="$(sed -rn 's/^#define UTS_RELEASE "(.*)-([0-9\.]+)-(.*)"/\2/p' "${KPATH}/include/generated/utsrelease.h")"

SLE_RELEASE_MAJOR="$(echo "${SLE_RELEASE}" | sed -rn 's/^([0-9]+)(.*)$/\1/p')"
SLE_RELEASE_MINOR="$(echo "${SLE_RELEASE}" | sed -rn 's/^([0-9]+)\.([0-9]+)(.*)$/\2/p')"
SLE_RELEASE_PATCH="$(echo "${SLE_RELEASE}" | sed -rn 's/^([0-9]+)\.([0-9]+)\.([0-9]+)(.*)$/\3/p')"

# Minor and patch versions can be omitted
if [ "x$SLE_RELEASE_MINOR" = "x" ]; then
	SLE_RELEASE_MINOR=0
fi
if [ "x$SLE_RELEASE_PATCH" = "x" ]; then
	SLE_RELEASE_PATCH=0
fi

# Combine all update numbers into one
SLE_API_VERSION="$((SLE_RELEASE_MAJOR * 10000 + SLE_RELEASE_MINOR * 100 + SLE_RELEASE_PATCH))"

echo ${SLE_API_VERSION}
