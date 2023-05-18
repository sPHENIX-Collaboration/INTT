import numpy as np
from dam import *
import time
import datetime
import intt
#import intt_ext
# import ROOT as root

import sys
import os
import subprocess

if len(sys.argv) != 8 :
    print( "It requires 7 arguments!" )
    print( "Arg0:", "ROC number for FELIX port 0 [0/2/4/6]" )
    print( "Arg1:", "ROC number for FELIX port 1 [1/3/5/7]" )
    print( "Arg2:", "Direction of the first ROC [south/north]" )
    print( "Arg3:", "Direction of the second ROC [south/north]" )
    print( "Arg4:", "note : min" )
    print( "Arg5:", "note : sec" )
    print( "Arg6:", "note : threshold (DAC0)" )
    print( "\n", "For example:", "python3", sys.argv[0], "2 3 north north 3 20 15", end="  -> " )
    print( "means selftrigger with with DAC0=15 using RC-2N and RC-3N" )
    sys.exit()

# Access command line arguments
arg1 = sys.argv[1] # ROC number for FELIX port 0 [0/2/4/6]
arg2 = sys.argv[2] # ROC number for FELIX port 1 [1/3/5/7]
arg3 = sys.argv[3] # Direction of the first ROC [south/north]
arg4 = sys.argv[4] # Direction of the second ROC [south/north]

arg5 = int(sys.argv[5]) # note : min
arg6 = int(sys.argv[6]) # note : sec

arg7 = sys.argv[7] # note : threshold (DAC0)


used_roc  = [int(arg1),int(arg2)]
used_side_0 = arg3 # "north"
used_side_1 = arg4 # "north"

print("ROC :", used_roc, "side_1 :", used_side_0, "side_2", used_side_1)
print("run time : ", arg5,"min",arg6,"sec")

def mask_ch_convert (port, chip, channel):
    
    if (chip > 13) : 
        chip_conv = chip - 13
        wedge_index = 1
    else :
        chip_conv = chip
        wedge_index = 0
    
    print("mask channel, chip_conv :",chip_conv, "wedge :", portid_to_wedge_map[port][wedge_index], "chan :", channel)
    intt.mask_channel(d, chip_conv, portid_to_wedge_map[port][wedge_index], channel=channel)

def file_mask_channel (roc_id_in, root_file_in):
    f1 = root.TFile("%s" %(root_file_in))
    noisy_ch_tree = f1.Get("noisy_ch")
    N_noisy_ch = noisy_ch_tree.GetEntries()
    print("N_noisy_ch :", N_noisy_ch)

    for i in range (N_noisy_ch) :
        noisy_ch_tree.GetEntry(i)
        
        noisy_roc        = noisy_ch_tree.roc_id
        noisy_port       = noisy_ch_tree.port_id
        noisy_chip       = noisy_ch_tree.chip_id
        noisy_chan       = noisy_ch_tree.chan_id
        noisy_felix_ch   = noisy_ch_tree.felix_ch
        noisy_ch_entry   = noisy_ch_tree.ch_entry
        
        if (noisy_roc == roc_id_in):
            print("channel mask, ROC :", noisy_roc, "FC :", noisy_felix_ch, "port :", noisy_port, "chip :", noisy_chip, "chan :", noisy_chan, "entry :",noisy_ch_entry)
            mask_ch_convert(noisy_port, noisy_chip, noisy_chan)
            print("")


def file_mask_channel_txt (roc_id_in, txt_file_in):

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

            if (noisy_roc == roc_id_in and noisy_ch_entry > 700):
                print("channel mask, ROC :", noisy_roc, "FC :", noisy_felix_ch, "port :", noisy_port, "chip :", noisy_chip, "chan :", noisy_chan, "entry :",noisy_ch_entry)
                mask_ch_convert(int(noisy_port), int(noisy_chip), int(noisy_chan))
                print("")
            
    



# mask_ch_map = {"south" : "/home/inttdev/data/IR/commissioning/20230424_calib_pedestal/data_analyzer/Bad_ch_south.root", "north" : "/home/inttdev/data/IR/commissioning/20230424_calib_pedestal/data_analyzer/Bad_ch_north.root"}
# mask_ch_map = {"south" : "/home/phnxrc/INTT/cwshih/raul_readout/Bad_ch_south_noisy_ch.txt", "north" : "/home/phnxrc/INTT/cwshih/raul_readout/Bad_ch_north_noisy_ch.txt"} 
mask_ch_map = {"south" : "/home/phnxrc/INTT/cwshih/raul_readout/ch_property_south.txt", "north" : "/home/phnxrc/INTT/cwshih/raul_readout/ch_property_north.txt"} 

Port_0_ch = [0,1,2,3,4,5,6]
Port_1_ch = [7,8,9,10,11,12,13]

if (used_roc[0] == 2 and used_side_0 == "south" and used_roc[1] == 3 and used_side_1 == "south"):
    print("close Felix ch : ", 2)
    Port_0_ch.remove(2)

if (used_roc[0] == 4 and used_side_0 == "south" and used_roc[1] == 5 and used_side_1 == "south"):
    print("close Felix ch : ", 1)
    Port_0_ch.remove(1)

if (used_roc[0] == 4 and used_side_0 == "north" and used_roc[1] == 5 and used_side_1 == "north"):
    print("close Felix ch : ", 1, 12)
    Port_0_ch.remove(1)
    Port_1_ch.remove(12)

# Port_0_ch = [0]
# Port_1_ch = [7]

portid_to_wedge_map = {0:[8,9]   , 1 :[16,17], 2 :[24,25],  # note : column A
                       3:[10,11] , 4 :[18,19], 5 :[26,27],  # note : column B
                       6:[12,13] , 7 :[20,21], 8 :[28,29],  # note : column C
                       9:[14,15] , 10:[22,23], 11:[30,31]}  # note : column D

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

port_selection = {"talk_NO_port" : 0x0, "talk_port_0" : 0x1, "talk_port_1" : 0x2, "talk_All_port" : 0x3}

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
d.reg.sc_target = port_selection["talk_All_port"] #note : felix port selection

intt.cold_start(d)
intt.verify_latch(d)

today = datetime.datetime.now()
filename = ("/home/phnxrc/INTT/data/"+"selftrig_packv5_"+today.strftime("%m%d%y_%H%M")+"_RC-%s%s"%(used_roc[0],used_side_0)+"_RC-%s%s"%(used_roc[1],used_side_1)+"_DAC0_%s_"%arg7+"runtime_%smin_%ssec"%(arg5,arg6))

print("file name : ", filename)
time.sleep(2)

intt.macro_calib(d)
# intt_ext.apply_dac0( d )

# note : sort like the comment out the BCO filter
d.reg.n_collisions=130
d.reg.roc_wildcard |= 1<<6 # start of run

d.reg.sc_target = port_selection["talk_All_port"]
intt.enable_ro(d)

# note : first unmask all the channels
d.reg.sc_target = port_selection["talk_All_port"]
intt.unmask_channel(d, 21, 0xff)

# note : mask the noisy channel Felix port 0
# note : 0xff wildcard
d.reg.sc_target = port_selection["talk_port_0"]
file_mask_channel_txt(used_roc[0],mask_ch_map[used_side_0])

if (used_roc[0] == 4 and used_side_0 == "south"):
    print(" -- MANUAL MASK channel  ROC1, RC-",used_roc[0],used_side_0)
    mask_ch_convert(int(port_to_id_map["B1"]), 8, 0)
    mask_ch_convert(int(port_to_id_map["B1"]), 3, 21)
  
if (used_roc[0] == 6 and used_side_0 == "south"):
    print(" -- MANUAL MASK channel  ROC1, RC-",used_roc[0],used_side_0)
    intt.mask_channel(d, 2, portid_to_wedge_map[port_to_id_map["C1"]][0],channel=24) # note : RC-6S, FC2, chip 2, chan_id 24

if (used_roc[0] == 4 and used_side_0 == "north"):
    print(" -- MANUAL MASK channel  ROC1, RC-",used_roc[0],used_side_0)
    mask_ch_convert(int(port_to_id_map["C3"]), 11, 0)

if (used_roc[0] == 2 and used_side_0 == "south"):
    print(" -- MANUAL MASK channel  ROC1, RC-",used_roc[0],used_side_0)
    mask_ch_convert(int(port_to_id_map["A2"]), 15, 0)
    mask_ch_convert(int(port_to_id_map["C2"]), 7, 23)
    # intt.mask_channel(d, 2, portid_to_wedge_map[port_to_id_map["A2"]][1]) # note : RC-2S, FC2 (A2), chip 15 all channel

    # intt.mask_channel(d, 1, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 2, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 3, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 4, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 5, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 6, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 7, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 8, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 9, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 10, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 11, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 12, portid_to_wedge_map[port_to_id_map["A2"]][1])
    # intt.mask_channel(d, 13, portid_to_wedge_map[port_to_id_map["A2"]][1])

    # intt.mask_channel(d, 1, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 2, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 3, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 4, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 5, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 6, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 7, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 8, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 9, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 10, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 11, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 12, portid_to_wedge_map[port_to_id_map["A2"]][0])
    # intt.mask_channel(d, 13, portid_to_wedge_map[port_to_id_map["A2"]][0])


# for channel in range(128):
#    if channel <= 32:
#        intt.mask_channel(d, 6, 0xff, channel=channel)

print("")

# note : mask the noisy channel Felix port 1
# note : 0xff wildcard
d.reg.sc_target = port_selection["talk_port_1"]
file_mask_channel_txt(used_roc[1],mask_ch_map[used_side_1])

if (used_roc[1] == 5 and used_side_1 == "south"):
    print(" -- MANUAL MASK channel  ROC2, RC-",used_roc[1],used_side_1)
    mask_ch_convert(int(port_to_id_map["B2"]), 25, 20)

if (used_roc[1] == 7 and used_side_1 == "south"):
    print(" -- MANUAL MASK channel  ROC2, RC-",used_roc[1],used_side_1)
    intt.mask_channel(d, 1, portid_to_wedge_map[port_to_id_map["D2"]][0]) # note : RC-7S, FC13, non-bias
    intt.mask_channel(d, 2, portid_to_wedge_map[port_to_id_map["D2"]][0])
    intt.mask_channel(d, 3, portid_to_wedge_map[port_to_id_map["D2"]][0])
    intt.mask_channel(d, 4, portid_to_wedge_map[port_to_id_map["D2"]][0])
    intt.mask_channel(d, 5, portid_to_wedge_map[port_to_id_map["D2"]][0])
    intt.mask_channel(d, 1, portid_to_wedge_map[port_to_id_map["D2"]][1])
    intt.mask_channel(d, 2, portid_to_wedge_map[port_to_id_map["D2"]][1])
    intt.mask_channel(d, 3, portid_to_wedge_map[port_to_id_map["D2"]][1])
    intt.mask_channel(d, 4, portid_to_wedge_map[port_to_id_map["D2"]][1])
    intt.mask_channel(d, 5, portid_to_wedge_map[port_to_id_map["D2"]][1]) 

if (used_roc[1] == 3 and used_side_1 == "north"):
    print(" -- MANUAL MASK channel  ROC2, RC-",used_roc[1],used_side_1)
    intt.mask_channel(d, 6, portid_to_wedge_map[port_to_id_map["B1"]][0]) # note : RC-3N, FC1, non-bias
    intt.mask_channel(d, 7, portid_to_wedge_map[port_to_id_map["B1"]][0])
    intt.mask_channel(d, 8, portid_to_wedge_map[port_to_id_map["B1"]][0])
    intt.mask_channel(d, 9, portid_to_wedge_map[port_to_id_map["B1"]][0])
    intt.mask_channel(d, 10, portid_to_wedge_map[port_to_id_map["B1"]][0])
    intt.mask_channel(d, 11, portid_to_wedge_map[port_to_id_map["B1"]][0])
    intt.mask_channel(d, 12, portid_to_wedge_map[port_to_id_map["B1"]][0])
    intt.mask_channel(d, 13, portid_to_wedge_map[port_to_id_map["B1"]][0])
    
    intt.mask_channel(d, 6, portid_to_wedge_map[port_to_id_map["B1"]][1])
    intt.mask_channel(d, 7, portid_to_wedge_map[port_to_id_map["B1"]][1])
    intt.mask_channel(d, 8, portid_to_wedge_map[port_to_id_map["B1"]][1])
    intt.mask_channel(d, 9, portid_to_wedge_map[port_to_id_map["B1"]][1])
    intt.mask_channel(d, 10, portid_to_wedge_map[port_to_id_map["B1"]][1])
    intt.mask_channel(d, 11, portid_to_wedge_map[port_to_id_map["B1"]][1])
    intt.mask_channel(d, 12, portid_to_wedge_map[port_to_id_map["B1"]][1])
    intt.mask_channel(d, 13, portid_to_wedge_map[port_to_id_map["B1"]][1]) 

if (used_roc[1] == 5 and used_side_0 == "north"):
    print(" -- MANUAL MASK channel  ROC2, RC-",used_roc[1],used_side_1)
    mask_ch_convert(int(port_to_id_map["D2"]), 5, 83)
    mask_ch_convert(int(port_to_id_map["B3"]), 19, 0)

# for channel in range(128):
#    if channel <= 32:
#        intt.mask_channel(d, 21, 0xff, channel=channel)


# note : Felix channel, open the gate of the felix 0 to 13.
for ch in (Port_0_ch + Port_1_ch):
    intt.enable_channel(d, ch)

print("opened Felix channel :",Port_0_ch,Port_1_ch)
time.sleep(2)

start = time.time()
end = time.time()

a = []
b = []
PEDACO = 10000000
MINUTES = arg5
SECONDS = arg6
TIME = MINUTES*60 + SECONDS # [s]

limit_count = 0
no_limit_count = 0

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

d.reg.ldd_enable = 0

d.reg.n_collisions = 0
d.reg.roc_wildcard &= ~(1<<6)

a.append(d.read(PEDACO))

if len(a) != 0:
	np.save(filename,a)
	print("------------------------------------------------------------------------------------")    
	print("file done,file name : ",filename, "run time : ", arg5,"min",arg6,"sec")
	print("file size : ")
	os.system("du -h %s.npy"%filename)
	print("opened Felix channel :",Port_0_ch,Port_1_ch)

	command = "scp " + filename + ".npy " + "inttdev@inttdaq:~/data/IR_DAQ_server/"
	process = subprocess.Popen( command, shell=True )
        
