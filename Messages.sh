#! /bin/sh
$EXTRACTRC `find . -name \*.ui`          >>  rc.cpp
$XGETTEXT rc.cpp src/*.cpp src/*.h -o $podir/kteatime.pot
rm -f rc.cpp
