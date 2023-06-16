
# note : This code can be first version of the calibration database in PSQL
# note : the current concept is not to use the main table. So all this kind of ladder information is kept in each subtables, independently.
# note : that is easier to do so

# from googleapiclient.discovery import build # note : for google sheet API
# from google.oauth2 import service_account   # note : for google sheet API
import numpy as np

import psycopg2 # note : the interface of the psql and python
import psycopg2.extras
import pandas as pd

# import glob, sys, fitz # note : for converting the pdf file to png, run "pip install PyMuPDF" prior to the compilation
# import matplotlib.pyplot as plt
# import matplotlib.image as img

import time

# note : ======== ======== ======== ======== ========link the databse ======== ======== ======== ======== ========
hostname = 'localhost_test'
database = 'intt_cali_cabling_test_1'
username = 'chengweishi_test'
pwd      = 'admin_test' # note : not pretty sure it is correct or not.
port_id  = '5432_test'

# note : ======== ======== ======== ======== ======== the header of the ladder_info_table ======== ======== ======== ======== ======== 
ladder_info_table_name = 'ladder_info'
header_name_ladder = [
    'Side',
    'Position',
    'Ladder_name'
]
condition_ladder = [
    'VARCHAR(4) NOT NULL',
    'VARCHAR(6) NOT NULL',
    'VARCHAR(8) NOT NULL'
]

# note : ======== ======== ======== ======== ======== the header of the cali_info ======== ======== ======== ======== ======== 

cali_info_table_name = 'cali_info'
header_name_cali = [
    'cali_time',
    'data_server',
    'cali_file',
    'map_file',
    'direction',
    'ROC',
    'ladder_pos',
    # 'ladder_name',
    'module_ID',
    'port',
    'cali_current_AB',
    'cali_Temp_AB',
    'cali_status',
    'total_n_bad_ch', # note : ID 12
    'run_mode',
    'threshold',
    'run_time' # note : 15
]
condition_cali_info = [
    'timestamptz NOT NULL', # note : for timezone
    'VARCHAR(200) NOT NULL', # note : data server
    'VARCHAR(1000) NOT NULL', # note : file_name, with directory
    'VARCHAR(1000) NOT NULL', # note : map file, with directory
    'VARCHAR(1) NOT NULL',  # note : NS
    'VARCHAR(100) NOT NULL', # note : ROC
    'VARCHAR(6) NOT NULL',  # note : ladder pos
    # 'VARCHAR(8) NOT NULL',  # note : ladder name
    'integer NOT NULL',     # note : module ID
    'VARCHAR(2) NOT NULL',  # note : port
    'NUMERIC(7,1)[]',
    'NUMERIC(7,1)[]',
    'VARCHAR(100) NOT NULL',    # note : status 
    'integer', # note : total_n_bad_ch
    'VARCHAR(100)', # note : run_mode
    'integer', # note : threshold
    'integer' # note : run_time
]

# note : print the used DB link para
def print_DB_link_par () :
    print( ' ============= Printing the DB par =============')
    print( 'hostname =', hostname )
    print( 'database =', database )
    print( 'username =', username )
    print( 'pwd      =', pwd      ) 
    print( 'port_id  =', port_id  )
    print( '============= Printing the DB par =============')

# note : initialize the DB, for cur and conn
def initialize_psql_server():
    try : 
        conn = psycopg2.connect (host = hostname, dbname = database, user = username, password = pwd, port = port_id) 
        # cur = conn.cursor()
        cur = conn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        return cur, conn
    except Exception as error : 
        print(error)


# note : create the table (if is not there)
def create_table(cur, conn, input_table_name, input_header_list, input_condition_list) :
    create_table_script = 'CREATE TABLE IF NOT EXISTS '
    create_table_script += input_table_name# note : the table name
    create_table_script += ' ('
    create_table_script += ' id BIGSERIAL PRIMARY KEY, '

    for i in range (len(input_header_list)) :
        create_table_script += input_header_list[i] # note : the column name
        create_table_script += ' '
        create_table_script += input_condition_list[i] # note : the conditio of the column
    
        if (i != len(input_header_list)-1) :
           create_table_script += ', '

    create_table_script += ');'

    print('the script to create the table : ', create_table_script)
    
    cur.execute(create_table_script)
    conn.commit()


# note : dump the data from the cvs file to PSQL
# note : the function to import the CVS file to the PSQL databse
# note : this is for saving the ladder_pos, ladder_name information in the new databsae
def dump_the_cvs (cur,conn,table_name, column_list, cvs_file_directory):
    dump_script = 'COPY '
    dump_script += table_name
    dump_script += ' ('
    for i in range(len(column_list)):
        dump_script += column_list[i]
        dump_script += ' '    

        if (i != len(column_list)-1) :
           dump_script += ', '

    dump_script += ') From '
    dump_script += '\'' + cvs_file_directory + '\''
    dump_script += ' DELIMITER \',\';' 

    # print(dump_script)

    cur.execute(dump_script)
    conn.commit()


def count_N_column (cur,conn,name_of_table = 'person'):

    # cur = conn.cursor()
    cur.execute('select * from %s;'%(name_of_table))
    colnames = [desc[0] for desc in cur.description]
    # df = pd.DataFrame(data = cur.fetchall(), columns = colnames)
    # print(df)
    # print(df.iloc[0,0])

    # for record in cur.fetchall():
    #     print(type(record))
    #     print(record[1])

    conn.commit()

    return colnames

def print_table_in_db(cur,conn):
    table_list = []
    cur.execute("""SELECT table_name FROM information_schema.tables
       WHERE table_schema = 'public'""")
    for table in cur.fetchall():
        print('tabel in the DB : ',table)
        table_list.append(table)
    conn.commit()

    return table_list

#note : fill the rows in the calibration database
def fill_calib_data (cur, conn, table_name, data_server, file_name, map_file, N_or_S, ROC, ladder_pos, module_id, port, status, run_mode, threshold, run_time):
    # note : len(ladder_pos) should equal to len(module_id)

    # note : the number of the rows added depends on the Length of the ladder_pos 
    for i in range (len(ladder_pos)):
        insert_script = 'INSERT INTO ' + table_name + '( '+ header_name_cali[0] + ', ' # note : the first column is the timestamp, should be added automatically
        insert_script += (header_name_cali[1] + ', ') # note : data_server
        insert_script += (header_name_cali[2] + ', ') # note : cali_file
        insert_script += (header_name_cali[3] + ', ') # note : map file
        insert_script += (header_name_cali[4] + ', ') # note : cali_NS
        insert_script += (header_name_cali[5] + ', ') # note : ROC
        insert_script += (header_name_cali[6] + ', ') # note : ladder pos
        insert_script += (header_name_cali[7] + ', ') # note : module ID
        insert_script += (header_name_cali[8] + ', ') # note : port
        insert_script += (header_name_cali[11]+ ', ') # note : status
        insert_script += (header_name_cali[13]+ ', ') # note : run_mode
        insert_script += (header_name_cali[14]+ ', ') # note : threshold
        insert_script += (header_name_cali[15])       # note : run_time

        insert_script += ') VALUES ( ' + 'current_timestamp, '

        insert_script += ('\'' + data_server             + '\'' + ', ')     # note : data_server
        insert_script += ('\'' + file_name               + '\'' + ', ')     # note : file name
        insert_script += ('\'' + map_file                + '\'' + ', ')     # note : map file
        insert_script += ('\'' + N_or_S                  + '\'' + ', ')     # note : N_or_S
        insert_script += ('\'' + ROC                     + '\'' + ', ')     # note : ROC
        insert_script += ('\'' + ladder_pos[i]           + '\'' + ', ')     # note : ladder pos, it's a list
        insert_script += (       '%s'%(module_id[i])            + ', ')     # note : module ID, it's a list
        insert_script += ('\'' + port[i]                 + '\'' + ', ')     # note : port ID, it's a list
        insert_script += ('\'' + status[i]               + '\'' + ', ')     # note : status, it's a list
        insert_script += ('\'' + run_mode                + '\'' + ', ')     # note : run_mode
        insert_script += (       '%s'%(threshold)               + ', ')     # note : threshold
        insert_script += (       '%s'%(run_time)                + ')')      # note : run_time
        print('the insertion script : ',insert_script)
        cur.execute(insert_script)
        
    conn.commit()



# note : the function is only for cali
# note : to fill the rest of the information manually
def fill_psql_row_rest_id (cur,conn, cali_id = -300, current_ab = [], Tab = [], cali_status = 9999, table_name = cali_info_table_name, list_column = header_name_cali):

    Cab_ticket    = 1 if len(current_ab) == 2 else 0
    Tab_ticket    = 1 if len(Tab)        == 2 else 0
    status_ticket = 1 if cali_status    != 9999 else 0

    if (Cab_ticket + Tab_ticket + status_ticket == 0 or cali_id == -300) :
        print('It seems to be no input or the ID is wrong? close this function')
        return

    print('--------------------Please confirm your input--------------------')
    print('The calibration id : %s' %(cali_id))

    print('Current A & B :     %s' %(current_ab)) if Cab_ticket == 1 else print('',end='')
    print('Temperature A & B : %s' %(Tab)) if Tab_ticket == 1 else print('',end='')
    print('ladder status : %s' %(cali_status)) if status_ticket else print('',end='')

    print('--------- Is the input correct ? Please type (YES or NO) ---------')
    print('Your answer : ', end=' ')
    the_input_from_user = input()
    print(" ")

    confirm_ticket = False

    if (the_input_from_user == 'YES'): # note : confirmed, delete the data
        print('Confirmed, updating the row.')
        confirm_ticket = True

    elif (the_input_from_user == 'NO'):
        print('The input is NO, doing nothing.')
    
    else :
        print('Not quite sure what you mean, do nothing.')


    if (confirm_ticket == True):
        # note : update current AB (array)
        if (Cab_ticket == 1):
            insert_script = 'UPDATE ' + table_name + ' SET ' + list_column[5] + ' = ' + 'ARRAY[%s,%s]'%(current_ab[0],current_ab[1]) + ' WHERE id = ' + '%s ;'%(cali_id)
            # print(insert_script)
            cur.execute(insert_script)

        if (Tab_ticket == 1):
            # note : update T AB (array)
            insert_script = 'UPDATE ' + table_name + ' SET ' + list_column[6] + ' = ' + 'ARRAY[%s,%s]'%(Tab[0],Tab[1]) + ' WHERE id = ' + '%s ;'%(cali_id)
            # print(insert_script)
            cur.execute(insert_script)

        if (status_ticket == 1):
            # note : update status
            insert_script = 'UPDATE ' + table_name + ' SET ' + list_column[7] + ' = ' + '%s'%(cali_status) + ' WHERE id = ' + '%s ;'%(cali_id)
            # print(insert_script)
            cur.execute(insert_script)

        conn.commit()


# note : the function is only for cali
# note : to fill the rest of the information manually
def fill_psql_row_rest_name (cur, conn, file_name = 'no_name', ladder_pos = 'no_name', current_ab = [], Tab = [], cali_status = 9999, table_name = cali_info_table_name, list_column = header_name_cali):

    Cab_ticket    = 1 if len(current_ab) == 2 else 0
    Tab_ticket    = 1 if len(Tab)        == 2 else 0
    status_ticket = 1 if cali_status    != 9999 else 0

    if (Cab_ticket + Tab_ticket + status_ticket == 0 or file_name == 'no_name' or ladder_pos == 'no_name') :
        print('It seems to be no input or no file name or ladder pos?')
        return

    print('--------------------Please confirm your input--------------------')
    print('The calibration file name : %s' %(file_name))
    print('The calibration ladder pos : %s' %(ladder_pos))

    print('Current A & B :     %s' %(current_ab)) if Cab_ticket == 1 else print('',end='')
    print('Temperature A & B : %s' %(Tab)) if Tab_ticket == 1 else print('',end='')
    print('ladder status : %s' %(cali_status)) if status_ticket else print('',end='')

    print('--------- Is the input correct ? Please type (YES or NO) ---------')
    print('Your answer : ', end=' ')
    the_input_from_user = input()
    print(" ")

    confirm_ticket = False

    if (the_input_from_user == 'YES'): # note : confirmed, delete the data
        print('Confirmed, updating the row.')
        confirm_ticket = True

    elif (the_input_from_user == 'NO'):
        print('The input is NO, doing nothing.')
    
    else :
        print('Not quite sure what you mean, do nothing.')


    if (confirm_ticket == True):
        # note : update current AB (array)
        if (Cab_ticket == 1):
            insert_script = 'UPDATE ' + table_name + ' SET ' + list_column[5] + ' = ' + 'ARRAY[%s,%s]'%(current_ab[0],current_ab[1]) + ' WHERE ' + header_name_cali[1] + ' = ' + '\'' + file_name + '\'' + ' AND ' + header_name_cali[2] + ' = ' + '\'' +ladder_pos + '\'' + ';' 
            # print(insert_script)
            cur.execute(insert_script)

        if (Tab_ticket == 1):
            # note : update T AB (array)
            insert_script = 'UPDATE ' + table_name + ' SET ' + list_column[6] + ' = ' + 'ARRAY[%s,%s]'%(Tab[0],Tab[1]) + ' WHERE ' + header_name_cali[1] + ' = ' + '\'' + file_name + '\'' + ' AND ' + header_name_cali[2] + ' = ' + '\'' +ladder_pos + '\'' + ';' 
            # print(insert_script)
            cur.execute(insert_script)

        if (status_ticket == 1):
            # note : update status
            insert_script = 'UPDATE ' + table_name + ' SET ' + list_column[7] + ' = ' + '%s'%(cali_status) + ' WHERE ' + header_name_cali[1] + ' = ' + '\'' + file_name + '\'' + ' AND ' + header_name_cali[2] + ' = ' + '\'' +ladder_pos + '\'' + ';' 
            # print(insert_script)
            cur.execute(insert_script)

        conn.commit()



# note : show the data as function of time
def show_cali_table(cur,conn, table_name = cali_info_table_name) : 
    run_script = 'SELECT * FROM ' + table_name +' ORDER BY '+ header_name_cali[0] + ' DESC;'
    
    cur.execute(run_script)
    colnames = [desc[0] for desc in cur.description]
    df = pd.DataFrame(data = cur.fetchall(), columns = colnames)
    print(df)

    conn.commit()

# note : show the data of the specific ladder position as function of time.
def show_single_ladder(cur,conn,ladder_pos, table_name = cali_info_table_name) : 
    run_script = 'SELECT * FROM ' + table_name + ' WHERE ' + header_name_cali[2] + ' = ' + '\'' +ladder_pos + '\'' +' ORDER BY '+ header_name_cali[0] + ' DESC;'
    
    cur.execute(run_script)
    colnames = [desc[0] for desc in cur.description]
    df = pd.DataFrame(data = cur.fetchall(), columns = colnames)
    print(df)

    conn.commit()

#note : close the PSQL
def close_psql_server (cur,conn):
    print("closing the psql")
    if cur is not None : 
        cur.close()
    if conn is not None :
        conn.close()















# note : useless
# def update_DB_link_par(input_hostname, input_database, input_username, input_pwd, input_port_id):
#     hostname = input_hostname
#     database = input_database
#     username = input_username
#     pwd      = input_pwd
#     port_id  = input_port_id
#     print( 'hostname =', hostname )
#     print( 'database =', database )
#     print( 'username =', username )
#     print( 'pwd      =', pwd      ) 
#     print( 'port_id  =', port_id  )