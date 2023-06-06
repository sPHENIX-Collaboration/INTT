import argparse
import datetime
import logging
import os
import socket
import subprocess
import sys

class Process() :
    def __init__( self, args ) :
        self.is_dry_run = args.dry_run if args.dry_run is not None else False
        self.run = args.run
        self.root_dir = args.root_dir if args.root_dir is not None else "commissioning_6_2/"
        self.root_subdir = args.root_subdir if args.root_subdir is not None else "hit_files/"
        self.run_type = args.run_type if args.run_type is not None else "beam"
        self.processes = []
        
    def Print( self ) :
        print( '+' + '-'*50 + '+' )
        print( "| Run:", self.run )
        print( "| Run type:", self.run_type )
        print( "| Is dry run:", self.is_dry_run )
        print( "| ROOT file directory:", self.root_dir )
        print( "| ROOT file sub-directory:", self.root_subdir )
        print( '+' + '-'*50 + '+' )

    def GetEventFilePath( self ) :
        directory = "/bbox/commissioning/INTT/" + self.run_type + "/"
        name = self.run_type + "_" + "inttX" + "-" + str(self.run).zfill( 8 ) + "-" + "0000" + ".evt"
        return directory + name
    
    def GetRootFilePath( self, is_event_wise=False ) :
        directory = "/home/inttdev/INTT/data/" + self.root_dir + self.root_subdir 
        name = self.run_type + "_" + "inttX" + "-" + str(self.run).zfill( 8 ) + "-" + "0000"
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
            if self.is_dry_run is True : 
                print( "SendCommandToAll:", command )
            else:
                self.SendCommand( num, command )


    def Decode( self ) :
        init_commands = "source /etc/profile ; source .bashrc ; source .bash_profile ; "
        # cd ${conversion_to_hit_base_dir}; ls ${data_dir}${data_name1}?${data_name2}????${raw_suffix}  | xargs -I {} -P 8 bash ./myfirstproject.sh {} "
        command_to_dir = "cd /home/phnxrc/INTT/jbertaux ; "
        command_decode = "ls " + self.GetEventFilePath().replace( "-0000.", "-????." ) + " | xargs -I {} bash ./myfirstproject.sh {}"
        #command_decode = "ls " + self.GetEventFilePath().replace( "-0000.", "-????." ) + " | xargs -I {} ls {}"
        whole_command = init_commands + command_to_dir + command_decode
        if self.is_dry_run is True : 
            print( "Decode:", whole_command )
        else:
            self.SendCommandToAll( whole_command )
            for proc in self.processes :
                print( "Waiting for decoding", proc )
                proc.wait()

            print( "All decoding ended" )

    def Do( self ) :
        # show evt files to be processed
        print( "test" )
        self.Decode()
        
        
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

    parser.add_argument( "--only-decode",
                         action=argparse.BooleanOptionalAction,
                         help="" )

    parser.add_argument( "--only-making-plots",
                         action=argparse.BooleanOptionalAction,
                         help="" )

    parser.add_argument( "--only-decode-event-wise",
                         action=argparse.BooleanOptionalAction,
                         help="" )

    parser.add_argument( "--only-decode-hit-wise",
                         action=argparse.BooleanOptionalAction,
                         help="" )


    #parser.add_argument( "--n_collisions", type=int, help="A value of one of FELIX parameter n_collisions, which is a size of an acceptance window for hits BCO value. An unsigned integer is expected." )
    #parser.add_argument( "--open_time", type=int, help="A value of one of FELIX parameter open_time, which is waiting time for arrival of hit information from ROCs in the units of BCO. An unsigned integer is expected." )

    args = parser.parse_args()
    process = Process( args )
    process.Print()
    process.Do()

