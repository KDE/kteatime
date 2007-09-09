#! /bin/sh
$EXTRACTRC `find . -name \*.ui`          >  rc.cpp
$XGETTEXT *.cpp *.h -o $podir/kteatime.pot
