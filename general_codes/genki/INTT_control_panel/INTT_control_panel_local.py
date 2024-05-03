import PySimpleGUIQt as sg
import subprocess as sp
import os
import signal
import time
import screeninfo
import logging
import platform

web_browser = "firefox"
print( platform.system() )
if platform.system() == "Darwin" :
    web_browser = "open -a Firefox"

year = 2024

font_family = "Helvetica"
font_title = ( font_family, 25 )
font_button = ( font_family, 15 )
font_def = ( font_family, 20 )

#button_size = (100, 70 ) # in px
button_size = (80, 60 ) # in px
button_size_double = (button_size[0]*2, button_size[1])
button_size_triple = (button_size[0]*3, button_size[1])
button_size_small = (button_size[0]/2, button_size[1])
button_size_15 = (button_size[0]*1.5, button_size[1])
button_border = 1
button_color_close = ('', '' )

sg.set_options( font=font_def,
                border_width=button_border,
#                button_element_size = button_size, # doesn't work
                #element_padding=(0,0) # not good
               )

theme = 'LightGreen'
sg.theme( theme )

def GetLeftMonitor() :
    # Loop over all monitors to find the monitor at 0 in x
    for monitor in screeninfo.get_monitors() :
        if monitor.x == 0 :
            return monitor

    # just in case
    return None

def GetPrimaryMonitor() :
    # Loop over all monitors to find the monitor at 0 in x
    for monitor in screeninfo.get_monitors() :
        if monitor.is_primary : 
            return monitor

    # just in case
    return None


def GetLargestMonitor( mode="width" ) : 
    size = 0

    # Loop over all monitors to get the largest number
    for monitor in screeninfo.get_monitors() :
        monitor_size = 0
        if mode == "width" :
            monitor_size = monitor.width
        elif mode == "height" :
            monitor_size = monitor.height

        # comparison to the size of the previous monitor, and take larget number
        if size < monitor_size :
            size = monitor_size

    # Loop over all monitors again to return the largest one
    for monitor in screeninfo.get_monitors() :
        if mode == "width" and size == monitor.width :
            return monitor
        elif mode == "height" and size == monitor.height :
            return monitor

    # just in case
    return None

def GetWindowLocation( pad_x=3, pad_y=3, pad_num_x=3, pad_num_y=3 ) :
    left_monitor = GetLeftMonitor()
    largest_height = GetLargestMonitor( mode="height" ).height
    window_x = int( left_monitor.width * (pad_x-1) / pad_num_x )
    window_y = int( left_monitor.height * (pad_y-1) / pad_num_y )
    offset_y = largest_height - left_monitor.height
    return ( window_x, window_y + offset_y )
            
def LaunchTerminal( window, keyword ) :
    num = keyword[-1]
    window_size = [ 2560 / 3, 1440 / 3 ]
    offset= [ 0,  720 ]
    
    column = int(num) % 3
    row = int( int(num) / 3 )
    #position = [ int(window_size[0] * column + offset[0]), int(window_size[1] * row + offset[1]) ]
    location = GetWindowLocation( column+1, row+1 ) 
    command = "exec gnome-terminal " \
        + "--window " \
        + "--geometry=104x25+" + str(location[0]) + "+" + str(location[1]) + " " \
        + "--profile=in_WQHD " \
        + "--command \"" + "ssh intt" + num  + "\"" 
    
    print( command )
    
    process = sp.Popen( command, shell=True )
    return process

def LaunchAllTerminal( window ) :
    for num in range( 0, 8 ) :
        keyword = "terminal_intt" + str(num)
        print( keyword )
        print(window[ keyword ])
        LaunchTerminal( window, keyword )

def LaunchOpc0Tunnel() :
    primary_monitor = GetPrimaryMonitor()
    location = [ primary_monitor.x + primary_monitor.width,
                 primary_monitor.y + primary_monitor.height - 100 ]

    command = "gnome-terminal --title=OPC0_tunnel " \
        + "--window --geometry=23x13+" \
        + str(location[0]) + "+" + str(location[1]) + " " \
        + "-- ssh opc0"

    #print( command ) 
    process = sp.Popen( command, shell=True )
    time.sleep( 1 ) 
    return process

def LaunchLV() :    
    command = "/home/inttdev/soft/visionclientlauncher/app/visionclientlauncher.sh"
    process = sp.Popen( command, shell=False )
    return process

def LaunchHV() :
    primary_monitor = GetPrimaryMonitor()
    gui_size = [ 3016, 973 ]
    location = [ primary_monitor.x + primary_monitor.width - gui_size[0],
                 primary_monitor.y + primary_monitor.height - gui_size[1] - 100 ]

    command = "gnome-terminal --window --geometry=230x13+" \
        + str(location[0]) + "+" + str(location[1]) + " " \
        + "-- ~/bin/hv"
    process = sp.Popen( command, shell=True )
    return process

def LaunchDAQ() :
    primary_monitor = GetPrimaryMonitor()
    gui_size = [ 1118, 1116 ]
    location_gui = [ primary_monitor.x, primary_monitor.y ] # y offset for the title bar
    location_terminal = [ primary_monitor.x, primary_monitor.y + gui_size[1] + 100 ] # y offset for the title bar
    command = "gnome-terminal -p --window --geometry=86x36+" \
        + str(location_terminal[0]) + "+" + str(location_terminal[1]) + " " \
        + "-- source ~/.bashrc ; ~/bin/daq --location=" + str(location_gui[0]) + "+" + str(location_gui[1])

    print( command )
    process = sp.Popen( command, shell=True )
    return process

def OpenRunPage():
    command = web_browser + " https://sphenix-intra.sdcc.bnl.gov/WWW/run/" + str(year)
    process = sp.Popen( command, shell=True )
    return process

def LaunchExpertGui() :
    primary_monitor = GetPrimaryMonitor()
    gui_size = [ 1200, 700 ]
    location = [ primary_monitor.x + primary_monitor.width - gui_size[0],
                 primary_monitor.y + primary_monitor.height - gui_size[1] - 100 ]

    command = "gnome-terminal --window --geometry=92x26+" \
        + str(location[0]) + "+" + str(location[1]) + " " \
        + "-- ~/bin/expert_gui"
    process = sp.Popen( command, shell=True )
    return process

def OperateExpertGuiGrpc( mode="start" ) : 
    primary_monitor = GetPrimaryMonitor()
    gui_size = [ 1200, 700 ]
    location = [ primary_monitor.x + primary_monitor.width - gui_size[0],
                 primary_monitor.y + primary_monitor.height - gui_size[1] - 300 ]
    command = "gnome-terminal --window --geometry=92x26+" \
        + str(location[0]) + "+" + str(location[1]) + " " \
        + "-- ~/bin/operate_grpc_servers " \
        + mode

    process = sp.Popen( command, shell=True )
    return process

def LaunchElog() :
    command = web_browser + " https://sphenix-intra.sdcc.bnl.gov/WWW/elog/INTT/"
    process = sp.Popen( command, shell=True )
    return process

def LaunchRunLog() :
    # command = "firefox https://docs.google.com/spreadsheets/d/1dkvDEc5iUQd_xskGzAvR5JQ_HzxxxeJfPXEdy0TMKas/edit#gid=0" # Run23
    command = web_browser + " https://docs.google.com/spreadsheets/d/1qCxH_oOPnLCEpg9fgW_zC0G5__piRsRhklkb0tKznRQ/edit?usp=sharing" # Run24
    process = sp.Popen( command, shell=True )
    return process

def LaunchWiki() :
    command = web_browser + " https://wiki.sphenix.bnl.gov/index.php/Intermediate_Tracker_\(INTT\)"
    print( command ) 
    process = sp.Popen( command, shell=True )
    return process

def LaunchHomepage() :
    command = web_browser + " https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/intt/"
    print( command ) 
    process = sp.Popen( command, shell=True )
    return process

def LaunchBeamMonitor() :
    command = web_browser + " https://cadweb.bnl.gov/dashs/Operations/BroadcastWeb/BroadcastMain.dash"
    print( command ) 
    process = sp.Popen( command, shell=True )
    return process

def LaunchHowTo() :
    command = web_browser + " https://docs.google.com/presentation/d/1w16_TctvFx27ZP1ikPqal-Ndi733mIvDfO8xNmMXGvg/edit?usp=sharing"
    print( command ) 
    process = sp.Popen( command, shell=True )
    return process

def UpdatePlot( window, run_type ) :
    run_str = window[ 'run_number' ].get()
    try:
        run = str( int( run_str ) ).zfill( 8 )
    except ValueError:
        sg.popup_error( "Run number must be an integer" )
        window[ 'run_number' ].update( 0 )
        process = sp.Popen( "echo", shell=True )
        return process

    command = "ssh inttdaq /home/inttdev/bin/process_data "\
        + ' --update-plot '\
        + ' --run-type ' + run_type + ' '\
        + ' ' + run + ' '\

    command_2 = web_browser + " https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/intt/commissioning_plots/" + str(year) + '/' + run + '/'

    command += '; ' + command_2
    print( command ) 
    process = sp.Popen( command, shell=True )
    return process

def OpenRunSummary( window ) :
    run_str = window[ 'run_number' ].get().zfill( 8 )
    try:
        run = str( int( run_str ) ).zfill( 8 )
    except ValueError:
        sg.popup_error( "Run number must be an integer" )
        window[ 'run_number' ].update( 0 )
        process = sp.Popen( "echo", shell=True )
        return process
    
    command = web_browser + " https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/intt/commissioning_plots/" + str(year) + '/' + run_str
    print( command )

    process = sp.Popen( command, shell=True )
    return process


def SendData( window, run_type ) :
    run_str = window[ 'run_number' ].get()
    try:
        run = str( int( run_str ) ).zfill( 8 )
    except ValueError:
        sg.popup_error( "Run number must be an integer" )
        window[ 'run_number' ].update( 0 )
        process = sp.Popen( "echo", shell=True )
        return process

    command = "ssh inttdaq /home/inttdev/bin/process_data "\
        + ' --send-SDCC '\
        + ' --no-check-bbox ' \
        + ' --root-dir bbox0 ' \
        + ' --force-run ' \
        + ' --run-type ' + run_type + ' '\
        + ' ' + run + ' '\

    print( command )
    process = sp.Popen( command, shell=True )
    return process

def MakePlot( window, run_type, condor=False, quick=False ) :
    run_str = window[ 'run_number' ].get()
    try:
        run = str( int( run_str ) ).zfill( 8 )
    except ValueError:
        sg.popup_error( "Run number must be an integer" )
        window[ 'run_number' ].update( 0 )
        process = sp.Popen( "echo", shell=True )
        return process

    #command = "/home/inttdev/bin/process_data "\
    #command = "/Users/genki/bin/process_data "\
    command = "~/bin/process_data "\
        + ' --calib-2024 '\
        + ' --run-type ' + run_type + ' '\
        + ' ' + run + ' '\

    if window[ 'quick' ].get() is True :
        command += ' --quick '
        
    if window[ 'condor' ].get() is True :
        command += ' --condor '

    command_2 = web_browser + " https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/intt/commissioning_plots/" + str(year) + '/' + run + '/'

    command += '; ' + command_2
    print( command ) 
    process = sp.Popen( command, shell=True )
    return process

def DSTProcess( window, run_type, event ) :
    run_str = window[ 'run_number' ].get()
    try:
        run = str( int( run_str ) ).zfill( 8 )
    except ValueError:
        sg.popup_error( "Run number must be an integer" )
        window[ 'run_number' ].update( 0 )
        process = sp.Popen( "echo", shell=True )
        return process

    
    #if window[ 
    #command = "/home/inttdev/bin/process_data "\
    #command = "/Users/genki/bin/process_data "\
    command = "~/bin/process_data "\
        + ' ' + event + ' '\
        + ' --run-type ' + run_type + ' '\
        + ' ' + run + ' '
        #+ ' --dry-run ' \

    if window[ 'quick' ].get() is True :
        command += ' --quick '
        
    if window[ 'condor' ].get() is True :
        command += ' --condor '
        
    print( command )
    proc = sp.Popen( command, shell=True )
    return proc
                         
def RaulDaq( window ) :
    run_str1 = window[ 'run_date' ].get()
    run_str2 = window[ 'run_time' ].get()

#    command = "ssh inttdaq \"source /usr/local/root/bin/thisroot.sh; cd /home/inttdev/macro/ ; ./process_latest_data_2024.sh\""

    command = "ssh inttdaq \"source /usr/local/root/bin/thisroot.sh; cd /home/inttdev/macro/ ; ./process_latest_data_2024_all.sh" \
        + ' ' + run_str1 + ' ' + run_str2 + "\""

    command_2 = web_browser + " https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/intt/commissioning_plots/" + str(year) + '/'

    command += '; ' + command_2
    
    process = sp.Popen( command, shell=True )
    return process
    
def GetLog():
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.INFO)
    formatter = logging.Formatter(' %(asctime)s -- %(message)s')
    file_handler = logging.FileHandler('/tmp/INTT_control_panel.log')
    file_handler.setLevel(logging.INFO)
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)
    return logger

def LaunchAll( window ) :
    LaunchLV()
    LaunchHV()
    LaunchAllTerminal( window )
    LaunchDAQ()

def app() :
    layout_ssh = [
        [
            sg.Button( "intt 0\n0S, 1S", key="terminal_intt0", size_px=button_size, font=font_button ),
            sg.Button( "intt 1\n2S, 3S", key="terminal_intt1", size_px=button_size, font=font_button ),
            sg.Button( "intt 2\n4S, 5S", key="terminal_intt2", size_px=button_size, font=font_button )
        ],
        [
            sg.Button( "intt 3\n6S, 7S", key="terminal_intt3", size_px=button_size, font=font_button ),
            sg.Button( "intt 4\n0N, 1N", key="terminal_intt4", size_px=button_size, font=font_button ),
            sg.Button( "intt 5\n2N, 3N", key="terminal_intt5", size_px=button_size, font=font_button )
            ],
        [
            sg.Button( "intt 6\n4N, 5N", key="terminal_intt6", size_px=button_size, font=font_button ),
            sg.Button( "intt 7\n6N, 7N", key="terminal_intt7", size_px=button_size, font=font_button ),
            sg.Button( "ALL intt", key="terminal_all", size_px=button_size, font=font_button ),
        ]
        ]
    
    frame = sg.Frame( "", 
                      layout_ssh, 
                      element_justification='center',
                      border_width=4
                     )
    layout = [
        
        #[
        #sg.Text( "INTT Operation", font=font_title, justification="center" )
        #],
        [
            #sg.Button( "ALL", size_px=button_size, font=font_button ),
            sg.Button( "LV", size_px=button_size, font=font_button ),
            sg.Button( "HV", size_px=button_size, font=font_button ),
            #sg.Button( "DAQ", size_px=button_size, font=font_button),
            #sg.Button( "Start\ngRPCs", key="expert_gui_start_grpc", size_px=button_size, font=font_button ),
            #sg.Button( "Kill\ngRPCs", key="expert_gui_kill_grpc", size_px=button_size, font=font_button ),
            sg.Button( "Expert\nGUI", key="expert_gui", size_px=button_size, font=font_button ),
            sg.Button( "OPC0\nTunnel", key="opc0_tunnel", size_px=button_size, font=font_button),
            sg.Button( "HowTo", size_px=button_size, font=font_button )
        ],
        [ frame ],
        [
            sg.Button( "Run\nPage", key='open_run_page', size_px=button_size, font=font_button),
            sg.Button( "INTT\nPage", key="opne_hp", size_px=button_size, font=font_button ),
            sg.Button( "Elog", size_px=button_size, font=font_button ),
            sg.Button( "Run\nLog", key="run_log", size_px=button_size, font=font_button ),
            sg.Button( "Wiki", size_px=button_size, font=font_button ),
            sg.Button( "Beam", size_px=button_size, font=font_button )
            #sg.Button( "Close", size_px=button_size, font=font_button, button_color=button_color_close )
        ],
        [
            sg.Button( "↓", key="decrease_run_number", size_px=button_size_small ),
            sg.Input( 39494, key="run_number", size_px=button_size_double ),
            sg.Button( "↑", key="increase_run_number", size_px=button_size_small ),
            #sg.Spin(values=[ i for i in range(0, 99) ], key="run_number_spin", size_px=button_size_double),
            sg.Button( "Send\nData", key="send_data", size_px=button_size, font=font_button )
        ],
        [
            sg.Text( "", size_px=button_size_small ),
            sg.Combo( values=( "beam", "cosmics", "calib", "pedestal", "junk"), default_value="cosmics", key='run_type', size_px=button_size_double ),
            sg.Text( "", size_px=button_size_small ),
            sg.Button( "Open Run\nSummary", key="open_run_summary", size_px=button_size_15, font=font_button ),
            sg.Button( "Update Run\nSummary", key="update_plot", size_px=button_size_15, font=font_button )
        ],
        
        [
            sg.Checkbox( "Only first 1M hits", default=True, key="quick", size_px=button_size_triple ),
            sg.Checkbox( "Condor batchjob", default=False, key="condor", size_px=button_size_triple )
            
        ],
        [
            sg.Text( "Hit-Base\nProcesses", size_px=button_size_double, font=font_button, justification="rigth" ),
            #sg.Button( "", key="make_plots_quick", size_px=button_size, font=font_button ),
            sg.Button( "Make\nPlots!", key="make_plots", size_px=button_size, font=font_button )
            #sg.Button( "Condor\nPlots!", key="make_plots_condor", size_px=button_size, font=font_button ),
        ],
        [
            sg.Text( "DST\nProcesses", size_px=button_size_double, font=font_button, justification="right" ),
            sg.Button( "RAW\nHIT", key="--DST-INTTRAW", size_px=button_size, font=font_button, disabled=False, button_color=button_color_close ),
            sg.Button( "Hitmap", key="--DST-INTTRAW-hitmap", size_px=button_size, font=font_button, disabled=False, button_color=button_color_close ),
            sg.Button( "Hot ch", key="--DST-INTTRAW-hot-ch", size_px=button_size, font=font_button, disabled=False, button_color=button_color_close ),
            sg.Button( "BCO\ndiff", key="--DST-INTTRAW-bco-diff", size_px=button_size, font=font_button, disabled=False, button_color=button_color_close ),
        ],
        [
            sg.Text( "", size_px=button_size_double, font=font_title, justification="right" ),
            sg.Button( "Trkr\nHit", key="--DST-TrkrHit", size_px=button_size, font=font_button, disabled=False, button_color=button_color_close ),
            sg.Button( "Trkr\nCluster", key="--DST-TrkrCluster", size_px=button_size, font=font_button, disabled=False, button_color=button_color_close ),
            sg.Button( "Do All", key="--DST-all", size_px=button_size, font=font_button ),
        ],
        [
            sg.Input( 240311, key="run_date", size_px=button_size_double ),
            sg.Input(   1207, key="run_time", size_px=button_size ),
            sg.Button( "RaulDAQ\nProcess", key="raul_daq", size_px=button_size_double, font=font_button ),
            sg.Button( "Close", size_px=button_size, font=font_button, button_color=button_color_close )
        ]
        
    ]


    window = sg.Window( "INTT Operation App Launcher", layout,
                        keep_on_top=True,
                        grab_anywhere=True,
                        no_titlebar=True,
                        alpha_channel=1,
                        location=GetWindowLocation() ,
                        finalize=True )
    #window.bind("<Control-q>", "Ctrl-q")

    ## added for GUI log
    logg = GetLog()

    processes = []
    while True :
        event, values = window.read( timeout=1000 )

        ## added for GUI log
        run = window[ 'run_number' ].get()

        if event is not None and event != "__TIMEOUT__" :
            print( "event:", event)


        ###############################################################
        # for events which don't need a function
        if event == sg.WIN_CLOSED or event == 'Close' or event == 'Ctrl-q' or event == 'Exit':
            break

        if event == "__TIMEOUT__" :
            continue
        if event == "ALL" :
            LaunchAll( window )
        elif event.find( "terminal_intt" ) != -1 :
            processes.append( LaunchTerminal( window, event ) )

        elif event == "terminal_all" :
            LaunchAllTerminal( window )

        elif event == "LV" :
            processes.append( LaunchLV() )
            
        elif event == "HV" :
            processes.append( LaunchHV() )

        elif event == "open_run_page" :
            processes.append( OpenRunPage() )
        elif event == "DAQ" :
            location = [window.current_location()[0], window.current_location()[1] ]
            location = [ location[0]-200, location[1] ]
            sg.popup( "Due to technical difficulties, the button cannot launch the GUI properly. Please login to inttdaq and start the GUI by yourself...",
                      auto_close = True,
                      auto_close_duration = 30,
                      font = font_title,
                      location=location,
                      no_titlebar = True,
                      keep_on_top = True,
                      #any_key_closes = True,
                      non_blocking=True
                     )
            processes.append( LaunchDAQ() )
        elif event == "Elog" :
            processes.append( LaunchElog() )
        elif event == "Wiki" :
            processes.append( LaunchWiki() )
        elif event == "run_log" :
            processes.append( LaunchRunLog() )
        elif event == "Beam" :
            processes.append( LaunchBeamMonitor() )
        elif event == "HowTo" :
            processes.append( LaunchHowTo() )
        elif event == "opne_hp" :
            processes.append( LaunchHomepage() )
        elif event == "opc0_tunnel" :
            processes.append( LaunchOpc0Tunnel() )
        elif event == "expert_gui" :
            processes.append( LaunchExpertGui() )
        elif event == "expert_gui_start_grpc" :
            processes.append( OperateExpertGuiGrpc( mode="start" ) )
        elif event == "expert_gui_kill_grpc" :
            processes.append( OperateExpertGuiGrpc( mode="kill" ) )
        elif event == "update_plot" :
            processes.append( UpdatePlot( window, values['run_type'] ) )
        elif event == "open_run_summary" :
            logg.info("Opening Run page: " + run)
            processes.append( OpenRunSummary( window ) )
        elif event == "send_data" :
            processes.append( SendData( window, values['run_type'] ) )
            logg.info("Send Data Run: " + run)
        elif event == "make_plots" :
            processes.append( MakePlot( window, values['run_type'] ) )
            logg.info("Make Plot Run: " + run)
        elif event == "make_plots_quick" :
            processes.append( MakePlot( window, values['run_type'], quick=True ) )
            logg.info("QuickLook Run: " + run)
        elif event == "make_plots_condor" :
            processes.append( MakePlot( window, values['run_type'], condor=True ) )
            logg.info("Make Plot Run: " + run)
        elif event == "increase_run_number" :
            window[ 'run_number' ].update( str(int(window['run_number'].get()) + 1) )
        elif event == "decrease_run_number" :
            window[ 'run_number' ].update( str(int(window['run_number'].get()) - 1) )
        elif "DST" in event :
            logg.info( event + ":" + run )
            processes.append( DSTProcess( window, values[ 'run_type'], event ) )
        elif event == "raul_daq" :
            processes.append( RaulDaq( window ) )
            logg.info("Process RaulDAQ data" )

    print( "Terminate all processes" )
    for proc in processes :
        #os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
        proc.kill()
        print( proc )
                       
if __name__ == "__main__" :
    app()
