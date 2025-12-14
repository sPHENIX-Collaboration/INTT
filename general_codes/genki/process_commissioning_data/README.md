# process_commissioning_data
A program to do some processes, i.e. decoding, transferring, making plots, etc, for INTT data taken in 1008.

# before running the script, make the log directory
# default log directory is following:
#  log=/home/inttdev/INTT/log/inttdaq_cron/
ln -s  /home/inttdev/INTT/log/inttdaq_cron/ log

# How to use
Run
```
process_data --help
```
, so you will see something like:
```
$ python3 process_data --help
usage: process_commissioning_data.py [-h] [--run-type {beam,calib,calibration,commissioning,commissioning_5_23,commissioning_5_30,commissioning_6_2,commissioning_6_4,commissioning_6_6,cosmics,josephb,junk,pedestal,root_files_obsolete}]
                                     [--only | --no-only] [--dry-run | --no-dry-run] [--force-run | --no-force-run] [--auto-update | --no-auto-update] [--update-list | --no-update-list] [--root-dir ROOT_DIR] [--root-subdir ROOT_SUBDIR]
                                     [--decode | --no-decode] [--decode-hit-wise | --no-decode-hit-wise] [--decode-event-wise | --no-decode-event-wise] [--make-plot | --no-make-plot] [--make-symbolic | --no-make-symbolic]
                                     [--transfer-plot | --no-transfer-plot] [--transfer-dir TRANSFER_DIR] [--send-SDCC | --no-send-SDCC] [--process-SDCC | --no-process-SDCC]
                                     run

A python program for decoding event files, making plots, transferring plots to your local env.
See https://indico.bnl.gov/event/19834/contributions/77742/attachments/48046/81565/20230615_data_process.pdf for some explanation.

positional arguments:
  run                   **Mandatory** The run number to be processed. It doesn't need to be 8 digit. (required)

optional arguments:
  -h, --help            show this help message and exit

  --run-type {beam,calib,calibration,commissioning,commissioning_5_23,commissioning_5_30,commissioning_6_2,commissioning_6_4,commissioning_6_6,cosmics,josephb,junk,pedestal,root_files_obsolete}
                        A type of run, which determins the directory in the buffer box, i.e. /bbox/commissioning/INTT/{here}. A name of event files also depends on the run type.  You can give: beam/calib/junk/calibration. "beam" is default.

  --only, --no-only     Only specified steps are done if specified. Otherwise (default), all steps are done unless "no" options are specified.

  --dry-run, --no-dry-run
                        A flag for testing the processes. It's better to use it before launching processes.

  --force-run, --no-force-run
                        A flag to force running this program even if other processes with the same name are running.Using this flag to decode large evt files are not recommended.

  --auto-update, --no-auto-update
                        Automatic updating process (decode, making plots, transferring plots) for the new runs appear in /bbox/commissioning/INTT/{run-type} is done if specified. {run-type} is given by --run-type. Default: off

  --update-list, --no-update-list
                        The current run list (evt_list.txt) and the previous run list (run_list_previous.txt) are updated if specified (default).Turning off is mainly for debugging.

  --root-dir ROOT_DIR   A name of directory that contains ROOT files. "commissioning" is default. You may use commissioning_6_6, 6_4, 6_2, 5_30, or 5_23for old data. You don't need to change it normally.

  --root-subdir ROOT_SUBDIR
                        A name of sub-directory that contains ROOT files. "hit_files" is default. You don't need to change it normally.

  --decode, --no-decode
                        Decoding to make a hit-wise and an event-wise TTree is done if specified (default).

  --decode-hit-wise, --no-decode-hit-wise
                        Decoding to make a hit-wise Tree is done if specified (default).

  --decode-event-wise, --no-decode-event-wise
                        Decoding to make a event-wise Tree is done if specified (default).

  --make-plot, --no-make-plot
                        Making plots of ADC and channel dists in intt is done if specified (default).

  --make-symbolic, --no-make-symbolic
                        Making symbolic link in inttdaq to the ROOT files in the sPHENIX common disc is done if specified (default).

  --transfer-plot, --no-transfer-plot
                        Transferring the plots to your local env is done if specified ((default). Use --transfer-dir to select a directory to contain the plots.

  --transfer-dir TRANSFER_DIR
                        A directory to contain the plots can be specified (default: .).

  --send-SDCC, --no-send-SDCC
                        Transferring evt files from the buffer box to SDCC if it's ON.

  --process-SDCC, --no-process-SDCC
                        Some processes (decoding for hit-wise and event-wise TTrees, and making plots) are done in SDCC.

Examples:
  * I just want to see the plots of run 123456!!!
    $ process_data 123456

  * Only making hit-wise TTree of run 123
    $ process_data --only --decode-hit-wise 123

  * Re-analyze run 123 without making event-wise TTree to save time
    $ process_data --no-decode-event-wise 123

  * Only getting plots of run 123 to my PC
    $ process_data --only --transfer-plot 123

  * Analyzing runs from 100 to 120
    $ seq 100 120 | xargs -I {} process_data {}
    It's better to stop the cron job in advance not to be interfered. Do in inttdaq:
    $ crontab -e
    And Comment the line out by adding # at the begginig ofthe line.
    The line to be commented out is something like: */1 * * * * /home/inttdev/bin/process_data --root-dir

  * I want to process new runs by hand
    $ process_data --auto-update 0
    Because the run number is mandatory, you need to give a fake value, for example, 0
```
