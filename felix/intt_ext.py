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
import os
import re # for str comparison using regular expression
import socket # to get hostname
import sys
import time
import sys

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
    
def ReadServerRocMap() :
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
    
############################################################
# Overwritting functions                                   #
############################################################

def send_fphxparam( d, cheat ) :
    """!
    @brief Given FPHX commands are sent
    @param d the dam object
    @param cheat A list of FPHX commands
    """    
    for item in cheat:
        intt.send_fphx_cmd(d, item)
        time.sleep(.001)        

def macro_pedestal(d, spacing =1199, n_pulses =10, n_ampl_steps =63, ampl_step =1, fphxparam=None):
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
        intt_ext.send_fphxparam( d, fphxparam )
        
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
    macro_pedestal(d, spacing, n_pulses, n_ampl_steps, ampl_step, fphxparam)
    intt_ext.send_calib_param(d, spacing, n_pulses, n_ampl_steps, ampl_step)

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
        latch_fpga(d)
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

    print( dac0_map )
    print( ladder_map )
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

# mask the channel
def mask_ch_convert (d, port, chip, channel):
    
    if (chip > 13) : 
        chip_conv = chip - 13
        wedge_index = 1
    else :
        chip_conv = chip
        wedge_index = 0
    
    wedge = GetPortID( port )[ wedge_index ]
    print("mask channel, chip_conv :",chip_conv, "wedge :", wedge, "chan :", channel)
    intt.mask_channel(d, chip_conv, wedge, channel=channel)

# read the txt file and ask the mask_ch_convert to mask the channel, channel by channel
def file_mask_channel_txt (d, server, flx_port, txt_file_array = mask_ch_array): # note : two files 

    txt_file_in = txt_file_array[0] if int(server[4]) < 4 else txt_file_array[1]

    #ROC_index = int(server_ROC_map[server][flx_port][3])
    ROC_index = int( GetServerRocMap[server][flx_port][3] )

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
