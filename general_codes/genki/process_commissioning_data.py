import argparse
import datetime
import logging
import os
import socket
import subprocess
import sys
import time

class Process() :
    def __init__( self, args ) :
        # storing flags
        self.is_dry_run = args.dry_run if args.dry_run is not None else False
        self.run = str(args.run).zfill( 8 ) 
        self.root_dir = args.root_dir if args.root_dir is not None else "commissioning_6_4/"
        # add a directory separator if it's not at the end of string
        if self.root_dir[-1] != "/" :
            self.root_dir += "/"
            
        self.root_subdir = args.root_subdir if args.root_subdir is not None else "hit_files/"
        # add a directory separator if it's not at the end of string
        if self.root_subdir[-1] != "/" :
            self.root_subdir += "/"
            
        self.run_type = args.run_type if args.run_type is not None else "beam"
        self.plotting_server = "inttdaq"
        self.home_dir_in_plotting_server = "/home/inttdev/"
        self.root_dir_in_plotting_server = "/1008_home/phnxrc/INTT/" + self.root_dir + self.root_subdir
        self.data_dir_in_plotting_server = self.home_dir_in_plotting_server + "INTT/data/" + self.root_dir + self.root_subdir

        # process flags
        self.process_only = args.only if args.only is not None else False
        default = not self.process_only
        
        self.decode = args.decode if args.decode is not None else default
        self.decode_hit_wise = args.decode_hit_wise if args.decode_hit_wise is not None else default
        self.decode_event_wise = args.decode_event_wise if args.decode_event_wise is not None else default
        self.make_symbolic = args.make_symbolic if args.make_symbolic is not None else default
        self.make_plot = args.make_plot if args.make_plot is not None else default
        self.transfer_plot = args.transfer_plot if args.transfer_plot is not None else default

            
        # misc
        self.plotter = self.home_dir_in_plotting_server + "macro/FelixQuickViewer_1Felix.C"

        self.processes = []


        if args.only is True :
            print( "Only ..." )

    def PrintLine( self ) :
        print( '+' + '-'*50 + '+' )
        
    def Print( self ) :
        self.PrintLine()
        print( "| Run:", self.run )
        print( "| Run type:", self.run_type )
        print( "| Is dry run:", self.is_dry_run )
        print( "| ROOT file directory:", self.root_dir )
        print( "| ROOT file sub-directory:", self.root_subdir )
        self.PrintLine()
        print( "| Does decode?", self.decode )
        print( "|    hit-wise?", self.decode_hit_wise )
        print( "|  event-wise?", self.decode_event_wise )
        self.PrintLine()
        print( "| Making symbolics?", self.make_symbolic )
        print( "|    Data dir in", self.plotting_server, ":", self.data_dir_in_plotting_server )
        print( "|    root dir in", self.plotting_server, ":", self.root_dir_in_plotting_server )
        self.PrintLine()

    def GetEventFilePath( self ) :
        directory = "/bbox/commissioning/INTT/" + self.run_type + "/"
        #name = self.run_type + "_" + "inttX" + "-" + str(self.run).zfill( 8 ) + "-" + "0000" + ".evt"
        name = self.run_type + "_" + "inttX" + "-" + self.run + "-" + "0000" + ".evt"
        return directory + name
    
    def GetRootFilePath( self, is_event_wise=False ) :
        directory = "/home/inttdev/INTT/data/" + self.root_dir + self.root_subdir 
        #name = self.run_type + "_" + "inttX" + "-" + str(self.run).zfill( 8 ) + "-" + "0000"
        name = self.run_type + "_" + "inttX" + "-" + self.run + "-" + "0000"
        if is_event_wise is True :
            name += ".root"
        else :
            name += "_event_base.root"
            
        return directory + name
    
    def SendCommand( self, intt_num, command ) :
        server="intt" + str( intt_num )
        ssh_command = "ssh " + server + " \"" + command.replace( "inttX" , "intt"+str(intt_num) ) + "\""
        if self.is_dry_run is True : 
            print( "SendCommand:", ssh_command )
        else:
            proc = subprocess.Popen( ssh_command, shell=True )
            self.processes.append( proc )

    def SendCommandToAll( self, command ) :
        for num in range(0, 8 ) :
            #if num ==1 or num==7 :
            #continue
            
            if self.is_dry_run is True : 
                print( "SendCommandToAll:", command )

            self.SendCommand( num, command )

    def Decode( self, is_event_wise=False ) :
        init_commands = "source /etc/profile ; source .bashrc ; source .bash_profile ; "
        # cd ${conversion_to_hit_base_dir}; ls ${data_dir}${data_name1}?${data_name2}????${raw_suffix}  | xargs -I {} -P 8 bash ./myfirstproject.sh {} "

        if is_event_wise is False :
            command_to_dir = "cd /home/phnxrc/INTT/jbertaux ; "
            command_decode = "ls -r " + self.GetEventFilePath().replace( "-0000.", "-????." ) + " | xargs -I {} bash ./myfirstproject.sh {}"
            #command_decode = "ls " + self.GetEventFilePath().replace( "-0000.", "-????." ) + " | xargs -I {} ls {}"
            whole_command = init_commands + command_to_dir + command_decode
        else:
            command_to_dir = "cd /home/phnxrc/INTT/hachiya/convertInttRaw/test1/ ; "
            command_decode = "ls -r " + self.GetEventFilePath().replace( "-0000.", "-????." ) + " | xargs -I {} root -l -q -b 'runConvertInttData.C(\\\"{}\\\")'"
            #command_decode = "ls " + self.GetEventFilePath().replace( "-0000.", "-????." ) + " | xargs -I {} ls {}"
            whole_command = init_commands + command_to_dir + command_decode
            
        if self.is_dry_run is True : 
            print( "Decode:", whole_command )

        self.SendCommandToAll( whole_command )
        for proc in self.processes :
            print( "Waiting for decoding", proc )
            proc.wait()

        print( "All decoding ended" )

    def MakeSymbolic( self ) :
        command = "ssh " + self.plotting_server + " \""
        command += "cd " + self.data_dir_in_plotting_server + " ; "
        command += "ls " + self.root_dir_in_plotting_server + "/*.root | xargs -I {} ln -s {} 2>/dev/null"
        command += "\""
        print( command )

        if self.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
            proc.wait()
        
    def MakePlot( self ) :
        command = "ssh " + self.plotting_server + " \""
        command += "cd " + self.data_dir_in_plotting_server + " ; "
        command += "ls *" + self.run + "*.root | grep -v event_base | xargs -I {} -P 8  root -q -l -b \'" + self.plotter + "(\\\"{}\\\", \\\".pdf\\\" )\'"
        command += "\""
        print( command )

        if self.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
            proc.wait()
            print( "All plots were made." )

    def TransferPlot( self ) :
        command = "scp " + self.plotting_server + ":" + self.data_dir_in_plotting_server + "*" + self.run + "*.pdf ."
        print( command )

        if self.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
            proc.wait()
            print( "All plots were transfered to here." )
        
    def Do( self ) :
        # show evt files to be processed

        if self.decode is True :
            if self.decode_hit_wise is True:
                self.Decode()
            if self.decode_event_wise is True:
                self.Decode( is_event_wise=True )

        if self.make_symbolic is True : 
            self.MakeSymbolic()

        if self.make_plot is True : 
            self.MakePlot()

        if self.transfer_plot is True :
            self.TransferPlot()
        
if __name__ == "__main__" :
    # A argument parser class to accept command-line options
    parser = argparse.ArgumentParser()
    parser.add_argument( "run", type=int,
                         help="" )
    
    parser.add_argument( "--run-type", type=str,
                         help="beam/calib/junk/calibration" )
    
    parser.add_argument( "--root-dir", type=str,
                         help="A name of directory that contains ROOT files. commissioning_6_2 is default." )
    
    parser.add_argument( "--root-subdir", type=str,
                         help="A name of sub-directory that contains ROOT files. hit_files is default." )
    
    
    parser.add_argument( "--dry-run",
                         action=argparse.BooleanOptionalAction,
                         help="A type of ADC configuration for DAC scan. 1 to 10 as integers are accepted. " )

    # process flags
    parser.add_argument( "--decode",
                         action=argparse.BooleanOptionalAction,
                         help="" )

    parser.add_argument( "--decode-hit-wise",
                         action=argparse.BooleanOptionalAction,
                         help="" )

    parser.add_argument( "--decode-event-wise",
                         action=argparse.BooleanOptionalAction,
                         help="" )

    parser.add_argument( "--make-symbolic",
                         action=argparse.BooleanOptionalAction,
                         help="" )
    
    parser.add_argument( "--make-plot",
                         action=argparse.BooleanOptionalAction,
                         help="" )

    parser.add_argument( "--transfer-plot",
                         action=argparse.BooleanOptionalAction,
                         help="" )

    parser.add_argument( "--only",
                         action=argparse.BooleanOptionalAction,
                         help="" )

    # 
    #parser.add_argument( "--n_collisions", type=int, help="A value of one of FELIX parameter n_collisions, which is a size of an acceptance window for hits BCO value. An unsigned integer is expected." )
    #parser.add_argument( "--open_time", type=int, help="A value of one of FELIX parameter open_time, which is waiting time for arrival of hit information from ROCs in the units of BCO. An unsigned integer is expected." )

    args = parser.parse_args()
    print( args )
    process = Process( args )
    process.Print()
    process.Do()

    time.sleep( 1 )
    print( "All processes were ended" )

