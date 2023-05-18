#!/usr/bin/env python3
import mmap
import struct
import time
import numpy as np

from fcntl import ioctl
from select import select

BLOCK_SIZE = (1 << 16)-1
BAR2_OFFSET = 0

reg_field_dma_busy_status = {
  "TOHOST_BUSY" : (0x1,  0),
  "TOHOST_BUSY_LATCHED" : (0xe,  1),
  "ENABLE" : (0x10,  4),
}

reg_field_int_test = {
  "IRQ" : (0xf,  0),
}

reg_field_generic_constants = {
  "DESCRIPTORS" : (0xff,  0),
  "INTERRUPTS" : (0xff00,  8),
}

reg_field_dma_test = {
  "LENGTH" : (0xffff,  0),
}

reg_field_si5345_pll = {
  "i2c_busy" : (0x1,  0),
  "nLOL" : (0x2,  1),
  "program" : (0x4,  2),
}

reg_field_roc_trigger = {
  "start_sm" : (0x1,  0),
  "load_datasize" : (0x2,  1),
}

reg_field_clks_ctl = {
  "rstbco_stclk" : (0x3,  0),
  "calib_stclk" : (0xc,  2),
  "rst_clksgth" : (0x10,  4),
  "rst_clks" : (0x20,  5),
  "rst_standalone" : (0x40,  6),
}



regs = {
  "reg_map_version" : (0x0000, 1, 16, None),
  "board_id_timestamp" : (0x0010, 1, 16, None),
  "git_commit_time" : (0x0020, 1, 16, None),
  "git_tag" : (0x0030, 1, 16, None),
  "git_commit_number" : (0x0040, 1, 16, None),
  "git_hash" : (0x0050, 1, 16, None),
  "status_leds" : (0x0060, 1, 16, None),
  "dma_busy_status" : (0x0070, 1, 16, reg_field_dma_busy_status),
  "int_test" : (0x0800, 1, 16, reg_field_int_test),
  "generic_constants" : (0x0810, 1, 16, reg_field_generic_constants),
  "num_of_channels" : (0x0820, 1, 16, None),
  "card_type" : (0x0830, 1, 16, None),
  "opto_trx_num" : (0x0840, 1, 16, None),
  "cr_internal_loopback_mode" : (0x0850, 1, 16, None),
  "debug_mode" : (0x0860, 1, 16, None),
  "firmware_mode" : (0x0870, 1, 16, None),
  "gtrefclk_source" : (0x0880, 1, 16, None),
  "blocksize" : (0x0890, 1, 16, None),
  "pcie_endpoint" : (0x08A0, 1, 16, None),
  "scratch_pad" : (0x08B0, 1, 16, None),
  "dma_test" : (0x08C0, 1, 16, reg_field_dma_test),
  "lmk_locked" : (0x08D0, 1, 16, None),
  "phy_reset" : (0x08E0, 1, 16, None),
  "si5345_pll" : (0x08F0, 1, 16, reg_field_si5345_pll),
  "fee_stream_enable" : (0x0900, 1, 16, None),
  "dma_packet_chunk_size" : (0x0910, 1, 16, None),
  "dma_fifo_full_count" : (0x0920, 1, 16, None),
  "tx_locked" : (0xC000, 1, 16, None),
  "rx_locked" : (0xC010, 1, 16, None),
  "gtm_recv_clock_freq" : (0xC020, 1, 16, None),
  "gtm_bco" : (0xC030, 1, 16, None),
  "roc_command" : (0xE000, 1, 16, None),
  "roc_wedge" : (0xE010, 1, 16, None),
  "roc_dataword" : (0xE020, 1, 16, None),
  "roc_sizeword" : (0xE030, 1, 16, None),
  "roc_readout" : (0xE040, 1, 16, None),
  "roc_wildcard" : (0xE050, 1, 16, None),
  "roc_trigger" : (0xE060, 1, 16, reg_field_roc_trigger),
  "clks_ctl" : (0xE070, 1, 16, reg_field_clks_ctl),
  "fphx_readback0" : (0xE080, 1, 16, None),
  "fphx_readback1" : (0xE090, 1, 16, None),
  "sync_ok" : (0xE0A0, 1, 16, None),
  "sc_target" : (0xE0B0, 1, 16, None),
  "sc_sync_start" : (0xE0C0, 1, 16, None),
  "ldd_enable" : (0xF000, 1, 16, None),
  "latch_sync" : (0xF010, 1, 16, None),
  "latch_sync_rst" : (0xF020, 1, 16, None),
  "rst" : (0xF030, 1, 16, None),
  "rst_gth" : (0xF040, 1, 16, None),
  "open_time" : (0xF050, 1, 16, None),
  "n_collisions" : (0xF060, 1, 16, None),
  "dma_counter_length" : (0xF070, 1, 16, None),
  "selector_overflow_count" : (0xF080, 1, 16, None),
}

class reg_field(object):
    def __init__(self, dam, base_addr, count, size, field):
        self.__dam__ = dam
        self.__field__ = field
        self.base_addr = base_addr
        self.count = count
        self.size = size

    def __repr__(self):
        return f'reg_field: base_addr:{self.base_addr}, {self.__field__.keys()} , {self.count}'

    def __len__(self):
        return self.count

    def __iter__(self):
        self.offset = 0
        return self

    def __next__(self):
        if (self.offset <= self.count):
            self.offset += 1
            return reg_field(self.__dam__, self.base_addr+(self.offset-1)*(self.size//self.count), 1, 16, self.__field__)
        else:
            raise StopIteration

    def __setitem__(self, key, value):
        if (self.count > 1):
            return reg_field(self.__dam__, self.base_addr+int(key)*(self.size//self.count), 1, 16, self.__field__)
        else:
            mask, shift = self.__field__[key]
            val = self.__dam__.reg_read(self.base_addr)
            val = (val & ((1 << shift)-1)) | (value << shift)
            self.__dam__.reg_write(self.base_addr, val)

    def __getitem__(self, key):
        if (self.count > 1):
            return reg_field(self.__dam__, self.base_addr+int(key)*(self.size//self.count), 1, 16, self.__field__)
        else:
            mask, shift = self.__field__[key]
            return (self.__dam__.reg_read(self.base_addr) & mask) >> shift

    def __setattr__(self, key, value):
        if key.startswith('_'):
            super().__setattr__(key, value)
            return

        if (key in self.__field__):
            self.__setitem__(key, value)
        else:
            super().__setattr__(key, value)

    def __getattr__(self, key):
        if key.startswith('_'):
            super().__getattr__(key)
            return

        if (key in self.__field__):
            return self.__getitem__(key)
        else:
            return object.__getattribute__(self, key)

    def __dir__(self):
        return self.__field__.keys()

class reg_group(object):
    def __init__(self, dam, base_addr, count, size, reg_group):
        self.__dam__ = dam
        self.__base__ = base_addr
        self.__group__ = reg_group
        self.__count = count
        self.__size = size

    def __repr__(self):
        return f'reg_group: base_addr:{self.__base__}, {self.__group__.keys()} , {self.__count}'

    def __setitem__(self, key, value):
        if (self.__count > 1):
            return reg_group(self.__dam__, self.__base__ + (int(key)*self.__size//self.__count), 1, self.__size, self.__group__)
        else:
            addr, count, size, fields = self.__group__[key]
            base_addr = self.__base__ + (addr*size//count)
            reg = reg_field(self.__dam__, base_addr, 1, size, fields)
            reg[key] = value

    def __getitem__(self, key):
        if (self.__count > 1):
            return reg_group(self.__dam__, self.__base__ + (int(key)*self.__size//self.__count), 1, self.__size, self.__group__)
        else:
            addr, count, size, fields = self.__group__[key]
            base_addr = self.__base__ + (addr*size//count)
            return reg_field(self.__dam__, base_addr, 1, size, fields)

    def __setattr__(self, key, value):
        if key.startswith("_"):
            super(reg_group, self).__setattr__(key, value)
            return

        if (key in self.__group__):
            self.__setitem__(key, value)
        else:
            super(reg_group, self).__setattr__(key, value)

    def __getattribute__(self, key):
        if key.startswith("_"):
            return super(reg_group, self).__getattribute__(key)

        if (key in self.__group__):
            return self.__getitem__(key)
        else:
            return super(reg_group, self).__getattribute__(key)

    def __dir__(self):
        return sorted(list(self.group.keys()) + ["keys"])


class reg_map(object):
    def __init__(self, dam, reg_map):
        self.dam = dam
        self.regs = reg_map

    def keys(self):
        return self.regs.keys()

    def __len__(self):
        return len(self.regs)

    def __setitem__(self, key, value):
        addr, count, size, fields = self.regs[key]
        if (fields == None):
            self.dam.reg_write(addr, value)
        elif (addr == 0xD000): # FIXME
            return reg_group(self.dam, addr, count, size, fields)
        else:
            return reg_field(self.dam, addr, count, size, fields)

    def __getitem__(self, key):
        addr, count, size, fields = self.regs[key]
        if (fields == None):
            return self.dam.reg_read(addr)
        elif (addr == 0xD000): # FIXME
            return reg_group(self.dam, addr, count, size, fields)
        else:
            return reg_field(self.dam, addr, count, size, fields)

    def __setattr__(self, key, value):
        if (key in regs):
            self.__setitem__(key, value)
        else:
            super().__setattr__(key, value)

    def __getattr__(self, key):
        if (key in regs):
            return self.__getitem__(key)
        else:
            return object.__getattribute__(self, key)

    def __dir__(self):
        return sorted(list(self.regs.keys()) + ["keys"])

class dam(object):
    def __init__(self, dev_name="/dev/dam0"):
        self.__fp__ = open(dev_name, 'r+b', buffering=0)
        self.mem = mmap.mmap(self.__fp__.fileno(), BLOCK_SIZE, offset=BAR2_OFFSET)
        self.reg = reg_map(self, regs)

    def wait(self):
       select([self.__fp__], [], [])

    def dump_dma_status(self):
        DMA_STATUS = ord('p') << (4*2) | 18
        ioctl(self.__fp__, DMA_STATUS, 0)

    def reset(self):
        self.__fp__.seek(0)
        DMA_RESET = ord('p') << (4*2) | 19
        ioctl(self.__fp__, DMA_RESET, 0)

    def read(self, length):
        return np.frombuffer(self.__fp__.read(length), dtype=np.uint16)

    def reg_read(self, addr):
        ''' Read a 64-bit word (8 bytes) '''
        return struct.unpack("<Q", self.mem[addr:addr+8])[0]

    def reg_write(self, addr, data):
        ''' Write a 64-bit word into addr '''
        self.mem[addr:addr+8] = struct.pack("<Q", data)
