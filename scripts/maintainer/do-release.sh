#!/bin/sh

# invoke with do-release 2.N.M, or 2.N.M-rcXX

REL=$1
SRCDIR=~/git/lttng-modules
# The output files are created in ${HOME}/stable/
OUTPUTDIR=${HOME}/stable

if [ x"$1" = x"" ]; then
	echo "1 arg : VERSION";
	exit 1;
fi

cd ${OUTPUTDIR}

echo Doing LTTng modules release ${REL}

mkdir lttng-modules-${REL}
cd lttng-modules-${REL}
cp -ax ${SRCDIR}/. .

#cleanup
make clean
git clean -xdf

for a in \*.orig \*.rej Module.markers Module.symvers; do
	find . -name "${a}" -exec rm '{}' \;;
done
for a in outgoing .tmp_versions .git .pc; do
	find . -name "${a}" -exec rm -rf '{}' \;;
done

cd ..
tar cvfj lttng-modules-${REL}.tar.bz2 lttng-modules-${REL}
mksums lttng-modules-${REL}.tar.bz2
signpkg lttng-modules-${REL}.tar.bz2
