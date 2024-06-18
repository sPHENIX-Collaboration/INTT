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
import pathlib

import Printer
        
class Information() :
    def __init__( self, args ) :
        self.printer = Printer.Printer()
        
        #############################################################################
        # Initial processes for some general flags                                  #
        #############################################################################
        self.is_dry_run = args.dry_run if args.dry_run is not None else False
        self.year       = args.year
        
        if self.year > 2025 :
            self.printer.AddLine( "==== WARNING ====" )
            self.printer.AddLine( self.year, "is used as the year of the data." )
            self.printer.AddLine( "Please set it manually by --year option." )
            self.printer.AddLine( "2025 is set for now" )
            self.year = 2025
            self.printer.Print( color="Red" )
            self.printer.Clear()

        #############################################################################
        # Constant values                                                           #
        #############################################################################
        self.LUSTRE_DIR = pathlib.Path( "/sphenix/lustre01/sphnxpro/" )
        self.RAW_DATA_DIR = ""
        self.INTT_ROOT_DIR = pathlib.Path( "/sphenix/tg/tg01/commissioning/INTT/" )

        # under data dir
        self.DATA_ROOT_DIR = self.INTT_ROOT_DIR / "data"
        self.ROOT_FILE_DIR = self.DATA_ROOT_DIR / "root_files" / str( self.year ) 
        self.MERGED_ROOT_FILE_DIR = pathlib.Path( self.DATA_ROOT_DIR / "merged_root_files" / str( self.year ) )
        self.DST_FILE_DIR = pathlib.Path( self.DATA_ROOT_DIR / "dst_files" / str( self.year ) )

        # under QA dir
        self.QA_DIR = self.INTT_ROOT_DIR / "QA"
        self.HITMAP_DIR = self.QA_DIR / "hitmap"
        self.HOTMAP_DIR = self.QA_DIR / "hotdeadmap"
        self.BCODIFF_DIR = self.QA_DIR / "bco_bcofull_difference" 
        self.BCODIFF_DIR = self.QA_DIR / "bco_bcofull_difference"
        self.RAWHIT_DIR = self.QA_DIR / "raw_hit" / str( self.year )
        self.TRKRHIT_DIR = self.QA_DIR / "trkr_hit" / str( self.year )
        self.TRKRCLUSTER_DIR = self.QA_DIR / "cluster_hit" / str( self.year )
        self.COSMIC_DIR = self.QA_DIR / "cosmics"
        self.QA_SUBDIR_LIST = [ "root", "plots", "txt" ]

        #############################################################################
        # Processes for flags determined by command-line arguments                  #
        #############################################################################
        # Mode flags
        self.plot_mode = args.plot

        self.homepage_run_mode = args.homepage_run or args.homepage
        self.homepage_run_clean = args.homepage_run_clean
        self.homepage_title_mode = args.homepage_title or args.homepage
        self.homepage_mode = self.homepage_run_mode or self.homepage_title_mode
        
        self.calib_summary_mode = args.calib_summary
                    
        #############################################################################
        # Processes for plot mode                                                   #
        #############################################################################
        self.plot_file = args.file
        self.plot_skip_hist_generation = args.plot_skip_hist_generation
        
        self.plot_exe_type = args.exe_type
            
        #############################################################################
        # Processes for data                                                        #
        #############################################################################
        self.data_dir = pathlib.Path( args.data_dir )
        self.run_type = args.run_type
        self.felix_server = args.felix_server
        
        if args.run is not None:
            self.run        = str(args.run).zfill( 8 ) # fill all 8-digit length with 0 if given number is not 8-digit
        else:
            self.run = None

        if args.chunk is not None:
            self.chunk        = str(args.chunk).zfill( 4 ) # fill all 8-digit length with 0 if given number is not 8-digit
        else:
            self.chunk = None

        #############################################################################
        # Some more processes...                                                    #
        #############################################################################
        self.InitFlags()
        
        self.homepage_plots = []
        self.GetHomepagePlots()
        self.GetHotChannelPlots()
        self.GetRawHitPlots()
        self.GetCosmicPlots()
        self.GetBcoDiffPlots()
        self.GetTrkrHitPlots()
        
        #self.root_dir   = args.root_dir if args.root_dir is not None else "commissioning/"
        # add a directory separator if it's not at the end of string
        #if self.root_dir[-1] != "/" :
        #    self.root_dir += "/"
            
        # self.root_subdir = args.root_subdir if args.root_subdir is not None else "hit_files/"
        # # add a directory separator if it's not at the end of string
        # if self.root_subdir[-1] != "/" :
        #     self.root_subdir += "/"

        self.processes = []
        
        #if args.only is True :
        #    print( "Only ..." , flush=True )

    def InitFlags( self ) :
        if self.plot_file is not None :
            ############################################################
            # A data file name is assumed to be                        #
            # [run_type]_[felix_server]-[run_number]-[chunk].root      #
            #                                                          #
            #   [run_type]:                                            #
            #   [felix_server]: intt[num], where [num] is from 0 to 7  #
            #   [run_number]: 8-digit integer                          #
            #   [chunk]: 4-digit integer                               #
            ############################################################
            
            # If given file cannot be found...
            if os.path.exists( self.plot_file ) is False:
                self.printer.AddLine( "==== WARNING ====" )
                self.printer.AddLine( self.plot_file, "is not found." )
                self.printer.AddLine( "Plot mode is turned OFF" )
                self.plot_mode = False
                self.printer.Print( color="Red" )
                self.printer.Clear()

            # for run type
            if self.run_type is None :
                self.run_type = os.path.basename( self.plot_file ).split( '_' )[0]
                if self.run_type == "intt" :
                    print( "Run type cannot be detected from the file name." \
                           "Please use --run-type flag to give it." \
                           "junk is used..." )
                    self.run_type = "junk"

            # for felix server
            if self.felix_server is None :
                self.felix_server = os.path.basename( self.plot_file ).split( '-' )[0].split( '_' )[1]
                
            # for run
            if self.run is None :
                self.run = os.path.basename( self.plot_file ).split( '-' )[1]

            # for chunk
            if self.chunk is None :
                self.chunk = os.path.basename( self.plot_file ).split( '-' )[2]

        else : # in the case no plot_file is given, let's guess from run number            
            if (self.run is None or self.felix_server is None or self.run_type is None or self.chunk is None) and self.plot_mode is True:
                print( "No information about file to be used is given.\n" \
                       "Please user\n" \
                       "  --file or\n" \
                       "  --run , --run_type , --felix-server ,and --chunk.",
                       file=sys.stderr )

            elif self.plot_mode is True:
                self.plot_file = self.data_dir / pathlib.Path( self.run_type + "_" + self.felix_server + "-" + self.run + "-" + self.chunk + ".root" )


        # If no file is given...
        if self.plot_mode is True and self.plot_file is None :
            self.printer.AddLine( "==== WARNING ====" )
            self.printer.AddLine( "No file is given for plot mode." )
            self.printer.AddLine( "Plot mode is turned OFF" )
            self.plot_mode = False
            self.printer.Print( color="Red" )
            self.printer.Clear()

    def GetPlots( self, dir, suffix=".pdf" ) :
        """
        @brief A function to make a list of plots using arguments
        @param dir It should be pathlib.Path or anything that str() works
        @param container A list to contain the list. A member variable of a list is good.
        @param suffix The suffix to be used for the file serch
        """
        return sorted( list( dir.glob( "*" + str(self.run) + "*" + suffix) ) )
    
    def GetHomepagePlots( self ) :
        # For example:
        # <class 'pathlib.PosixPath'> beam_intt0-00023726-0000_adc.png
        # <class 'pathlib.PosixPath'> beam_intt0-00023726-0000_entryvschan.png
        self.homepage_plots = sorted( list(self.ROOT_FILE_DIR.glob( "*" + str(self.run) + "*.png" ) ) )
        
    def GetHotChannelPlots( self ) :
        # For example:
        path = self.HOTMAP_DIR / "plots" / str( self.year )
        self.hot_channel_plots = sorted( list( path.glob( "*" + str(self.run) + "*.pdf" ) ) )
        
        path = self.HOTMAP_DIR / "txt" / str( self.year )
        self.hot_channel_txt = sorted( list( path.glob( "*" + str(self.run) + "*.txt" ) ) )        

    def GetCosmicPlots( self ) :
        #path = self.COSMIC_DIR / "plots" / str( self.year )
        #self.cosmic_plots = sorted( list( path.glob( "*" + str(self.run) + "*.pdf" ) ) )
        self.cosmic_plots = self.GetPlots( self.COSMIC_DIR / "plots" )

    def GetBcoDiffPlots( self ) :
        self.bco_diff_plots = self.GetPlots( self.BCODIFF_DIR / "plots" / str(self.year) )
        
    def GetRawHitPlots( self ) :
        print( "Raw hit plot" )
        self.raw_hit_plots = self.GetPlots( self.RAWHIT_DIR / "plots" )

    def GetTrkrHitPlots( self ) :
        print( "Trkrhit plot" )
        self.trkr_hit_plots = self.GetPlots( self.TRKRHIT_DIR / "plots" )
        print( self.trkr_hit_plots )

    def GetTrkrClusterPlots( self ) :
        self.trkr_cluster_plots = self.GetPlots( self.TRKRCLUSTER_DIR / "plots" ) 

    def PrintLine( self ) :
        print( '+' + '-'*50 + '+' , flush=True )
        
    def Print( self ) :
        header_color = 'Yellow'

        # General flags/parameters
        self.printer.AddLine( "General flags/paremeters", color=header_color )
        self.printer.AddLine( "Is dry mode?       ", self.is_dry_run )
        self.printer.AddLine( "Data year:         ", self.year )

        # Flags for the mode selection
        self.printer.AddSeparator()
        self.printer.AddLine( "Mode"               , color=header_color )
        self.printer.AddLine( "Run type:          ", self.run_type )
        self.printer.AddLine( "Plot mode:         ", self.plot_mode )
        self.printer.AddLine( "Homepage mode:     ", self.homepage_mode )
        if self.homepage_mode:
            self.printer.AddLine( "   + Run page:     ", self.homepage_run_mode )
            self.printer.AddLine( "   + Title page:   ", self.homepage_title_mode )
            
        self.printer.AddLine( "Calib summary mode:", self.calib_summary_mode )

        self.printer.AddSeparator()
        self.printer.AddLine( "Data"               , color=header_color )        
        self.printer.AddLine( "File:              ", self.plot_file )
        self.printer.AddLine( "Run type:          ", self.run_type )
        self.printer.AddLine( "FELIX:             ", self.felix_server )
        self.printer.AddLine( "Run:               ", self.run )
        self.printer.AddLine( "Chunk:             ", self.chunk )

        if self.plot_mode is True :
            self.printer.AddSeparator()
            self.printer.AddLine( "Parameters for plot mode", color=header_color )
            self.printer.AddLine( "Execution type:      ", self.plot_exe_type )
            self.printer.AddLine( "Skip hist generation:", self.plot_skip_hist_generation )
            print( os.path.exists( self.plot_file ) )
            
        if self.homepage_mode is True :
            self.printer.AddSeparator()
            self.printer.AddLine( "Parameters for homepage mode", color=header_color )

            if len( self.homepage_plots) > 0 : 
                self.printer.AddLine( "Directory:         ", self.homepage_plots[0].parent )
                self.PrintHomepagePlots()
                
        if self.calib_summary_mode is True :
            self.printer.AddSeparator()
            self.printer.AddLine( "Parameters for calib summary mode", color=header_color )
            
        """
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

        self.printer.AddSeparator()
        self.printer.AddLine( "Auto Update?", self.auto_update, color=header_color )
        self.printer.AddLine( "Update list?", self.update_list, color=header_color )
        self.printer.AddLine( "    New list:", self.evt_list )
        self.printer.AddLine( "    Old list:", self.evt_list_previous )
        """
        self.printer.Clear()

    def PrintHomepagePlots( self ) :
        for image in self.homepage_plots :
            self.printer.AddLine( "    -", str(image.name) )
            
        self.printer.Print()

    def GetFilePrefix( self, run_type ) :
        """
        @brief A prefix of a file (evt, root) is returned. It's needed because there are some exceptionals...
        """
        if run_type == "pedestal" or self.run_type == "junk" : 
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
    
