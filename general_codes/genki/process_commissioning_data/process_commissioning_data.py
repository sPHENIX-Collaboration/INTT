import argparse
import logging
import time

import MyHelpFormatter
import Process

if __name__ == "__main__" :
    # A argument parser class to accept command-line options

    description="A python program for decoding event files, making plots, transferring plots to your local env. \n" \
        "See https://indico.bnl.gov/event/19834/contributions/77742/attachments/48046/81565/20230615_data_process.pdf for some explanation." 
    epilog = "Examples:\n" \
        "  * I just want to see the plots of run 123456!!!\n" \
        "    $ process_data 123456\n\n" \
        "  * Only making hit-wise TTree of run 123\n" \
        "    $ process_data --only --decode-hit-wise 123\n\n" \
        "  * Re-analyze run 123 without making event-wise TTree to save time\n" \
        "    $ process_data --no-decode-event-wise 123\n\n" \
        "  * Only getting plots of run 123 to my PC\n" \
        "    $ process_data --only --transfer-plot 123\n\n" \
        "  * Analyzing runs from 100 to 120\n" \
        "    $ seq 100 120 | xargs -I {} process_data {}\n" \
        "    It's better to stop the cron job in advance not to be interfered. Do in inttdaq:\n" \
        "    $ crontab -e\n" \
        "    And Comment the line out by adding # at the begginig ofthe line.\n" \
        "    The line to be commented out is something like: */1 * * * * /home/inttdev/bin/process_data --root-dir\n\n" \
        "  * I want to process new runs by hand\n" \
        "    $ process_data --auto-update 0\n" \
        "    Because the run number is mandatory, you need to give a fake value, for example, 0\n"\

    parser = argparse.ArgumentParser( description=description,
                                      epilog=epilog,
                                      formatter_class=MyHelpFormatter.MyHelpFormatter
                                     )

    ####################################################
    # Mandatory parameter                              #
    ####################################################
    parser.add_argument( "run", type=int,
                         help="**Mandatory** The run number to be processed. It doesn't need to be 8 digit." )

    ####################################################
    # General flags                                    #
    ####################################################
    run_types = ["beam",               "calib",              "calibration",       "commissioning",
                 "commissioning_5_23", "commissioning_5_30", "commissioning_6_2", "commissioning_6_4",
                 "commissioning_6_6",  "cosmics",            "josephb",           "junk",
                 "pedestal",           "root_files_obsolete"
                 ]
    
    parser.add_argument( "--run-type", type=str,
                         choices=run_types,
                         help="A type of run, which determins the directory in the buffer box, i.e. /bbox/commissioning/INTT/{here}. "\
                         "A name of event files also depends on the run type.  You can give: beam/calib/junk/calibration. \"beam\" is default." )
    
    parser.add_argument( "--only",
                         action=argparse.BooleanOptionalAction,
                         help="Only specified steps are done if specified. Otherwise (default), all steps are done unless \"no\" options are specified." )

    parser.add_argument( "--dry-run",
                         action=argparse.BooleanOptionalAction,
                         help="A flag for testing the processes. It's better to use it before launching processes." )

    parser.add_argument( "--force-run",
                         action=argparse.BooleanOptionalAction,
                         help="A flag to force running this program even if other processes with the same name are running."\
                         "Using this flag to decode large evt files are not recommended." )

    # Auto-update flags
    parser.add_argument( "--auto-update",
                         action=argparse.BooleanOptionalAction,
                         help="Automatic updating process (decode, making plots, transferring plots) for the new runs appear "\
                         "in /bbox/commissioning/INTT/{run-type} is done if specified. {run-type} is given by --run-type. Default: off" )

    parser.add_argument( "--update-list",
                         action=argparse.BooleanOptionalAction,
                         help="The current run list (evt_list.txt) and the previous run list (run_list_previous.txt) are updated if specified (default)."\
                         "Turning off is mainly for debugging." )

    # arguments for detailed control, almost absolete
    parser.add_argument( "--root-dir", type=str,
                         help="A name of directory that contains ROOT files. \"commissioning\" is default. You may use commissioning_6_6, 6_4, 6_2, 5_30, or 5_23"\
                         "for old data. You don't need to change it normally." )
    
    parser.add_argument( "--root-subdir", type=str,
                         help="A name of sub-directory that contains ROOT files. \"hit_files\" is default. You don't need to change it normally." )
        
    ####################################################
    # Decoding and making plots (general)              #
    ####################################################
    parser.add_argument( "--decode",
                         action=argparse.BooleanOptionalAction,
                         help="Decoding to make a hit-wise and an event-wise TTree is done if specified (default)." )

    parser.add_argument( "--decode-hit-wise",
                         action=argparse.BooleanOptionalAction,
                         help="Decoding to make a hit-wise Tree is done if specified (default)." )

    parser.add_argument( "--decode-event-wise",
                         action=argparse.BooleanOptionalAction,
                         help="Decoding to make a event-wise Tree is done if specified (default)." )

    parser.add_argument( "--make-plot",
                         action=argparse.BooleanOptionalAction,
                         help="Making plots of ADC and channel dists in intt is done if specified (default)." )

    ####################################################
    # Processes in inttdaq (obsolate )                 #
    ####################################################
    parser.add_argument( "--make-symbolic",
                         action=argparse.BooleanOptionalAction,
                         help="Making symbolic link in inttdaq to the ROOT files in the sPHENIX common disc is done if specified (default). " )
    
    parser.add_argument( "--transfer-plot",
                         action=argparse.BooleanOptionalAction,
                         help="Transferring the plots to your local env is done if specified ((default). Use --transfer-dir to select a directory to contain the plots." )

    parser.add_argument( "--transfer-dir", type=str,
                         help="A directory to contain the plots can be specified (default: .)." )

    ####################################################
    # Processes at SDCC                                #
    ####################################################
    parser.add_argument( "--send-SDCC",
                         action=argparse.BooleanOptionalAction,
                         help="Transferring evt files from the buffer box to SDCC if it's ON." )

    parser.add_argument( "--process-SDCC",
                         action=argparse.BooleanOptionalAction,
                         help="Some processes (decoding for hit-wise and event-wise TTrees, and making plots) are done in SDCC." )


    args = parser.parse_args()
    #print( args )
    process = Process.Process( args )
    process.Do()

    time.sleep( 1 )
    print( "All processes were ended" )
