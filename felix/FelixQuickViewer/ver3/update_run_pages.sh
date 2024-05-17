#!/bin/bash

year=$1

dir="/sphenix/WWW/subsystem/intt/commissioning_plots/${year}"

time ls -d $dir/000* | sed -e "s|/| |g" | awk '{print $7}' | xargs -I {} -P 5 ./FelixQuickViewer --year ${year} --homepage-run --run {}

./FelixQuickViewer --year ${year} --homepage-title
