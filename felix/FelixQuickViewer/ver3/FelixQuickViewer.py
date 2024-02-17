import argparse
import subprocess as sp
import datetime

import MyHelpFormatter
import Information
import Homepage

if __name__ == "__main__" :

    description = "FELIX Quick Viewer\n" \
        "Not only plotting also does more...."
    epilog = "Some additional information\n" \
        "comes here."
    
    parser = argparse.ArgumentParser( description=description,
                                      epilog=epilog,
                                      formatter_class=MyHelpFormatter.MyHelpFormatter
                                     )

    ####################################################
    # List of flags                                    #
    ####################################################
    # --dry-run
    # --year
    # --plot
    # --file
    # --exe-type [macro, exe]
    # --homepage
    # --run
    # --calib-summary
    # --run-type [beam, cosmic, calib, pedestal]
    
    ####################################################
    # Mandatory parameter                              #
    ####################################################
    ####################################################
    # General flags                                    #
    ####################################################
    parser.add_argument( "--dry-run",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A flag for testing the processes. It's better to use it before launching processes." )

    year_list = [2023, 2024, 2025 ]
    parser.add_argument( "--year", type=int,
                         choices=year_list,
                         default=datetime.date.today().year,
                         help="The year of the datat." )

    run_type_list = [ None, "beam", "cosmics", "calib", "calibration", "pedestal", "junk" ]
    parser.add_argument( "--run-type", type=str,
                         choices=run_type_list,
                         default=None,
                         help="Choose the run type." )

    felix_list = [ "intt" + str(num) for num in range(0, 8) ] # ["intt0", "intt1", ..., "intt7"]
    felix_list.append( None ) 
    parser.add_argument( "--felix-server", type=str,
                         choices=felix_list,
                         default=felix_list[-1],
                         help="The run number to be processed. It doesn't need to be 8 digit." )

    parser.add_argument( "--run", type=str,
                         help="The run number to be processed. It doesn't need to be 8 digit." )

    parser.add_argument( "--chunk", type=str,
                         default="0000",
                         help="The chunk number to be processed. It doesn't need to be 4 digit." )

    parser.add_argument( "--file", type=str,
                         default=None,
                         help="File path to be used to make plots. " )

    ####################################################
    # For plot mode                                    #
    ####################################################
    parser.add_argument( "--plot",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Use to generate plots from a single ROOT file.\n" )

    parser.add_argument( "--plot-skip-hist-generation",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Histogram generation, which is necessary to update histograms, is skipped.\n" )

    exe_list = [ "macro", "exe" ]
    parser.add_argument( "--exe-type", type=str,
                         choices=exe_list,
                         default="macro",
                         help="Execution type for plot mode." )
    

    ####################################################
    # For homepage mode                                #
    ####################################################
    parser.add_argument( "--homepage",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Use to generate homepage for a run.\n" )

    
    ####################################################
    # For calib summary mode                           #
    ####################################################
    parser.add_argument( "--calib-summary",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Use to generate calibration summary.\n" )
    
    args = parser.parse_args()
    info = Information.Information( args )
    info.Print()
    
    exe_type = "exe"
    exe_type = None

    if info.plot_mode is True : 
        if info.plot_exe_type == "macro" : 
            app = "root"
            option = "-q -b -l"
            #option = " "
            macro = "FelixQuickViewer.cc"
            #file_path = "/sphenix/tg/tg01/commissioning/INTT/root_files/calib_intt7-00025922-0000.root"
            #file_path = "calib_intt7-00025922-0000.root"
            # file_path = "calib_intt0-12345678-0000.root"

            command = app + " " + option + " \'" + macro \
                + "( " \
                + "\"" + info.plot_file + "\", " \
                + "\"" + info.run_type + "\"," \
                + str( int(info.plot_skip_hist_generation) ) \
                + ")\'"
            print( command )

            if info.is_dry_run is False : 
                sp.run( command, shell=True )
                
        elif info.plot.exe_type == "exe" : 
            app = "./FelixQuickViewer.exe"
            option = ""

            command = app + " " + option + " " + info.plot_file + ""
            print( command )
            if info.is_dry_run is False : 
                sp.run( command, shell=True )
        # end of if info.plot_exe_type == "macro" :
        
        if info.is_dry_run is True:
            print( "Dry mode. Nothing is done" )

    if info.homepage_mode is True :
        hp = Homepage.Homepage( info )
        
