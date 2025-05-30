import ROOT
import os
import pyodbc
import sys

ROOT.gROOT.SetBatch(True)

def create_table():
    """Create intt_qa_expert table if it does not exist"""
    connection_string = (
        'DRIVER={PostgreSQL};'
        'HOST=sphnxdbmaster;'
        'DATABASE=intt;'
        'PORT=5432;'
    )
    
    create_table_sql = """
    CREATE TABLE intt_qa_expert (
        runnumber INT PRIMARY KEY,
        runtime INT,
        runmode INT,
        nevents INT,
        goodchanratio DOUBLE PRECISION,
        N_dead INT,
        N_cold INT,
        N_hot INT,
        intt_bco_diff_qa INT
    );
    """
    
    try:
        with pyodbc.connect(connection_string) as conn:
            cursor = conn.cursor()
            cursor.execute(create_table_sql)
            conn.commit()
        print("✅ Table 'intt_qa_expert' checked/created successfully.")
    except Exception as e:
        print(f"❌ Error creating table: {e}")

def init_into_runtriage():
    column = "INTT"
    conn = pyodbc.connect("DSN=Production_write")
    cur = conn.cursor()
    query = f"UPDATE goodruns SET {column.lower()} = (?, ?) WHERE runnumber = ?"
    for runnumber in range(42733, 54975):
        cur.execute(query, ("BAD", "", runnumber))
        conn.commit()



def insert_into_runtriage(runnumber, runclass, runnote):
    column = "INTT"
    conn = pyodbc.connect("DSN=Production_write")
    cur = conn.cursor()
    query = f"UPDATE goodruns SET {column.lower()} = (?, ?) WHERE runnumber = ?"
    cur.execute(query, (runclass, runnote, runnumber))
    conn.commit()

def insert_into_inttdb(runnum, runtime, runmode, nevents, goodchanratio, N_dead, N_cold, N_hot, intt_bco_diff_qa,runclass, runnote):
    """Insert data into the database"""
    connection_string = (
        'DRIVER={PostgreSQL};'
        'HOST=sphnxdbmaster;'
        'DATABASE=intt;'
        'PORT=5432;'
    )
    
    try:
        with pyodbc.connect(connection_string) as conn:
            cursor = conn.cursor()
            cursor.execute(
                """
                INSERT INTO intt_qa_expert 
                (runnumber, runtime, runmode, nevents, goodchanratio, N_dead, N_cold, N_hot, intt_bco_diff_qa) 
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);
                """,
                (runnum, int(runtime), int(runmode), int(nevents), float(goodchanratio), 
                 int(N_dead), int(N_cold), int(N_hot), int(intt_bco_diff_qa))
            )
            conn.commit()
        print(f"✅ Run {runnum} inserted successfully")
    except Exception as e:
        print(f"❌ Error inserting run {runnum}: {e}")

def process_root_file(root_file_path):
    if not os.path.isfile(root_file_path):
        print(f"❌ Error: File {root_file_path} not found.")
        return None

    file_root = ROOT.TFile.Open(root_file_path)
    if not file_root or file_root.IsZombie():
        print(f"❌ Error: Unable to open ROOT file {root_file_path}.")
        return None

    tree = file_root.Get("tree")
    if not tree:
        print(f"❌ Error: TTree 'tree' not found in {root_file_path}.")
        return None

    results = []
    runclass = []
    runnote = []
    n_entries = tree.GetEntries()
    print(f"🔄 Processing {n_entries} entries in {root_file_path}...")

    for i in range(n_entries):
        tree.GetEntry(i)
        runnum = int(tree.runnumber)
        runtime = int(tree.runtime)
        runmode = int(tree.runmode)  
        nevents = int(tree.nevents)
        goodchanratio = float(tree.goodchanratio)
        N_dead = int(tree.N_dead)
        N_cold = int(tree.N_cold)
        N_hot = int(tree.N_hot)
        intt_bco_diff_qa = int(tree.intt_bco_diff_qa)  # float → int
        
        runclass, runnote = classify_run(runnum, runtime, runmode, nevents, goodchanratio, intt_bco_diff_qa, N_dead, N_cold + N_hot)
        results.append((runnum, runtime, runmode, nevents, goodchanratio, N_dead, N_cold, N_hot, intt_bco_diff_qa,runclass,runnote))
    return results

def classify_run(runnumber, runtime, runmode, nevents, goodchanratio, intt_bco_diff_qa, N_dead, total_channel):
    runclass = "BAD"
    runnote = ""
    print(runnumber, runtime, runmode, nevents, goodchanratio, intt_bco_diff_qa, N_dead, total_channel)
    if runnumber >= 46560:
        if runtime < 300:
            runclass = "QUESTIONABLE"
            runnote = "Short run < 300s"
        elif runtime >= 300 and intt_bco_diff_qa == 1 and goodchanratio >= 90.0:
            if runmode == 0:
                runclass = "GOLDEN"
                if runnumber <=54000:
                    runnote = "pp trigger (BCO GOOD , Good chan ratio > 90%)"
                else:
                    runnote = "AuAu (BCO GOOD , Good chan ratio > 90%)"
            elif runmode == 1:
                runclass = "GOLDEN"
                runnote = "pp streaming (BCO GOOD , Good chan ratio > 90%)"
        elif runtime >= 300 and intt_bco_diff_qa == 1 and 80.0 <= goodchanratio < 90.0:
            runclass = "QUESTIONABLE"
            runnote = "GOOD BCO alignment but 80<good ch ratio <90"
        elif runtime >= 300 and (intt_bco_diff_qa == 0 or goodchanratio < 80.0):
            runclass = "BAD"
            runnote = "BAD BCO alignment or/and good ch ratio <80"

    return runclass, runnote

def print_help():
    print("""
Usage:
- create_inttdb: Creates the INTT database table.
- insert_into_inttdb: Inserts data from the ROOT file into the INTT database.
- insert_into_runtriage: Inserts data from the ROOT file into the Run Triage table.
- init_runtriage: Initializes the Run Triage table.
- help: Displays this help message.
    """)

def main():
    while True:
        command = input("Enter the task to execute (create_inttdb / insert_into_inttdb / insert_into_runtriage / init_runtriage / help / exit): ").strip()

        if command == "create_inttdb":
            create_table()
        elif command == "insert_into_inttdb":
            root_file_path = "/sphenix/user/jaein213/INTT_Jaein/INTT/QA_codes/QAEval/macro/InttQAEval_2.root"
            results = process_root_file(root_file_path)

            if results:
                for result in results:
                    insert_into_inttdb(*result)
                    print(f"Run {result[0]}: {runclass} ({runnote})")
            else:
                print("❌ No data to insert.")
        elif command == "insert_into_runtriage":
            root_file_path = "/sphenix/user/jaein213/INTT_Jaein/INTT/QA_codes/QAEval/macro/InttQAEval_2.root"
            results = process_root_file(root_file_path)

            if results:
                for result in results:
                    insert_into_runtriage(result[0], result[9], result[10])
                    print(f"Run {result[0]}: {result[9]} ({result[10]})")
            else:
                print("❌ No data to insert.")
        elif command == "init_runtriage":
            # Functionality for init_runtriage goes here
            print("Initializing Run Triage...")
            init_into_runtriage()
            print("Init Done!")
        elif command == "help":
            print_help()
        elif command == "exit":
            print("Exiting... Goodbye!")
            sys.exit(0)  # Program exits
        else:
            print("❌ Unknown command. Please try again.")
            continue  # Repeat the loop if the command is incorrect

if __name__ == "__main__":
    main()