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
import pprint

import Printer
        
class Information() :
    def __init__( self, args ) :
        self.printer = Printer.Printer()
        
        #############################################################################
        # Processes for flags determined by command-line arguments                  #
        #############################################################################
        # storing flags
        self.is_dry_run = args.dry_run if args.dry_run is not None else False
        self.is_force_run = args.force_run if args.force_run is not None else False

        #self.run        = str(args.run).zfill( 8 ) # fill all 8-digit length with 0 if given number is not 8-digit
        self.runs = [ str(element).zfill(8) for element in args.run ]
        self.run = -1 # self.runs[0]
        self.year = args.year
        if self.year is None:
            self.year = datetime.date.today().year
            
        self.runs_processed = [] # A list of runs to be processed. It's for auto-update mode for now.
        self.root_dir   = args.root_dir if args.root_dir is not None else "commissioning/"
        # add a directory separator if it's not at the end of string
        if self.root_dir[-1] != "/" :
            self.root_dir += "/"
            
        self.root_subdir = args.root_subdir if args.root_subdir is not None else "hit_files/"
        # add a directory separator if it's not at the end of string
        if self.root_subdir[-1] != "/" :
            self.root_subdir += "/"

        # A variable to specify the type of run. It's determined by rcdaq. The file name starts from this string.
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
        self.send_SDCC         = args.send_SDCC    if args.send_SDCC    is not None else False
        self.send_map_SDCC     = args.send_map_SDCC if args.send_map_SDCC    is not None else False
        self.process_SDCC      = args.process_SDCC if args.process_SDCC is not None else False
        self.calib_2024        = args.calib_2024   if args.calib_2024   is not None else False
        self.update_plot       = args.update_plot  if args.update_plot  is not None else False
        self.condor = args.condor
        self.quick = args.quick
        self.check_bbox        = args.check_bbox   if args.check_bbox   is not None else True
        self.bbox_commissioning  = args.bbox_commissioning
        self.end_process_SDCC = False # Some processes at the end of processes at SDCC

        if self.send_SDCC is True :
            self.send_map_SDCC = True
                    
        # Priority of send_SDCC and process_SDCC is higher than check_box.
        # If one of send_SDCC and process_SDCC is specified, but check_box was not given, determin check_box by the flag.
        if args.check_bbox is None :
            # If check_box is not given, set is_SDCC_used using send_SDCC and process_SDCC. Then check_box is determined by is_SDCC_used
            self.is_SDCC_used      = self.send_SDCC or self.process_SDCC
            self.check_bbox        = self.is_SDCC_used
        else :
            # If check_box is given, set is_SDCC_used using the 3 flags
            self.is_SDCC_used      = self.send_SDCC or self.process_SDCC or self.check_bbox

        if self.calib_2024 is True :  # for calibration 2024 mode
            self.process_SDCC = True  # Let's do processes at SDCC
            self.send_SDCC    = False # No need to send data. Let's assume it's already done
            self.check_bbox   = False # No need to check the buffer box because the data has been sent to SDCC
            self.end_process_SDCC = True
            self.bbox_commissioning  = True
            self.is_force_run = True
            self.year = 2024

        if self.update_plot is True :  # for calibration 2024 mode
            self.process_SDCC = False # No need for process. Only re-make and update plot
            self.send_SDCC    = False # No need to send data. Let's assume it's already done
            self.check_bbox   = False # No need to check the buffer box because the data has been sent to SDCC
            self.end_process_SDCC = True
            self.is_force_run = True
            self.year = 2024

        #############################################################
        # Flags for DST processes                                   #
        #############################################################
        self.dst_all = args.DST_all
        self.dst_raw = args.DST_INTTRAW or self.dst_all
        self.dst_raw_hitmap = args.DST_INTTRAW_hitmap or self.dst_all
        self.dst_raw_hot_ch = args.DST_INTTRAW_hot_ch or self.dst_all
        self.dst_raw_bco_diff = args.DST_INTTRAW_bco_diff or self.dst_all
        self.dst_TrkrHit = args.DST_TrkrHit or self.dst_all
        self.dst_TrkrCluster = args.DST_TrkrCluster or self.dst_all

        self.dst_any = \
            self.dst_raw or \
            self.dst_raw_hitmap or \
            self.dst_raw_hot_ch or \
            self.dst_raw_bco_diff or \
            self.dst_TrkrHit or \
            self.dst_TrkrCluster

        # It should be OK to force running for DST processes, I hope
        if self.dst_any is True : 
            self.is_force_run = True
            
        # paths
        self.plotter           = self.home_dir_in_plotting_server + "macro/FelixQuickViewer_1Felix.C"
        self.plotter_in_server = self.sphenix_intt_home + "macro/FelixQuickViewer_1Felix.C"
        self.bbox_dir = pathlib.Path( "/bbox/bbox?/INTT/" if self.bbox_commissioning is False else "/bbox/commissioning/INTT/" )

        source_dir             = os.path.dirname( __file__ ) + "/"
        self.config_dir             = str( pathlib.Path().home() / ".INTT" )
        self.evt_list          = self.config_dir + "/" + self.run_type + "_list.txt"
        self.evt_list_previous = self.config_dir + "/" + self.run_type + "_list_previous.txt"
        self.evt_list_temp     = self.config_dir + "/" + self.run_type + "_list_temp.txt"
        if pathlib.Path( self.config_dir ).exists() is False : 
            pathlib.Path( self.config_dir ).mkdir()

        self.processes = []

        if args.only is True :
            print( "Only ..." , flush=True )

    def SetFlagsForAutoUpdate( self ) :
        self.decode            = True
        self.decode_hit_wise   = True
        self.decode_event_wise = True
        self.make_symbolic     = True
        self.make_plot         = True
        self.transfer_plot     = True
        self.auto_update       = False

    def PrintLine( self ) :
        print( '+' + '-'*50 + '+' , flush=True )
        
    def Print( self ) :
        header_color = 'Green'
        self.printer.AddLine( "Run:", self.run, color=header_color )
        
        self.printer.AddLine( "Run type:", self.run_type )
        self.printer.AddLine( "Is dry run:", self.is_dry_run )
        self.printer.AddLine( "Force running?:", self.is_force_run )
        self.printer.AddLine( "ROOT file directory:", self.root_dir )
        self.printer.AddLine( "ROOT file sub-directory:", self.root_subdir )
        self.printer.AddSeparator()
        
        self.printer.AddLine( "Does decode?", self.decode, color=header_color )
        if self.decode is True : 
            self.printer.AddLine( "   hit-wise?", self.decode_hit_wise )
            self.printer.AddLine( " event-wise?", self.decode_event_wise )
        self.printer.AddSeparator()
        
        self.printer.AddLine( "Making symbolics?", self.make_symbolic, color=header_color )
        if self.make_symbolic is True : 
            self.printer.AddLine( "   Data dir in", self.plotting_server, ":", self.data_dir_in_plotting_server )
            self.printer.AddLine( "   root dir in", self.plotting_server, ":", self.root_dir_in_plotting_server )
            
        self.printer.AddSeparator()
        self.printer.AddLine( "Making plots?", self.make_plot, color=header_color )
        self.printer.AddLine( "   ROOT macro", self.plotter )
        self.printer.AddSeparator()
        
        self.printer.AddLine( "Transferring plots?", self.transfer_plot, color=header_color )
        if self.transfer_plot is True : 
            self.printer.AddLine( "    Output directory:", self.transfer_dir )

        self.printer.AddSeparator()
        self.printer.AddLine( "Is SDCC used?", self.is_SDCC_used, color=header_color )
        if self.is_SDCC_used :
            self.printer.AddLine( "    Check files to SDCC? ", self.check_bbox )
            self.printer.AddLine( "    Send files to SDCC?  ", self.send_SDCC )
            self.printer.AddLine( "    Do processes at SDCC?", self.process_SDCC )
            self.printer.AddLine( "    Calib 2024 mode?     ", self.calib_2024 )
            self.printer.AddLine( "    End processes?       ", self.end_process_SDCC )
            self.printer.AddLine( "    Only update plot?    ", self.update_plot )

        self.printer.AddSeparator()
        self.printer.AddLine( "Does DST processes?", self.dst_any )
        if self.dst_any is True :
            self.printer.AddLine( "    Make DST(INTTRAW)?", self.dst_raw )
            self.printer.AddLine( "    Make hitmap?", self.dst_raw_hitmap )
            self.printer.AddLine( "    Make hot ch map?", self.dst_raw_hot_ch )
            self.printer.AddLine( "    Make BCO diff CDB?", self.dst_raw_bco_diff )
            self.printer.AddLine( "    Make DST(Trkr_hit)?", self.dst_TrkrHit )
            self.printer.AddLine( "    Make DST(Trkr_cluster)?", self.dst_TrkrCluster )

        self.printer.AddSeparator()
        self.printer.AddLine( "Auto Update?", self.auto_update, color=header_color )
        self.printer.AddLine( "Update list?", self.update_list, color=header_color )
        self.printer.AddLine( "    New list:", self.evt_list )
        self.printer.AddLine( "    Old list:", self.evt_list_previous )

        self.printer.Print()

    def IsOtherProcessRunning( self ) :
        
        command = "ps aux | grep -v -e grep -v -e emacs -v -e vi "

        #if self.is_SDCC_used :
        #    command += "-e SDCC "
            
        command += " | grep " + __file__
        
        proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
        outputs = proc.communicate()[0].decode().split( '\n' )[0:-1]

        if len(outputs) > 1 :
            return True
        else:
            return False

    def GetBBoxServer( self, intt_num ) :
        """
        @brief The corresponding buffer box server to the given INTT Felix server is returned.
        @retval "bbox?" as a string, where ? is from 0 to 5.
        """
        return "bbox" + str( intt_num % 6 )

    def GetFilePrefix( self, run_type ) :
        """
        @brief A prefix of a file (evt, root) is returned. It's needed because there are some exceptionals...
        """
        if (run_type == "pedestal" or self.run_type == "junk") and self.year == 2023 : 
            rtn = "intt"
        else :
            rtn = run_type

        return rtn
    
    def GetEventFilePath( self ) :
        directory = str( self.bbox_dir ) + "/" + self.run_type + "/"
        name = self.GetFilePrefix( self.run_type ) + "_" + "intt?" + "-" + str(self.run) + "-" + "????" + ".evt"
        return directory + name
    
    def GetRootFilePath( self, is_event_wise=False ) :
        #directory = "/home/inttdev/INTT/data/" + self.root_dir + self.root_subdir 
        directory = "/bbox/commissioning/INTT/" + self.root_dir + self.root_subdir 
        #name = self.run_type + "_" + "inttX" + "-" + str(self.run).zfill( 8 ) + "-" + "0000"
        name = self.run_type + "_" + "inttX" + "-" + self.run + "-" + "0000"

        if self.run_type == "pedestal" : 
            name = "intt_" + "inttX" + "-" + self.run + "-" + "0000"
        else :
            name = self.run_type + "_" + "inttX" + "-" + self.run + "-" + "0000"

        if is_event_wise is False :
            name += ".root"
        else :
            name += "_event_base.root"
            
        return directory + name
    
