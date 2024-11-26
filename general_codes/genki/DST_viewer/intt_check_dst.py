#!/usr/bin/env python3

import os
import sys
import subprocess as sp

SCRIPT_PATH = ""
ROOT_MACRO_NAME = "show_dst_contents.cc"
KEYWORD = "List of Nodes in Fun4AllServer:"

def Check( data = "", debug=False ) :
    """
    @brief A function to check nodes in the given DST file
    """

    # Argument to root command depends on debug/normal mode
    if debug is False :
        # It's something like: root aaa.cc("data.root")
        root_macro = SCRIPT_PATH + "/" + ROOT_MACRO_NAME + "(\"" + data + "\")"
    else :
        # Here is for debug mode
        # It's something like: root aaa.cc
        root_macro = SCRIPT_PATH + "/" + ROOT_MACRO_NAME

    proc = sp.Popen( ["root", "-l", "-q", "-b", root_macro ], stdout=sp.PIPE )
    # I can add timeout feature..    
    proc.wait()

    # Get stdout ans show only the node information
    results = proc.communicate()[0].decode( "utf-8" )
    is_started = False
    print( "DST:", data )
    for line in results.split( "\n" ) : 
        if line == KEYWORD :
            is_started = True
        elif is_started == True and line == "" :
            break

        if is_started is True :
            # Cool tre structure... not made yet
            # words = line.replace( "  ", "aa" )
            # print( words )
            print( line )
    # End of for line in results.split( "\n" ) 

def HowToUse() :
    print( "Usage:", script_name, "[path to the DST to be checked]" )
    print( "For exmple:\n $", script_name,
           "/sphenix/lustre01/sphnxpro/physics/slurp/streaming/physics/inttonlyrun_00051100_00051200/DST_INTT_EVENT_run2pp_new_2024p002-00051100-00000.root" )
    
if __name__ == "__main__" :
    args = sys.argv

    # This script requires a path to the DST. If the format is different from the requirement, show how to use and exit
    if len( args ) != 2 :
        script_name = args[0].split( "/" )[-1]
        HowToUse()
        os.sys.exit()

    # Is global-like use OK?
    SCRIPT_PATH = os.path.dirname( args[0] )
    data = args[1]
    debug = False

    # Argument modification for the debug mode
    if data == "debug" :
        print( "Debug mode" )
        data = ""
        debug = True
    elif os.path.exists( data ) is False :
        # If the given DST is not found, do nothing
        print( data, "is not found" )
        HowToUse()
        os.sys.exit()

    Check( data, debug=debug )
