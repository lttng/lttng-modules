#!/bin/sh

# First argument is the path to the kernel headers.
KPATH=$1

if [ ! -f ${KPATH}/include/generated/uapi/linux/version.h ]; then
	echo 0
	exit 0
fi

# Assuming KPATH is the target kernel headers directory
RHEL_RELEASE=$(sed -rn 's/^#define RHEL_RELEASE "(.*)"/\1/p' ${KPATH}/include/generated/uapi/linux/version.h)

RHEL_RELEASE_MAJOR=$(echo ${RHEL_RELEASE} | sed -r 's/^([0-9]+)\.([0-9]+)\.([0-9]+)/\1/')
RHEL_RELEASE_MINOR=$(echo ${RHEL_RELEASE} | sed -r 's/^([0-9]+)\.([0-9]+)\.([0-9]+)/\2/')
RHEL_RELEASE_PATCH=$(echo ${RHEL_RELEASE} | sed -r 's/^([0-9]+)\.([0-9]+)\.([0-9]+)/\3/')

# Combine all update numbers into one
RHEL_API_VERSION=$((RHEL_RELEASE_MAJOR * 10000 + RHEL_RELEASE_MINOR * 100 + RHEL_RELEASE_PATCH))

echo ${RHEL_API_VERSION}
