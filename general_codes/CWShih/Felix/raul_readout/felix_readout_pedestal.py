import numpy as np
from dam import *
import time
import datetime
import intt

RC_DAQ = False

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

today = datetime.datetime.now()
filename = ("/home/phnxrc/INTT/data/"+"selftrig_packv5_"+today.strftime("%m%d%y_%H%M"))

intt.macro_calib(d)

d.reg.n_collisions=130
d.reg.roc_wildcard |= 1<<6 # start of run

print("file chekcing 1")
intt.enable_ro(d)



intt.unmask_channel(d, 21, 0xff)

for channel in range(128):
   if channel <= 80:
       intt.mask_channel(d, 21, 0xff, channel=channel)

for ch in [0,1,2,3,4,5,6,7,8,9,10,11,12,13]:
    intt.enable_channel(d, ch)

start = time.time()
end = time.time()

a = []
b = []
PEDACO = 10000000
MINUTES = 1
SECONDS = 0
TIME = MINUTES*60 + SECONDS # [s]

while (end - start < TIME):
    if RC_DAQ == True:
        rd = []
    else:
        rd = d.read(PEDACO)
    if len(rd) == 0:
        time.sleep(0.1)
    else:
        a.append(rd)

    if len(rd) < int(PEDACO/2) and len(rd) != 0:
        time.sleep(0.1)
        print(len(rd))
    if len(rd) == int(PEDACO/2):
        print("NO LIMITE ----------")
    end = time.time()

d.reg.ldd_enable = 0

d.reg.n_collisions = 0
d.reg.roc_wildcard &= ~(1<<6)

a.append(d.read(PEDACO))

if len(a) != 0:
	np.save(filename,a)
	print("file done,file name : ",filename)

