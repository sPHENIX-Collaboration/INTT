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
        "  * I'm taking data in pedestal run mode. I just want to decode the run 26098.\n"\
        "   $ process_data --run-type pedestal --only --decode --no-check-bbox --send-SDCC 26098\n"\

    parser = argparse.ArgumentParser( description=description,
                                      epilog=epilog,
                                      formatter_class=MyHelpFormatter.MyHelpFormatter
                                     )

    ####################################################
    # Mandatory parameter                              #
    ####################################################
    parser.add_argument( "run", type=int,
                         help="**Mandatory** The run number to be processed. It doesn't need to be 8 digit.",
                         nargs='*' )

    ####################################################
    # Switch: Processes at 1008 or SDCC                #
    ####################################################
    parser.add_argument( "--1008",
                         action=argparse.BooleanOptionalAction,
                         default=True,
                         help="Use it to process data at 1008 (INTT DAQ FELIX servers).\n" )

    parser.add_argument( "--SDCC",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Use it to process data at SDCC.\n" )

    parser.add_argument( "--year", type=int,
                         default=None,
                         help="The year of the data tobe processed. This year is taken as a default value. If you want to process data taken last year(s), use this.")
    
    ####################################################
    # General flags                                    #
    ####################################################
    run_types = ["beam",               "calib",              "calibration",       "commissioning",
                 "commissioning_5_23", "commissioning_5_30", "commissioning_6_2", "commissioning_6_4",
                 "commissioning_6_6",  "cosmics",            "josephb",           "junk",
                 "pedestal",           "root_files_obsolete", "physics"
                 ]
    
    parser.add_argument( "--run-type", type=str,
                         choices=run_types,
                         default="calib",
                         help="A type of run, which determins the directory in the buffer box, i.e. /bbox/commissioning/INTT/{here}. "\
                         "A name of event files also depends on the run type.  You can give: beam/calib/junk/calibration. \"calib\" is default." )
    
    parser.add_argument( "--only",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Only specified steps are done if specified. Otherwise (default), all steps are done unless \"no\" options are specified." )

    parser.add_argument( "--dry-run",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A flag for testing the processes. It's better to use it before launching processes." )

    parser.add_argument( "--force-run",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A flag to force running this program even if other processes with the same name are running."\
                         "Using this flag to decode large evt files are not recommended." )

    # Auto-update flags
    parser.add_argument( "--auto-update",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Automatic updating process (decode, making plots, transferring plots) for the new runs appear "\
                         "in /bbox/commissioning/INTT/{run-type} is done if specified. {run-type} is given by --run-type. Default: off" )

    parser.add_argument( "--update-list",
                         action=argparse.BooleanOptionalAction,
                         #default=True, # better not to specify the default value, so 3 states can be here
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
                         default=False,
                         help="Decoding to make a hit-wise and an event-wise TTree is done if specified (default)." )

    parser.add_argument( "--decode-hit-wise",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Decoding to make a hit-wise Tree is done if specified (default)." )

    parser.add_argument( "--decode-event-wise",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Decoding to make a event-wise Tree is done if specified (default)." )

    parser.add_argument( "--make-plot",
                         action=argparse.BooleanOptionalAction,
                         default=False,
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
    parser.add_argument( "--check-bbox",
                         action=argparse.BooleanOptionalAction,
                         help="Check whether the run files were successfully transferred the buffer box. Checks are done by --send-SDCC and --process-SDCC in any cases." )

    parser.add_argument( "--send-SDCC",
                         action=argparse.BooleanOptionalAction,
                         help="Transferring evt files from the buffer box to SDCC if it's ON." )

    parser.add_argument( "--send-map-SDCC",
                         action=argparse.BooleanOptionalAction,
                         help="Transferring the ladder map from 1008 to SDCC if it's ON." )

    parser.add_argument( "--process-SDCC",
                         action=argparse.BooleanOptionalAction,
                         help="Some processes (decoding for hit-wise and event-wise TTrees, and making plots) are done in SDCC." )

    parser.add_argument( "--calib-2024",
                         action=argparse.BooleanOptionalAction,
                         help="Some processes (decoding for hit-wise, making plots, and writting summarized results in the PostgreSQL in inttdaq) are done in SDCC locally." )

    parser.add_argument( "--bbox-commissioning",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="If it's true, files are seached under /bbox/commissioning/INTT. Otherwise, /bbox/bbox?/INTT will be searched (? is 0-5)." )

    parser.add_argument( "--DST-all",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A switch to choose whether all processes using a DST file is done or not. Default: False." )

    parser.add_argument( "--DST-INTTRAW",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A switch to choose whether a DST containing INTTRAWHIT is produced or not. Default: False." )
    
    parser.add_argument( "--DST-INTTRAW-hitmap",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A switch to choose whether hitmaps using INTTRAWHIT is generated not. Default: False." )
    
    parser.add_argument( "--DST-INTTRAW-hot-ch",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A switch to choose whether a hot channel map using the hitmaps are generated or not. Default: False." )
    
    parser.add_argument( "--DST-INTTRAW-bco-diff",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A switch to choose whether a CDB for the BCO difference cut is generated or not. Default: False." )
    
    parser.add_argument( "--DST-TrkrHit",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A switch to choose whether a DST containing Trkr_hit is produced using the hot channel map (+BCO diff cut) or not. Default: False." )
    
    parser.add_argument( "--DST-TrkrCluster",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A switch to choose whether a DST containing Trkr_cluster is produced using a DST containig Trkr_hit or not. Default: False." )
    
    parser.add_argument( "--official-DST",
                         #action=argparse.BooleanOptionalAction,
                         dest="official_DST",
                         action="store_true",
                         #default=True,
                         help="The official DST is used if specified. (default: True)" )

    parser.add_argument( "--own-DST",
                         #action=argparse.BooleanOptionalAction,
                         dest="official_DST",
                         action="store_false",
                         #default=True,
                         help="Own DST is used or produced if specified. (default: False)" )
    parser.set_defaults(  official_DST=True )

    parser.add_argument( "--condor",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="If it's used, the process at SDCC is executed as a batch job of condor." )

    parser.add_argument( "--quick",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="If it's used, only first about 100MB per intt to be processed." )

    parser.add_argument( "--update-plot",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Only process End of processSDCC. " )


    args = parser.parse_args()
    #print( args )
    process = Process.Process( args )
    process.info.Print()
    #process.info.GetEventFilePath()
    process.Do()

    time.sleep( 1 )
    #print( "All processes were ended" , flush=True )
