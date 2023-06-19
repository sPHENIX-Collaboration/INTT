import time
# import pandas as pd


RESET_FPHX = 0x0B
LATCH_FPGA = 0X09
CALIB = 0X05
FPHX_COMMAND = 0X03

def load_datansize(d):
	d.reg.roc_wildcard |= 1<<1
	time.sleep(0.001)
	d.reg.roc_wildcard &= ~(1<<1)

def start_sm(d):
	d.reg.roc_wildcard |= 1<<0
	time.sleep(0.001)
	d.reg.roc_wildcard &= ~(1<<0)

def dma_datasel(d, test_mode=False):
    if test_mode == True:
        d.reg.roc_wildcard |= 1<<4
    else:
        d.reg.roc_wildcard &= ~(1<<4)

def dma_datagen_trig(d):
    d.reg.roc_wildcard |= 1<<5
    d.reg.roc_wildcard &= ~(1<<5)

def reset_fpga(d):
	write_scparam(d, 0x04, 0x00, 0xff, 0xff)
	load_datansize(d)
	start_sm(d)

def reset_fphx(d):
	write_scparam(d, 0x0b, 0xff, 0xff, 0xff)
	load_datansize(d)
	start_sm(d)

def latch_fpga(d):
	write_scparam(d, 0x09, 0xff, 0xff, 0xff)
	load_datansize(d)
	start_sm(d)

def calib(d):
	write_scparam(d, 0x05, 0x00, 0x03, 0xff)
	load_datansize(d)
	time.sleep(1)
	start_sm(d)

def enable_ro(d):
	# send_fphx_cmd(d, 0xCF511070)
	send_fphx_cmd(d, 0xCF511050)
	send_fphx_cmd(d, 0xCF511070)


def enable_channel(d, chan):
	rd = d.reg.ldd_enable
	d.reg.ldd_enable = rd | (1<<chan)

def disable_channel(d, chan):
	rd = d.reg.ldd_enable
	d.reg.ldd_enable = rd & ~(1<<chan)

def macro_calib(d, spacing =1199, n_pulses =10, n_ampl_steps =63, ampl_step =1):
	reset_fpga(d)
	time.sleep(0.01)
	reset_fphx(d)
	time.sleep(0.01)
	ld_fphxparam(d) # note : standard calibration config
	# ld_fphxparam_high_daqs(d) # note : for the pedestal
	#ld_fphxparam_dac_scan(3, d)
        
	# ld_fphxparam_intt_selftrigger(d)
	time.sleep(0.01)
	enable_ro(d)
	time.sleep(0.01)
	latch_fpga(d)
	time.sleep(0.01)

	#send_calib_param(d, spacing, n_pulses, n_ampl_steps, ampl_step)

	return


def macro_calib_DACScan(run_mode, d, spacing =1199, n_pulses =10, n_ampl_steps =63, ampl_step =1):
	reset_fpga(d)
	time.sleep(0.01)
	reset_fphx(d)
	time.sleep(0.01)
	# ld_fphxparam(d)
	# ld_fphxparam_high_daqs(d)
	ld_fphxparam_dac_scan(run_mode, d)
        
	# ld_fphxparam_intt_selftrigger(d)
	time.sleep(0.01)
	enable_ro(d)
	time.sleep(0.01)
	latch_fpga(d)
	time.sleep(0.01)

	#send_calib_param(d, spacing, n_pulses, n_ampl_steps, ampl_step)

	return

def send_calib_param(d, spacing, n_pulses, n_ampl_steps, ampl_step):

	word = 0x0
	word |= (0xffff & spacing) << (48-16)
	word |= (0xff & n_pulses) << (48-16-8)
	word |= (0xffff & n_ampl_steps) << (48-16-8-16)
	word |= (0xff & ampl_step)

	comm_parsed = [ ((word >> (48-8-i)) & 0x000000000FF) for i in range(0,48,8)]

	for item in comm_parsed:
		write_scparam(d, 0x0c, 0xff, item, 0xff)
		load_datansize(d)
		time.sleep(.001)

	start_sm(d)


def send_fphx_cmd(d, fphx_word, wedge=0xff):
	comm_parsed = [ ((fphx_word >> (32-8-i)) & 0x000000FF) for i in range(0,32,8)]

	for item in comm_parsed:
		write_scparam(d, 0x03, wedge, item, 0xff)
		load_datansize(d)
#		time.sleep(.001)

	start_sm(d)

def make_fphx_cmd(chipId, regId, cmd, data):
	header = 0x67
	trailer = 0x0
	word = int(0)
	word = header << (32-7)
	word |= (0x1f & chipId) << (32-12)
	word |= (0x1f & regId) << (32-17)
	word |= (0x7 & cmd) << (32-20)
	word |= (0xff & data) << (32-28)
	word |= (0xf & trailer)
	return word

#FVTX Default parameters for FPHX Chips
def ld_fphxparam_fvtx(d, cheat=None):
	cheat = [
	0xCF50A800,
	0XCF511050,
	0XCF519010,
	0XCF521140,
	0XCF529190,
	0XCF5311E0,
	0XCF539230,
	0XCF541280,
	0XCF5492D0,
	0XCF551320,
	0XCF559370,
	0XCF561460,
	0XCF569040,
	0XCF571400,
	0XCF579420,
	0XCF581050,
	0XCF5893F0,
	]

	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)

#INTT Default parameters for FPHX Chips
def ld_fphxparam(d, cheat=None):
	cheat = [
	0xCF50A800,
	0XCF511050,
	0XCF519010,
	0xCF5210F0, # DAC0 15
	0xCF529130, # DAC1 19
	0xCF531170, # DAC2 23
	0xCF5391B0, # DAC3 27
	0xCF5411F0, # DAC4 31
	0xCF549230, # DAC5 35
	0xCF551270, # DAC6 39
	0xCF5592B0, # DAC7 43
	0XCF561460,
	0XCF569040,
	0XCF571400,
	0XCF579420,
	0XCF581050,
	0XCF5893F0,
	]

	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)


#INTT Default self-trigger config
def ld_fphxparam_intt_selftrigger(d, cheat=None):
	cheat = [
	0xCF50A800,
	0XCF511050,
	0XCF519010,
	0xcf5210f0, # DAC0 15
	0xcf5291e0, # DAC1 30
	0xcf5313c0, # DAC2 60
	0xcf5395a0, # DAC3 90
	0xcf541780, # DAC4 120
	0xcf549960, # DAC5 150
	0xcf551b40, # DAC6 180
	0xcf559d20, # DAC7 210
	0XCF561460,
	0XCF569040,
	0XCF571400,
	0XCF579420,
	0XCF581050,
	0XCF5893F0,
	]

	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)
      


# FPHX parameters for DAC scan run1
def ld_fphxparam_dac_scan1(d, cheat=None):
	cheat = [
                0xcf50a800, # Mask = N/A
                0xcf511050, # DigiCtrl = 5
                0xcf519010, # Vref = 1
                0xcf521080, # DAC0 = 8
                0xcf5290c0, # DAC1 = 12
                0xcf531100, # DAC2 = 16
                0xcf539140, # DAC3 = 20
                0xcf541180, # DAC4 = 24
                0xcf5491c0, # DAC5 = 28
                0xcf551200, # DAC6 = 32
                0xcf559240, # DAC7 = 36
                0xcf561460, # N1sel/N2sel = 6/4
                0xcf569040, # FB1sel/Leaksel = 4/0
                0xcf571400, # P3sel/P2sel = 0/4
                0xcf579420, # Gsel/BWsel = 2/8
                0xcf581050, # P1sel/Injsel = 5/0
                0xcf5893f0, # LVDS = 63
        ]
	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)

# FPHX parameters for DAC scan run1
def ld_fphxparam_dac_scan2(d, cheat=None):
	cheat = [
                0xcf50a800, # Mask = N/A
                0xcf511050, # DigiCtrl = 5
                0xcf519010, # Vref = 1
                0xcf5211c0, # DAC0 = 28
                0xcf529200, # DAC1 = 32
                0xcf531240, # DAC2 = 36
                0xcf539280, # DAC3 = 40
                0xcf5412c0, # DAC4 = 44
                0xcf549300, # DAC5 = 48
                0xcf551340, # DAC6 = 52
                0xcf559380, # DAC7 = 56
                0xcf561460, # N1sel/N2sel = 6/4
                0xcf569040, # FB1sel/Leaksel = 4/0
                0xcf571400, # P3sel/P2sel = 0/4
                0xcf579420, # Gsel/BWsel = 2/8
                0xcf581050, # P1sel/Injsel = 5/0
                0xcf5893f0, # LVDS = 63
        ]
	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)

# FPHX parameters for DAC scan run1
def ld_fphxparam_dac_scan3(d, cheat=None):
	cheat = [
                0xcf50a800, # Mask = N/A
                0xcf511050, # DigiCtrl = 5
                0xcf519010, # Vref = 1
                0xcf521300, # DAC0 = 48
                0xcf529340, # DAC1 = 52
                0xcf531380, # DAC2 = 56
                0xcf5393c0, # DAC3 = 60
                0xcf541400, # DAC4 = 64
                0xcf549440, # DAC5 = 68
                0xcf551480, # DAC6 = 72
                0xcf5594c0, # DAC7 = 76
                0xcf561460, # N1sel/N2sel = 6/4
                0xcf569040, # FB1sel/Leaksel = 4/0
                0xcf571400, # P3sel/P2sel = 0/4
                0xcf579420, # Gsel/BWsel = 2/8
                0xcf581050, # P1sel/Injsel = 5/0
                0xcf5893f0, # LVDS = 63
        ]
	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)

# FPHX parameters for DAC scan run1
def ld_fphxparam_dac_scan4(d, cheat=None):
	cheat = [
                0xcf50a800, # Mask = N/A
                0xcf511050, # DigiCtrl = 5
                0xcf519010, # Vref = 1
                0xcf521440, # DAC0 = 68
                0xcf529480, # DAC1 = 72
                0xcf5314c0, # DAC2 = 76
                0xcf539500, # DAC3 = 80
                0xcf541540, # DAC4 = 84
                0xcf549580, # DAC5 = 88
                0xcf5515c0, # DAC6 = 92
                0xcf559600, # DAC7 = 96
                0xcf561460, # N1sel/N2sel = 6/4
                0xcf569040, # FB1sel/Leaksel = 4/0
                0xcf571400, # P3sel/P2sel = 0/4
                0xcf579420, # Gsel/BWsel = 2/8
                0xcf581050, # P1sel/Injsel = 5/0
                0xcf5893f0, # LVDS = 63
        ]
	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)

# FPHX parameters for DAC scan run1
def ld_fphxparam_dac_scan5(d, cheat=None):
	cheat = [
                0xcf50a800, # Mask = N/A
                0xcf511050, # DigiCtrl = 5
                0xcf519010, # Vref = 1
                0xcf521580, # DAC0 = 88
                0xcf5295c0, # DAC1 = 92
                0xcf531600, # DAC2 = 96
                0xcf539640, # DAC3 = 100
                0xcf541680, # DAC4 = 104
                0xcf5496c0, # DAC5 = 108
                0xcf551700, # DAC6 = 112
                0xcf559740, # DAC7 = 116
                0xcf561460, # N1sel/N2sel = 6/4
                0xcf569040, # FB1sel/Leaksel = 4/0
                0xcf571400, # P3sel/P2sel = 0/4
                0xcf579420, # Gsel/BWsel = 2/8
                0xcf581050, # P1sel/Injsel = 5/0
                0xcf5893f0, # LVDS = 63
        ]
	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)

# FPHX parameters for DAC scan run1
def ld_fphxparam_dac_scan6(d, cheat=None):
	cheat = [
                0xcf50a800, # Mask = N/A
                0xcf511050, # DigiCtrl = 5
                0xcf519010, # Vref = 1
                0xcf5216c0, # DAC0 = 108
                0xcf529700, # DAC1 = 112
                0xcf531740, # DAC2 = 116
                0xcf539780, # DAC3 = 120
                0xcf5417c0, # DAC4 = 124
                0xcf549800, # DAC5 = 128
                0xcf551840, # DAC6 = 132
                0xcf559880, # DAC7 = 136
                0xcf561460, # N1sel/N2sel = 6/4
                0xcf569040, # FB1sel/Leaksel = 4/0
                0xcf571400, # P3sel/P2sel = 0/4
                0xcf579420, # Gsel/BWsel = 2/8
                0xcf581050, # P1sel/Injsel = 5/0
                0xcf5893f0, # LVDS = 63
        ]
	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)

# FPHX parameters for DAC scan run1
def ld_fphxparam_dac_scan7(d, cheat=None):
	cheat = [
                0xcf50a800, # Mask = N/A
                0xcf511050, # DigiCtrl = 5
                0xcf519010, # Vref = 1
                0xcf521800, # DAC0 = 128
                0xcf529840, # DAC1 = 132
                0xcf531880, # DAC2 = 136
                0xcf5398c0, # DAC3 = 140
                0xcf541900, # DAC4 = 144
                0xcf549940, # DAC5 = 148
                0xcf551980, # DAC6 = 152
                0xcf5599c0, # DAC7 = 156
                0xcf561460, # N1sel/N2sel = 6/4
                0xcf569040, # FB1sel/Leaksel = 4/0
                0xcf571400, # P3sel/P2sel = 0/4
                0xcf579420, # Gsel/BWsel = 2/8
                0xcf581050, # P1sel/Injsel = 5/0
                0xcf5893f0, # LVDS = 63
        ]
	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)

# FPHX parameters for DAC scan run1
def ld_fphxparam_dac_scan8(d, cheat=None):
	cheat = [
                0xcf50a800, # Mask = N/A
                0xcf511050, # DigiCtrl = 5
                0xcf519010, # Vref = 1
                0xcf521940, # DAC0 = 148
                0xcf529980, # DAC1 = 152
                0xcf5319c0, # DAC2 = 156
                0xcf539a00, # DAC3 = 160
                0xcf541a40, # DAC4 = 164
                0xcf549a80, # DAC5 = 168
                0xcf551ac0, # DAC6 = 172
                0xcf559b00, # DAC7 = 176
                0xcf561460, # N1sel/N2sel = 6/4
                0xcf569040, # FB1sel/Leaksel = 4/0
                0xcf571400, # P3sel/P2sel = 0/4
                0xcf579420, # Gsel/BWsel = 2/8
                0xcf581050, # P1sel/Injsel = 5/0
                0xcf5893f0, # LVDS = 63
        ]
	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)

def ld_fphxparam_dac_scan(scan_num, d, cheat=None):
        if scan_num == 1 :
                ld_fphxparam_dac_scan1(d, cheat=None)
        elif scan_num == 2 :
                ld_fphxparam_dac_scan2(d, cheat=None)
        elif scan_num == 3 :
                ld_fphxparam_dac_scan3(d, cheat=None)
        elif scan_num == 4 :
                ld_fphxparam_dac_scan4(d, cheat=None)
        elif scan_num == 5 :
                ld_fphxparam_dac_scan5(d, cheat=None)
        elif scan_num == 6 :
                ld_fphxparam_dac_scan6(d, cheat=None)
        elif scan_num == 7 :
                ld_fphxparam_dac_scan7(d, cheat=None)
        elif scan_num == 8 :
                ld_fphxparam_dac_scan8(d, cheat=None)
        else :
                print( "def ld_fphxparam_dac_scan(scan_num, d, cheat=None):" )
                print( "The scan num you gave was", scan_num )
                sys.eixt()


def ld_fphxparam_high_daqs(d, cheat=None):
	cheat = [
	0xCF50A800,
	0XCF511050,
	0XCF519010,

        # 0xcf5211c0 , #DAC 0 28
        # 0xcf529200 , #DAC 1 32
        # 0xcf531240 , #DAC 2 36
        # 0xcf539280 , #DAC 3 40
        # 0xcf5412c0 , #DAC 4 44
        # 0xcf549300 , #DAC 5 48
        # 0xcf551340 , #DAC 6 52
        # 0xcf559380 , #DAC 7 56

        # 0xcf521120, # DAC0 18
        # 0xcf5291e0, # DAC1 30
        # 0xcf5313c0, # DAC2 60
        # 0xcf5395a0, # DAC3 90
        # 0xcf541780, # DAC4 120
        # 0xcf549960, # DAC5 150
        # 0xcf551b40, # DAC6 180
        # 0xcf559d20, # DAC7 210

        # 0xcf521110, # DAC0 17
        # 0xcf5291e0, # DAC1 30
        # 0xcf5313c0, # DAC2 60
        # 0xcf5395a0, # DAC3 90
        # 0xcf541780, # DAC4 120
        # 0xcf549960, # DAC5 150
        # 0xcf551b40, # DAC6 180
        # 0xcf559d20, # DAC7 210

        #0xcf521170 , #DAC 0 23
        #0xcf5291b0 , #DAC 1 27
        #0xcf5311f0 , #DAC 2 31
        #0xcf539230 , #DAC 3 35
        #0xcf541270 , #DAC 4 39
        #0xcf5492b0 , #DAC 5 43
        #0xcf5512f0 , #DAC 6 47
        #0xcf559330 , #DAC 7 51
                
        #0xcf5211e0 , #DAC 0 30
        #0xcf529220 , #DAC 1 34
        #0xcf531260 , #DAC 2 38
        #0xcf5392a0 , #DAC 3 42
        #0xcf5412e0 , #DAC 4 46
        #0xcf549320 , #DAC 5 50
        #0xcf551360 , #DAC 6 54
        #0xcf5593a0 , #DAC 7 58
                
        # 0xcf521280 , # DAC0 40
        # 0xcf5211e0 , # DAC0 30
        # 0xcf521140 , # DAC0 20
        # 0xcf521120 , # DAC0 18
        # 0xcf521100 , # DAC0 16 
        # 0xcf5210f0 , # DAC0 15
        0xcf521110 ,# DAC0 17
        0xcf5292c0 ,# DAC1 44
        0xcf531300 ,# DAC2 48
        0xcf539340 ,# DAC3 52
        0xcf541380 ,# DAC4 56
        0xcf5493c0 ,# DAC5 60
        0xcf551400 ,# DAC6 64
        0xcf559440 ,# DAC7 68
	0XCF561460,
	0XCF569040,
	0XCF571400,
	0XCF579420,
	0XCF581050,
	0XCF5893F0,
	]

	for item in cheat:
		send_fphx_cmd(d, item)
		time.sleep(.001)


def write_scparam(d, command, wedge, dataword, sizeword):
	d.reg.roc_command = command
	d.reg.roc_wedge = wedge
	d.reg.roc_dataword = dataword
	d.reg.roc_sizeword = sizeword

def read_fphx(d, chip, register, wedge, roc):
    send_fphx_cmd(d, make_fphx_cmd(chip, register, 0, 0), wedge)
    time.sleep(0.1)
    output = [None,None]
    if roc == 1:
        readback = d.reg.fphx_readback1
    elif roc == 0:
        readback = d.reg.fphx_readback0
    else:
        raise Exception("Which ROC should I read? Accepted values are [0,1]")
        return output

    binary = bin(readback)
    reverse = binary[-1:1:-1]
    reverse = reverse + (8 - len(reverse))*'0'

    output[0] = int(reverse[:-1],2)
    output[1] = int(reverse[:-1], 2) + 128

    return output

def write_fphx(d, chip, register, wedge, value, cmd=1):
	fphx_word = make_fphx_cmd(chip, register, cmd, value)
	send_fphx_cmd(d, fphx_word, wedge)
	time.sleep(0.01)

'''
Input is dam object.
Output is -1 if error occurred, else the number of fibers latched, including zero.
'''
def verify_latch(d):
    d.reg.latch_sync_rst = 1
    d.reg.latch_sync_rst = 0
    if d.reg.latch_sync == 0:
        latch_fpga(d)
        time.sleep(1)
        
        latch_arr = bin(d.reg.latch_sync)[2:].zfill(28)
        blatch_ok = [True if i=='1' else False for i in latch_arr][::-1]

        count_latch = latch_arr.count('1')

        R = '\033[31m'
        W = '\033[0m'
        G = '\033[32m'

        for c,i in enumerate(blatch_ok):
            color = G if i else R
            print("{2}Ladder Channel #{0:3}: {1}{3}".format(c//2,i,color,W))
        return count_latch
    else:
        print('Reset of Latch Sync Diagnostics NOT SUCCESSFULL. Verify Latch halted!')
        return -1


"""
[UNDER DEVELOPMENT]
Brings FLX board to a state that can interact with the ROC.

BCO and ST CLK generated, internal logic reset, transceivers properly initialized.

Inputs: dam object
Outputs: None
"""
def cold_start(d):
    dma_datasel(d, test_mode=False)

    # Placeholder
    gtm_locked = True

    # Makes sure commands are sent to both ROCs
    d.reg.sc_target = 0x3
    # Initial clock distribution 
    if d.reg.si5345_pll.nLOL == 0:
        d.reg.si5345_pll.program = 1
        time.sleep(5)
        if d.reg.lmk_locked == 1:
            init_clocks(d)
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
    reset_fpga(d)
    d.reg.latch_sync_rst = 1
    time.sleep(0.1)
    d.reg.latch_sync_rst = 0
    #verify_latch(d)
    latch_fpga(d)
    time.sleep(0.1)
    # 
    d.reg.rst=3
    d.reg.rst=0
    time.sleep(0.5)
    latch_fpga(d)
    time.sleep(0.5)
    reset_fphx(d)
   
    bsync_ok = [True if i=='1' else False for i in bin(d.reg.sync_ok)[2:].zfill(2)][::-1]
    blatch_ok = [True if i=='1' else False for i in bin(d.reg.latch_sync)[2:].zfill(28)][::-1]

    R = '\033[31m'
    W = '\033[0m'
    G = '\033[32m'

    for c,i in enumerate(bsync_ok):
        color = G if i else R
        print("{2}Slow Control #{0:3}: {1}{3}".format(c,i,color,W))

def check_register(d, var=101, wedge_set=range(32), chip_set=range(1,14), reg_set=range(4,12), roc=0):
    reset_fpga(d)
    time.sleep(1)

    reset_fphx(d)
    time.sleep(1)

    good_ones = pd.DataFrame(columns=['wedge','port','chip','register'])
    for wedge in wedge_set:
        print('WEDGE: '+str(wedge))
        port = GetPort(wedge)
        for chip in chip_set:
            print('CHIP: '+str(chip))
            for reg in reg_set:
                write_fphx(d, chip, reg, wedge, var)
                a=read_fphx(d, chip, reg, wedge, roc)
                if var in a:
                    new = pd.DataFrame({'wedge':[wedge], 'port':[port], 'chip':[chip], 'register':[reg]})
                    good_ones = pd.concat([good_ones, new], ignore_index=True)
    return good_ones

def mask_channel(d, chip, wedge, channel=128):
	write_fphx(d, chip, 1, wedge, channel, cmd=2)

def unmask_channel(d, chip, wedge, channel=128):
	write_fphx(d, chip, 1, wedge, channel, cmd=5)

def init_clocks(d):
    # reset standalone clk gen module
    #d.reg.clks_ctl.rst_standalone=1
    #d.reg.clks_ctl.rst_standalone=0
    time.sleep(0.01)

    # reset gth of all bco & start clocks
    d.reg.clks_ctl.rst_clksgth=1
    d.reg.clks_ctl.rst_clksgth=0
    time.sleep(0.01)

    # reset clk_gen logic
    d.reg.clks_ctl.rst_clks=1
    d.reg.clks_ctl.rst_clks=0

# port should be, for example, "A1"
def GetWedgeIDs(port):
    first_wedge = int(port[1])*8 + 2*(ord(port[0]) - 65)
    second_wedge = first_wedge + 1

    return [ first_wedge, second_wedge ]

def GetPort(wedge):
    port = None
    col_idx = (wedge%8) // 2
    row_idx = wedge//8

    if col_idx == 0:
        port = "A" + str(row_idx)
    elif col_idx == 1:
        port = "B" + str(row_idx)
    elif col_idx == 2:
        port = "C" + str(row_idx)
    elif col_idx == 3:
        port = "D" + str(row_idx)

    return port

def getTxResetDone(d):
    all_diag = bin(d.reg.tx_locked)[2:].zfill(32)[::-1]
    
    R = '\033[31m'
    W = '\033[0m'
    G = '\033[32m'
    
    print("--- [GTH TX] Diagnostics ---")
    all_diag = [True if i=='1' else False for i in all_diag]

    for c,i in enumerate(all_diag[:28]):
        color = G if i else R
        status = True if i else False
        print("{2}Ladder GTH #{0:3}: {1}{3}".format(c//2,status,color,W))
    for c,i in enumerate(all_diag[28:30]):
        color = G if i else R
        status = True if i else False
        print("{2}Slow Control GTH #{0:3}: {1}{3}".format(c,status,color,W))
    for c,i in enumerate(all_diag[30:32]):
        color = G if i else R
        status = True if i else False
        print("{2}BCO & STCLK GTH #{0:3}: {1}{3}".format(c,status,color,W))

def getRxResetDone(d):
    all_diag = bin(d.reg.rx_locked)[2:].zfill(32)
    all_diag = all_diag[::-1]
    
    R = '\033[31m'
    W = '\033[0m'
    G = '\033[32m'
    
    print("--- [GTH RX] Diagnostics ---")
    all_diag = [True if i=='1' else False for i in all_diag]

    for c,i in enumerate(all_diag[:28]):
        color = G if i else R
        status = True if i else False
        print("{2}Ladder GTH #{0:3}: {1}{3}".format(c//2,status,color,W))
    for c,i in enumerate(all_diag[28:30]):
        color = G if i else R
        status = True if i else False
        print("{2}Slow Control GTH #{0:3}: {1}{3}".format(c,status,color,W))
    for c,i in enumerate(all_diag[30:32]):
        color = G if i=='1' else R
        status = True if i else False
        print("{2}BCO & STCLK GTH #{0:3}: {1}{3}".format(c,status,color,W))

