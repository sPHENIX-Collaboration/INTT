import argparse
import subprocess as sp

if __name__ == "__main__" :
    print( __name__ )
    exe_type = "exe"
    
    if exe_type == "macro" : 
        app = "root"
        option = "-q -b -l"
        macro = "FelixQuickViewer.cc"
        #file_path = "/sphenix/tg/tg01/commissioning/INTT/root_files/calib_intt7-00025922-0000.root"
        #file_path = "calib_intt7-00025922-0000.root"
        file_path = "calib_intt0-12345678-0000.root"

        command = app + " " + option + " \'" + macro + "( \"" + file_path + "\" )\'"
        print( command )
        sp.run( command, shell=True )
    elif exe_type == "exe" : 
        app = "./FelixQuickViewer.exe"
        option = ""
        #file_path = "/sphenix/tg/tg01/commissioning/INTT/root_files/calib_intt7-00025922-0000.root"
        #file_path = "calib_intt7-00025922-0000.root"
        file_path = "calib_intt0-12345678-0000.root"

        command = app + " " + option + " " + file_path + ""
        print( command )
        sp.run( command, shell=True )
