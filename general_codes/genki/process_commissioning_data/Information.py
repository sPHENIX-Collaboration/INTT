import argparse
import datetime
import logging
import os
import pathlib
import pprint
import socket
import subprocess
import sys
import time

class Information() :
    def __init__( self, args ) :
        #############################################################################
        # Processes for flags determined by command-line arguments                  #
        #############################################################################
        # storing flags
        self.is_dry_run = args.dry_run if args.dry_run is not None else False
        self.is_force_run = args.force_run if args.force_run is not None else False
        
        self.run        = str(args.run).zfill( 8 ) # fill all 8-digit length with 0 if given number is not 8-digit
        self.root_dir   = args.root_dir if args.root_dir is not None else "commissioning/"
        # add a directory separator if it's not at the end of string
        if self.root_dir[-1] != "/" :
            self.root_dir += "/"
            
        self.root_subdir = args.root_subdir if args.root_subdir is not None else "hit_files/"
        # add a directory separator if it's not at the end of string
        if self.root_subdir[-1] != "/" :
            self.root_subdir += "/"
            
        self.run_type                    = args.run_type if args.run_type is not None else "beam"
        self.plotting_server             = "inttdaq" # Obsolete, We don't use inttdaq to make plots anymore
        self.home_dir_in_plotting_server = "/home/inttdev/" # Obsolete, We don't use inttdaq to make plots anymore
        self.root_dir_in_plotting_server = "/1008_home/phnxrc/INTT/" + self.root_dir + self.root_subdir # Obsolete, We don't use inttdaq to make plots anymore
        self.data_dir_in_plotting_server = self.home_dir_in_plotting_server + "INTT/data/" + self.root_dir + self.root_subdir # Obsolete, We don't use inttdaq to make plots anymore
        self.sphenix_common_home = "/home/phnxrc/"
        self.sphenix_intt_home = self.sphenix_common_home + "INTT/"
        
        # process flags ####################3
        self.process_only = args.only if args.only is not None else False
        default = not self.process_only
        
        self.decode            = args.decode            if args.decode            is not None else default
        self.decode_hit_wise   = args.decode_hit_wise   if args.decode_hit_wise   is not None else default
        self.decode_event_wise = args.decode_event_wise if args.decode_event_wise is not None else default
        # check confliction, if hit_wise or event_wise is True, self.decode should be True. The opposite pattern may not happen
        if self.decode is not (self.decode_hit_wise or self.decode_event_wise ) :
            if args.decode is not None :
                self.decode_hit_wise  = self.decode
                self.decode_event_wise = self.decode
            else:
                self.decode            =  (self.decode_hit_wise or self.decode_event_wise )
        
        self.make_symbolic     = args.make_symbolic     if args.make_symbolic     is not None else False
        self.make_plot         = args.make_plot         if args.make_plot         is not None else default
        self.transfer_plot     = args.transfer_plot     if args.transfer_plot     is not None else False
        self.transfer_dir      = args.transfer_dir      if args.transfer_dir      is not None else "./"

        self.auto_update       = args.auto_update       if args.auto_update       is not None else False
        self.update_list       = args.update_list       if args.update_list       is not None else self.auto_update

        # Flags related to SDCC
        self.send_SDCC         = args.send_SDCC if args.send_SDCC is not None else False
        self.process_SDCC      = args.process_SDCC if args.process_SDCC is not None else False
        self.is_SDCC_used = self.send_SDCC or self.process_SDCC
        
        # misc
        self.plotter           = self.home_dir_in_plotting_server + "macro/FelixQuickViewer_1Felix.C"
        self.plotter_in_server = self.sphenix_intt_home + "macro/FelixQuickViewer_1Felix.C"

        source_dir             = os.path.dirname( __file__ ) + "/"
        config_dir             = str( pathlib.Path().home() / ".INTT" )
        self.evt_list          = config_dir + "/" + self.run_type + "_list.txt"
        self.evt_list_previous = config_dir + "/" + self.run_type + "_list_previous.txt"
        if pathlib.Path( config_dir ).exists() is False : 
            pathlib.Path( config_dir ).mkdir()

        self.processes = []

        if args.only is True :
            print( "Only ..." )

    def SetFlagsForAutoUpdate( self ) :
        self.decode            = True
        self.decode_hit_wise   = True
        self.decode_event_wise = True
        self.make_symbolic     = True
        self.make_plot         = True
        self.transfer_plot     = True
        self.auto_update       = False


    def PrintLine( self ) :
        print( '+' + '-'*50 + '+' )
        
    def Print( self ) :
        self.PrintLine()
        print( "| Run:", self.run )
        print( "| Run type:", self.run_type )
        print( "| Is dry run:", self.is_dry_run )
        print( "| Force running?:", self.is_force_run )
        print( "| ROOT file directory:", self.root_dir )
        print( "| ROOT file sub-directory:", self.root_subdir )
        self.PrintLine()
        print( "| Does decode?", self.decode )
        if self.decode is True : 
            print( "|    hit-wise?", self.decode_hit_wise )
            print( "|  event-wise?", self.decode_event_wise )
        self.PrintLine()
        
        print( "| Making symbolics?", self.make_symbolic )
        if self.make_symbolic is True : 
            print( "|    Data dir in", self.plotting_server, ":", self.data_dir_in_plotting_server )
            print( "|    root dir in", self.plotting_server, ":", self.root_dir_in_plotting_server )
            
        self.PrintLine()
        print( "| Making plots?", self.make_plot )
        print( "|    ROOT macro", self.plotter )
        self.PrintLine()
        
        print( "| Transferring plots?", self.transfer_plot )
        if self.transfer_plot is True : 
            print( "|     Output directory:", self.transfer_dir )

        self.PrintLine()
        print( "| Is SDCC used?", self.is_SDCC_used )
        if self.is_SDCC_used :
            print( "|     Send files to SDCC?", self.send_SDCC )
            print( "|     Do processes at SDCC?", self.process_SDCC )
        
        self.PrintLine()
        print( "| Auto Update?", self.auto_update )
        print( "| Update list?", self.update_list )
        print( "|     New list:", self.evt_list )
        print( "|     Old list:", self.evt_list_previous )

        self.PrintLine()

    def IsOtherProcessRunning( self ) :
        command = "ps aux | grep -v -e grep -v -e emacs -v -e vi | grep " + __file__
        print( command )
        #command = "ls -1"
        proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
        outputs = proc.communicate()[0].decode().split( '\n' )[0:-1]
        print( "ps:", outputs )
        print( type(outputs), len(outputs) )

        if len(outputs) > 1 :
            return True
        else:
            return False

    def GetEventFilePath( self ) :
        directory = "/bbox/commissioning/INTT/" + self.run_type + "/"
        #name = self.run_type + "_" + "inttX" + "-" + str(self.run).zfill( 8 ) + "-" + "0000" + ".evt"
        name = self.run_type + "_" + "inttX" + "-" + self.run + "-" + "0000" + ".evt"
        return directory + name
    
    def GetRootFilePath( self, is_event_wise=False ) :
        #directory = "/home/inttdev/INTT/data/" + self.root_dir + self.root_subdir 
        directory = "/bbox/commissioning/INTT/" + self.root_dir + self.root_subdir 
        #name = self.run_type + "_" + "inttX" + "-" + str(self.run).zfill( 8 ) + "-" + "0000"
        name = self.run_type + "_" + "inttX" + "-" + self.run + "-" + "0000"
        if is_event_wise is False :
            name += ".root"
        else :
            name += "_event_base.root"
            
        return directory + name
    
