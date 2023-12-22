#! /bin/bash

FILE="$1"

if [ -z "$FILE" ] ; then

    root -l myfirstproject.C\(0\)
else

    root -l -q -b myfirstproject.C\(\"$FILE\"\)
fi
