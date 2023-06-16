"""!
@breif An extended library for INTT.
@author G. Nukazuka
@date 2023/May/02
@details
# About
Some people copied and custmized Raul's intt.py to have better usability.
It's clear that we will be bothered by variety of versions in the future.
Following Raul's update manually isn't easy, and it cause uneccesary effort for debugging, I believe.
Hence all customize by INTT member except Raul, have to be in this extention library.

# How to use
To use functions in this file, 

    import intt_ext

is enough. You have to be careful that you need to import intt module to use functions in the module. It's different from C/C++.
So your code is probably,

    import dam
    import intt
    import intt_ext

    d = dam.dam()
    d.reset()

    intt.cold_start( d ) # for example
    intt_ext.cold_start( d ) # If you know the difference from the original cold start...

"""
import datetime
import numpy as np
import os
import pprint
import re # for str comparison using regular expression
import socket # to get hostname
import subprocess
import sys
import time

current_dir = os.path.join( os.path.dirname(__file__) ) 
sys.path.append( current_dir )

import dam 
import intt

############################################################
# General functions                                        #
############################################################

def ReadMap( map_path=None, element_num=None, debug=False ) :

    try :
        int( element_num )
    except ValueError:
        print( "element_num must be a integer.\nStopped." )
        return None
        
    if os.path.exists( map_path ) is False :
        print( map_path, "cannot be found....", file=sys.stderr )
        return None
        #sys.exit()

    rtn_maps = [] # a list of [Felix ch] [chip ID] [DAC0]

    # read the DAC0 map file
    with open( map_path ) as file :

        if debug is True :
            print( "Contents in the map file" )
            
        # read each line
        for line in file :
            if debug is True :
                print( line, end='' )

            # for comment out or empty line (work???)
            if line[0] == "#" or len(line.split()) == 0 :
                continue

            if element_num is None : 
                rtn_map = line.split()
            else :
                rtn_map = line.split()[ 0 : element_num ]

            if debug is True :
                print( "Filling ", rtn_map )

            rtn_maps.append( rtn_map )

            # end of for line in file
        # end of with open( map_file ) as file
        
    if debug is True :
        print( "Map to be returned:" )
        print( rtn_maps )
        
    return rtn_maps
    
def ReadDac0Map( map_path=None, debug=False ) :
    """!
    @biref 
    @param map_path A path to the DAC0 map file. If nothing give, the default map is taken.
    @retval A list of map for the INTT DAQ server (intt0, intt1, ...) (I assumed that it's used in an INTT DAQ server) is returned. See below for more details.
    @details More explanations...
    # The place for the file and filename

    The path to the file is assumed to be /home/phnxrc/INTT/map_dac0/
    Each dataset should be in a sub-directory under the map_dac0 directory.
    You have to make a symbolic link to the sub-directory to be used.
    The link must be named "latest".
    The hostname of the INTT DAQ servers is intt[0-7]. So the file name should be, for example, intt4_dac0.txt.
    You should make a file with a different name from the assumed one. Then you can make a symbolic link to the file.
    For example, 
      $ ln -s /home/phnxrc/INTT/map_dac0/intt3_dac0_ver20230501.txt. /home/phnxrc/INTT/map_dac0/intt3_dac0.txt    

    ## Example
    It helps you to understand it.

        [inttdev@inttdaq 19:04:33 map_dac0] $ tree
        .
        ├── latest -> ver_dummy
        └── ver_dummy
            ├── intt0_dac0.txt
            ├── intt1_dac0.txt
            ├── intt2_dac0.txt
            ├── intt3_dac0.txt
            ├── intt4_dac0.txt
            ├── intt5_dac0.txt
            ├── intt6_dac0.txt
            └── intt7_dac0.txt

    # Format

    The format of the DAC0 map file is assumed to be 
        [INTT DAQ server num] [Felix ch] [chip ID] [DAC0]
    Here, 
        - INTT DAQ server num: from 0 to 7 (integer)
        - Felix ch: from 0 to 13
        - chip ID: from 1 26 (not from 1 to 13 + side selection)
        - DAC0: from 0 to 255
    Values are separated by a whilte space or tab.
    Lines starting with "#" are ignored.

    # Returned value
    A list of the DAC0 map for the INTT DAQ server is returned. It's [FELIX ch][chip id][DAC 0].
    """
    # Map file for DAC0 config #####################################

    # None is the default
    hostname = socket.gethostname()
    if map_path is None : 
        username = os.getlogin() # notmally phnxrc, it can be inttdev in the test environment
        map_dir = "/home/" + username + "/INTT/map_dac0/latest/"    
        map_file = "dac0_map.txt"
        map_path = map_dir + map_file

    print( "DAC0 Map:", map_path )
    dac0_maps = ReadMap( map_path=map_path, element_num=4, debug=debug )
    # a list of [INTT DAQ server] [Felix ch] [chip ID] [DAC0], All integer
    # but returned list (if it's in the INTT DAQ server) is [Felix ch] [chip ID] [DAC0]
    
    dac_server_name_pattern = re.compile( "intt[0-7]" )
    if bool( dac_server_name_pattern.match( hostname ) ) is False :
        if debug is True :
            print( "You are not in the INTT DAQ server. Let\'s assume you are in intt1" )
        server_num = 1
    else :
        server_num = int( hostname[-1] ) # it's safe conversion!

    rtn_maps = []
    for dac0_map in dac0_maps :

        if debug is True :
            print( "This map", dac0_map, "is ", end='' )
        # If the DAQ number in the map is difference from the server num, skip it
        if int(dac0_map[0]) != server_num :
            if debug is True :
                print( "not taken." )
            continue

        if debug is True :
            print( "taken." )
        # take Felix ch, chip ID, and DAC0 as a list
        rtn_maps.append( [ int(val) for val in dac0_map[1:] ] )

    if debug is True :
        print( "DAC0 map:" )
        print( rtn_maps )
        
    return rtn_maps

def ReadLadderMap( map_path = None ) :
    """!
    @biref Ladder map (relation of Felix ch, ROC port, and the ladder name) is taken from the map file
    @param map_path The path to the map file. If nothing given, the default map depending on the INTT DAQ server will be used.
    @retval The ladder map as a dictionary. See below for more details.
    @details
    # The place for the file and filename

    I think /home/phnxrc/INTT/map_ladder/${hostname}_map.txt is fine.
    When the map needs to be updated (it shound't happen so often), you can make a new map file, namely ${hostname}_map_{whatever you like}.txt.
    Then you can delete the curreny symbolic link and make new one linked to the new map file.

    # Format 
    Each line should be
      [Felix ch] [ROC port] [Ladder Name]
    For example, 
      3  A3  B0L000N
    Values should be separated by a white space or a tab.
    For the moment, I don't use [Ladder Name]. If requested, I'll take the parameter too.
    Lines starting with "#" are ignored.
    It's the same format as the one used by the standalone DAQ.

    # Return value
    A dictonary is returned. The key is FELIX channel, and ROC port can be found by the key. For example:
      -1: "None"
      0 : "A1"
      1 : "A2"
      ...

    Key -1 has "None" in any cases.
    Ladder name is not included for the moment.

    # Misc
    Text file mapping will be replaced by sPHENIX Database.
    """
    
    # it's the default way to get the map path
    if map_path is None :
        hostname = socket.gethostname()
        username = os.getlogin() # notmally phnxrc, it can be inttdev in the test environment
        map_dir = "/home/" + username + "/INTT/map_ladder/"    
        map_file = hostname + "_map.txt"
        map_path = map_dir + map_file
        
    print( "Ladder Map:", map_path )

    ladder_maps = ReadMap( map_path=map_path, element_num=3 )

    # convert FELIX ch (ladder_maps[0]) from str to int
    for ladder_map in ladder_maps :
        ladder_map[0] = int( ladder_map[0] )


    # Guarantee that the map is sorted by FELIX ch
    ladder_maps = sorted( ladder_maps )
    return ladder_maps

def ReadServerMap( map_path = None ) :
    """!
    @biref Server map (relation of hostname, IP address, and covering direction (South/North)) is taken from the map file
    @param map_path The path to the map file. If nothing given, the default map depending on the INTT DAQ server will be used.
    @retval The ladder map as a dictionary. See below for more details.
    @details
    # The place for the file and filename

    I think /home/phnxrc/INTT/map_server/map_server is fine.
    Most probabaly, it will not be changed.

    # Format 
    Each line should be
      [Hostname] [IP address] [Direction]
    For example, 
      intt9 255.255.255.255 East
    Values should be separated by a white space or a tab.
    Lines starting with "#" are ignored.

    # Return value
    A dictonary is returned. The key is hostname. For example:
      "intt8" : [ "255.255.255.254", "West" ]
      "intt9" : [ "255.255.255.255", "East" ]
      ...

    # Misc
    Text file mapping will be replaced by sPHENIX Database.
    """
    
    # it's the default way to get the map path
    if map_path is None :
        hostname = socket.gethostname()
        username = os.getlogin() # notmally phnxrc, it can be inttdev in the test environment
        map_dir = "/home/" + username + "/INTT/map_server/"
        map_file = "map_server.txt"
        map_path = map_dir + map_file
        
    print( "Server Map:", map_path )

    server_maps = ReadMap( map_path=map_path, element_num=3 )
    #server_dict = { -1 : [ "0.0.0.0", "None" ] }
    server_dict = {}
    # convert FELIX ch (ladder_maps[0]) from str to int
    for server_map in server_maps :
        print( server_map )
        server_dict[ server_map[0] ] = [ server_map[1], server_map[2] ]

    return server_dict
    
def GetServerRocMap() :
    """!
    @brief
    @retval
    @details
    """
    server_ROC_map = {
        "intt0" : ['RC-0S','RC-1S'],
        "intt1" : ['RC-2S','RC-3S'],
        "intt2" : ['RC-4S','RC-5S'],
        "intt3" : ['RC-6S','RC-7S'],
        "intt4" : ['RC-0N','RC-1N'],
        "intt5" : ['RC-2N','RC-3N'],
        "intt6" : ['RC-4N','RC-5N'],
        "intt7" : ['RC-6N','RC-7N']
    }
    return server_ROC_map
    
def GetPortIDWithName( port_name=None ) :    
    """!
    @brief
    @retval
    @details
    """
    
    port_to_id_map = {"A1" : 0,  "A2" : 1, "A3" : 2,
                      "B1" : 3,  "B2" : 4, "B3" : 5,
                      "C1" : 6,  "C2" : 7, "C3" : 8,
                      "D1" : 9, "D2" : 10, "D3" : 11
                      }
    
    # if no name if given, return all of them
    if port_name is None :
        return port_to_id_map
    
    return port_to_id_map[ port_name ]

#def GetWedgesWithPortID( wedge ) :
"""!
@brief
@retval
@details
"""

def GetFPHXParameterAddress() :
    """!
    @brief 
    """
    parameter_address_dict = {}
    parameter_address_dict["Mask"] = 1
    parameter_address_dict["DigiCtrl"] = 2 
    parameter_address_dict["Vref"]     = 3 
    parameter_address_dict["DAC0"]     = 4 
    parameter_address_dict["DAC1"]     = 5 
    parameter_address_dict["DAC2"]     = 6 
    parameter_address_dict["DAC3"]     = 7 
    parameter_address_dict["DAC4"]     = 8 
    parameter_address_dict["DAC5"]     = 9 
    parameter_address_dict["DAC6"]     = 10
    parameter_address_dict["DAC7"]     = 11
    parameter_address_dict["N1sel"]    = 12
    parameter_address_dict["N2sel"]    = 12
    parameter_address_dict["FB1sel"]   = 13
    parameter_address_dict["Leaksel"]  = 13
    parameter_address_dict["P3sel"]    = 14
    parameter_address_dict["P2sel"]    = 14
    parameter_address_dict["Gsel"]     = 15
    parameter_address_dict["BWsel"]    = 15
    parameter_address_dict["P1sel"]    = 16
    parameter_address_dict["Injsel"]   = 16
    parameter_address_dict["LVDS"]     = 17
    return parameter_address_dict

def Is1stFPHXParam( name ) :
    """!
    @brief If the argument is the 1st parameter, which shares the same register with the 2nd parameter, True is returned.
    """
    if name == "N1sel" or name == "FB1sel" or name == "P3sel" or name == "Gsel" or name == "P1sel" :
        return True

    return False

def Is2ndFPHXParam( name ) :
    """
    @brief If the argument is the 2nd parameter, which shares the same register with the 1st parameter, True is returned.
    """
    if name == "N2sel" or name == "Leaksel" or name == "P2sel" or name == "BWsel" or name == "Injsel" :
        return True

    return False

def GenerateCommand( name, value_first, value_second ) :
    rtn = 0x0
    shift_val = 4 # for N2sel, Leaksel, P2sel
    #if name == "N2sel" or name == "Leaksel" or name == "P2sel":
    #    value = (int(value_second) << 4 ) + value_first
    #    rtn = intt.make_fphx_cmd( 21, FPHXRegister.register[name][0], 1, value )
    if name == "BWsel" or name == "Injsel":
        shift_val = 3
        
    value = (int(value_second) << shift_val ) + value_first
    address = GetFPHXParameterAddress()[ name ] 
    rtn = intt.make_fphx_cmd( 21, address, 1, value )
        
    return rtn
 
############################################################
# Overwritting functions                                   #
############################################################

def send_fphxparam( d, cheat, verbosity=0 ) :
    """!
    @brief Given FPHX commands are sent
    @param d the dam object
    @param cheat A list of FPHX commands
    """    
    for item in cheat:
        if verbosity > 0 :
            print( "FPHX command", item, "is sent." )
        intt.send_fphx_cmd(d, item)
        time.sleep(.001)        

def send_fphxparam_from_file( d, file_path, verbosity=0 ) :
    """!
    @brief FPHX parameters written in a text are sent to all FPHX chips.
    @param d
    @param file_path A path to a file containig FPHX parameters
    @param verbosity 
    @details The format of the file is
    [name] [value]
    here, [name] is the name of the parameter, and [value] is an integer value of the parameter.
    Available names are
        - Vref
        - DAC0
        - DAC1
        - DAC2
        - DAC3
        - DAC4
        - DAC5
        - DAC6
        - DAC7
        - N1sel
        - N2sel
        - FB1sel
        - Leaksel
        - P3sel
        - P2sel
        - Gsel
        - BWsel
        - P1sel
        - Injsel
        - LVDS

    Remember that N1sel, FB1sel, P3sel, Gsel, P1sel, N2sel, Leaksel, P2sel, BWsel, Injsel cannot be alone. 
    You have to provide pairs.
    Lines starting with "#" are ignored.    
"""

    parameters = ReadMap( map_path=file_path, element_num=20, debug=verbosity )
    if parameters is None :
        return None
    
    parameter_dict = {}
    for param in parameters :
        parameter_dict[ param[0] ] = int(param[1])

    print( parameter_dict )
    parameter_address_dict = GetFPHXParameterAddress()
    #print( "send_fphxparam_from_file:", parameters )

    commands = []
    command_sets = []
    # loop over all FPHX parameters to generate commands
    for keyword in parameter_dict :

        #def make_fphx_cmd(chipId, regId, cmd, data), normaly chipID=21(all), cmd=1(?)
        if keyword == "Mask" :
            command = intt.make_fphx_cmd( 21, 1, 2, 128 ) # 0xcf50a800 #  hex(make_fphx_cmd( 21, 1, 2, 128 ) )
            command_set = [ keyword, "N/A", command ]
            command_sets.append( command_set )
        else : # if not Mask
            address = parameter_address_dict[ keyword ]
            value = int( parameter_dict[ keyword ] )
            command = intt.make_fphx_cmd( 21, address, 1, value )

        # for parameters sharing the same register
        if Is1stFPHXParam( keyword ) is True :
            # keep the value and process in the next loop
            value_prev = value
            command_set = [ keyword, str(value), 0 ]
            command_sets.append( command_set )
            continue
        elif Is2ndFPHXParam( keyword ) is True :
            # make the command using the value of 2 parameters
            command = GenerateCommand( keyword, value_first=value_prev, value_second=value )
            command_sets[ -1 ][0] += "/" + keyword
            command_sets[ -1 ][1] += '/' +str(value)
            command_sets[ -1 ][2] = command
        else :
            command_set = [ keyword, value, command]
            command_sets.append( command_set )

        commands.append( command )
        # end of for keyword, maybe...
        
    if verbosity > 0 : 
        for command_set in command_sets : 
            print( command_set[0], command_set[1], command_set[2], "->", hex( command_set[2] ) )

    send_fphxparam( d, commands, verbosity=verbosity )
    
def macro_self(d, spacing =1199, n_pulses =10, n_ampl_steps =63, ampl_step =1, fphxparam=None):
    """!
    @brief A set of commands for pedestal (self-trigger) run
    @param d the dam object
    @param spacing
    @param n_pulses
    @param n_pulses
    @param n_ampl_steps
    @param ampl_step
    @param fphxparam A list of FPHX commands to be sent
    @details
    Some commands in the original macro (intt.macro_calib) are executed. The rest of them are for calibration runs.
    Users can give own set of FPHX commands. It's useful, for example, DAC scan runs.
    """
    intt.reset_fpga(d)
    time.sleep(0.01)
    intt.reset_fphx(d)
    time.sleep(0.01)
        
    if fphxparam == None :
        #ld_fphxparam(d)
        intt.ld_fphxparam_high_daqs(d)
    else:
        send_fphxparam( d, fphxparam )
        
    time.sleep(0.01)
    intt.enable_ro(d)
    time.sleep(0.01)
    intt.latch_fpga(d)
    time.sleep(0.01)
    return None

def macro_calib(d, spacing =1199, n_pulses =10, n_ampl_steps =63, ampl_step =1, fphxparam=None):
    """!
    @brief A set of commands for calibration run
    @details Extended version of intt.macro_calib.
    Users can give FPHX commands.
    """
    macro_self(d, spacing, n_pulses, n_ampl_steps, ampl_step, fphxparam)
    intt.send_calib_param(d, spacing, n_pulses, n_ampl_steps, ampl_step)

    return None

def verify_latch(d):
    '''!
    @param d dam object.
    @retval -1 if error occurred, else the number of fibers latched, including zero.
    @details The difference from the original version is the return values. 
    '''
    d.reg.latch_sync_rst = 1
    d.reg.latch_sync_rst = 0
    if d.reg.latch_sync == 0:
        intt.latch_fpga(d)
        time.sleep(1)
        
        latch_arr = bin(d.reg.latch_sync)[2:].zfill(28)
        blatch_ok = [True if i=='1' else False for i in latch_arr][::-1]

        count_latch = latch_arr.count('1')
        
        R = '\033[31m'
        W = '\033[0m'
        G = '\033[32m'

        rtn_string = ""
        for c,i in enumerate(blatch_ok):
            color = G if i else R
            print("{2}Ladder Channel #{0:3}: {1}{3}".format(c//2,i,color,W))
            rtn_string += "{2}Ladder Channel #{0:3}: {1}{3}".format(c//2,i,color,W) + "\n"
        return count_latch, rtn_string
    else:
        return -1

def verify_latch_new(d): # H.Imai, I changed return
    d.reg.latch_sync_rst = 1
    d.reg.latch_sync_rst = 0
    if d.reg.latch_sync == 0:
        intt.latch_fpga(d)
        time.sleep(1)
        
        latch_arr = bin(d.reg.latch_sync)[2:].zfill(28)
        blatch_ok = [True if i=='1' else False for i in latch_arr][::-1]

        count_latch = latch_arr.count('1')

        return blatch_ok

    else:
        return -1

"""
[UNDER DEVELOPMENT]
Brings FLX board to a state that can interact with the ROC.

BCO and ST CLK generated, internal logic reset, transceivers properly initialized.

Inputs: dam object
Outputs: None
"""
def cold_start(d):
    intt.dma_datasel(d, test_mode=False)

    # Placeholder
    gtm_locked = True

    # Makes sure commands are sent to both ROCs
    d.reg.sc_target = 0x3
    # Initial clock distribution 
    if d.reg.si5345_pll.nLOL == 0:
        d.reg.si5345_pll.program = 1
        time.sleep(5)
        if d.reg.lmk_locked == 1:
                intt.init_clocks(d)
        else:
                raise Exception("LMK not properly locked. Cold Start aborted.")

    # Reset Slow Control and Data Channel GTH
    d.reg.rst_gth = 0xf
    time.sleep(0.1)
    d.reg.rst_gth = 0
    time.sleep(1)

    """
    [ CHECK IF GTH RESETS ARE DONE ]
        - Data Channels [27-0]
        - Slow Control tx and rx [29-28];
        - BCO clk gen tx and rx [31-30];
    """

    # 
    intt.reset_fpga(d)
    d.reg.latch_sync_rst = 1
    time.sleep(0.1)
    d.reg.latch_sync_rst = 0
    #verify_latch(d)
    intt.latch_fpga(d)
    time.sleep(0.1)
    # 
    d.reg.rst=3
    d.reg.rst=0
    time.sleep(0.5)
    intt.latch_fpga(d)
    time.sleep(0.5)
    intt.reset_fphx(d)
   
    bsync_ok = [True if i=='1' else False for i in bin(d.reg.sync_ok)[2:].zfill(2)][::-1]
    blatch_ok = [True if i=='1' else False for i in bin(d.reg.latch_sync)[2:].zfill(28)][::-1]

    R = '\033[31m'
    W = '\033[0m'
    G = '\033[32m'

    rtn_string = ""
    for c,i in enumerate(bsync_ok):
        color = G if i else R
        print("{2}Slow Control #{0:3}: {1}{3}".format(c,i,color,W))
        rtn_string += "{2}Slow Control #{0:3}: {1}{3}".format(c,i,color,W) + "\n"

    return rtn_string

def cold_start_new(d): # H.Imai
    intt.dma_datasel(d, test_mode=False)

    # Placeholder
    gtm_locked = True

    # Makes sure commands are sent to both ROCs
    d.reg.sc_target = 0x3
    # Initial clock distribution 
    if d.reg.si5345_pll.nLOL == 0:
        d.reg.si5345_pll.program = 1
        time.sleep(5)
    if d.reg.lmk_locked == 1:
        intt.init_clocks(d)
    else:
        raise Exception("LMK not properly locked. Cold Start aborted.")

    # Reset Slow Control and Data Channel GTH
    d.reg.rst_gth = 0xf
    time.sleep(0.1)
    d.reg.rst_gth = 0
    time.sleep(1)

    """
    [ CHECK IF GTH RESETS ARE DONE ]
        - Data Channels [27-0]
        - Slow Control tx and rx [29-28];
        - BCO clk gen tx and rx [31-30];
    """

    # 
    intt.reset_fpga(d)
    d.reg.latch_sync_rst = 1
    time.sleep(0.1)
    d.reg.latch_sync_rst = 0

    # 
    d.reg.rst=3
    d.reg.rst=0
    time.sleep(0.5)
    intt.latch_fpga(d)
    time.sleep(0.5)
    intt.reset_fphx(d)
   
    bsync_ok = [True if i=='1' else False for i in bin(d.reg.sync_ok)[2:].zfill(2)][::-1]
    blatch_ok = [True if i=='1' else False for i in bin(d.reg.latch_sync)[2:].zfill(28)][::-1]

    R = '\033[31m'
    W = '\033[0m'
    G = '\033[32m'

    rtn_string = ""
    for c,i in enumerate(bsync_ok):
        color = G if i else R
        print("{2}Slow Control #{0:3}: {1}{3}".format(c,i,color,W))
        rtn_string += "{2}Slow Control #{0:3}: {1}{3}".format(c,i,color,W) + "\n"

    return bsync_ok

""" #old version, Don't use it 
def check_register(d, var=101, wedge_set=range(32), chip_set=range(1,14), reg_set=range(4,12), roc=0):
	reset_fpga(d)
	time.sleep(1)

	reset_fphx(d)
	time.sleep(1)

	faulty = pd.DataFrame(columns=['wedge','chip','register'])
	for wedge in wedge_set:
		print('WEDGE: '+str(wedge))
		for chip in chip_set:
			print('CHIP: '+str(chip))
			for reg in reg_set:
				write_fphx(d, chip, reg, wedge, var)
				a=read_fphx(d, chip, reg, wedge, roc)
				if var not in a:
					new = pd.DataFrame({'wedge':[wedge], 'chip':[chip], 'register':[reg]})
					faulty = pd.concat([faulty, new], ignore_index=True)
	return faulty
"""

def check_register( d, chip_set=range(1, 14), reg_set=range(4, 12), ladder_map_path=None ) :
    """!
    @brief Check register for INTT DAQ servers
    @param d the dam object
    @param chip_set A list of FPHX chips to be tested. All chip It's same as the original's argument.
    @param reg_set A list of FPHX register to be tested. 
    @retval A list of good results (each element is for each ROC port) 
    @details
    Since the ladder condiguration was fixed for each INTT DAQ server, check register have to be done for each server easily.
    This function provides such great usability to you!
    """
    ladder_maps = ReadLadderMap( map_path=ladder_map_path )
    rtn = []
    for ladder_map in ladder_maps : 

        roc = 0 if int( ladder_map[0] ) < 7 else 1 
        wedge_set = intt.GetWedgeIDs( ladder_map[1] )
        good_results = intt.check_register( d, wedge_set=wedge_set, chip_set=chip_set, reg_set=reg_set, roc=roc )

        rtn.append( good_results )
        # end of for ladder_map in ladder_maps

    return rtn
    
# ###########################################################
# Functions not in intt.py
# ###########################################################

def apply_dac0( d=None, dac0_map_path=None, ladder_map_path=None, continue_until_success=False, trial_num=None, debug=False ) :
    """!
    @brief The DAC0 setting is applied
    @param d The dam module
    @param dac0_map_path If None(default), the default DAC0 map file (see ReadDac0Map) is used. Otherwise, give file will be used.
    @param ladder_map_path If None(default), the default ladder map file (see ReadLadderMap) is used. Otherwise, give file will be used.
    @param continue_until_success If it's true, it keeps writing DAC0 parameters untill the same value is read out from the FPHX register. It's disabled by default.
    @param trial_num If it's None (default), it keeps writing DAC0 values for infinit times untill success. If a number is given, it gives up at the given number of trials.
    @param debug If it's true, more information is shown on your terminal. False is the default.
    @retval Nothing for the moment.
    @details DAC0 configuration is taken from a map file and applied for each chip.
    A ladder map file is also needed to find ROC ports using Felix channel.
    
    I assumed that this function is executed in the INTT DAQ server. In this case, you can do just
    
    import dam
    import intt_ext
    d = dam.dam()
    d.reset()
    intt_ext.apply_dac( d )
    
    If you are not in the INTT DAQ server, it's better to give a DAC0 map and a ladder map thorugh arguments.
    """
    dac0_map = ReadDac0Map( map_path=dac0_map_path, debug=debug )
    ladder_map = ReadLadderMap( map_path=ladder_map_path )

    print( "DAC0 map:", dac0_map )
    print( "Ladder map:", ladder_map )
    
    for params in dac0_map :
        felix_ch = params[0]
        chip = params[1]
        dac0 = params[2]
        #roc_port = ladder_map[ felix_ch ]

        try : 
            roc_port = ladder_map[ felix_ch ][1] # FELIX channel should correspond to the index of the ladder if everything is as expected.
        except IndexError :
            print( "Failed finding FELIX ch", felix_ch, ", which was taken from the DAC0 map, in the ladder map...", file=sys.stderr )
            print( "It can happen, for example, if a ladder(s) are not in the ladder map. I recommend you to check your ladder map.", file=sys.stderr )
            print( "Let's try search the channel in the ladder map...", file=sys.stderr )

            list_found = [ (felix_ch in line ) for line in ladder_map ] # for example, [False, False, True, ... ]
            if True not in list_found :
                print( "Ah, FELIX ch", felix_ch, "is not in the ladder map", file=sys.stderr )
                print( "Check the map!!! Bye!", file=sys.stderr )
                sys.exit()
            else:
                print( "FELIX ch", felix_ch, "is found in the ladder map.", file=sys.stderr )
                index = list_found.index( True ) # take an index of the first appearance
                roc_port = ladder_map[ index ][1]

        if debug is True : 
            print( felix_ch, chip, dac0, roc_port )
            
        chip_0_13 = chip if chip < 14 else chip - 13
        wedges = intt.GetWedgeIDs( roc_port ) # a pair of the DF18 connectors
        wedge = wedges[0] if chip < 14 else wedges[1] # the one for the chip

        #################
        # ROC selection: d.reg.sc_target
        if d is not None : 
            if felix_ch < 7 : 
                d.reg.sc_target = 0x1
            else :
                d.reg.sc_target = 0x2

        #################
        # Sending a command to a FPHX chip
        dac0_address = 4
        intt.write_fphx( d, chip=chip_0_13, register=dac0_address, wedge=wedge, value=dac0 )

        # printed when debug=True
        command = intt.make_fphx_cmd( chip, dac0_address, 1, dac0 ) # chipId, regId, cmd, data
        if debug is True : 
            print( "FELIX CH", felix_ch, "ROC port", roc_port, "Wedges", wedges, "Chip", chip, "Wedge", wedge,  "DAC0", dac0, hex(command) )

        # repeat untill readout value is same as the written value. It's enabled when continue_until_success=True
        if continue_until_success is True :
            counter = 0

            roc = 0 if chip < 14 else 1 # ROC selection for read out
            rtn = intt.read_fphx( d, chip=chip_0_13, register=dac0_address, wedge=wedge, roc=roc ) # 2 values are returned due to a loss of a bit when a bus extender is used.

            # repeat until the read value is turned to same as the written value
            while command not in rtn :
                counter += 1

                # If it's limited trial mode and the number of attemps is more than the limit, break the loop
                if trial_num is not None :
                    if trial_num <= counter :
                        if debug is True :
                            print( trial_num, "times attemped, but written(", command, ") and read-out(", rtn, ") didn\'t match. I give up.", file=sys.stderr )
                            break
                    
                intt.write_fphx( d, chip=chip_0_13, register=dac0_address, wedge=wedge, value=dac0 )
                rtn = intt.read_fphx( d, chip=chip_0_13, register=dac0_address, wedge=wedge, roc=roc )
                if debug is True :
                    print( "\t", "#Repat:", counter,
                           "FELIX CH", felix_ch, "ROC port", roc_port, "Wedges", wedges,
                           "Chip", chip, "Wedge", wedge,  "DAC0", dac0,
                           hex(command),
                           "Readout:", hex(rtn[0]), hex(rtn[1]), rtn )
                # end of while command not in rtn
            # end of if continue_until_success
        # end of for params in dac0_map

    time.sleep( 0.01 ) # needed? I'm not sure...
    d.reg.sc_target = 0x3

""" moved to def GetServerRocMap(), It has to be hard coded but should not be writable by users.
server_ROC_map = {
    "intt0" : ['RC-0S','RC-1S'],
    "intt1" : ['RC-2S','RC-3S'],
    "intt2" : ['RC-4S','RC-5S'],
    "intt3" : ['RC-6S','RC-7S'],
    "intt4" : ['RC-0N','RC-1N'],
    "intt5" : ['RC-2N','RC-3N'],
    "intt6" : ['RC-4N','RC-5N'],
    "intt7" : ['RC-6N','RC-7N']
}
"""

""" Use intt.GetWedgeIDs to minize hard coding... if possible """
portid_to_wedge_map = {0:[8,9]   , 1 :[16,17], 2 :[24,25],  # note : column A
                       3:[10,11] , 4 :[18,19], 5 :[26,27],  # note : column B
                       6:[12,13] , 7 :[20,21], 8 :[28,29],  # note : column C
                       9:[14,15] , 10:[22,23], 11:[30,31]}  # note : column D


""" Hard coding is the only solution, but why don't you make a function or define in the function?
port_to_id_map = {"A1" : 0,
                  "A2" : 1,
                  "A3" : 2,
                
                  "B1" : 3,
                  "B2" : 4,
                  "B3" : 5,
                
                  "C1" : 6,
                  "C2" : 7,
                  "C3" : 8,
                
                  "D1" : 9,
                  "D2" : 10,
                  "D3" : 11}
"""

port_selection = {"talk_NO_port" : 0x0, "talk_port_0" : 0x1, "talk_port_1" : 0x2, "talk_All_port" : 0x3}

mask_ch_array = ["/home/phnxrc/INTT/sphenix_inttpy/run_scripts/mask_ch_south_v1.txt","/home/phnxrc/INTT/sphenix_inttpy/run_scripts/mask_ch_north_v1.txt"]
close_FC_map = "/home/phnxrc/INTT/sphenix_inttpy/run_scripts/close_FC_gate.txt"

def mask_ch_convert (d, port, chip, channel):
    """!
    @brief mask the channel
    """

    if (chip > 13) : 
        chip_conv = chip - 13
        wedge_index = 1
    else :
        chip_conv = chip
        wedge_index = 0
    
    wedge = portid_to_wedge_map[ port ][ wedge_index ]
    print("mask channel, chip_conv :",chip_conv, "wedge :", wedge, "chan :", channel)
    intt.mask_channel(d, chip_conv, wedge, channel=channel)


def file_mask_channel_txt (d, server, flx_port, txt_file_array = mask_ch_array, verbosity=0 ): # note : two files 
    """!
    @brief read the txt file and ask the mask_ch_convert to mask the channel, channel by channel
    """    
    txt_file_in = txt_file_array[0] if int(server[4]) < 4 else txt_file_array[1]

    #ROC_index = int(server_ROC_map[server][flx_port][3])
    ROC_index = int( GetServerRocMap()[server][flx_port][3] )

    with open( txt_file_in ) as file :
        for line in file :
            # for empty line, does it work?
            if line[0] == "" :
                continue

            # if the number of elements is smaller than 2 (at least Felix ch and ROC port should be), skip it
            if len( line ) < 2 :
                continue

            noisy_roc        = int(line.split()[0])
            noisy_port       = int(line.split()[2])
            noisy_chip       = int(line.split()[3])
            noisy_chan       = int(line.split()[4])
            noisy_felix_ch   = int(line.split()[1])
            noisy_ch_entry   = int(line.split()[5])

            if (noisy_roc == ROC_index):
                print("channel mask, ROC :", noisy_roc, "FC :", noisy_felix_ch, "port :", noisy_port, "chip :", noisy_chip, "chan :", noisy_chan, "entry :",noisy_ch_entry)
                mask_ch_convert(d, int(noisy_port), int(noisy_chip), int(noisy_chan))
                print("")

def disable_FC(server, txt_file_in = close_FC_map):
    """!
    @brief
    @param server
    @param txt_file_in
    @todo Name is confusing. Does it return a list of channels to be enabled or disabled?
    @details
    """
    Port_ch = [0,1,2,3,4,5,6, 7,8,9,10,11,12,13]

    with open( txt_file_in ) as file :
        for line in file :
            # for empty line, does it work?
            if line[0] == "" or line[0] == '#' :
                continue
            # if the number of elements is smaller than 2 (at least Felix ch and ROC port should be), skip it
            if len( line ) < 2 :
                continue
            text_array = line.split()

            if (text_array[0] == server) : # note : for example, intt0 == intt0
                for i in range (len(text_array[:])):
                    if (i != 0) :
                        Port_ch.remove(int(text_array[i]))
                        print("Server %s"%text_array[0],"FC-%s close"%text_array[i])

    return Port_ch

def LVDS_setting(d,current):
    time.sleep(0.3)

    if (current > 8 or current < 0):
        print("wrong LVDS current input, range : 1 to 8 mA")
        return
        
    command = intt.make_fphx_cmd( 21, 17, 1, (pow(2,current)-1) ) # note : register 17 : LVDS
    intt.send_fphx_cmd(d, command)
    print("Current %i"%current, "equals to %i"%(pow(2,current)-1))

def threshold_setting(d,threshold):
    time.sleep(0.3)
        
    command = intt.make_fphx_cmd( 21, 4, 1, threshold ) # note : register 4 : ADC0
    intt.send_fphx_cmd(d, command)
    print("Set threshold %i"%threshold)

def take_data(
        take_data=True,
        mode="calibration",
        fphx_parameters=None,
        customize_dac0=False,
        customize_dac=False,
        mask_channel=False,
        does_scp=False,
        mask_felix_ch=False,
        is_rcdaq=False,
        is_gtmcalib=True,
        output_name=None,
        output_dir=None,
        measurement_time=None,
        verbosity=0,
        help=False
) :
    """!
    @biref
    @param take_data A flag to do the DAQ loop or not. The default is True. For RCDAQ, give False.
    @param mode "calibration" (default) or "self" are available.
    @param fphx_parameters None (default) not to do anything, a path to a file (see send_fphxparam_from_file), or list of commands, which have to be integer, are allowed.
    @param customize_dac0 A flag to apply the individual DAC0 setting (True) or do nothing (False, default)
    @param customize_dac A flag to apply the individual DAC setting (True) or do nothing (False, default). NOT READY.
    @param mask_channel A flag to mask channels using Cheng-Wei's map (True) or do nothing (False, default).
    @param does_scp A flag to send the data file to inttdev@inttdaq (True) or do nothing (False, default).
    @param mask_felix_ch A flag to disable FELIX channels using Cheng-Wei's map (True) or do nothing (False, default).
    @param is_rcdaq A flag to switch RCDAQ mode (True).
    @param is_gtmcalib
    @param output_name
    @param measurement_time in sec
    @param verbosity An integer to show more information on your terminal. It's from 0 (default, minimum) to ?
    @param help
    @retval
    @details
    """

    if help is True :
        print( "I help you" )
        return None
    
    ###############################################################
    # Show parameters                                             #
    ###############################################################
    if verbosity > 1 :
        print( "\t", "- Take data?:", take_data )
        if take_data is True :
            print( "\t", "- Mode:", mode )
            if mode == "calibration" : 
                print( "\t", "- GTM calib mode?:", is_gtmcalib )
        print( "\t", "- RCDAQ mode?:", is_rcdaq )
        print( "\t", "- General FPHX parameters:", end="" )
        if fphx_parameters is None :
            print( "Not used" )
        elif type( fphx_parameters ) is str :
            print( "Written in", fphx_parameters )
        elif type( fphx_parameters ) is list :
            print( fphx_parameters )
        print( "\t", "- Indv. DAC0?:", customize_dac0 )
        print( "\t", "- Indv. DAC?:", customize_dac )
        print( "\t", "- Mask CH?  :", mask_channel )
        print( "\t", "- Mask FELIX CH?:", mask_felix_ch )
        print( "\t", "- Mask CH?  :", mask_channel )
        print( "\t", "- Output dir:", "Default" if output_dir is None else output_dir )
        print( "\t", "- Output name:", "Default" if output_name is None else output_name )

    RC_DAQ = is_rcdaq
    CALIB_STCLK = True
    GTM_CALIB = is_gtmcalib

    # initialize dam
    d = dam.dam()
    # reset dam
    d.reset()
    # disable all readout channels
    d.reg.ldd_enable=0
    #reset FELIX logic
    d.reg.rst=3
    d.reg.rst=0

    d.reg.sc_target = 0x3

    d.reg.n_collisions = 0

    # some variables to be used
    today = datetime.datetime.now()
    script_dir = os.path.realpath(os.path.dirname(__file__))
    hostname = socket.gethostname()
    
    #@todo check whether output_name has a suffix or not
    if output_name is None and output_dir is None :
        # If both output_name and output_dir are not given, just do like Raul
        output_dir = script_dir + "/data/"
        filename = output_dir + "calib_packv5_" + str(os.uname()[1]) + "_" + today.strftime("%m%d%y_%H%M")
    elif output_dir is None :
        # If output_dir is given...
        if output_name is not None : 
            filename = output_dir + output_name
        else : 
            filename = output_dir + "calib_packv5_" + str(os.uname()[1]) + "_" + today.strftime("%m%d%y_%H%M")
    else :
        # If output_dir is not given...
        output_dir = script_dit + "/data/"
        filename = output_dir + output_name

    if mode == "calibration" : 
        macro_calib(d)
    elif mode == "self" :
        macro_self(d)
        d.reg.n_collisions=130
        d.reg.open_time=15
        d.reg.roc_wildcard |= 1<<6
        # end of if mode == "calibration"
    else :
        print( "Invalid mode" )
        
    ###############################################################
    # General FPHX parameters                                     #
    ###############################################################
    print( "General FPHX parameter configuration:", end="" )
    
    if type( fphx_parameters ) is list :
        print( fphx_parameters )
        send_fphxparam( d , fphx_parameters, verbosity=verbosity )
    elif type( fphx_parameters ) is str :
        print( "written in", fphx_parameters, "are applied for all FPHX chips at first." )
        send_fphxparam_from_file( d, file_path=fphx_parameters, verbosity=verbosity )
    elif fphx_parameters is None :
        print( "Nothing given. The default is used." )
    else :
        print( "Given parameter is not valid:", type( fphx_parameters ), fphx_parameters )
        print( "Please give a list of parameters (integer) or a path to a file containig parameters" )

    ###############################################################
    # Custom DAC0                                                 #
    ###############################################################
    if customize_dac0 is True :
        print( "Customize DAC0 configuraton applied." )
        apply_dac0( d, debug=(verbosity>0), continue_until_success=False, trial_num=5 )

    ###############################################################
    # FELIX channel selection                                     #
    ###############################################################
    if mask_felix_ch is False :
        # If no mask provided, use all 13 channels
        enabled_channels = [ l for l in range(0, 14) ]
    else :
        # note : Felix channel, open the gate of the felix 0 to 13.
        enabled_channels = disable_FC( server=hostname ) # <-- I think it's the list of channels to be used (Genki).
        if verbosity > 0 :
            print( "Take Cheng-Wei\'s FELIX channel mask. Hostname:", hostname )

    if verbosity > 0 :
        print( "Enabled FELIX channels:", *enabled_channels )
        
    intt.enable_ro(d)

    ###############################################################
    # Channel selection                                           #
    ###############################################################
    # unmask all channels
    intt.mask_channel(d, 21, 0xff)

    if mask_channel is True : 
        # note : first unmask all the channels
        d.reg.sc_target = port_selection["talk_All_port"]
        intt.unmask_channel(d, 21, 0xff)

        # I think they are test codes
        #for channel in range(128):
        #    if channel <= 32:
        #        intt.mask_channel(d, 21, 0xff, channel=channel)

        # note : mask the noisy channel Felix port 0
        # note : 0xff wildcard
        d.reg.sc_target = port_selection["talk_port_0"]
        file_mask_channel_txt(d, hostname, 0, verbosity=verbosity )

        # note : mask the noisy channel Felix port 1
        # note : 0xff wildcard
        d.reg.sc_target = port_selection["talk_port_1"]
        file_mask_channel_txt(d, hostname, 1, verbosity=verbosity )
        # end of if mask_channel is True
    
    #d.reg.ldd_enable = 1<<12
    
    for ch in enabled_channels:
        intt.enable_channel(d, ch)

    if mode == "calibration" : 
        if GTM_CALIB == False:
            if CALIB_STCLK == True:
                time.sleep(1)
                d.reg.clks_ctl.calib_stclk = 1
                time.sleep(1)
                d.reg.clks_ctl.calib_stclk = 0
            else:
                intt.calib(d)

    if is_rcdaq is True :
        return 0
    
    #######################################################
    # only if local mode

    #intt.unmask_channel(d, 20, 0xff)
    start = time.time()
    end = time.time()

    a = []
    b = []
    PEDACO = 10000000

    if measurement_time is not None :
        TIME = measurement_time
    else:
        MINUTES = 4
        SECONDS = 0
        TIME = MINUTES*60 + SECONDS # [s]

    was_killed = False # a flag to know whether the while loop was stopped by an user or not
    if take_data is False :
        d.reg.ldd_enable = 0
        d.reg.n_collisions = 0
        d.reg.roc_wildcard &= ~(1<<6)
        return None
    
    print('Taking Data')
    try:
        while (end - start < TIME):

            # switch process depending on RC_DAQ mode or not
            if RC_DAQ == True:
                rd = []
            else:
                rd = d.read(PEDACO)

            # if the number of data in the buffer (rd) is 0, wait shortly, otherwise add them to another buffer (a) to write then in a npy file
            if len(rd) == 0:
                time.sleep(0.1)
            else:
                a.append(rd)

            # comment will be written
            if len(rd) < int(PEDACO/2) and len(rd) != 0:
                time.sleep(0.1)
                print(len(rd))

            # comment will be written
            if len(rd) == int(PEDACO/2):
                print("NO LIMITE ----------")

            # take the current time to judge whether this loop shold be finished or not
            end = time.time()
            # end of while (end - start < TIME )

    except KeyboardInterrupt:
        # If users stop calibration process by ctrl+c, set FELIX registers to the default values
        print( "Killed by hand, how dare you !!!" )
        d.reg.ldd_enable = 0
        d.reg.n_collisions = 0
        d.reg.roc_wildcard &= ~(1<<6)
        was_killed = True 
        #end of except KeyboardInterrupt


    # After all processes, set FELIX registers to the default values, which are for calibration measurement
    d.reg.ldd_enable = 0
    d.reg.n_collisions = 0
    d.reg.roc_wildcard &= ~(1<<6)

    a.append(d.read(PEDACO))

    # roc_wildcard(2) is used after data acquisition is done to fill up the fifos with
    # dummy data, since we can't flush its contents to server memory. This garantees
    # that all valid data is transferred to server memory, as well as (some) dummy data
    # as well.
    if len(a) != 0 and RC_DAQ is False and was_killed is False :
        np.save(filename,a)

        if verbosity > 1 : 
            print( '-'*100 )
            print("file done, file name : ",filename)
            print("run time : ", int( TIME/60 ), "min", TIME % 60 ,"sec")
            os.system("du -h %s.npy"%filename)
            print("")
            
            print("opened Felix channel :", enabled_channels )

        if does_scp is True : 
            command = "scp " + filename + ".npy " + "inttdev@inttdaq:~/data/IR_DAQ_server/"
            process = subprocess.Popen( command, shell=True )
            # wait until scp is done
            process.wait()
            print("scp done, job finish")
        
    d.reg.ldd_enable = 0
