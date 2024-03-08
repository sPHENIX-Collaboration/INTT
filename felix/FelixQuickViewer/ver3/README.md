# FelixQuickViewer ver3

## Features/Difference from ver2
* Plots are optimized depending on the run type.
* Intermediate ROOT files, which contain histograms to be plotted, are generated so that you don't need to take time to just make plots again.
* Plots can be uploaded to [the INTT homepage](https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/intt/). Go to Commissioing plots -> each year to check the plots.

## Tutorials

### Dependencies
* ROOT (ver6)
* Python3 (ver?)
    * gspread (accessing Google spreadsheet)

#### Confliction of library version for gspread
gspread library of Python is used to access Google spreadsheet. Unfortunately, rucio library, which is originally developped for ATLAS(?), is installed in the SDCC environment and requires google-auth v2.6.0, just installing gspread like
```
pip3 install --user gspread
```
makes confliction. You can specify the version of google-auth to resolve the probrem:
```
pip3 install --user google-auth==2.6.0 gspread
```


### Installation
You need to be in the SDCC machine for the homepage feature.
Making plots may work in your local environment.
1. Download this Git repository: `git clone https://github.com/sPHENIX-Collaboration/INTT.git`
2. Move to here: `cd felix/FelixQuickViewer/ver3`
3. Execute the file (it's a shell script): `FelixQuickViewer`
4. If it works, that's all.

### Making plots
For example, junk-intt0-12345678-1234_hist.root is generated from junk-intt0-12345678-1234.root. The directory of the new file is the same as the hit-base ROOT file.
Some png files (junk-intt0-12345678-1234_hist_*.png) are generated in the same directory.

#### Case 1
```
FelixQuickViewer --plot --file {file_path}
```
You need to give the path to a ROOT file, which must contain a hit-base TTree named "tree".
Run type is taken from the file name in this case.

For example,
```
FelixQuickViewer --plot --file beam_intt7-00020869-0000.root
```

#### Case 2
```
FelixQuickViewer --plot --run-type {run_type} --felix-server {server} --run {run_number} --chunk {chunk_number}
```
Here,
* {run_type}: The run type. Followings are accepted: beam, cosmics, calib, calibration, pedestal, junk
* {server}: The FELIX server used to take this data. Accepted strings are: intt0, intt1, ..., intt7.
* {run_number}: The run to be processed. It doesn't need to be 8 digit.
* {chunk_number}: The last 4 digit of the data file.
are necessary. Those parameters makes a file name {run_type}_{server}-{run_number}-{chunk_number}.root.
If needed, you can add a path to the directory with `--data-dir {data_dir}` option.

For example,
```
FelixQuickViewer --plot --run-type beam --felix-server intt7 --run 20869 --chunk 0000
```

### Homepage

#### Run page
If you execute the following
```
FelixQuickViewer --homapage-run --run {run_number}
```
png files of the run are copied to the web page.

#### Title page
Just execute
```
FelixQuickViewer --homapage-title
```
The index.html in `https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/intt/commissioning_plots/{year}` is updated.
{year} is taken the timing when you execute it is taken. If you want to change it by yourself, use `--year {year}`.

### To do everything
You can just add all options mentioned above. For example,
```
FelixQuickViewer --plot --homepage --file {file_path}
```
or [the parameters](#Case 2) instead of `--file`.

## Help
```
[nukazuka@sphnx03 00:08:29 ~] $ FelixQuickViewer -h
usage: FelixQuickViewer.py [-h] [--dry-run | --no-dry-run] [--year {2023,2024,2025}] [--data-dir DATA_DIR] [--run-type {None,beam,cosmics,calib,calibration,pedestal,junk}] [--felix-server {intt0,intt1,intt2,intt3,intt4,intt5,intt6,intt7,None}]
                           [--run RUN] [--chunk CHUNK] [--file FILE] [--plot | --no-plot] [--plot-skip-hist-generation | --no-plot-skip-hist-generation] [--exe-type {macro,exe}] [--homepage | --no-homepage] [--homepage-run | --no-homepage-run]
                           [--homepage-title | --no-homepage-title] [--calib-summary | --no-calib-summary]

FELIX Quick Viewer
Not only plotting also does more....

options:
  -h, --help            show this help message and exit

  --dry-run, --no-dry-run
                        A flag for testing the processes. It's better to use it before launching processes. (default: False)

  --year {2023,2024,2025}
                        The year of the datat.

  --data-dir DATA_DIR   Choose the directory path for the data.

  --run-type {None,beam,cosmics,calib,calibration,pedestal,junk}
                        Choose the run type.

  --felix-server {intt0,intt1,intt2,intt3,intt4,intt5,intt6,intt7,None}
                        The run number to be processed. It doesn't need to be 8 digit.

  --run RUN             The run number to be processed. It doesn't need to be 8 digit.

  --chunk CHUNK         The chunk number to be processed. It doesn't need to be 4 digit.

  --file FILE           File path to be used to make plots.

  --plot, --no-plot     Use to generate plots from a single ROOT file.
                         (default: False)

  --plot-skip-hist-generation, --no-plot-skip-hist-generation
                        Histogram generation, which is necessary to update histograms, is skipped.
                         (default: False)

  --exe-type {macro,exe}
                        Execution type for plot mode.

  --homepage, --no-homepage
                        Use to generate homepage for both the run and title pages.
                         (default: False)

  --homepage-run, --no-homepage-run
                        Use to generate homepage for the run page.
                         (default: False)

  --homepage-title, --no-homepage-title
                        Use to generate homepage for the title page
                         (default: False)

  --calib-summary, --no-calib-summary
                        Use to generate calibration summary.
                         (default: False)

Some additional information
comes here.
```