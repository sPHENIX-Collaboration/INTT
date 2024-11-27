import ROOT
import numpy as np
import glob
import os
import pyodbc
import csv
import collections
import math
from datetime import datetime

ROOT.gROOT.SetBatch(True)

def get_run_events():
    """Query database for run events"""
    connection_string = (
        'DRIVER={PostgreSQL};'
        'SERVER=sphnxdaqdbreplica;'
        'DATABASE=daq;'
        'PORT=5432;'
    )
    with pyodbc.connect(connection_string) as conn:
        cursor = conn.cursor()
        cursor.execute("select * from run where runtype = 'physics' order by runnumber;")
        return cursor.fetchall()

def insert_data(runnum, dead_count, runtime, bco_stdev, bco_peak):
    """Insert data into the database"""
    connection_string = (
        'DRIVER={PostgreSQL};'
        'SERVER=sphnxdbmaster;'
        'DATABASE=intt;'
        'PORT=5432;'
    )
    with pyodbc.connect(connection_string) as conn:
        cursor = conn.cursor()
        cursor.execute(
            "INSERT INTO intt_qa_expert (runnumber, nohitch, runtime, bco_stdev, bco_peak) VALUES (?, ?, ?, ?, ?);",
            (runnum, dead_count, runtime, bco_stdev, bco_peak)
        )
        conn.commit()

def process_run_file(hot_file_path):
    Dead = 0
    Cold = 0
    Hot = 0
    if os.path.isfile(hot_file_path):
        file_hot = ROOT.TFile.Open(hot_file_path)
        tree_hot = file_hot.Get("Multiple")
        if tree_hot:
            nEvent = tree_hot.GetEntries()
            i = 0
            while i < nEvent:
                tree_hot.GetEvent(i)
                if tree_hot.Iflag == 1:
                    Dead += 1
                elif tree_hot.Iflag == 4:
                    Cold += 1
                elif tree_hot.Iflag == 8:
                    Hot += 1
                i += 1
        if not tree_hot:
            return None
    if not os.path.isfile(hot_file_path):
        return None
    return [Dead, Cold, Hot]

def process_bco_file(bco_file_path):
    BCO_stdev = 0
    BCO_peak = 0
    counter = []
    if os.path.isfile(bco_file_path):
        file_bco = ROOT.TFile.Open(bco_file_path)
        tree_bco_multi = file_bco.Get("Multiple")
        tree_bco_single = file_bco.Get("Single")
        if tree_bco_multi:
            nEvent = tree_bco_multi.GetEntries()
            i = 0
            while i < nEvent:
                tree_bco_multi.GetEvent(i)
                counter.append(tree_bco_multi.Ibco_diff)
                i += 1
            c = collections.Counter(counter)
            BCO_peak = c.most_common()[0][0]
            if BCO_peak == -1:
                if len(c.most_common()) > 1:
                    BCO_peak = c.most_common()[1][0]
        if not tree_bco_multi:
            return None

        if tree_bco_single:
            nEvent = tree_bco_single.GetEntries()
            i = 0
            while i < nEvent:
                tree_bco_single.GetEvent(i)
                BCO_stdev = tree_bco_single.DStdDev
                if math.isnan(BCO_stdev):
                    BCO_stdev = -1
                i += 1
        if not tree_bco_single:
            return None
    if not os.path.isfile(bco_file_path):
        return None
    return [BCO_stdev, BCO_peak]

def calculate_runtime(brtimestamp, ertimestamp):
    """Calculate runtime in minutes"""
    if not brtimestamp or not ertimestamp:
        return -1
    try:
        if isinstance(brtimestamp, datetime) and isinstance(ertimestamp, datetime):
            start_time = brtimestamp
            end_time = ertimestamp
        else:
            start_time = datetime.strptime(brtimestamp, '%Y-%m-%d %H:%M:%S')
            end_time = datetime.strptime(ertimestamp, '%Y-%m-%d %H:%M:%S')
        
        runtime = (end_time - start_time).total_seconds() / 60
        return int(runtime)
    except ValueError:
        return -1    
    
    
def main():
    rows = get_run_events()
    rownum = len(rows)
    i = 6
    while i < rownum:
        runnum = str(rows[i][0])
        brtimestamp = rows[i][2]
        ertimestamp = rows[i][3]
        runtime = calculate_runtime(brtimestamp, ertimestamp)
        file_paths = f'/sphenix/user/jaein213/macros/inttcalib/test/hotmap_cdb_1106/hotmap_run_{runnum.zfill(8)}.root'
        bco_file_paths = f'/sphenix/user/jaein213/macros/inttcalib/test/bcomap_cdb_1106/bcomap_run_{runnum.zfill(8)}.root'
        if file_paths and bco_file_paths:
            result = process_run_file(file_paths)
            bco_result = process_bco_file(bco_file_paths)
            if result and bco_result:
                Dead, Cold, Hot = result
                BCO_stdev, BCO_peak = bco_result
                BCO_stdev = round(BCO_stdev, 4) 
                insert_data(runnum, Dead, runtime, BCO_stdev, BCO_peak)
                print(runnum)
                print(f"Dead: {Dead}, Cold: {Cold}, Hot: {Hot}, Runtime: {runtime} minutes, BCO_stdev: {BCO_stdev}, BCO_peak: {BCO_peak}")
            else:
                print(f"run {runnum} is missing")
        else:
            print(f"run {runnum} is missing")
        i += 1
        # if(i==10):
        #     break

if __name__ == "__main__":
    main()
