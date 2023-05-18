import numpy as np
from dam import *
import time
import datetime
import intt
import intt_ext
# import ROOT as root

import sys
import os
import subprocess
import socket



if len(sys.argv) < 4 :
    print( "It requires 3 arguments!" )
    print( "Arg0:", "note : min" )
    print( "Arg1:", "note : sec" )
    print( "Arg2:", "note : run_mode, 0 -> DAC0 scan, 1 -> DAC Scan, 2 -> ChangeDAC0, 3 -> LVDS Scan" )
    print( "Arg3:", "note : config variable (DAC0 value or DAC-Scan config ID, LVDS current)" )
    print( "\n", "For example:", "python3", sys.argv[0], "3 20 0 15", end="  -> " )
    print( "means selftrigger with with DAC0 = 15 and the run time is 3 mins and 20 seconds" )
    sys.exit()

server_name = socket.gethostname()

run_min  = int(sys.argv[1]) # note : min
run_sec  = int(sys.argv[2]) # note : sec
run_mode = int(sys.argv[3]) # note : run mode  
config   = False
if (len(sys.argv) == 5) :
    config = sys.argv[4] # note : DAC0 value or DAC Scan config id or LVDS current

print("You are using server %s."%server_name, "It has ",intt_ext.server_ROC_map[server_name])
mode_map = {
    0 : "DAC0 mode, threshold : %s"%config, # note : DAC0 Scan, you have to set the threshold anually in intt.py
    1 : "DAC Scan mode, config : %s"%config,
    2 : "Change DAC0 mode",
    3 : "LVDS Scan mode, current : %s"%config
}
print(mode_map[run_mode])

print("")
print("run time : ", run_min,"min",run_sec,"sec")

time.sleep(1)

RC_DAQ = False

# note : initialize dam
d = dam()
# note : reset dam
d.reset()

# note : it's bit thing, so 0 equals all 00000000 (no matter how many digits you have)
# note : it's like a reset, first close all the gates.
d.reg.ldd_enable=0

# note : reset FELIX logic
d.reg.rst=3
d.reg.rst=0

# note : disable all readout channels, 0 = 00->both ROCs off, 1 = 01-> Felix port 0 ON, 2 = 10->Felix port 1 ON, 3 = 11->both ROCs ON
d.reg.sc_target = intt_ext.port_selection["talk_All_port"] #note : felix port selection

print("--------------------- cold start ---------------------")
intt.cold_start(d)
print("--------------------- verify latch ---------------------")
intt.verify_latch(d)

today = datetime.datetime.now()

filename = ("/home/phnxrc/INTT/data/"+"selftrig_packv5_"+today.strftime("%m%d%y_%H%M_")+server_name+"_runpurpose_"+"runtime_%smin_%ssec"%(run_min,run_sec))

if (run_mode == 0): # note : DAC0 Scan
    filename = filename.replace("_runpurpose_","_DAC0_%s_"%config)
    intt.macro_calib(d)
    intt_ext.threshold_setting(d,int(config)) 
    
elif (run_mode == 1): # note : DAC Scan
    filename = filename.replace("_runpurpose_","_DACScan_%s_"%config)
    intt.macro_calib_DACScan(int(config),d)

elif (run_mode == 2):
    filename = filename.replace("_runpurpose_","_DAC0_change_")
    intt.macro_calib(d)
    intt_ext.apply_dac0( d , debug=True )

elif (run_mode == 3):
    filename = filename.replace("_runpurpose_","_LVDS_Scan_%s_"%config)
    intt.macro_calib(d)
    intt_ext.LVDS_setting(d,int(config))


print("file name : %s.npy"%filename)
time.sleep(2)
    

# note : sort like the comment out the BCO filter
d.reg.n_collisions=130
d.reg.roc_wildcard |= 1<<6 # start of run

d.reg.sc_target = intt_ext.port_selection["talk_All_port"]
intt.enable_ro(d)

# note : first unmask all the channels
d.reg.sc_target = intt_ext.port_selection["talk_All_port"]
intt.unmask_channel(d, 21, 0xff)

for channel in range(128):
   if channel <= 32:
       intt.mask_channel(d, 21, 0xff, channel=channel)

# note : mask the noisy channel Felix port 0
# note : 0xff wildcard
d.reg.sc_target = intt_ext.port_selection["talk_port_0"]
intt_ext.file_mask_channel_txt(d, server_name, 0)

print("")

# note : mask the noisy channel Felix port 1
# note : 0xff wildcard
d.reg.sc_target = intt_ext.port_selection["talk_port_1"]
intt_ext.file_mask_channel_txt(d, server_name, 1)

# note : Felix channel, open the gate of the felix 0 to 13.
Open_FC = intt_ext.disable_FC(server = server_name)
for ch in (Open_FC):
    intt.enable_channel(d, ch)

print("opened Felix channel :",Open_FC)
time.sleep(2)

start = time.time()
end = time.time()

a = []
b = []
PEDACO = 10000000
MINUTES = run_min
SECONDS = run_sec
TIME = MINUTES*60 + SECONDS # [s]

limit_count = 0
no_limit_count = 0

try : 
    while (end - start < TIME):
        if RC_DAQ == True:
            rd = []
        else:
            rd = d.read(PEDACO)

        if len(rd) == int(PEDACO/2):
            print("NO LIMITE ----------")
            no_limit_count += 1

            if (no_limit_count < 20) :
                continue
        

        if len(rd) == 0:
            time.sleep(0.1)
        else:
            a.append(rd)

        if len(rd) < int(PEDACO/2) and len(rd) != 0:
            time.sleep(0.1)
            print("time : %.1f"%(end - start),len(rd))
        

        if len(rd) > 800000:
            limit_count += 1

        if (limit_count == 3):
            print("hard break, run time :",end - start," seconds")
            break

        end = time.time()

finally: 
    d.reg.ldd_enable = 0

    d.reg.n_collisions = 0
    d.reg.roc_wildcard &= ~(1<<6)
    print("Killed by hand, how dare you !!!")

d.reg.ldd_enable = 0
d.reg.n_collisions = 0
d.reg.roc_wildcard &= ~(1<<6)

a.append(d.read(PEDACO))

if len(a) != 0:
    np.save(filename,a)
    print("------------------------------------------------------------------------------------")    
    print("file done, file name : ",filename)
    print("run time : ", run_min,"min",run_sec,"sec")
    os.system("du -h %s.npy"%filename)
    print("")
    print("opened Felix channel :",Open_FC)

    command = "scp " + filename + ".npy " + "inttdev@inttdaq:~/data/IR_DAQ_server/"
    process = subprocess.Popen( command, shell=True )
    print("scp done, job finish")
        
