# FelixQuickViewer ver3

## Features/Dirrefence from ver2
* Plots are optimized depending on the run type.
* Intermediate ROOT files, which contain histograms to be plotted, are generated so that you don't need to take time to just make plots again.
* Plots can be uploaded to [the INTT homepage](https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/intt/). Go to Commissioing plots -> each year to check the plots.

## Tutorials

### Installation

### Making plots
#### Case 1
```
FelixQuickViewer --plot --file {file_path}
```
You need to give the path to a ROOT file, which must contain a hit-base TTree named "tree".
Run type is taken from the file name in this case.

#### Case 2
```
FelixQuickViewer --plot --run-type {run_type} --felix-server {server} --run {run_number} --chunk {chunk_number}
```
Here,
* {run_type}: The run type. Followings are accepted: beam, cosmics, calib, calibration, pedestal, junk
* {server}: The FELIX server used to take this data. Accepted strings are: intt0, intt1, ..., intt7.
* {run_number}: The run to be processed. It doesn't need to be 8 digit.
* {chunk_number}: The last 4 digit of the data file.
are necessary. Those parameters makes a file name {run_type}_{server}-{run_number}-{chunk_number}.evt.
If needed, you can add a path to the directory with `--data-dir {data_dir}` option.

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