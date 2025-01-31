#!/bin/sh

if [ $# != 2 ]; then
  echo "Usage $0 [year] [runnumber]"
  exit
fi

rdir="/sphenix/tg/tg01/commissioning/INTT/data/root_files"
ynum=$1
rnum=$2

fnum=`ls -1 $rdir/$ynum/*$rnum*.{root,png,txt} 2>/dev/null | wc -l`
echo $ynum $rnum $fnum

if [ $fnum -gt -1 ]; then
  echo "Run$2 files are found."

  if [ ! -d $rdir/$ynum/$rnum ]; then
    echo "Run directory has not been made yet. Make a run directory..."
    mkdir -m 775 $rdir/$ynum/$rnum
  fi

  if [ ! -w $rdir/$ynum/$rnum ]; then
    echo "Run directory is not writable. Add write permission to the run directory..."
    chmod 775 $rdir/$ynum/$rnum
  fi

  echo "Moving output files to the run directory $rdir/$ynum/$rnum"
  mv $rdir/$ynum/*$rnum*.{root,png,txt} $rdir/$ynum/$rnum

fi
  
exit
