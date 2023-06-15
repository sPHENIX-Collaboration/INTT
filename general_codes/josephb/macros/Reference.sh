#!/bin/bash

if [ $# -gt 2 ]
then
	echo "More than 2 arguments; taking first two arguments and excess will be ignored"
fi

if [ $# -eq 2 ]
then
	root -l Reference.C\(\"$1\",\"$2\"\)
else
	echo "Script should be called with at least 2 arguments"
fi
