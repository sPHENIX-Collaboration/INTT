import numpy as np
from dam import *
import time
import datetime
import intt
import sys

import os

# initialize dam
d = dam()
# reset dam
d.reset()
# disable all readout channels
d.reg.ldd_enable=0
#reset FELIX logic

d.reg.sc_target = 0x3


R = '\033[31m'
W = '\033[0m'
G = '\033[32m'

bsync_ok = [True if i=='1' else False for i in bin(d.reg.sync_ok)[2:].zfill(2)][::-1]
for c,i in enumerate(bsync_ok):
    color = G if i else R
    print("{2}Slow Control #{0:3}: {1}{3}".format(c,i,color,W))

intt.verify_latch(d)
