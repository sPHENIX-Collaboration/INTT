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
    # Mandatory parameter                              #
    ####################################################
    
    ####################################################
    # General flags                                    #
    ####################################################
    parser.add_argument( "--dry-run",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="A flag for testing the processes. It's better to use it before launching processes." )
    
    parser.add_argument( "--year", type=int,
                         default=datetime.date.today().year,
                         help="The year of the datat." )
    
    ####################################################
    # For plot mode                                    #
    ####################################################
    parser.add_argument( "--plot",
                         action=argparse.BooleanOptionalAction,
                         default=False,
                         help="Use to generate plots from a single ROOT file.\n" )

    parser.add_argument( "--file", type=str,
                         default=None,
                         help="File path to be used to make plots. " )

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

    parser.add_argument( "--run", type=str,
                         help="The run number to be processed. It doesn't need to be 8 digit." )

    
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
            macro = "FelixQuickViewer.cc"
            #file_path = "/sphenix/tg/tg01/commissioning/INTT/root_files/calib_intt7-00025922-0000.root"
            #file_path = "calib_intt7-00025922-0000.root"
            file_path = "calib_intt0-12345678-0000.root"

            command = app + " " + option + " \'" + macro + "( \"" + file_path + "\" )\'"
            print( command )
            sp.run( command, shell=True )
        elif info.plot.exe_type == "exe" : 
            app = "./FelixQuickViewer.exe"
            option = ""
            #file_path = "/sphenix/tg/tg01/commissioning/INTT/root_files/calib_intt7-00025922-0000.root"
            #file_path = "calib_intt7-00025922-0000.root"
            file_path = "calib_intt0-12345678-0000.root"

            command = app + " " + option + " " + file_path + ""
            print( command )
            sp.run( command, shell=True )

    if info.homepage_mode is True :
        hp = Homepage.Homepage( info )
        
