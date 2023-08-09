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
        self.processes = []

        # (supporsed to be constant) variables for the buffer box
        self.bbox = "bbox3"
        self.bbox_transfer_dir = "/home/sphnxpro/mlp_transfer"
        self.bbox_transfer_macro = "./submit_file_farm.sh"

        # (supporsed to be constant) variables for SDCC
        self.sdcc_ssh = "rcas_nukazuka"
        self.sdcc_bbox_dir = pathlib.Path( "/sphenix/lustre01/sphnxpro/commissioning/INTT" )
        self.sdcc_work_dir = pathlib.Path( "/sphenix/tg/tg01/commissioning/INTT/work/genki/commissioning_production/condorjobsintt" )
        self.sdcc_job_dir = self.sdcc_work_dir / pathlib.Path( "genki/commissioning_production/condorjobsintt" )

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
            " | xargs -I {} bash " + self.bbox_transfer_macro + " {}\""

        print( "Command:", command )
        if self.info.is_dry_run is False : 
            proc = subprocess.Popen( command, shell=True )
        # I don't wait till the end of the process

    def CheckSendEnd( self ) :
        print( "Checking whether sending data to SDCC has been finished or not" )
        
    def ProcessSDCC( self ) :
        """!
        @brief Execute some processes at SDCC
        @details The processes are defined by Milan's application. This function just submits jobs.
        """
        print( "Processes at SDCC" )
        self.CheckSendEnd()
        
        command = "ssh " + self.sdcc_ssh + " \"" + \
            "ls " + str(self.sdcc_job_dir) + \
            "\""

        print( command )
        proc = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE )
        #proc.wait( timeout=60 )
        proc.wait()
        outputs = proc.communicate()[0].decode().split( '\n' )
        print( outputs )

if __name__ == "__main__" :
    print( "ProcessSDCC main function." )
