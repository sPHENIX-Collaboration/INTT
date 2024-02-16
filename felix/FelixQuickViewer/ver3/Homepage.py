import datetime
import logging
import os
import sys
import time
import pprint

import re
import shutil
import pathlib

import Information

"""
@bried
@details
"""
class Homepage() :
    def __init__( self, args ) :
        self.info = args

        self.HOMEPAGE_ROOT_DIR = pathlib.Path( "/sphenix/WWW/subsystem/intt" )
        self.COMMISSIONING_DIR = pathlib.Path( self.HOMEPAGE_ROOT_DIR / "commissioning_plots" )
        self.YEAR_DIR = pathlib.Path( self.COMMISSIONING_DIR / str(self.info.year) )

        if self.info.run is not None:
            self.RUN_DIR = pathlib.Path( self.YEAR_DIR  / self.info.run )
        else:
            self.RUN_DIR = pathlib.Path( self.YEAR_DIR )

        self.run_types = [ "calib", "calibration", "pedestal",
                           "cosmics", "beam", "junk"
                          ]
        self.run_directories = sorted( list(self.YEAR_DIR.glob( '[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]/' ) ) ) # because ChatGPT said so...
        
        self.Process()

    def Process( self ) :

        if self.info.run is not None:
            self.ProcessRunPage()
            
        #self.ProcessYearPage()
        self.ProcessYearPageBetter()

    def ProcessYearPageBetter( self ) : # WIP
        index_template = self.YEAR_DIR / "index_template.html"
        year_page = self.YEAR_DIR / "index.html"
        shutil.copy( index_template, year_page )


        with open( str(year_page), mode='a' ) as file :

            file.write( "<div id=\"col\" style=\"column-count:" + str(len(self.run_types)) + ";\">\n" )
            for run_type in self.run_types :
                print( run_type )
                file.write( "  <h3>" + run_type + "</h3>\n" )
                file.write( "  <p>\n" )
                file.write( "    <ul>\n" )
                counter = 0
                for run_dir in self.run_directories :
                    files = list( run_dir.glob( run_type + '*' ) )
                    #if self.info.year == 2023 or self.info.year == 2024 and (run_type == "pedestal" or run_type == "junk" ) :
                    #if (self.info.year == 2023 and (run_type == "pedestal" or run_type == "junk" ) ) or \
                    #   self.info.year == 2024 and (run_type == "pedestal" or run_type == "junk" ) and False:
                    #    files = list( run_dir.glob( "intt_intt" + '*' ) )
                        
                    if len(files) == 0 :
                        continue

                    counter += 1
                    contents = \
                        "      <li>" + "<a href=\"./" + run_dir.name + "/index.html\">" + run_dir.name + "</a></li>\n"
                    file.write( contents )

                for num in range( 0, self.GetMaxDirNum() - counter ) :
                    file.write( "      <li></li>\n" )
                    
                file.write( "    </ul>\n" )
                file.write( "  </p>\n" )
            file.write( "</div>\n" )
        
    def ProcessRunPage( self ) :
        # Make directory for the run if it's not found
        if self.RUN_DIR.exists() is False : 
            self.RUN_DIR.mkdir()

        # Copy the template of index.html
        index_template = self.COMMISSIONING_DIR / "index_template.html"
        self.index_html = self.RUN_DIR / "index.html"
        shutil.copy( index_template, self.index_html )
        print( self.index_html )

        # Write header
        with open( str(self.index_html), mode='a' ) as file :
            file.write( "<h1>" + "Run " + str( int(self.info.run) ) + "</h1>\n" )

        for felix in range(0, 8 ):
            self.WriteFelixSection( felix )
            
        # Write footer
        with open( str(self.index_html), mode='a' ) as file :
            contents = \
            	"<script src=\"https://cdnjs.cloudflare.com/ajax/libs/lightbox2/2.11.3/js/lightbox-plus-jquery.min.js\"></script>\n" \
		"<script>\n" \
		"  lightbox.option({\n" \
		"    'fadeDuration': 70,\n" \
		"    'imageFadeDuration': 180,\n" \
		"    'resizeDuration': 30,\n" \
		"    'alwaysShowNavOnTouchDevices': true,\n" \
                "    'disableScrolling': true,\n" \
		"    'wrapAround': true\n" \
		"  })\n" \
		"</script>\n" \
                "<body>\n" \
                "</html>\n"
            #file.write( "</body>\n" )
            #file.write( "</html>\n" )
            file.write( contents )

    def WriteFelixSection( self, felix ) :
        with open( str(self.index_html), mode='a' ) as file :
            server = "intt" + str(felix)
            file.write( "<h2>" + server + "</h2>\n" )

            counter = 0
            for plot in self.info.homepage_plots :
                #print( plot, server, server in str(plot) )
                if server in str(plot) :
                    counter += 1
                    image = plot.name
                    
                    run_type = re.split( '[-_.]', image )[0]
                    chunk = re.split( '[-_.]', image )[3]
                    plot_type = re.split( '[-_.]', image) [4]
                    if plot_type == "entryvschan" : 
                        plot_title = "Channel distribution"
                    elif plot_type == "adc" :
                        plot_title = "ADC distribution"
                    elif plot_type == "amplvsadc" : 
                        plot_tile = "ADC vs Ampl"
                    else:
                        plot_title = "Unknown type"

                    caption = run_type + "<br> " + plot_title + "<br> chunk " + chunk

                    shutil.copy( plot, self.RUN_DIR / image )
                    contents = \
                        "    <figure>\n" \
                        "      <a href=\"" + image + "\" data-lightbox=\"images\" data-title=\"" + image + "\">\n" \
                        "        <img " + "src=\"" + image + "\" " + "alt=\"" + image + "\"\>\n" \
                        "      </a>\n" \
                        "      <figcaption>" + caption + "</figcaption>\n" \
                        "    </figure>\n"

                    file.write( contents )

            if counter == 0 :
                file.write( "No data found<br>\n\n" )

    def GetMaxDirNum( self ) :
        """
        @berief
        @details
        """
        number_of_runs = []

        for run_type in self.run_types :
            counter = 0
            for run_dir in self.run_directories :
                files = list( run_dir.glob( run_type + '*' ) )
                #if self.info.year == 2023 or self.info.year == 2024 and (run_type == "pedestal" or run_type == "junk" ) :
                if self.info.year == 2023 and (run_type == "pedestal" or run_type == "junk" ) :
                    files = list( run_dir.glob( "intt_intt" + '*' ) )
                        
                if len(files) == 0 :
                    continue

                counter += 1

            number_of_runs.append( counter )

        return max( number_of_runs )
