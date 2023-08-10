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

class ProcessSDCC() :
    def __init__( self, info ) :
        print( "class ProcessSDCC" )
        self.info = info

        # (supporsed to be constant) variables for the buffer box
        self.bbox = "bbox3"
        self.bbox_transfer_dir = "/home/sphnxpro/mlp_transfer"
        self.bbox_transfer_script = "./submit_file_farm.sh"
        self.bbox_check_script = "/home/phnxrc/INTT/genki/check_transfer.sh"
        
        # (supporsed to be constant) variables for SDCC
        self.sdcc_ssh = "rcas_nukazuka"
        self.sdcc_bbox_dir = pathlib.Path( "/sphenix/lustre01/sphnxpro/commissioning/INTT" )
        self.sdcc_work_dir = pathlib.Path( "/sphenix/tg/tg01/commissioning/INTT/work" )
        self.sdcc_job_dir = self.sdcc_work_dir / pathlib.Path( "genki/commissioning_production/condorjobsintt" )
        self.sdcc_file_list = pathlib.Path( "infiles.list" )
        self.sdcc_process_script = pathlib.Path( "submitjobs.sh" )
        
        
    def Print( self ) :
        print( "ProcessSDCC" )

    def SendSDCC( self ) :
        """!
        @brief Sending evt files from the buffer box to SDCC.
        @This is done by sphnxpro@bbox0. The introcution for transferring data is in https://sphenix-intra.sdcc.bnl.gov/WWW/run/2023/filetransfer.html
        """
        print( "Sending evt files from the buffer box to SDCC. Run", self.info.run )

        command = "ssh " + self.bbox + " \"" \
            "cd " + self.bbox_transfer_dir + " ; " \
            "ls " + self.info.GetEventFilePath().replace ( "inttX", "intt?" ).replace( "-0000.", "-????." ) + \
            " | xargs -I {} bash " + self.bbox_transfer_script + " {}\""

        print( "Command:", command )
        if self.info.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
        # I don't wait till the end of the process

    def CheckSendEnd( self ) :
        print( "Checking whether sending data to SDCC has been finished or not" )
        command = "ssh " + self.bbox + " \"" \
            "ls " + self.info.GetEventFilePath().replace ( "inttX", "intt?" ).replace( "-0000.", "-????." ) + \
            "\""

        print( "- Get list of files:", command )
        proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
        proc.wait()
        files = proc.communicate()[0].decode().split( '\n' )

        processes = []
        # loop over all files and check whether the transfer was successfully done or not
        for afile in files[0:-1] : 
            command = "ssh " + self.bbox + " \"" + \
                self.bbox_check_script + " " + afile + \
                "\""

            print( "- Check", afile, "by the command:", command )
            proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
            processes.append( proc )
            #proc.wait()
            #print( "-"*50, "\n", proc.communicate()[0].decode().split( '\n' ), "\n\n" )
            #result = int(proc.returncode) # 0 means exited properly

            #break

        
            #" | xargs -I {} bash " + self.bbox_transfer_script + " {}\""

        # wait for the end of all processes
        for proc in processes :
            proc.wait()

        # Let's see the results
        all_done = True
        for proc in processes :
            afile = proc.args.split()[-1].replace( "\"", "" ) # the last element is the file path
            print( afile, proc.returncode )
            all_done = all_done and ( proc.returncode ==  "0" ) # True && (file0 OK?) && ... && (fileN OK?)

        return all_done
        
    def ProcessSDCC( self ) :
        """!
        @brief Execute some processes at SDCC
        @details The processes are defined by Milan's application. This function just submits jobs.
        """
        print( " - Processes at SDCC." )
        self.CheckSendEnd()

        # Make the list of files to be processed from the real paths
        directory = self.sdcc_bbox_dir / self.info.run_type
        file_expression = self.info.GetEventFilePath().replace ( "inttX", "intt?" ).replace( "-0000.", "-????." ).split( "/")[-1]

        # It's something like
        # ssh rcas_nukazuka "ls /sphenix/lustre01/sphnxpro/commissioning/INTT/cosmics/cosmics_intt?-00025193-????.evt"
        command = "ssh " + self.sdcc_ssh + " \"" + \
            "ls " + str(directory) + "/" + file_expression + "\""

        print( command )
        proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
        proc.wait()
        outputs = proc.communicate()[0].decode().split( '\n' )

        # Write the list of files to a text file
        print( " - Files to be processed:" )
        with open( str(self.sdcc_file_list), 'w' ) as file :
            for output in outputs[0:-1] :
                print( "   -", output )
                file.write( output + "\n" )

        # Send the file list with scp. The command looks like:
        # scp infiles.list rcas_nukazuka:/sphenix/tg/tg01/commissioning/INTT/work/genki/commissioning_production/condorjobsintt/infiles.list
        command = "scp " + str(self.sdcc_file_list) + " " + \
            self.sdcc_ssh + ":" + str(self.sdcc_job_dir / self.sdcc_file_list )
        print( " - Send the file list", self.sdcc_file_list, "to SDCC by the command:" )
        print( command )
        proc = subprocess.Popen( command, shell=True )
        proc.wait()

        # Execute Milan's system to process the files in the list
        command = "ssh " + self.sdcc_ssh + " \"" + \
            "cd " + str(self.sdcc_job_dir) + "; " + \
            "bash " + str(self.sdcc_process_script) + "\""

        print( " - Submit jobs to process the files with Milan\'s system." )
        print( command )
        proc = subprocess.Popen( command, shell=True )
        proc.wait()
        

if __name__ == "__main__" :
    print( "ProcessSDCC main function." )
