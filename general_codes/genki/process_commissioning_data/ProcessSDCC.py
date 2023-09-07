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
import Printer

class ProcessSDCC() :
    def __init__( self, info ) :
        self.info = info

        # (supporsed to be constant) variables for the buffer box
        self.bbox = "bbox3"
        self.bbox_transfer_dir = "/home/sphnxpro/mlp_transfer"
        self.bbox_log_dir = "/logdisk/sphnxpro/condor_logs"
        self.bbox_transfer_script = "./submit_file_farm.sh"
        self.bbox_check_script = "/home/phnxrc/INTT/genki/check_transfer.sh"
        
        # (supporsed to be constant) variables for SDCC
        self.sdcc_ssh = "sphnx_nukazuka"
        self.sdcc_bbox_dir = pathlib.Path( "/sphenix/lustre01/sphnxpro/commissioning/INTT" )
        self.sdcc_work_dir = pathlib.Path( "/sphenix/tg/tg01/commissioning/INTT/" )
        self.sdcc_job_dir = self.sdcc_work_dir / pathlib.Path( "commissioning_production/condorjobsintt" )
        self.sdcc_file_list = pathlib.Path( self.info.config_dir ) / pathlib.Path( "infiles.list" )
        self.sdcc_process_script = pathlib.Path( "submitjobs.sh" )

        # something else
        self.sending_trial = 0 
        self.sending_trial_max = 5
        
    def Print( self ) :
        print( "ProcessSDCC" , flush=True )

    def SendSDCC( self ) :
        """!
        @brief Sending evt files from the buffer box to SDCC.
        @This is done by sphnxpro@bbox0. The introcution for transferring data is in https://sphenix-intra.sdcc.bnl.gov/WWW/run/2023/filetransfer.html
        """

        # check the buffer box only if specified
        if self.CheckSendEnd() is True:
                return None

        if self.sending_trial > self.sending_trial_max :
            print( "- #trial of sending files from 1008 to SDCC is", \
                   self.sending_trial, \
                   ". It's too much. Let's give up.", \
                   flush=True, file=sys.stderr )
            
            return True
            
        print( "Sending evt files from the buffer box to SDCC. Run", self.info.run , flush=True )

        command = "ssh " + self.bbox + " \"" \
            "cd " + self.bbox_transfer_dir + " ; " \
            "ls " + self.info.GetEventFilePath().replace ( "inttX", "intt?" ).replace( "-0000.", "-????." ) + \
            " | xargs -I {} bash " + self.bbox_transfer_script + " {}\""

        print( "Command:", command , flush=True )
        if self.info.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
        # I don't wait till the end of the process

        return False

    def CheckSendEnd( self ) :
        if self.info.check_bbox is False :
            return None
        
        print( "Checking whether sending data to SDCC has been finished or not" , flush=True )
        command = "ssh " + self.bbox + " \"" \
            "ls " + self.info.GetEventFilePath().replace ( "inttX", "intt?" ).replace( "-0000.", "-????." ) + \
            "\""

        print( "- Get list of files:", command , flush=True )
        proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
        proc.wait()
        files = proc.communicate()[0].decode().split( '\n' )

        processes = []
        # loop over all files and check whether the transfer was successfully done or not
        for afile in files[0:-1] : 
            command = "ssh " + self.bbox + " \"" + \
                self.bbox_check_script + " " + afile + \
                "\""

            print( "- Check", afile, "by the command:", command , flush=True )
            if self.info.is_dry_run is False : 
                proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
                processes.append( proc )
        
        # wait for the end of all processes
        if self.info.is_dry_run is False : 
            for proc in processes :
                proc.wait()

        # Let's see the results
        print( " - All checking processes were finished. Let's see the results" , flush=True )
        all_done = True
        if self.info.is_dry_run is False : 
            for proc in processes :
                afile = proc.args.split()[-1].replace( "\"", "" ) # the last element is the file path
                all_done = all_done and ( proc.returncode ==  0 ) # True && (file0 OK?) && ... && (fileN OK?)
                print( " -", afile, "return code:", proc.returncode, "--->", all_done , flush=True )

            print( " - Have all files been sent?\t", Printer.Printer.ColorBoolean(all_done) , flush=True )

        return all_done
        
    def ProcessSDCC( self ) :
        """!
        @brief Execute some processes at SDCC
        @details The processes are defined by Milan's application. This function just submits jobs.
        """
        print( " - Processes at SDCC.", flush=True ) # Let's wait for 120s." , flush=True )

        # If the number of sending data to SDCC is more than threashold, give it up
        if self.sending_trial > self.sending_trial_max :
            print( " - Sending files to SDCC was repeated for", self.sending_trial, "."\
                   "Process run", self.info.run, "by hand.", \
                   file=sys.stderr, flush=True )

            if self.info.auto_update is True :
                print( " - It's auto-update mode. Reset the #trial counter from", self.sending_trial, "to 0.", flush=True )
                self.sending_trial = 0
                
            return None

        # Check whether evt files are available in SDCC or not
        is_ended = self.CheckSendEnd()
        if is_ended is False and self.info.is_dry_run is False : 
            print( " - Some files have not been sent. No process is done." , flush=True )
            print( " - Send the files." , flush=True )
            self.sending_trial += 1
            result = self.SendSDCC()

            # If evt files have not been sent to SDCC, repeat it
            if result is False:
                print( " - Wait for 200s." , flush=True )
                time.sleep( 200 )

                print( " - OK, let\'s try again.\n" , flush=True )
                self.ProcessSDCC()

            return None

        # Make the list of files to be processed from the real paths
        directory = self.sdcc_bbox_dir / self.info.run_type
        file_expression = self.info.GetEventFilePath().replace ( "inttX", "intt?" ).replace( "-0000.", "-????." ).split( "/")[-1]

        # It's something like
        # ssh rcas_nukazuka "ls /sphenix/lustre01/sphnxpro/commissioning/INTT/cosmics/cosmics_intt?-00025193-????.evt"
        command = "ssh " + self.sdcc_ssh + " \"" + \
            "ls " + str(directory) + "/" + file_expression + "\""

        print( command, flush=True ) # example: ssh rcas_nukazuka "ls /sphenix/lustre01/sphnxpro/commissioning/INTT/junk/intt_intt?-00025661-????.evt, 
        proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
        proc.wait()
        outputs = proc.communicate()[0].decode().split( '\n' )

        # Write the list of files to a text file
        print( " - Files to be processed:" , flush=True )
        # change the name of the file list from infiles.list to infiles_${run}.list
        # not to confuse runs among batch jobs
        #self.sdcc_file_list = pathlib.Path( str(self.sdcc_file_list).split( '.' )[0] + "_" + str(self.info.run) + ".list" )
        if self.info.is_dry_run is False : 
            with open( str(self.sdcc_file_list), 'w' ) as file :
                for output in outputs[0:-1] :
                    print( "   -", output , flush=True )
                    file.write( output + "\n" )

        # Send the file list with scp. The command looks like:
        # scp infiles.list rcas_nukazuka:/sphenix/tg/tg01/commissioning/INTT/work/genki/commissioning_production/condorjobsintt/infiles.list
        command = "scp " + str(self.sdcc_file_list) + " " + \
            self.sdcc_ssh + ":" + str(self.sdcc_job_dir / self.sdcc_file_list.name )
        print( " - Send the file list", self.sdcc_file_list, "to SDCC by the command:" , flush=True )
        print( command , flush=True )

        if self.info.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
            proc.wait()

        # Execute Milan's system to process the files in the list
        command = "ssh " + self.sdcc_ssh + " \"" + \
            "cd " + str(self.sdcc_job_dir) + "; " + \
            "bash " + str(self.sdcc_process_script) + " " + str(self.sdcc_file_list.name) + "\""

        print( " - Submit jobs to process the files with Milan\'s system." , flush=True )
        print( command , flush=True )
        if self.info.is_dry_run is False :
            proc = subprocess.Popen( command, shell=True )
            proc.wait()

    def Do( self ) :
        if self.info.IsOtherProcessRunning() is True:
            print( "Other process is running. Nothing done.", "(dry run)" if self.info.is_dry_run else "" , flush=True )

            if self.info.is_force_run is True :
                print( "** But the force flag is True. This program runs anyway. **" , flush=True )
            else:
                return None

        #self.info.Print()
        #time.sleep( 5 ) 
            
        print( "Do process!", "(dry run)" if self.info.is_dry_run else "", flush=True )
        if self.info.auto_update is True or self.info.update_list is True:
            self.AutoUpdate()
            return None

        if self.info.check_bbox is True :
            is_ended = self.CheckSendEnd()
            done_or_not = Printer.Printer.ColorIt( "been done.", 'Green') if is_ended else Printer.Printer.ColorIt( "not been done.", 'Red')
            print( " - Sending", self.info.run, "has", done_or_not , flush=True )

        # Sending evt files to SDCC
        if self.info.send_SDCC is True :
            self.proc_sdcc.SendSDCC()

        # Processes at SDCC
        if self.info.process_SDCC is True :
            self.proc_sdcc.ProcessSDCC()
        

if __name__ == "__main__" :
    print( "ProcessSDCC main function." , flush=True )
