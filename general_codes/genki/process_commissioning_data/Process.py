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

import Information
import ProcessSDCC
import Printer

class Process() :
    def __init__( self, args ) :
        #############################################################################
        # Processes for flags determined by command-line arguments                  #
        #############################################################################
        self.info = Information.Information( args )
        self.proc_sdcc = ProcessSDCC.ProcessSDCC( self.info )
        self.processes = []
    
    def SendCommand( self, intt_num, command ) :
        server="intt" + str( intt_num )
        ssh_command = "ssh " + server + " \"" + command.replace( "inttX" , "intt"+str(intt_num) ) + "\""
        if self.info.is_dry_run is True : 
            print( "SendCommand:", ssh_command , flush=True )
        else:
            proc = subprocess.Popen( ssh_command, shell=True )
            self.processes.append( proc )

    def SendCommandToAll( self, command ) :
        for num in range(0, 8 ) :
            if self.info.is_dry_run is True : 
                print( "SendCommandToAll:", command , flush=True )

            #if num ==1:
            #    print( "intt", num, "is skipped" , flush=True )
            #    continue
            
            self.SendCommand( num, command )

    def Decode( self, is_event_wise=False ) :
        init_commands = "source /etc/profile ; source .bashrc ; source .bash_profile ; "
        # cd ${conversion_to_hit_base_dir}; ls ${data_dir}${data_name1}?${data_name2}????${raw_suffix}  | xargs -I {} -P 8 bash ./myfirstproject.sh {} "

        if is_event_wise is False :
            command_to_dir = "cd /home/phnxrc/INTT/jbertaux ; "
            #command_decode = "ls -r " + self.info.GetEventFilePath_Commissioning().replace( "-0000.", "-????." ) + " | xargs -I {} nice bash ./myfirstproject.sh {}"
            command_decode = "ls -r " + self.info.GetEventFilePath() + " | xargs -I {} bash ./myfirstproject.sh {}"

            whole_command = init_commands + command_to_dir + command_decode
        else:
            command_to_dir = "cd /home/phnxrc/INTT/hachiya/convertInttRaw/test1/ ; "
            #command_decode = "ls -r " + self.info.GetEventFilePath_Commissioning().replace( "-0000.", "-????." ) + " | xargs -I {} nice root -l -q -b 'runConvertInttData.C(\\\"{}\\\")'"
            command_decode = "ls -r " + self.info.GetEventFilePath() + " | xargs -I {} root -l -q -b 'runConvertInttData.C(\\\"{}\\\")'"
            
            whole_command = init_commands + command_to_dir + command_decode
            
        if self.info.is_dry_run is True : 
            print( "Decode:", whole_command , flush=True )

        self.SendCommandToAll( whole_command )
        for proc in self.processes :
            #print( "Waiting for decoding" , flush=True )
            proc.wait()

        print( "All decoding ended" , flush=True )

    def MakeSymbolic( self ) :
        command = "ssh " + self.info.plotting_server + " \""
        command += "cd " + self.info.data_dir_in_plotting_server + " ; "
        command += "ls " + self.info.root_dir_in_plotting_server + "/*.root | xargs -I {} ln -s {} 2>/dev/null"
        command += "\""
        print( command , flush=True )

        if self.info.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
            proc.wait()
        
    def MakePlot( self ) :
        command = "ssh " + self.info.plotting_server + " \""
        command += "cd " + self.info.data_dir_in_plotting_server + " ; "
        command += "ls *" + self.info.run + "*.root | grep -v event_base | xargs -I {} -P 8  root -q -l -b \'" + self.info.plotter + "(\\\"{}\\\", \\\".pdf\\\" )\'"
        command += "\""
        print( command , flush=True )

        if self.info.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
            proc.wait()
            print( "All plots were made." , flush=True )

    def MakePlotInServer( self ) :
        init_commands = "source /etc/profile ; source .bashrc ; source .bash_profile ; "
        # cd ${conversion_to_hit_base_dir}; ls ${data_dir}${data_name1}?${data_name2}????${raw_suffix}  | xargs -I {} -P 8 bash ./myfirstproject.sh {} "

        command_to_dir = "cd " + self.info.sphenix_intt_home + "/macro; "
        command_root = "ls -r " + self.info.GetRootFilePath(is_event_wise=False).replace( "-0000.", "-????." ) + " | grep -v event_base | xargs -I {} -P 8  root -q -l -b \'" + self.info.plotter_in_server + "(\\\"{}\\\", \\\".pdf\\\" )\'"

        whole_command = init_commands + command_to_dir + command_root
            
        if self.info.is_dry_run is True : 
            print( "Making plots in each DAQ server:", whole_command , flush=True )
            #return None

        self.SendCommandToAll( whole_command )
        for proc in self.processes :
            ##print( "Waiting for decoding" , flush=True )
            proc.wait()

        print( "All plots were made." , flush=True )

    def TransferPlot( self ) :
        command = "scp " + self.info.plotting_server + ":" + self.info.data_dir_in_plotting_server + "*" + self.info.run + "*.pdf " + self.info.transfer_dir
        print( command , flush=True )

        if self.info.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
            proc.wait()
            print( "All plots were transfered to", self.info.transfer_dir , flush=True )
        
    def MakeEvtList( self, ignored_runs=[] ):
        print( "MakeEvtList" , flush=True )
        print( "Ignored runs:", ignored_runs , flush=True )
        #command = "ssh intt2 " + "ls -1t " + os.path.dirname( self.info.GetEventFilePath_Commissioning() ) + " | head -n 400"
        #command = "ssh intt2 " + "ls -1t " + os.path.dirname( str(self.info.GetEventFilePath()) ) + " | head -n 400"
        command = "ssh intt2 \"find  " + str( os.path.dirname( self.info.GetEventFilePath() ) ) + " -type f | xargs ls -t1 \" | head -n 400 2>/dev/null"
        print( command , flush=True )

        if self.info.is_dry_run is False :
            proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
            proc.wait( timeout=60 )
            outputs = proc.communicate()[0].decode().split( '\n' )

            all_runs = []
            for output in outputs[0:-2] :
                #pos_left = len( self.info.run_type ) + 7
                #pos_right = pos_left + 8

                pos_right = len( output ) - ( 3 + 1 + 4 + 1 ) # lest pos - ( "evt" - "." - "0000" )
                pos_left = pos_right - 8 # pos_right - "00000000"
                
                #print( type(pos_left), pos_left, type(pos_right), pos_right, type(output), output , flush=True )

                # There may be files with a name different from the assumed format
                # ( {self.info.run_type}_intt?-????????-????.evt, e.g. beam_intt1-00001234-0056.evt)
                # For the moment, I exclude files with shorter name than the assumed format not to affect in reading file.
                run = output[ pos_left:pos_right ]

                #print( "debugging:", run ), flush=True )
                #print( "MakeEvtList:", ignored_runs, run, run in ignored_runs , flush=True )
                if run != '' :
                    if run not in ignored_runs : 
                        all_runs.append( run )
                # end of for output in outputs[0:-2]
            # end of if self.info.is_dry_run is False
            
            # keep only unique elements
            #runs = sorted( list(set(all_runs)) )
            runs = sorted( list(set(all_runs)) )

            with open( self.info.evt_list, 'w' ) as file :
                for run in runs :
                    file.write( run+"\n" )

    def GetNewRuns( self ) :
        runs = []
        with open( self.info.evt_list ) as file :
            for line in file :
                runs.append( line.split()[0] )

        print( "Runs in the current list:", runs , flush=True )
        runs_previous = []
        with open( self.info.evt_list_previous ) as file :
            for line in file :
                runs_previous.append( line.split()[0] )
        print( "Runs in the previous list:", runs_previous , flush=True )
        if len( runs_previous ) == 0 :
            print( "No runs found in the previous list. Something wrong!!!" , flush=True )
            print( "No runs to be processed\n" , flush=True )
            return []
        
        runs_processed = []
        for run in runs :
            if run not in runs_previous :
                runs_processed.append( run )

        print( "Automatic update, Runs to be processed:", runs_processed , flush=True )
        return runs_processed

    def AutoUpdate( self ) :
        """!
        @brief A function to update (decoding, etc) automatically.
        @details It does
            1. Update evt list (if --update-list is given)
                1. If the evt list is not found (it happen rarely), an empty file is generated.
                2. If the previous evt list is not found (it happen rarely), an empty file is generated.
                3. The current evt list "in the last execution" is renamed as the current list. It means the precious list is deleted.
            2.  
        """
        
        print( "AutoUpdate!" , flush=True )
        ignored_runs = [ '00000001', '00000002', '00000003', '00000012', '00000013',
                         '00000014', '00000015', '00009999', '00099000', '00099001',
                         '00099002', '00099003', '00099004', '00099005', '00099006',
                         '00099019', '00099020', '00099021', '00099022', '00099023',
                         '00099024', '00099025', '03000002', '03000003', '03000005'
                        ]

        # Step 1
        if self.info.update_list is True :
            print( "Update list", "(dry run)" if self.info.is_dry_run else "!" , flush=True )

            # copy the current list as a temp file
            command = "cp " + self.info.evt_list + " " + self.info.evt_list_temp
            print( "Copy the current list to a temp file as a backup." , flush=True )
            print( command, "\n" , flush=True )
            if self.info.is_dry_run is False:
                proc = subprocess.Popen( command, shell=True )
                proc.wait()
            
            # Change the current list (generated before) to a previous list
            if os.path.exists( self.info.evt_list ) : 
                command = "mv " + self.info.evt_list + " " + self.info.evt_list_previous
                print( "Rename the current list, which was generated before, to a previous list." , flush=True )
                print( command, "\n" , flush=True )
                if self.info.is_dry_run is False:
                    proc = subprocess.Popen( command, shell=True )
                    proc.wait()

            else:
                print( self.info.evt_list, "is not found. The previous list is not updated." , flush=True )

                # Step 1.1 Make an evt file if it's not found
                touch_file = pathlib.Path( self.info.evt_list )
                touch_file.touch()
                print( self.info.evt_list, "was generated. It's an empty file." , flush=True )

            # Step 1.2 Make a previous evt file if it's not found
            if pathlib.Path( self.info.evt_list_previous ).exists() is False :
                print( self.info.evt_list_previous, "is not found." , flush=True )
                pathlib.Path( self.info.evt_list_previous ).touch()
                print( self.info.evt_list_previous, "was generated. It's an empty file." , flush=True )
                
            # Generate new list
            print( "Let\'s make a current run list." , flush=True )
            self.MakeEvtList( ignored_runs=ignored_runs )

            # if --only is used (with --update-list is assumed), here is the end of processes
            if self.info.process_only is True and self.info.update_list is True and self.info.auto_update is False : 
                self.GetNewRuns()[0:-1] # skip the latest run since it may be running now
                return None
            # end of if self.info.update_list is True 

        print( "Let\'s find new runs!" , flush=True )
        self.info.runs_processed = self.GetNewRuns()[0:-1] # skip the latest run since it may be running now
        #self.info_runs_processed = runs_processed # give the list of runs to be processed.
        
        if self.info.runs_processed == [] :
            print( "No runs processed" , flush=True )
            # Get the current list back from the temp file
            command = "cp " + self.info.evt_list_temp + " " + self.info.evt_list
            print( "  --> Restore the status. Copy the temp file as a previous list." , flush=True )
            print( command, "\n" , flush=True )
            if self.info.is_dry_run is False:
                proc = subprocess.Popen( command, shell=True )
                proc.wait()
            return None
        
        # Update the run list without the last run in the new run list otherwise it will be in the previous run list
        # (the previous run list means a list of runs already processed)
        ignored_runs.append( self.GetNewRuns()[-1] ) 
        self.MakeEvtList( ignored_runs=ignored_runs )

        self.info.auto_update = False
        self.info.update_list = False
        # loop over all runs to be processed
        counter = 0
        print( "End of autoupdate function" )
        #print( runs_processed )
        print( self.info.runs_processed )
        self.DoIteratively()

    def DoIteratively( self ) :        
        print( "Loop over new runs", self.info.runs_processed , flush=True )
        for run in self.info.runs_processed :
            self.info.run = run # change the run to be processed
            print( "Processing Run", self.info.run , flush=True )
            self.Do()
            # end of for run in runs_processed

        print( "Processed runs:", self.info.runs_processed , flush=True )
            
    def Do( self ) :
        if self.info.is_force_run is False :
            if self.info.IsOtherProcessRunning() is True:
                print( "** But the force flag is True. This program runs anyway. **" , flush=True )
            else:
                print( "Other process is running. Nothing done.", "(dry run)" if self.info.is_dry_run else "" , flush=True )
                return None

        #self.info.Print()
        #time.sleep( 5 ) 
            
        print( "Do process!", "(dry run)" if self.info.is_dry_run else "" , flush=True )
        if self.info.auto_update is True or self.info.update_list is True:
            #self.info.Print()
            self.AutoUpdate()
            return None

        # it's historical reason... if self.info.run is a list, execute DoIteratively (it's always a list now...)
        #if type( self.info.runs ) is list :
        if self.info.run == -1 :
            print( self.info.runs, self.info.runs_processed )
            self.info.runs_processed = self.info.runs
            self.info.run = 0
            print( self.info.run, self.info.runs_processed )
            self.DoIteratively()
            return None
        
        if self.info.check_bbox is True :
            is_ended = self.proc_sdcc.CheckSendEnd()
            done_or_not = Printer.Printer.ColorIt( "been done.", color='Green') if is_ended else Printer.Printer.ColorIt( "not been done.", color='Red')
            print( " - Sending run", self.info.run, "has", done_or_not , flush=True, file=sys.stderr )

        # Sending evt files to SDCC
        if self.info.send_SDCC is True :
            self.proc_sdcc.SendSDCC()
        
        if self.info.decode is True :
            if self.info.decode_hit_wise is True:
                self.Decode()
                print( "Hit-wise TTree made!", "(dry run)" if self.info.is_dry_run else "" , flush=True )
            if self.info.decode_event_wise is True:
                self.Decode( is_event_wise=True )
                print( "Event-wise TTree made!", "(dry run)" if self.info.is_dry_run else "" , flush=True )

        if self.info.make_symbolic is True : 
            self.MakeSymbolic()
            print( "Symbolics made!", "(dry run)" if self.info.is_dry_run else "" , flush=True )

        if self.info.make_plot is True : 
            #self.MakePlot()
            self.MakePlotInServer()
            print( "Plots made!", "(dry run)" if self.info.is_dry_run else "" , flush=True )

        if self.info.transfer_plot is True :
            self.TransferPlot()
            print( "Plots transferred!", "(dry run)" if self.info.is_dry_run else "" , flush=True )

        # copy ladder map files from opc0 to SDCC
        if self.info.send_map_SDCC is True :
            self.proc_sdcc.SendMapSDCC()

        # Processes at SDCC
        if self.info.process_SDCC is True :
            self.proc_sdcc.ProcessSDCC( condor=self.info.condor , quick=self.info.quick )

        if self.info.dst_any is True :
            self.proc_sdcc.DstProcess( condor=self.info.condor , quick=self.info.quick )

        if self.info.end_process_SDCC is True :
            self.proc_sdcc.EndProcessSDCC()
