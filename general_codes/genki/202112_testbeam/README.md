# README
Macros for the ELPH data analysis are here.

## Analysis steps
1. Removing clone hits: trim_clone_hit.cc -> runX_no_clone.root and runX_clone.root
2. Making BCO group: make_bco_grouping.cc -> results/ELPH/trim_data/bco_group/runX.root
3. Removing noise hits, and then cluestering: make_cluester.cc -> run_X_no_clone_clustered.root
4. Alignment



## Class index
Documents for the class index, etc. are in html.zip.
You can read it as follows:
```
unzip html.zip
open html/index.html # for Mac
firefox html/index.html # for Linux & firefox
/mnt/c/Program\ Files/Mozilla\ Firefox/firefox.exe `wslpath -w html/index.html` # for WSL & firefox
```
You can open the index.html by clicking the file as well.

## Useful frameworks

### Database
You can get information written in run_list with this class. See Database.hh for more details. database_demo.cc shows how to use it.

## Online macros

### show_status.cc
It generates
- summary plots of modules
- parameter distribution
- parameter as a function of timing.
It was used in the online analysis.

### show_camac.cc
It can be used to see CAMAC parameters over all runs to know which channels were used.
work in progress

### trim_clone_hit.cc
The original INTT hits and the clone hits are saved into 2 files.

## Misc

### Doxyfile
A configuration file to generate class index.
