#!/bin/sh

if [ ! -f ChangeLog ]
then
    touch ChangeLog
fi

echo "Running aclocal..." ; aclocal $ACLOCAL_FLAGS || exit 1
echo "Running libtoolize..." ; libtoolize || exit 1
echo "Running autoheader..." ; autoheader || exit 1
echo "Running autoconf..." ; autoconf || exit 1
echo "Running automake..." ; automake --add-missing --copy --gnu || exit 1
