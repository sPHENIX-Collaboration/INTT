# Database Scripts

This directory contains scripts for processing and analyzing data related to the sPHENIX psql database especially for INTT QA Database. Below are descriptions of the main scripts:
- main Author : Takahiro Kikuchi(Rikkyo Univ.), Jaein Hwang(Korea Univ.)
## put_in_database.py

This script processes run files and inserts relevant data into the database.

### Functions

- `get_run_events()`: Queries the database for run events of type 'physics'.
- `insert_data(runnum, dead_count, runtime, bco_stdev, bco_peak)`: Inserts processed data into the `intt_qa_expert` table.
- `process_run_file(hot_file_path)`: Processes a BADMAP CDB file to count dead, cold, and hot channels.
- `process_bco_file(bco_file_path)`: Processes a BCO CDB file to calculate the standard deviation and peak of BCO differences.
- `calculate_runtime(brtimestamp, ertimestamp)`: Calculates the runtime of a run in minutes.
- `main()`: Main function that orchestrates the processing of run files and insertion of data into the database.


## make_plots.py
This script generates various histograms and a tree from the database data related to INTT QA Database.
 
 ### Functions
- `get_run_events()`: Queries the database for run events.
get_num_events_from_daq(runnumbers): Queries the database for the number of events from DAQ.
- `process_run_file(hot_file_path)`: Processes a run file to count dead, cold, and hot channels.
create_histograms(): Creates histograms for no hit channels, runtime, BCO peak standard deviation, and BCO peak position.
- `create_histograms_with_event_count()`: Creates histograms with event count for no hit channels, runtime, BCO peak standard deviation, and BCO peak position.
- `create_canvas()`: Creates canvases for drawing histograms.
- `create_tree()`: Creates a ROOT tree and branches for run data.
- `fill_histograms_and_tree(rows, hist_nohitch, hist_nohitch_zoom, hist_runtime, hist_bco_stdev, hist_bco_peak, tree, _runnum, _runtime, _nohitch, _bco_stdev, _bco_peak): Fills histograms and tree with data from the database.
- `fill_histo_with_numevents(rows, num_events, hist_nohitch, hist_nohitch_zoom, hist_runtime, hist_bco_stdev, hist_bco_peak)`: Fills histograms with event count data.
- ` save_histograms_and_tree(hist_nohitch, hist_nohitch_zoom, hist_runtime, hist_bco_stdev, hist_bco_peak, tree)` : Saves histograms and tree to a ROOT file.

### Usage

Run the script using Python:
python3 make_plots.py
