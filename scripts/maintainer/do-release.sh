#!/bin/bash

set -eu
set -o pipefail

# invoke with do-release 2.N.M, or 2.N.M-rcXX

# Default maintainer values
SRCDIR="${HOME}/git/lttng-modules"
# The output files are created in ${HOME}/stable/
OUTPUTDIR="${HOME}/stable"
SIGN="yes"
VERBOSE=""

usage() {
	echo "Usage: do-release.sh [OPTION]... RELEASE"
	echo
	echo "Mandatory arguments to long options are mandatory for short options too."
	echo "  -s, --srcdir DIR               source directory"
	echo "  -o, --outputdir DIR            output directory, must exist"
	echo "  -n, --no-sign                  don't GPG sign the output archive"
	echo "  -v, --verbose                  verbose command output"
}

POS_ARGS=()
while [[ $# -gt 0 ]]
do
	arg="$1"

	case $arg in
	-n|--no-sign)
		SIGN="no"
		shift 1
	;;

	-s|--srcdir)
		SRCDIR="$2"
		shift 2
	;;

	-o|--outputdir)
		OUTPUTDIR="$2"
		shift 2
	;;

	-v|--verbose)
		VERBOSE="-v"
		shift 1
	;;

	# Catch unknown arguments
	-*)
		usage
		exit 1
	;;

	*)
	POS_ARGS+=("$1")
	shift
	;;
	esac
done
set -- "${POS_ARGS[@]}"

REL=${1:-}

if [ x"${REL}" = x"" ]; then
	usage
	exit 1;
fi

echo "Doing LTTng modules release ${REL}"
echo "  Source dir: ${SRCDIR}"
echo "  Output dir: ${OUTPUTDIR}"
echo "  GPG sign: ${SIGN}"

# Make sure the output directory exists
if [ ! -d "${OUTPUTDIR}" ]; then
	echo "Output directory '${OUTPUTDIR}' doesn't exist."
	exit 1
fi

# Make sure the source directory is a git repository
if [ ! -r "${SRCDIR}/.git/config" ]; then
	echo "Source directory '${SRCDIR}' isn't a git repository."
	exit 1
fi

# Set the git repo directory for all further git commands
export GIT_DIR="${SRCDIR}/.git/"

# Check if the release tag exists
if ! git rev-parse "refs/tags/v${REL}" >/dev/null 2>&1; then
	echo "Release tag 'v${REL}' doesn't exist."
	exit 1
fi

# Generate the compressed tar archive, the git attributes from the tag will be used.
git archive $VERBOSE --format=tar --prefix="lttng-modules-${REL}/" "v${REL}" | bzip2 > "${OUTPUTDIR}/lttng-modules-${REL}.tar.bz2"

pushd "${OUTPUTDIR}" >/dev/null
# Generate the hashes
md5sum "lttng-modules-${REL}.tar.bz2" > "lttng-modules-${REL}.tar.bz2.md5"
sha256sum "lttng-modules-${REL}.tar.bz2" > "lttng-modules-${REL}.tar.bz2.sha256"

if [ "x${SIGN}" = "xyes" ]; then
	# Sign with the default key
	gpg --armor -b "lttng-modules-${REL}.tar.bz2"
fi
popd >/dev/null
