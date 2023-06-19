import argparse
import datetime
import logging
import os
import pprint
import socket
import subprocess
import sys
import time

class Process() :
    def __init__( self, args ) :
        # storing flags
        self.is_dry_run = args.dry_run if args.dry_run is not None else False
        self.run        = str(args.run).zfill( 8 ) 
        self.root_dir   = args.root_dir if args.root_dir is not None else "commissioning/"
        # add a directory separator if it's not at the end of string
        if self.root_dir[-1] != "/" :
            self.root_dir += "/"
            
        self.root_subdir = args.root_subdir if args.root_subdir is not None else "hit_files/"
        # add a directory separator if it's not at the end of string
        if self.root_subdir[-1] != "/" :
            self.root_subdir += "/"
            
        self.run_type                    = args.run_type if args.run_type is not None else "beam"
        self.plotting_server             = "inttdaq"
        self.home_dir_in_plotting_server = "/home/inttdev/"
        self.root_dir_in_plotting_server = "/1008_home/phnxrc/INTT/" + self.root_dir + self.root_subdir
        self.data_dir_in_plotting_server = self.home_dir_in_plotting_server + "INTT/data/" + self.root_dir + self.root_subdir

        # process flags
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
        
        self.make_symbolic     = args.make_symbolic     if args.make_symbolic     is not None else default
        self.make_plot         = args.make_plot         if args.make_plot         is not None else default
        self.transfer_plot     = args.transfer_plot     if args.transfer_plot     is not None else default
        self.transfer_dir      = args.transfer_dir      if args.transfer_dir      is not None else "./"

        self.auto_update       = args.auto_update       if args.auto_update       is not None else False
        self.update_list       = args.update_list       if args.update_list       is not None else self.auto_update
        
        # misc
        self.plotter           = self.home_dir_in_plotting_server + "macro/FelixQuickViewer_1Felix.C"
        source_dir             = os.path.dirname( __file__ ) + "/"
        self.evt_list          = source_dir + "evt_list.txt"
        self.evt_list_previous = source_dir + "evt_list_previous.txt"

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
        print( "| Making plots?", self.make_plot )
        print( "|    ROOT macro", self.plotter )
        self.PrintLine()
        print( "| Transferring plots?", self.transfer_plot )
        print( "|     Output directory:", self.transfer_dir )
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
            command_decode = "ls -r " + self.GetEventFilePath().replace( "-0000.", "-????." ) + " | xargs -I {} nice bash ./myfirstproject.sh {}"
            #command_decode = "ls " + self.GetEventFilePath().replace( "-0000.", "-????." ) + " | xargs -I {} ls {}"
            whole_command = init_commands + command_to_dir + command_decode
        else:
            command_to_dir = "cd /home/phnxrc/INTT/hachiya/convertInttRaw/test1/ ; "
            command_decode = "ls -r " + self.GetEventFilePath().replace( "-0000.", "-????." ) + " | xargs -I {} nice root -l -q -b 'runConvertInttData.C(\\\"{}\\\")'"
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
        command = "scp " + self.plotting_server + ":" + self.data_dir_in_plotting_server + "*" + self.run + "*.pdf " + self.transfer_dir
        print( command )

        if self.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
            proc.wait()
            print( "All plots were transfered to", self.transfer_dir )

    def MakeEvtList( self, ignored_runs=[] ):
        command = "ssh intt2 " + "ls -1t " + os.path.dirname( self.GetEventFilePath() ) + " | head -n 400"
        print( command )

        if self.is_dry_run is False :
            proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
            proc.wait( timeout=30 )
            outputs = proc.communicate()[0].decode().split( '\n' )

            all_runs = []
            for output in outputs[0:-2] :
                pos_left = len( self.run_type ) + 7
                pos_right = pos_left + 8
                #print( type(pos_left), pos_left, type(pos_right), pos_right, type(output), output )

                # There may be files with a name different from the assumed format
                # ( {self.run_type}_intt?-????????-????.evt, e.g. beam_intt1-00001234-0056.evt)
                # For the moment, I exclude files with shorter name than the assumed format not to affect in reading file.
                run = output[ pos_left:pos_right ]
                #print( "MakeEvtList:", ignored_runs, run, run in ignored_runs )
                if run != '' :
                    if run not in ignored_runs : 
                        all_runs.append( run )
                # end of for output in outputs[0:-2]
            # end of if self.is_dry_run is False
                
            # keep only unique elements
            runs = sorted( list(set(all_runs)) )
            #print( runs )
            
            with open( self.evt_list, 'w' ) as file :
                for run in runs :
                    file.write( run+"\n" )

    def GetNewRuns( self ) :
        runs = []
        with open( self.evt_list ) as file :
            for line in file :
                runs.append( line.split()[0] )

        print( "Runs in the current list:", runs )
        runs_previous = []
        with open( self.evt_list_previous ) as file :
            for line in file :
                runs_previous.append( line.split()[0] )
        print( "Runs in the previous list:", runs_previous )

        runs_processed = []
        for run in runs :
            if run not in runs_previous :
                runs_processed.append( run )

        print( "Automatic update, Runs to be processed:", runs_processed )
        return runs_processed

    def AutoUpdate( self ) :
        print( "AutoUpdate!" )
        
        if self.update_list is True :
            print( "Update list", "(dry run)" if self.is_dry_run else "!" )
            
            # Change the current list (generated before) to a previous list
            if os.path.exists( self.evt_list ) : 
                command = "mv " + self.evt_list + " " + self.evt_list_previous
                print( "Rename the current list, which was generated before, to a previous list." )
                print( command )
                if self.is_dry_run is False:
                    proc = subprocess.Popen( command, shell=True )
                    proc.wait()

            else:
                print( self.evt_list, "is not found. The previous list is not updated." )
                    
            # Generate new list
            print( "Let\'s make a current run list." )
            self.MakeEvtList()

            # if --only is used (with --update-list is assumed), here is the end of processes
            if self.process_only is True :
                self.GetNewRuns()[0:-1] # skip the latest run since it may be running now
                return None
            # end of if self.update_list is True 

        print( "Let\'s find new runs!" )
        runs_processed = self.GetNewRuns()[0:-1] # skip the latest run since it may be running now

        # Update the run list without the last run in the new run list otherwise it will be in the previous run list
        # (the previous run list means a list of runs already processed)
        self.MakeEvtList( [ self.GetNewRuns()[-1] ] )

        self.auto_update = False
        self.update_list = False
        # loop over all runs to be processed
        counter = 0

        print( "Loop over new runs", runs_processed )
        for run in runs_processed :
            self.run = run # change the run to be processed
            print( "Auto update, Processing Run", self.run )
            self.Do()
            # end of for run in runs_processed

        print( "Processed runs:", runs_processed )
            
    def Do( self ) :
        if self.IsOtherProcessRunning() is True:
            print( "Other process is running. Nothing done.", "(dry run)" if self.is_dry_run else "" )
            return None
        else:
            self.Print()
            
        print( "Do process!", "(dry run)" if self.is_dry_run else "" )
        if self.auto_update is True or self.update_list is True:
            self.AutoUpdate()
            return None
        
        if self.decode is True :
            if self.decode_hit_wise is True:
                self.Decode()
                print( "Hit-wise TTree made!", "(dry run)" if self.is_dry_run else "" )
            if self.decode_event_wise is True:
                self.Decode( is_event_wise=True )
                print( "Event-wise TTree made!", "(dry run)" if self.is_dry_run else "" )

        if self.make_symbolic is True : 
            self.MakeSymbolic()
            print( "Symbolics made!", "(dry run)" if self.is_dry_run else "" )

        if self.make_plot is True : 
            self.MakePlot()
            print( "Plots made!", "(dry run)" if self.is_dry_run else "" )

        if self.transfer_plot is True :
            self.TransferPlot()
            print( "Plots transferred!", "(dry run)" if self.is_dry_run else "" )


from argparse import (OPTIONAL, SUPPRESS, ZERO_OR_MORE,
                      ArgumentDefaultsHelpFormatter, ArgumentParser,
                      RawDescriptionHelpFormatter, RawTextHelpFormatter)


class MyHelpFormatter(
    RawTextHelpFormatter, RawDescriptionHelpFormatter, ArgumentDefaultsHelpFormatter
):
    def _format_action(self, action: argparse.Action) -> str:
        return super()._format_action(action) + "\n"

    def _get_help_string(self, action):
        help = action.help
        if action.required:
            help += " (required)"

        if "%(default)" not in action.help:
            if action.default is not SUPPRESS:
                defaulting_nargs = [OPTIONAL, ZERO_OR_MORE]
                if action.option_strings or action.nargs in defaulting_nargs:
                    if action.default is not None and not action.const:
                        help += " (default: %(default)s)"
        return help
    
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

    parser = argparse.ArgumentParser( description=description,
                                      epilog=epilog,
                                      formatter_class=MyHelpFormatter
                                     )

    parser.add_argument( "run", type=int,
                         help="**Mandatory** The run number to be processed. It doesn't need to be 8 digit." )

    parser.add_argument( "--only",
                         action=argparse.BooleanOptionalAction,
                         help="Only specified steps are done if specified. Otherwise (default), all steps are done unless \"no\" options are specified." )

    # process flags
    parser.add_argument( "--decode",
                         action=argparse.BooleanOptionalAction,
                         help="Decoding to make a hit-wise and an event-wise TTree is done if specified (default)." )

    parser.add_argument( "--decode-hit-wise",
                         action=argparse.BooleanOptionalAction,
                         help="Decoding to make a hit-wise Tree is done if specified (default)." )

    parser.add_argument( "--decode-event-wise",
                         action=argparse.BooleanOptionalAction,
                         help="Decoding to make a event-wise Tree is done if specified (default)." )

    parser.add_argument( "--make-symbolic",
                         action=argparse.BooleanOptionalAction,
                         help="Making symbolic link in inttdaq to the ROOT files in the sPHENIX common disc is done if specified (default). " )
    
    parser.add_argument( "--make-plot",
                         action=argparse.BooleanOptionalAction,
                         help="Making plots of ADC and channel dists in intt is done if specified (default)." )

    parser.add_argument( "--transfer-plot",
                         action=argparse.BooleanOptionalAction,
                         help="Transferring the plots to your local env is done if specified ((default). Use --transfer-dir to select a directory to contain the plots." )

    parser.add_argument( "--transfer-dir", type=str,
                         help="A directory to contain the plots can be specified (default: .)." )

    parser.add_argument( "--auto-update",
                         action=argparse.BooleanOptionalAction,
                         help="Automatic updating process (decode, making plots, transferring plots) for the new runs appear in /bbox/commissioning/INTT/{run-type} is done if specified. {run-type} is given by --run-type. Default: off" )

    parser.add_argument( "--update-list",
                         action=argparse.BooleanOptionalAction,
                         help="The current run list (evt_list.txt) and the previous run list (run_list_previous.txt) are updated if specified (default). Turning off is mainly for debugging." )

    # arguments for detailed control, almost absolete
    parser.add_argument( "--run-type", type=str,
                         help="A type of run, which determins the directory in the buffer box, i.e. /bbox/commissioning/INTT/{here}. A name of event files also depends on the run type.  You can give: beam/calib/junk/calibration. \"beam\" is default." )
    
    parser.add_argument( "--root-dir", type=str,
                         help="A name of directory that contains ROOT files. \"commissioning\" is default. You may use commissioning_6_6, 6_4, 6_2, 5_30, or 5_23 for old data. You don't need to change it normally." )
    
    parser.add_argument( "--root-subdir", type=str,
                         help="A name of sub-directory that contains ROOT files. \"hit_files\" is default. You don't need to change it normally." )
        
    parser.add_argument( "--dry-run",
                         action=argparse.BooleanOptionalAction,
                         help="A flag for testing the processes. It's better to use it before launching processes." )

    args = parser.parse_args()
    #print( args )
    process = Process( args )
    process.Do()

    time.sleep( 1 )
    print( "All processes were ended" )
