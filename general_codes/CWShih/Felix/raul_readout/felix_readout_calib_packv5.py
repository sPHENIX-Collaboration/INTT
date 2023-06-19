import numpy as np
from dam import *
import time
import datetime
import intt
import intt_ext
import sys

import os
import subprocess
import socket

RC_DAQ = False
CALIB_STCLK = True 
GTM_CALIB = False

server_name = socket.gethostname()

if len(sys.argv) < 2 :
    print( "It requires at least 1 argument!" )
    print( "Arg0:", "note : run_mode, 0 -> simple calib, 1 -> LVDS Scan, 2 -> ChangeDAC0" )
    sys.exit()

run_mode = int(sys.argv[1]) # note : LVDS current scan

if (len(sys.argv) == 3) :
    config = sys.argv[2] # note : LVDS current, for example



# initialize dam
d = dam()
# reset dam
d.reset()
# disable all readout channels
d.reg.ldd_enable=0
#reset FELIX logic
d.reg.rst=3
d.reg.rst=0

d.reg.sc_target = 0x3

d.reg.n_collisions = 0

print("--------------------- cold start ---------------------")
intt.cold_start(d)
print("--------------------- verify latch ---------------------")
intt.verify_latch(d)

today = datetime.datetime.now()
# script_dir = os.path.realpath(os.path.dirname(__file__))
# print("test path : ",script_dir)

Open_FC = intt_ext.disable_FC(server = server_name)

filename = "/home/phnxrc/INTT/data/"+"calib_packv5_"+today.strftime("%m%d%y_%H%M_")+server_name+"_runpurpose"

if (len(Open_FC) == 14):
    print("All Felix channel open")
    filename += "_all_FC"
else : 
    print("Opened felix channel :",Open_FC)
    filename += "_FC"
    for FC in Open_FC:
        filename += ("_%i"%FC)
        
intt.macro_calib(d)

if (run_mode == 0):
    print("run mode : basic calibration")
    filename = filename.replace("_runpurpose","_Calib")

elif (run_mode == 1):
    intt_ext.LVDS_setting(d,int(config))
    print("run mode LVDS current Scan, current :",config, "mA")
    filename = filename.replace("_runpurpose","_LVDS_Scan_%s"%config)

elif (run_mode == 2):
    intt_ext.apply_dac0( d )
    print("run mode DAC0 Change")
    filename = filename.replace("_runpurpose","_DAC0_Change")

print("target filename : %s.npy"%filename)
print("Opened FC : ",Open_FC)
time.sleep(2)

intt.enable_ro(d)
intt.mask_channel(d, 21, 0xff)

#d.reg.ldd_enable = 1<<12
for ch in Open_FC:
    intt.enable_channel(d, ch)
if GTM_CALIB == False:
    if CALIB_STCLK == True:
        time.sleep(1)
        d.reg.clks_ctl.calib_stclk = 1
        time.sleep(1)
        d.reg.clks_ctl.calib_stclk = 0
    else:
        intt.calib(d)

print('--------- Taking Data ---------')
time.sleep(2)
#intt.unmask_channel(d, 20, 0xff)


start = time.time()
end = time.time()

a = []
b = []
PEDACO = 10000000
MINUTES = 4
SECONDS = 0
TIME = MINUTES*60 + SECONDS # [s]

no_limit_count = 0 
limit_count = 0
try:
    while (end - start < TIME):
        if RC_DAQ == True:
            rd = []
        else:
            rd = d.read(PEDACO)

        if len(rd) == int(PEDACO/2):
            print("NO LIMITE ----------")
            no_limit_count += 1

            if (no_limit_count < 50) :
                continue

        if len(rd) == 0:
            time.sleep(0.1)
        else:
            a.append(rd)

        if len(rd) < int(PEDACO/2) and len(rd) != 0:
            time.sleep(0.1)
            print(len(rd))
        if len(rd) == int(PEDACO/2):
            print("NO LIMITE ----------")

        if len(rd) > 1e5:
            limit_count += 1

        if (limit_count == 3):
            print("hard break, run time :",end - start," seconds")
            break


        end = time.time()
finally:
    d.reg.ldd_enable = 0
    print("Did you just kill me ? how dare you !")
    print("My dad is ChatGPT-4 !")
    
d.reg.ldd_enable = 0

# roc_wildcard(2) is used after data acquisition is done to fill up the fifos with
# dummy data, since we can't flush its contents to server memory. This garantees
# that all valid data is transferred to server memory, as well as (some) dummy data
# as well.

if (limit_count == 3) :
    filename += "_HardBreak"

if len(a) != 0:
    np.save(filename,a)
    print("------------------------------------------------------------------------------------")    
    os.system("du -h %s.npy"%filename)
    print("")
    print("file done, file name %s.npy"%filename)

    command = "scp " + filename + ".npy " + "inttdev@inttdaq:~/data/IR_DAQ_server/"
    process = subprocess.Popen( command, shell=True )
