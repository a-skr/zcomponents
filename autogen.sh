#!/bin/sh

# Copyright (c) 2011 Alexandre Skrzyniarz.
#
# This file is part of oc2/zcomponents.
#
# oc2/zcomponents is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# oc2/zcomponents is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with oc2/zcomponents.  If not, see <http://www.gnu.org/licenses/>.

if [ ! -f ChangeLog ]
then
    touch ChangeLog
fi

echo "Running aclocal..." ; aclocal $ACLOCAL_FLAGS || exit 1
echo "Running libtoolize..." ; libtoolize || exit 1
echo "Running autoheader..." ; autoheader || exit 1
echo "Running autoconf..." ; autoconf || exit 1
echo "Running automake..." ; automake --add-missing --copy --gnu || exit 1
