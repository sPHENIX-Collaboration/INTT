import ROOT
import numpy as np
import pyodbc
import os
import collections
import math

ROOT.gROOT.SetBatch(True)

def get_info_from_database():
    """Query database for run events"""
    connection_string = (
        'DRIVER={PostgreSQL};'
        'SERVER=sphnxdbmaster;'
        'DATABASE=intt;'
        'PORT=5432;'
    )
    with pyodbc.connect(connection_string) as conn:
        cursor = conn.cursor()
        cursor.execute('SELECT runnumber, deadch, runtime, bco_stddev, bco_peak, hitrate0, hitrate1, hitrate2, hitrate3, hitrate4, hitrate5, hitrate6, hitrate7 from intt_qa_expert;')
        return cursor.fetchall()

def filter_runs(runs, runnumber_min, runnumber_max):
    """Filter runs based on criteria"""
    good_runs = []
    only_runtime = []
    only_bco_peak = []
    both_conditions = []
    for run in runs:
        runnumber, deadch, runtime, bco_stddev, bco_peak, *hitrates = run
        if runnumber_min <= runnumber <= runnumber_max:
            if runtime < 5 and bco_peak != 23:
                both_conditions.append(runnumber)
            elif runtime < 5:
                only_runtime.append(runnumber)
            elif bco_peak != 23:
                only_bco_peak.append(runnumber)
            else:
                good_runs.append(runnumber)
    return good_runs, only_runtime, only_bco_peak, both_conditions

def main():
    runs = get_info_from_database()
    runnumber_min = 52469 
    runnumber_max = 53880 
    good_runs, only_runtime, only_bco_peak, both_conditions = filter_runs(runs, runnumber_min, runnumber_max)
    print("Good runs:", good_runs)
    print("Only runtime < 5:", only_runtime)
    print("Only Bco peak != 23:", only_bco_peak)
    print("runtime < 5 and bco_peak != 23:", both_conditions)

if __name__ == "__main__":
    main()