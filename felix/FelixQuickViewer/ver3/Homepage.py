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

        self.HOMEPAGE_ADDRESS = "https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/intt/"
        self.HOMEPAGE_ROOT_DIR = pathlib.Path( "/sphenix/WWW/subsystem/intt" )
        self.COMMISSIONING_DIR = pathlib.Path( self.HOMEPAGE_ROOT_DIR / "commissioning_plots" )
        self.YEAR_DIR = pathlib.Path( self.COMMISSIONING_DIR / str(self.info.year) )

        if self.info.run is not None:
            self.RUN_DIR = pathlib.Path( self.YEAR_DIR  / self.info.run )
        else:
            self.RUN_DIR = pathlib.Path( self.YEAR_DIR )

        self.run_types = [ "calib",  "pedestal", #"calibration",
                           "cosmics", "physics", "beam", "junk"
                          ]
        self.run_directories = sorted( list(self.YEAR_DIR.glob( '[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]/' ) ) ) # because ChatGPT said so...
        
    def Process( self ) :
        if self.info.run is not None:
            self.ProcessRunPage()
            time.sleep( 1 )
            
        self.ProcessYearPage()

    def ProcessYearPage( self ) : # WIP
        # Update the list of runs
        self.run_directories = sorted( list(self.YEAR_DIR.glob( '[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]/' ) ) ) # because ChatGPT said so...
        index_template = self.YEAR_DIR / "index_template.html"
        print( "base file:", index_template )
        year_page = self.YEAR_DIR / "index.html"
        shutil.copy( index_template, year_page )
        
        print( "\n\n\n", year_page )
        
        with open( str(year_page), mode='a' ) as file :

            file.write( "<div id=\"col\" style=\"column-count:" + str(len(self.run_types)) + ";\">\n" )
            for run_type in self.run_types :
                file.write( "  <h3>" + run_type + "</h3>\n" )
                file.write( "  <p>\n" )
                file.write( "    <ul>\n" )
                counter = 0
                for run_dir in self.run_directories :
                    files = list( run_dir.glob( run_type + '*' ) )
                    if self.info.year == 2023 :
                        if run_type == "junk" or run_type == "pedestal" :
                            files = list( run_dir.glob( 'intt*' ) )
                            
                    #if self.info.year == 2023 or self.info.year == 2024 and (run_type == "pedestal" or run_type == "junk" ) :
                    #if (self.info.year == 2023 and (run_type == "pedestal" or run_type == "junk" ) ) or \
                    #   self.info.year == 2024 and (run_type == "pedestal" or run_type == "junk" ) and False:
                    #    files = list( run_dir.glob( "intt_intt" + '*' ) )

                    if len(files) == 0 :
                        continue

                    # Showing operated FELIX servers. I assume that at least 1 FELIX was operated.
                    contents_felixes = " (intt"                    
                    for felix in range(0, 8 ) :
                        files = list( run_dir.glob( '*intt' + str(felix) + '*' ) )
                        if len(files) != 0 :
                            contents_felixes += str(felix) + ", "

                    # end of for felix in range(0, 8 )
                    contents_felixes = contents_felixes[0:-2] + ")" # -2 means removing the last ","
                    # Shrink text size if many FELIX servers were operated. 2 step modification!
                    if len( contents_felixes ) > (7 + 10) :
                        contents_felixes = "<font size=\"-2\">" + contents_felixes + "</font>"
                    elif len( contents_felixes ) > (7 + 6 ) :
                        contents_felixes = "<font size=\"-1\">" + contents_felixes + "</font>"
                        
                    counter += 1
                    contents = \
                        "      <li>" + "<a href=\"./" + run_dir.name + "/index.html\">" + run_dir.name + "</a>"  + contents_felixes + "</li>\n"

                    file.write( contents )

                for num in range( 0, self.GetMaxDirNum() - counter ) :
                    file.write( "      <li style=\"color:white;\"></li>\n" )
                    
                file.write( "    </ul>\n" )
                file.write( "  </p>\n" )
            file.write( "</div>\n" )

        print( "  --->", year_page, "generated." )
        
    def ProcessRunPage( self ) :
        # Make directory for the run if it's not found
        if self.RUN_DIR.exists() is False : 
            self.RUN_DIR.mkdir()
            
        elif os.access( self.RUN_DIR, os.W_OK ) is False :
            print( self.RUN_DIR, "is not writable for you. Ask the owner")
            print( "\t", self.RUN_DIR.owner())
            print( "to change the access mode. Maybe" )
            print( "\t chmod g+w", self.RUN_DIR)
            print( "is OK to try." )
            return None
            
        elif self.info.homepage_run_clean is True :
            try :
                shutil.rmtree( self.RUN_DIR )
            except PermissionError:
                print( self.RUN_DIR, "cannot be deleted. The directory is not cleaned, and go on!" )
            
            os.mkdir( self.RUN_DIR )
        
        # Copy the template of index.html
        index_template = self.COMMISSIONING_DIR / "index_template.html"
        self.index_html = self.RUN_DIR / "index.html"
        shutil.copy( index_template, self.index_html )

        # Write header
        with open( str(self.index_html), mode='a' ) as file :
            file.write( "<h1>" + "Run " + str( int(self.info.run) ) + "</h1>\n" )

            year_page = self.HOMEPAGE_ADDRESS + "/commissioning_plots/" + str( self.info.year ) + "/index.html"
            file.write( "<a href=\"" + year_page + "\">" \
                        + "Go back to " + str(self.info.year) + " run list"\
                        + "</a>" )

        # for table of contents
        self.WriteToc()

        # QA section (hot channel/cosmic analysis)
        self.WriteQaSection()
        
        with open( str(self.index_html), mode='a' ) as file :
            file.write( "<h2>Plots for each FELIX</h2>\n" )
            
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
            
        print( "  --->", self.index_html, "generated." )

    def WriteToc( self ) :
        with open( str(self.index_html), mode='a' ) as file :
            felix_list = []
            for felix in range(0, 8 ) :
                server = "intt" + str(felix)
                # check whether at least one plot from this server is in the plot list 
                flag = True in [ server in str(val) for val in self.info.homepage_plots ]

                # store the FELIX server used in this run
                if flag : 
                    felix_list.append( server )

            # if the number of the servers in use is not so many, no toc is needed
            if len( felix_list ) < 3 :
                return None

            # Write toc!
            file.write( "<br><b>Table of Contents for each FELIX</b>:\n" )
            file.write( "<ul>\n" )
            for server in felix_list :
                file.write( "    <li><a href=\"#" + server + "\">" + server + "</a></li>\n" )

            file.write( "</ul>\n\n" )

    def WriteQaSection( self ) :
        with open( str(self.index_html), mode='a' ) as file :
            file.write( "<h2>QA</h2>\n" )

        self.WriteRawHitSection()
        self.WriteTrkrHitSection()
        self.WriteHotChannelSection()
        self.WriteBcoDiffCutSection()
        self.WriteCosmicSection()

    def WriteRawHitSection( self ) :
        # If no plots are found, let's finish
        if len( self.info.raw_hit_plots ) == 0 :
            return None
        
        with open( str(self.index_html), mode='a' ) as file :
            contents = "<h3>INTTRAWHIT </h3>\n"
            html_detail = "<details>\n" \
                "    <summary>Hide/Show</summary>\n"
            
            for plot in self.info.raw_hit_plots :
                try :
                    shutil.copy( str(plot), self.RUN_DIR/ plot.name )
                except PermissionError:
                    print( plot, "cannot be copied due to permission. Check the owner of the file and ask the person to add write permission to the file" )
                    continue
                
                print( self.RUN_DIR/ plot.name )
                contents += "<h4>" + plot.name + "</h4>\n"
                contents += html_detail + \
                "<div class=\"center\">\n" \
                    "    <object data=\"" + plot.name + "\"" \
                    "type=\"application/pdf\">\n" \
                    "    Your browser cannot show PDF. Why don't you use new one?\n"\
                    "    </object>\n" \
                    "</div>\n" \
                    "</details>\n\n"

            file.write( contents )
                
    def WriteTrkrHitSection( self ) :
        # If no plots are found, let's finish
        if len( self.info.trkr_hit_plots ) == 0 :
            return None
        
        with open( str(self.index_html), mode='a' ) as file :
            contents = "<h3>Trkr_hit </h3>\n"
            html_detail = "<details>\n" \
                "    <summary>Hide/Show</summary>\n"

            
            for plot in self.info.trkr_hit_plots :
                try :
                    shutil.copy( str(plot), self.RUN_DIR/ plot.name )
                except PermissionError:
                    print( plot, "cannot be copied due to permission. Check the owner of the file and ask the person to add write permission to the file" )
                    continue
                
                print( self.RUN_DIR/ plot.name )
                contents += "<h4>" + plot.name + "</h4>\n"
                contents += html_detail + \
                "<div class=\"center\">\n" \
                    "    <object data=\"" + plot.name + "\"" \
                    "type=\"application/pdf\">\n" \
                    "    Your browser cannot show PDF. Why don't you use new one?\n"\
                    "    </object>\n" \
                    "</div>\n" \
                    "</details>\n\n"

            file.write( contents )
    def WriteHotChannelSection( self ) :
        # If no plots are found, let's finish
        if len( self.info.hot_channel_plots ) == 0 :
            return None
        
        with open( str(self.index_html), mode='a' ) as file :
            contents = "<h3>Hot/Dead channel</h3>\n"
            html_detail = "<details>\n" \
                "    <summary>Hide/Show</summary>\n"
            
            contents_parts = ""
            for txt in self.info.hot_channel_txt :                
                print( txt )
                try :
                    shutil.copy( str( txt ), self.RUN_DIR/ txt.name )
                except PermissionError :
                    print( txt, "cannot be copied due to permission. Check the owner of the file and ask the person to add write permission to the file" )
                    continue

                hot_num = 0;
                
                with open( str(txt), mode='r' ) as hot_ch_list :
                    lines = hot_ch_list.readlines()
                    hot_num = len( lines ) - 1 # take 1 out for the header
                    # close txt
                    
                contents += "<h4>" + str( txt.name ) + "</h4>\n" \
                    "<a href=\"" + txt.name + "\">Hot ch list (txt)</a><br>\n" \
                    "#hot channels: " + str( hot_num ) + "\n"
                
                for plot in self.info.hot_channel_plots :
                    print( "- ", plot )
                    if plot.name.split( '.' )[0] != txt.name.split( '.' )[0] :
                        continue

                    try :
                        shutil.copy( str(plot), self.RUN_DIR/ plot.name )
                    except PermissionError:
                        print( plot, "cannot be copied due to permission. Check the owner of the file and ask the person to add write permission to the file" )
                        continue
                    
                    print( self.RUN_DIR/ plot.name )
                    if ".pdf" in plot.name : 
                        contents_parts += \
                            "<div class=\"center\">\n" \
                            "    <object data=\"" + plot.name + "\"" \
                            "type=\"application/pdf\">\n" \
                            "    Your browser cannot show PDF. Why don't you use new one?\n"\
                            "    </object>\n" \
                            "</div>\n"

            for plot in self.info.hot_channel_plots :
                if ".png" in plot.name :
                    try :
                        shutil.copy( str(plot), self.RUN_DIR/ plot.name )
                    except PermissionError:
                        print( plot, "cannot be copied due to permission. Check the owner of the file and ask the person to add write permission to the file" )
                        continue
                    
                    print( self.RUN_DIR/ plot.name )
                    contents_parts += \
                            "    <figure>\n" \
                            "      <a href=\"" + plot.name + "\" data-lightbox=\"images\" data-title=\"" + plot.name + "\">\n" \
                            "        <img " + "src=\"" + plot.name + "\" " + "alt=\"" + plot.name + "\"\>\n" \
                            "      </a>\n" \
                            "      <a href=\"" + plot.name + "\" download=\"" + plot.name + "\">\n" \
                            "        <figcaption>" + plot.name + "</figcaption>\n" \
                            "      </a>\n" \
                            "    </figure>\n"
                        
            contents += "<details>\n" \
                "    <summary>Hide/Show</summary>\n" \
                + contents_parts \
                + "</details>\n\n"

            file.write( contents )

    def WriteBcoDiffCutSection( self ) :
        # If no plots are found, let's finish
        
        if len( self.info.bco_diff_plots ) == 0 :
            return None
        
        with open( str(self.index_html), mode='a' ) as file :
            contents = "<h3>BCO difference cut </h3>\n" \
                "<details>\n" \
                "    <summary>Hide/Show</summary>\n"

            for plot in self.info.bco_diff_plots :
                try :
                    shutil.copy( str(plot), self.RUN_DIR/ plot.name )
                except PermissionError:
                    print( plot, "cannot be copied due to permission. Check the owner of the file and ask the person to add write permission to the file" )
                    continue
                
                print( self.RUN_DIR/ plot.name )
                if ".pdf" in plot.name : 
                    contents += \
                        "<div class=\"center\">\n" \
                        "    <object data=\"" + plot.name + "\"" \
                        "type=\"application/pdf\">\n" \
                        "    Your browser cannot show PDF. Why don't you use new one?\n"\
                        "    </object>\n" \
                        "</div>\n"
                elif ".png" in plot.name :
                    contents += \
                        "    <figure>\n" \
                        "      <a href=\"" + plot.name + "\" data-lightbox=\"images\" data-title=\"" + plot.name + "\">\n" \
                        "        <img " + "src=\"" + plot.name + "\" " + "alt=\"" + plot.name + "\"\>\n" \
                        "      </a>\n" \
                        "      <a href=\"" + plot.name + "\" download=\"" + plot.name + "\">\n" \
                        "        <figcaption>" + plot.name + "</figcaption>\n" \
                        "      </a>\n" \
                        "    </figure>\n"

            contents += "</details>\n"
            file.write( contents )
        
    def WriteCosmicSection( self ) :
        # self.COSMIC_DIR
        print( self.info.cosmic_plots )
        if len( self.info.cosmic_plots) == 0 :
            return None

        with open( str(self.index_html), mode='a' ) as file :
            contents = "<h2>Cosmic analysis</h2>\n" \
                "<details>\n" \
                "    <summary>Hide/Show</summary>\n"

            for plot in self.info.cosmic_plots :
                try :
                    shutil.copy( str(plot), self.RUN_DIR/ plot.name )
                except PermissionError:
                    print( plot, "cannot be copied due to permission. Check the owner of the file and ask the person to add write permission to the file" )
                    continue
                
                suffix = str(plot).split( "." )[1]

                if suffix == "pdf" :
                    contents += \
                    "<div class=\"center\">\n" \
                    "    <object data=\"" + plot.name + "\"" \
                    "type=\"application/pdf\">\n" \
                    "    Your browser cannot show PDF. Why don't you use new one?\n"\
                    "    </object>\n" \
                    "</div>\n" \

                print( str( plot ), suffix )
                # end of if suffix
            # end of for plot in ...
            contents += "</details>\n"
            file.write( contents )
        # end of with open()...
        
    def WriteFelixSection( self, felix ) :
        with open( str(self.index_html), mode='a' ) as file :
            server = "intt" + str(felix)

            counter = 0
            contents = "    <details open><summary>Hide/Show</summary>\n" 
            # loop over all plots
            for plot in self.info.homepage_plots :
                #print( plot, server, server in str(plot) )

                # if this plot is from this server, add it to the contents
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
                    elif plot_type == "hit" : 
                        plot_title = "Hist distributions"
                    else:
                        print( "unknown plot type:", plot_type )
                        plot_title = "Unknown type"

                    caption = run_type + "<br> " + plot_title + "<br> chunk " + chunk

                    print( "in:", plot )
                    print( "out:", self.RUN_DIR / image )
                    try :
                        shutil.copy( plot, self.RUN_DIR / image )
                    except PermissionError:
                        print( plot, "cannot be copied due to permission. Check the owner of the file and ask the person to add write permission to the file" )
                        continue
                    
                    contents += \
                        "    <figure>\n" \
                        "      <a href=\"" + image + "\" data-lightbox=\"images\" data-title=\"" + image + "\">\n" \
                        "        <img " + "src=\"" + image + "\" " + "alt=\"" + image + "\"\>\n" \
                        "      </a>\n" \
                        "      <a href=\"" + image + "\" download=\"" + image + "\">\n" \
                        "        <figcaption>" + caption + "</figcaption>\n" \
                        "      </a>\n" \
                        "    </figure>\n"


            # after loop over all plots, all contents are in the variable. It the contents is not nohting, write it down
            if counter != 0 :
                file.write( "<h3 id=\"" + server+ "\">" + server + "</h3>\n" )
                
                contents += "    </details>\n"
                file.write( contents )
                contents = ""

                #file.write( "No data found<br>\n\n" )

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
