#!/bin/sh
# genchanges.sh - generate a changes file for a deb file generated via
#	the make-kpkg utility

# KSRC KMAINT and KEMAIL are expected to be passed through the environment

set -e
umask 022

#KVERS=`cat debian/KVERS`
MODVERS=`cat debian/VERSION | sed s/:/\+/`
ARCH=`dpkg --print-architecture`

mprefix=`grep Package: debian/control.module | cut -d' ' -f 2 | cut -d= -f 1`

# the changes file's name
chfile="$KSRC/../$mprefix${KVERS}${INT_SUBARCH}_${MODVERS}_${ARCH}.changes"

dpkg-genchanges -b ${KMAINT:+-m"$KMAINT <$KEMAIL>"} -u"$KSRC/.." \
	-cdebian/control > "$chfile"
#pgp -fast ${KMAINT:+-u"$KMAINT"} < "$chfile.pt" > "$chfile"
rm "$chfile.pt"
