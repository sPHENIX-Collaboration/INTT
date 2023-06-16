
# note : this code combines some functions from the file generator
# note : the npy is used -> note : we used the ".pkl" before.

import tkinter as tk
import glob, os
from tkinter import filedialog
import PSQL_for_cabling_cali as PCC

import time as time
from datetime import date

hostname = '/var/run/postgresql'
database = 'cw_calib_test'
username = 'inttdev'
pwd      = 'admin' # note : not pretty sure it is correct or not.
port_id  = '5432'

PCC.hostname = hostname
PCC.database = database
PCC.username = username
PCC.pwd = pwd
PCC.port_id = port_id

# note cehck the used parameters for linking the database
PCC.print_DB_link_par()

# note : initialize the psql
cur,conn = PCC.initialize_psql_server()

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def comma_remove(*args):
    return " ".join(args)


def print_selection():
    print("Selected number:", var_ladder.get())

def simple_print(*args,var_barrel):
    print("aaa",var_barrel)


def update_dropdown(*args,var_barrel,var_ladder,dropdown_ladder):
    print("button feedback : ",var_barrel)
    if (var_barrel == "B0L0" or var_barrel == "B0L1" or var_barrel == "NaN ") :
        opt_ladder = ["00", "01", "02","03","04","05","06","07","08","09","10","11"]
    else:
        opt_ladder = ["00", "01", "02","03","04","05","06","07","08","09","10","11","12","13","14","15"]
    var_ladder.set(opt_ladder[0])
    
    # note : so it deletes the items in the list, and use the for loop to add them back, one by one.
    dropdown_ladder['menu'].delete(0, 'end')
    for option in opt_ladder:
        dropdown_ladder['menu'].add_command(label=option, command=tk._setit(var_ladder, option))


def print_map():
    global check_print_tag
    check_print_tag = 1

    ROC1_check_tag = 0
    ROC2_check_tag = 0

    print("----- map content -----")
    
    # if( file_name_entry.get()[ len(file_name_entry.get())-4 : ] != '.txt' ) :
    #     print('check the output file name')
    #     return

    # # todo : maybe not need it !
    # if( os.path.isfile(file_name_entry.get()) ):
    #     print('File already exists')
    #     return

    for i in range (len(ladder_text)) :
        if (var_barrel[i].get() != "NaN "):
            ROC1_check_tag = 1
            break

    for i in range (len(ladder_text)) :
        if (var_barrel_2[i].get() != "NaN "):
            ROC2_check_tag = 1
            break

    if (ROC1_check_tag + ROC2_check_tag == 0):
        print('Nothing filled !!!')
        check_print_tag = 0
        return

    if(var_NS.get() == opt_NS[0]) : # note : NaN
        print('Wrong NS side !!!')
        check_print_tag = 0
        return

    if (ROC1_entry.get() + ROC2_entry.get() == ''):
        print("both ROC1 and ROC2 no entry ? ")
        return

    if ( ROC1_check_tag == 1 and ROC1_entry.get() == '' ):
        print('Felix ch. 0 should not be empty !')
        return

    if ( ROC2_check_tag == 1 and ROC2_entry.get() == '' ):
        print('Felix ch. 1 should not be empty !')
        return
    
    # print("File :", file_name_entry.get())
    print('#felix_ch       ROC_port        Ladder')

    if (ROC1_check_tag == 1):
        for i in range (len(ladder_text)) :
            if (var_barrel[i].get() != "NaN ") :
                if (var_sharp[i].get() == False) :
                    print('%s   %s  %s%s%s'%( var_module[i].get(), var_port[i].get(), var_barrel[i].get(),var_ladder[i].get(),var_NS.get()))
                elif (var_sharp[i].get() == True):
                    print('# %s   %s  %s%s%s'%( var_module[i].get(), var_port[i].get(), var_barrel[i].get(),var_ladder[i].get(),var_NS.get()))
    
    if (ROC2_check_tag == 1):
        print(" ")
        for i in range (len(ladder_text)) :
            if (var_barrel_2[i].get() != "NaN ") :
                if (var_sharp_2[i].get() == False) :
                    print('%s   %s  %s%s%s'%( var_module_2[i].get(), var_port_2[i].get(), var_barrel_2[i].get(),var_ladder_2[i].get(),var_NS.get()))
                elif (var_sharp_2[i].get() == True) :
                    print('# %s   %s  %s%s%s'%( var_module_2[i].get(), var_port_2[i].get(), var_barrel_2[i].get(),var_ladder_2[i].get(),var_NS.get()))

    # if (ROC1_entry.get() !=""):
    #     print('# Felix ch. 0 ',ROC1_entry.get())
    # if (ROC2_entry.get() !=""):
    #     print('# Felix ch. 1',ROC2_entry.get())
    # if (note1_entry.get() != ""):
    #     print('#',note1_entry.get())
    # if (note2_entry.get() != ""):
    #     print('#',note2_entry.get())
    # if (note3_entry.get() != ""):
    #     print('#',note3_entry.get())


def sharp_color(in_var, input_CB):
    if(in_var == False):
        input_CB.configure(bg='green')


def reset_ladder_part ():
    # note : reset ladder related thing
    for i in range (len(ladder_text)) :
        # note : reset barrel
        var_barrel[i].set(opt_barrrel[0])
        dropdown_barrel_list[i]['menu'].delete(0, 'end')

        var_barrel_2[i].set(opt_barrrel[0])
        dropdown_barrel_list_2[i]['menu'].delete(0, 'end')

        for option_barrel in opt_barrrel:
            dropdown_barrel_list  [i]['menu'].add_command(label=option_barrel, command=tk._setit(var_barrel[i], option_barrel,lambda value, i=i: update_dropdown(var_barrel = value, var_ladder = var_ladder[i],dropdown_ladder=dropdown_ladder_list[i])))
            dropdown_barrel_list_2[i]['menu'].add_command(label=option_barrel, command=tk._setit(var_barrel_2[i], option_barrel, lambda value, i=i: update_dropdown(var_barrel = value, var_ladder = var_ladder_2[i],dropdown_ladder=dropdown_ladder_list_2[i])))
            # dropdown_barrel_2['menu'].add_command(command=lambda value_2, i=i: update_dropdown(var_barrel = value_2, var_ladder = var_ladder_2[i],dropdown_ladder=dropdown_ladder_list_2[i]))
        
        # note : reset ladder 
        var_ladder[i].set(opt_ladder[0])
        dropdown_ladder_list[i]['menu'].delete(0, 'end')

        var_ladder_2[i].set(opt_ladder[0])
        dropdown_ladder_list_2[i]['menu'].delete(0, 'end')

        for option_ladder in opt_ladder:
            dropdown_ladder_list  [i]['menu'].add_command(label=option_ladder, command=tk._setit(var_ladder[i], option_ladder))
            dropdown_ladder_list_2[i]['menu'].add_command(label=option_ladder, command=tk._setit(var_ladder_2[i], option_ladder))
            
        # note : reset module 
        var_module[i].set(opt_module[0])
        dropdown_module_list[i]['menu'].delete(0, 'end')

        var_module_2[i].set(opt_module_2[0])
        dropdown_module_list_2[i]['menu'].delete(0, 'end')

        for option_module in opt_module:
            dropdown_module_list[i]['menu'].add_command(label=option_module, command=tk._setit(var_module[i], option_module))
        
        for option_module in opt_module_2:
            dropdown_module_list_2[i]['menu'].add_command(label=option_module, command=tk._setit(var_module_2[i], option_module))


        # note : reset port 
        var_port[i].set(opt_port[0])
        dropdown_port_list[i]['menu'].delete(0, 'end')

        var_port_2[i].set(opt_port[0])
        dropdown_port_list_2[i]['menu'].delete(0, 'end')

        for option_port in opt_port:
            dropdown_port_list[i]['menu'].add_command(label=option_port, command=tk._setit(var_port[i], option_port))
            dropdown_port_list_2[i]['menu'].add_command(label=option_port, command=tk._setit(var_port_2[i], option_port))

        

        # note : reset status 
        var_status[i].set(opt_status[0])
        dropdown_status_list[i]['menu'].delete(0, 'end')

        var_status_2[i].set(opt_status[0])
        dropdown_status_list_2[i]['menu'].delete(0, 'end')

        for option_status in opt_status:
            dropdown_status_list[i]['menu'].add_command(label=option_status, command=tk._setit(var_status[i], option_status))
            dropdown_status_list_2[i]['menu'].add_command(label=option_status, command=tk._setit(var_status_2[i], option_status))


        # note : reset sharp '#'
        var_sharp[i].set(value = False)
        var_sharp_2[i].set(value = False)
    
    print(bcolors.OKBLUE + "ladder part has been reset first" + bcolors.ENDC)

def reset_all():
    file_name_entry.delete(0, 'end')
    map_name_entry.delete(0, 'end')
    ROC1_entry.delete(0, 'end')
    ROC2_entry.delete(0, 'end')
    note1_entry.delete(0,'end')
    note2_entry.delete(0,'end')
    note3_entry.delete(0,'end')
    threshold_entry.delete(0,'end')
    runtime_entry.delete(0,'end')

    # note : run mode reset
    var_run_mode.set(opt_run_mode[0])

    global check_print_tag
    check_print_tag = 0

    # note : reset data_server
    var_server.set(opt_server[0])
    dropdown_server['menu'].delete(0, 'end')
    for option_server in opt_server :
        dropdown_server['menu'].add_command(label=option_server, command=tk._setit(var_server, option_server))

    # note : reset NS
    var_NS.set(opt_NS[0])
    dropdown_NS['menu'].delete(0, 'end')
    for option_NS in opt_NS :
        dropdown_NS['menu'].add_command(label=option_NS, command=tk._setit(var_NS, option_NS))

    # note : reset the ladder part info.
    reset_ladder_part()

    print(bcolors.OKBLUE + "Universal reset done" + bcolors.ENDC)

def find_latest_file():
    # file_directory = "/home/inttdev/work/cwshih/calib_db/fake_folder/"
    # note : now move to .npy file
    # todo : currently, it just finds the latest file regardless the title of the file. 
    # todo : It should be fine for now as we will do the data logger on time, in principle. 
    list_of_files = glob.glob(file_default_directory+"/*.npy")
    latest_file = max(list_of_files, key=os.path.getctime)
    # latest_file = latest_file.replace(file_directory,'')

    return latest_file

def update_file_latest ():
    file_name_entry.delete(0,'end')
    file_name_entry.insert('end', find_latest_file())


def browse_file():
    file_path = filedialog.askopenfilename(initialdir=file_default_directory)
    file_name_entry.delete(0,'end')
    file_name_entry.insert('end', file_path)

def read_map_info_sharp(path_in):
    mapping = []
    sharp_code = []
    with open( path_in ) as file :
        for line in file :

            # for comment out
            if line[0] == "#" :
                if (line.find('B0L0') != -1 or line.find('B0L1') != -1 or line.find('B1L0') != -1 or line.find('B1L1') != -1 ):
                    if ( len(line.replace('#','').split()) == 3 ):
                        mapping.append( line.replace('#','').split() )
                        sharp_code.append(1)
                        continue
                    else :
                        continue

                else :
                    continue

            # for empty line, does it work?
            if line[0] == "" :
                continue

            # if the number of elements is smaller than 2 (at least Felix ch and ROC port should be), skip it
            if len( line ) < 2 :
                continue
            if (line.find('B') == -1) :
                continue

            # print( line, line.split() )
            mapping.append( line.split() )
            sharp_code.append(0)

    print("----------------------------- The selected map ----------------------------------")
    print("!!!! You are using the # included map, it may not work, please check !!!!")
    for i in range (len(mapping)) :
        if (sharp_code[i] == 1) : 
            print('#',mapping[i],'< -- with #')
        else :
            print(mapping[i])

    return mapping, sharp_code

# note : it has a new function, the ROC_name reader
def read_map_info(path_in):
    mapping = []
    ROC_name_array = []
    with open( path_in ) as file :
        for line in file :

            # for comment out
            if line[0] == "#" :
                if (line.find('# ROC') != -1):
                    if (line.find('RC-') != -1):
                        ROC_name_array.append(line[line.find('RC-'):line.find('RC-')+5]) # note : RC-XY (X : number, Y : N or S)
                continue

            # for empty line, does it work?
            if line[0] == "" :
                continue

            # if the number of elements is smaller than 2 (at least Felix ch and ROC port should be), skip it
            if len( line ) < 2 :
                continue
            if (line.find('B') == -1) :
                continue

            # print( line, line.split() )
            mapping.append( line.split() )

    print("----------------------------- The selected map ----------------------------------")
    for data in  mapping :
        print(data)

    for data in ROC_name_array :
        print('ROC :', data)

    return mapping , ROC_name_array




def browse_map_file():
    map_path = filedialog.askopenfilename(initialdir=map_default_directory)
    map_name_entry.delete(0,'end')
    map_name_entry.insert('end', map_path)

# note : the function to find the file for map
# note : ultra stupid 

# note : map, find the latest file
def find_latest_file_map():
    # file_directory = "/home/inttdev/work/cwshih/calib_db/fake_folder/"
    list_of_files = glob.glob(map_default_directory+"/*.txt")
    latest_file = max(list_of_files, key=os.path.getctime)
    # latest_file = latest_file.replace(file_directory,'')

    return latest_file

# note : map, find the latest file, and update it
def update_file_latest_map ():
    map_name_entry.delete(0,'end')
    map_name_entry.insert('end', find_latest_file_map())

# note : map, print the map information
def load_map_print ():
    read_map_info(map_name_entry.get())

# note : map, print the map information with sharp
def load_map_print_sharp ():
    read_map_info_sharp(map_name_entry.get())


def apply_map ():
    # note : reset the ladder part first
    reset_ladder_part()

    inputmap , ROC_name_array = read_map_info(map_name_entry.get())

    ROC1_entry.delete(0,'end')
    ROC2_entry.delete(0,'end')

    # note : new feature, apply the ROC ID
    if (len(ROC_name_array) == 1):
        ROC1_entry.delete(0,'end')
        ROC1_entry.insert('end', ROC_name_array[0])
    if (len(ROC_name_array) == 2):
        ROC1_entry.delete(0,'end')
        ROC1_entry.insert('end', ROC_name_array[0])
        ROC2_entry.delete(0,'end')
        ROC2_entry.insert('end', ROC_name_array[1])
    
    ROC1_index = 0
    ROC2_index = 0

    # note : set the N or S based on the first entry
    if (inputmap[0][2][6] == "N") : 
        var_NS.set(opt_NS[1])
    elif (inputmap[0][2][6] == "S") : 
        var_NS.set(opt_NS[2])
    else : # note : unknown character
        var_NS.set(opt_NS[0])

    # note : SL 0 : module ID
    # note : SL 1 : port ID
    # note : SL 2 : ladder and NS
    for SL in inputmap : 
        if ( -1 < int(SL[0])  < 7 ) : # note : ROC1
            var_module[ROC1_index].set(SL[0]) # note : 0 ~ 6
            var_port[ROC1_index].set(SL[1])   # note : A1 ~ D3, it's not in the list, let's try
            var_barrel[ROC1_index].set(SL[2][0:4]) # note : B0L0, for example
            var_ladder[ROC1_index].set(SL[2][4:6]) # note : 06, for example 
            ROC1_index += 1
        elif ( 6 < int(SL[0])  < 14 ) : # note : ROC2
            var_module_2[ROC2_index].set(SL[0]) # note : 7 ~ 13
            var_port_2[ROC2_index].set(SL[1])   # note : A1 ~ D3, it's not in the list, let's try
            var_barrel_2[ROC2_index].set(SL[2][0:4]) # note : B0L0, for example
            var_ladder_2[ROC2_index].set(SL[2][4:6]) # note : 06, for example 
            ROC2_index += 1
    print(bcolors.OKGREEN + "map loaded (no sharp reader mode) " + bcolors.ENDC)

def empty_map_slot():
    map_name_entry.delete(0,'end')

def apply_map_sharp ():
    # note : reset the ladder part first
    reset_ladder_part()
    
    inputmap, input_sharp_code = read_map_info_sharp(map_name_entry.get())
    
    ROC1_index = 0
    ROC2_index = 0

    ROC1_index_sharp = 0
    ROC2_index_sharp = 0

    # note : set the N or S based on the first entry
    if (inputmap[0][2][6] == "N") : 
        var_NS.set(opt_NS[1])
    elif (inputmap[0][2][6] == "S") : 
        var_NS.set(opt_NS[2])
    else : # note : unknown character
        var_NS.set(opt_NS[0])

    # note : SL 0 : module ID
    # note : SL 1 : port ID
    # note : SL 2 : ladder and NS
    for SL in inputmap : 
        if ( -1 < int(SL[0])  < 7 ) : # note : ROC1
            var_module[ROC1_index].set(SL[0]) # note : 0 ~ 6
            var_port[ROC1_index].set(SL[1])   # note : A1 ~ D3, it's not in the list, let's try
            var_barrel[ROC1_index].set(SL[2][0:4]) # note : B0L0, for example
            var_ladder[ROC1_index].set(SL[2][4:6]) # note : 06, for example 
            ROC1_index += 1
        elif ( 6 < int(SL[0])  < 14 ) : # note : ROC2
            var_module_2[ROC2_index].set(SL[0]) # note : 7 ~ 13
            var_port_2[ROC2_index].set(SL[1])   # note : A1 ~ D3, it's not in the list, let's try
            var_barrel_2[ROC2_index].set(SL[2][0:4]) # note : B0L0, for example
            var_ladder_2[ROC2_index].set(SL[2][4:6]) # note : 06, for example 
            ROC2_index += 1

    # note : add the sharp
    for  i in range (len(inputmap)) :
        if ( -1 < int(inputmap[i][0])  < 7 ) : # note : ROC1
            if ( input_sharp_code[i] == 1 ) :
                var_sharp[ROC1_index_sharp].set(value=True)
            ROC1_index_sharp += 1
        elif ( 6 < int(inputmap[i][0])  < 14 ) : # note : ROC2
            if ( input_sharp_code[i] == 1 ) :
                var_sharp_2[ROC2_index_sharp].set(value=True)
            ROC2_index_sharp += 1
       

def get_tail_name (string_in):
    find_slash = string_in.rfind('/')
    return string_in[find_slash+1:] 

def get_directory (string_in):
    find_slash = string_in.rfind('/')
    return string_in[:find_slash]

def print_all():

    global check_print_tag
    ROC1_check_tag = 0
    ROC2_check_tag = 0
    print("")
    print("")
    print("")
    print("")
    print("")
    print("====================================================================================")
    
    for i in range (len(ladder_text)) :
        if (var_barrel[i].get() != "NaN "):
            ROC1_check_tag = 1
            break

    for i in range (len(ladder_text)) :
        if (var_barrel_2[i].get() != "NaN "):
            ROC2_check_tag = 1
            break
    

    if (ROC1_check_tag + ROC2_check_tag == 0):
        print( bcolors.WARNING + 'No ladder filled !!' + bcolors.ENDC )
        return
    
    if (var_server.get() == "NaN"):
        print( bcolors.WARNING + 'no server !!!' + bcolors.ENDC )
        return

    if (var_NS.get() == "NaN"):
        print( bcolors.WARNING + " No NS !!!" + bcolors.ENDC )
        return

    if( file_name_entry.get()[ len(file_name_entry.get())-4 : ] != '.npy' ) :
        print( bcolors.WARNING + 'check the output file name' + bcolors.ENDC )
        return
    
    if (ROC1_entry.get() + ROC2_entry.get() == ''):
        print( bcolors.WARNING + "both Felix ch. 0 and 1 no entry ? " + bcolors.ENDC )
        return

    if ( ROC1_check_tag == 1 and ROC1_entry.get() == '' ):
        print( bcolors.WARNING + 'Felix ch. 0 should not be empty !' + bcolors.ENDC )
        return

    if ( ROC2_check_tag == 1 and ROC2_entry.get() == '' ):
        print( bcolors.WARNING + 'Felix ch. 1 should not be empty !' + bcolors.ENDC )
        return
    if (threshold_entry.get() == ''):
        print( bcolors.WARNING + 'Check the threshold !' + bcolors.ENDC )
        return
    if (var_run_mode.get() == opt_run_mode[0]):
        print( bcolors.WARNING + 'Check the run mode !!' + bcolors.ENDC )
        return
    if (var_run_mode.get() == opt_run_mode[2] or var_run_mode.get() == opt_run_mode[3]): # note : pedestal run or DAC Scan
        if (runtime_entry.get() == ''):
            print( bcolors.WARNING + 'In the ',var_run_mode.get(),'mode, no run time !!!' + bcolors.ENDC )
            return


    check_print_tag = 1

    print("#Server :",var_server.get())
    print("#File :", file_name_entry.get())
    print("#Map  :", map_name_entry.get())
    print("#Side :", var_NS.get())
    print("#Threshold :",threshold_entry.get())
    print("#Run_mode :",var_run_mode.get())
    if (var_run_mode.get() == opt_run_mode[2] or var_run_mode.get() == opt_run_mode[3]):
        print("#Run_time :",runtime_entry.get())
    print('')

    if (ROC1_check_tag == 1):
        for i in range (len(ladder_text)) :
            if (var_barrel[i].get() != "NaN " and var_sharp[i].get() == False) :
                print("ROC :",ROC1_entry.get(),"Ladder :", var_barrel[i].get()+var_ladder[i].get(),"moduleID :",var_module[i].get(),"port :",var_port[i].get(),"status :",dict_status[var_status[i].get()])

    
    if (ROC2_check_tag == 1):
        print(" ")
        for i in range (len(ladder_text)) :
            if (var_barrel_2[i].get() != "NaN " and var_sharp_2[i].get() == False) :
                print("ROC :",ROC2_entry.get(),"Ladder :", var_barrel_2[i].get()+var_ladder_2[i].get(),"moduleID :",var_module_2[i].get(),"port :",var_port_2[i].get(),"status :",dict_status[var_status_2[i].get()])
    print('')
    if (note1_entry.get() != ""):
        print('#log1 :', note1_entry.get())
    if (note2_entry.get() != ""):
        print('#log2 :',note2_entry.get())
    if (note3_entry.get() != ""):
        print('#log3 :',note3_entry.get())

    if (ROC1_entry.get() !=""):
        print('#Felix port 0 :',ROC1_entry.get())
    if (ROC2_entry.get() !=""):
        print('#Felix port 1 :',ROC2_entry.get())

    load_map_print_sharp()  
    print(bcolors.OKCYAN + "You've just clicked the print " + bcolors.ENDC)  
    # for i in var_barrel :
    #     print(i.get())
    
    # for il in var_ladder :
    #     print(il.get())

def swap_info ():
    ROC1_space = ROC1_entry.get()
    ROC2_space = ROC2_entry.get()

    ROC1_entry.delete(0,'end')
    ROC1_entry.insert(0,ROC2_space)

    ROC2_entry.delete(0,'end')
    ROC2_entry.insert(0,ROC1_space)

    ROC1_sharp_space  = []
    ROC1_barrel_space = []
    ROC1_ladder_space = []
    # ROC1_module_space = []
    ROC1_port_space   = []

    ROC2_sharp_space  = []
    ROC2_barrel_space = []
    ROC2_ladder_space = []
    # ROC2_module_space = []
    ROC2_port_space   = []

    for i in range ( len(ladder_text) ):
        ROC1_sharp_space.append( var_sharp[i].get() )
        ROC1_barrel_space.append( var_barrel[i].get() )
        ROC1_ladder_space.append( var_ladder[i].get() )
        # ROC1_module_space.append( var_module[i].get() )
        ROC1_port_space.append( var_port[i].get() )

        ROC2_sharp_space.append( var_sharp_2[i].get() )
        ROC2_barrel_space.append( var_barrel_2[i].get() )
        ROC2_ladder_space.append( var_ladder_2[i].get() )
        # ROC2_module_space.append( var_module_2[i].get() )
        ROC2_port_space.append( var_port_2[i].get() )

    # note : swap the information
    for i in range (len(ladder_text)):
        var_status[i].set(opt_status[0])
        var_status_2[i].set(opt_status[0])

        var_sharp [i].set(ROC2_sharp_space[i])
        var_barrel[i].set(ROC2_barrel_space[i])
        var_ladder[i].set(ROC2_ladder_space[i])
        # var_module[i].set(ROC2_module_space[i])
        var_port  [i].set(ROC2_port_space[i])

        var_sharp_2 [i].set(ROC1_sharp_space[i])
        var_barrel_2[i].set(ROC1_barrel_space[i])
        var_ladder_2[i].set(ROC1_ladder_space[i])
        # var_module_2[i].set(ROC1_module_space[i])
        var_port_2  [i].set(ROC1_port_space[i])
    print(bcolors.OKCYAN + "You just did the Felix port swap correction (check the swap result !)" + bcolors.ENDC)

def swap_ROC_name ():
    ROC1_space = ROC1_entry.get()
    ROC2_space = ROC2_entry.get()

    ROC1_entry.delete(0,'end')
    ROC1_entry.insert(0,ROC2_space)

    ROC2_entry.delete(0,'end')
    ROC2_entry.insert(0,ROC1_space)
    
    print(bcolors.OKCYAN + "You just swapped the ROC names" + bcolors.ENDC)

def ladder_reset ():
    for i in range (len(ladder_text)):
        # note : set the sharp '#' to be true
        var_sharp[i].set(value = True)
        var_sharp_2[i].set(value = True)
        var_status[i].set(opt_status[0])
        var_status_2[i].set(opt_status[0])

def txt_saver_in_Fill_PSQL(): # note : for the pedestal and DAC0 scan, for now
    
    ROC1_check_tag = 0
    ROC2_check_tag = 0
    
    for i in range (len(ladder_text)) :
        if (var_barrel[i].get() != "NaN "):
            ROC1_check_tag = 1
            break

    for i in range (len(ladder_text)) :
        if (var_barrel_2[i].get() != "NaN "):
            ROC2_check_tag = 1
            break
    

    if (ROC1_check_tag + ROC2_check_tag == 0):
        print('No ladder filled !!')
        return
    
    if (var_server.get() == "NaN"):
        print('no server !!!')
        return

    if (var_NS.get() == "NaN"):
        print(" No NS !!!")
        return

    if( file_name_entry.get()[ len(file_name_entry.get())-4 : ] != '.npy' ) :
        print('check the output file name')
        return
    
    if (ROC1_entry.get() + ROC2_entry.get() == ''):
        print("both Felix ch. 0 and 1 no entry ? ")
        return

    if ( ROC1_check_tag == 1 and ROC1_entry.get() == '' ):
        print('Felix ch. 0 should not be empty !')
        return

    if ( ROC2_check_tag == 1 and ROC2_entry.get() == '' ):
        print('Felix ch. 1 should not be empty !')
        return
    if (threshold_entry.get() == ''):
        print('Check the threshold !')
        return
    if (var_run_mode.get() == opt_run_mode[0]):
        print('Check the run mode !!')
        return
    if (var_run_mode.get() == opt_run_mode[2] or var_run_mode.get() == opt_run_mode[3]): # note : pedestal run or DAC Scan
        if (runtime_entry.get() == ''):
            print('In the ',var_run_mode.get(),'mode, no run time !!!')
            return

    txt_file = open("%s/INTTT_Commissioning_%s.txt"%(elog_gen_directory,str(date.today().strftime("%b-%d-%Y"))), "a")  # note : append mode
    txt_file.write(comma_remove("\n"))
    txt_file.write(comma_remove("====================================================================================\n"))

    txt_file.write(comma_remove("#Server :",var_server.get(),"\n"))
    txt_file.write(comma_remove("#File :", file_name_entry.get(),"\n"))
    txt_file.write(comma_remove("#Map  : ", map_name_entry.get(),"\n"))
    txt_file.write(comma_remove("#Side :", var_NS.get(),"\n"))
    txt_file.write(comma_remove("#Threshold : ",threshold_entry.get(),"\n"))
    txt_file.write(comma_remove("#Run_mode :",var_run_mode.get(),"\n"))
    if (var_run_mode.get() == opt_run_mode[2] or var_run_mode.get() == opt_run_mode[3]):
        txt_file.write(comma_remove("#Run_time :",runtime_entry.get(),"\n"))
    
    txt_file.write(comma_remove('\n'))

    if (ROC1_check_tag == 1):
        for i in range (len(ladder_text)) :
            if (var_barrel[i].get() != "NaN " and var_sharp[i].get() == False) :
                txt_file.write(comma_remove("ROC :",ROC1_entry.get(),"Ladder :", var_barrel[i].get()+var_ladder[i].get(),"moduleID :",var_module[i].get(),"port :",var_port[i].get(),"status :",dict_status[var_status[i].get()],'\n'))

    
    if (ROC2_check_tag == 1):
        txt_file.write(comma_remove("\n"))
        for i in range (len(ladder_text)) :
            if (var_barrel_2[i].get() != "NaN " and var_sharp_2[i].get() == False) :
                txt_file.write(comma_remove("ROC :",ROC2_entry.get(),"Ladder :", var_barrel_2[i].get()+var_ladder_2[i].get(),"moduleID :",var_module_2[i].get(),"port :",var_port_2[i].get(),"status :",dict_status[var_status_2[i].get()],'\n'))
    
    txt_file.write(comma_remove('\n'))
    
    if (note1_entry.get() != ""):
        txt_file.write(comma_remove('#log1 :', note1_entry.get(),"\n"))
    if (note2_entry.get() != ""):
        txt_file.write(comma_remove('#log2 :',note2_entry.get(),"\n"))
    if (note3_entry.get() != ""):
        txt_file.write(comma_remove('#log3 :',note3_entry.get(),"\n"))

    if (ROC1_entry.get() !=""):
        txt_file.write(comma_remove('#Felix port 0 :',ROC1_entry.get(),"\n"))
    if (ROC2_entry.get() !=""):
        txt_file.write(comma_remove('#Felix port 1 :',ROC2_entry.get(),"\n"))

    # load_map_print_sharp()    
    # for i in var_barrel :
    #     print(i.get())
    
    # for il in var_ladder :
    #     print(il.get())


    txt_file.close()

    print( bcolors.OKGREEN + "elog txt gen for : ",file_name_entry.get(),"is done" + bcolors.ENDC)



def fill_psql ():

    global check_print_tag
    if (check_print_tag == 0):
        print(bcolors.WARNING + "!!! Check print first !!!" + bcolors.ENDC)
        return

    table_name = PCC.cali_info_table_name
    # file_name  = get_tail_name(file_name_entry.get())
    # map_file   = get_tail_name(map_name_entry.get())
    file_name       = file_name_entry.get() # note : now let's try to save the whole directory as well
    map_file        = map_name_entry.get()  # note : now let's try to save the whole directory as well
    N_or_S          = var_NS.get()
    data_server     = var_server.get()
    run_mode_word   = var_run_mode.get()
    threshold_word  = threshold_entry.get()
    if (runtime_entry.get() == ''):
        run_time_word = '-1'
    else :
        run_time_word = runtime_entry.get()

    ladder_pos_1 = [] # note : for the ROC 1
    module_id_1  = []
    port_1       = []
    status_1     = []
    ladder_pos_2 = [] # note : for the ROC 2
    module_id_2  = []
    port_2       = [] 
    status_2     = []

    # note : check the entry
    ROC1_check_tag = 0
    ROC2_check_tag = 0

    for i in range (len(ladder_text)) :
        if (var_barrel[i].get() != "NaN "):
            ROC1_check_tag = 1
            break

    for i in range (len(ladder_text)) :
        if (var_barrel_2[i].get() != "NaN "):
            ROC2_check_tag = 1
            break


    if (ROC1_check_tag + ROC2_check_tag == 0) :
        print('no entry, no entry filled to the PSQL')
        return

    if (ROC1_check_tag == 1) :
        ROC = ROC1_entry.get()

        for i in range (len(ladder_text)) :
            if (var_barrel[i].get() != "NaN " and var_sharp[i].get() == False):
                ladder_pos_1.append(var_barrel[i].get()+var_ladder[i].get()) 
                module_id_1.append(var_module[i].get())  
                port_1.append(var_port[i].get())
                status_1.append(dict_status[ var_status[i].get() ])      

        PCC.fill_calib_data (cur, conn, table_name, data_server, file_name, map_file, N_or_S, ROC, ladder_pos_1, module_id_1, port_1, status_1, run_mode_word, threshold_word, run_time_word)

    
    if (ROC2_check_tag == 1) :
        ROC = ROC2_entry.get()

        for i in range (len(ladder_text)) :
            if (var_barrel_2[i].get() != "NaN " and var_sharp_2[i].get() == False):
                ladder_pos_2.append(var_barrel_2[i].get()+var_ladder_2[i].get()) 
                module_id_2.append(var_module_2[i].get())  
                port_2.append(var_port_2[i].get())
                status_2.append(dict_status[ var_status_2[i].get() ])       

        PCC.fill_calib_data (cur, conn, table_name, data_server, file_name, map_file, N_or_S, ROC, ladder_pos_2, module_id_2, port_2, status_2, run_mode_word, threshold_word, run_time_word)
    
    print(bcolors.OKGREEN + 'PSQL updated' + bcolors.ENDC)

    if (var_run_mode.get() == opt_run_mode[2] or var_run_mode.get() == opt_run_mode[3]): # note : the pedestal and the DAC0 scan
        txt_saver_in_Fill_PSQL()
        file_name_entry.delete(0,'end') # note : the line is comment out, because the run_ana and fill_bad_channel need it.
        threshold_entry.delete(0,'end')
        runtime_entry.delete(0,'end')
        var_run_mode.set(opt_run_mode[0])
        print(bcolors.OKBLUE + "file_name, threshold, run_mode and run_time have been reset" + bcolors.ENDC)
    check_print_tag = 0 
    print(bcolors.OKBLUE + "print_tag has been reset" + bcolors.ENDC)
            
def close_win ():

    print(bcolors.WARNING  + "!!!!!! Remember to update the E-log !!!!!!" + bcolors.ENDC)
    time.sleep(2)

    PCC.close_psql_server(cur,conn)
    root.quit()


def clear_note_1():
    note1_entry.delete(0,'end')

def clear_note_2():
    note2_entry.delete(0,'end')

def clear_note_3():
    note3_entry.delete(0,'end')

def open_GUI_cmd():
    os.system('python3 PSQL_GUI.py &')

# note : this function can not handle two ROC IDs
def ROOT_ana_ladder_1():
    global check_print_tag
    if (check_print_tag == 0):
        print( bcolors.WARNING + "!!! Check print first !!!" + bcolors.ENDC)
        return
    
    # todo : the ROC ID is still not that functional
    if (ROC1_entry.get() != "" and ROC2_entry.get() != "" ):
        ROC_index = 999
    elif (ROC1_entry.get() != ""): # note : format : RC-?S
        ROC_index = int(ROC1_entry.get()[3])
    elif (ROC2_entry.get() != ""):
        ROC_index = int(ROC2_entry.get()[3])

    data_in_word =  '\\\"%s\\\"'%(file_name_entry.get().replace("npy","root"))
    map_in_word  =  '\\\"%s\\\"'%map_name_entry.get()
    threshold    =  int(threshold_entry.get())
    print("The ROC index : ",ROC_index) 
    os.system('nohup root -l -b -q /home/inttdev/work/cwshih/calib_db/file_logger_PSQL/ladder_ana/analyzer_run_PSQL.cpp\\(%s,%s,%i,%i\\)&>out.out&'%(data_in_word, map_in_word, threshold, ROC_index))

# note : try to handle 2 ROC IDs.
def ROOT_ana_ladder_2():
    global check_print_tag
    if (check_print_tag == 0):
        print(bcolors.WARNING + "!!! Check print first !!!" + bcolors.ENDC)
        return

    DataOut_directory = "/home/inttdev/data/IR/folder_cali_ana"    

    ROC1_check_tag = 0
    ROC2_check_tag = 0

    for i in range (len(ladder_text)) :
        if (var_barrel[i].get() != "NaN "):
            ROC1_check_tag = 1
            break

    for i in range (len(ladder_text)) :
        if (var_barrel_2[i].get() != "NaN "):
            ROC2_check_tag = 1
            break

    directory_in_word =  '\\\"%s\\\"'%(get_directory(file_name_entry.get()))
    map_in_word  =  '\\\"%s\\\"'%map_name_entry.get()
    directory_out_word = "\\\"%s\\\""%DataOut_directory
    file_name_word =  '\\\"%s\\\"'%(get_tail_name(file_name_entry.get()).replace("npy","root"))
    threshold    =  int(threshold_entry.get())

    if (ROC1_check_tag == 1):
        for i in range (len(ladder_text)) :
            if (var_barrel[i].get() != "NaN " and var_sharp[i].get() == False) :                
                ROC_index = int(ROC1_entry.get()[3])
                print("")
                print( bcolors.OKCYAN + "Job submission, ",ROC1_entry.get()," #FC",var_module[i].get() + bcolors.ENDC) 
                os.system("nohup root -l -b -q /home/inttdev/work/cwshih/calib_db/file_logger_PSQL/ladder_ana/analyzer_multi.cpp\\(%s,%s,%s,%s,%s,%i,%i\\)&>nohup.out&"%(var_module[i].get(), directory_in_word, map_in_word, directory_out_word, file_name_word, ROC_index, threshold))
                
                time.sleep(0.7)

    
    if (ROC2_check_tag == 1):
        print(" ")
        for i in range (len(ladder_text)) :
            if (var_barrel_2[i].get() != "NaN " and var_sharp_2[i].get() == False) :  
                ROC_index = int(ROC2_entry.get()[3])    
                print("")
                print( bcolors.OKCYAN + "Job submission, ",ROC2_entry.get()," #FC",var_module_2[i].get() + bcolors.ENDC)
                os.system("nohup root -l -b -q /home/inttdev/work/cwshih/calib_db/file_logger_PSQL/ladder_ana/analyzer_multi.cpp\\(%s,%s,%s,%s,%s,%i,%i\\)&>nohup.out&"%(var_module_2[i].get(), directory_in_word, map_in_word, directory_out_word, file_name_word, ROC_index, threshold))
                
                time.sleep(0.7)

    print(bcolors.OKGREEN + "Jobs submission done, please check the run status by \"htop\"" + bcolors.ENDC)

def print_all_final(N_bad_ch_map):

    ROC1_check_tag = 0
    ROC2_check_tag = 0
    print("")
    print("")
    print("")
    print("")
    print("")
    print("==================================== ! Final ! ================================================")
    
    for i in range (len(ladder_text)) :
        if (var_barrel[i].get() != "NaN "):
            ROC1_check_tag = 1
            break

    for i in range (len(ladder_text)) :
        if (var_barrel_2[i].get() != "NaN "):
            ROC2_check_tag = 1
            break
    

    if (ROC1_check_tag + ROC2_check_tag == 0):
        print('No ladder filled !!')
        return
    
    if (var_server.get() == "NaN"):
        print('no server !!!')
        return

    if (var_NS.get() == "NaN"):
        print(" No NS !!!")
        return

    if( file_name_entry.get()[ len(file_name_entry.get())-4 : ] != '.npy' ) :
        print('check the output file name')
        return
    
    if (ROC1_entry.get() + ROC2_entry.get() == ''):
        print("both Felix ch. 0 and 1 no entry ? ")
        return

    if ( ROC1_check_tag == 1 and ROC1_entry.get() == '' ):
        print('Felix ch. 0 should not be empty !')
        return

    if ( ROC2_check_tag == 1 and ROC2_entry.get() == '' ):
        print('Felix ch. 1 should not be empty !')
        return
    if (threshold_entry.get() == ''):
        print('Check the threshold !')
        return
    if (var_run_mode.get() == opt_run_mode[0]):
        print('Check the run mode !!')
        return
    if (var_run_mode.get() == opt_run_mode[2] or var_run_mode.get() == opt_run_mode[3]): # note : pedestal run or DAC Scan
        if (runtime_entry.get() == ''):
            print('In the ',var_run_mode.get(),'mode, no run time !!!')
            return

    print("#Server :",var_server.get())
    print("#File :", file_name_entry.get())
    print("#Map  : ", map_name_entry.get())
    print("#Side :", var_NS.get())
    print("#Threshold : ",threshold_entry.get())
    print("#Run_mode :",var_run_mode.get())
    if (var_run_mode.get() == opt_run_mode[2] or var_run_mode.get() == opt_run_mode[3]):
        print("#Run_time :",runtime_entry.get())
    
    print('')

    if (ROC1_check_tag == 1):
        for i in range (len(ladder_text)) :
            if (var_barrel[i].get() != "NaN " and var_sharp[i].get() == False) :
                print("ROC :",ROC1_entry.get(),"Ladder :", var_barrel[i].get()+var_ladder[i].get(),"moduleID :",var_module[i].get(),"port :",var_port[i].get(),"status :",dict_status[var_status[i].get()],"total bad :",N_bad_ch_map[var_module[i].get()])

    
    if (ROC2_check_tag == 1):
        print(" ")
        for i in range (len(ladder_text)) :
            if (var_barrel_2[i].get() != "NaN " and var_sharp_2[i].get() == False) :
                print("ROC :",ROC2_entry.get(),"Ladder :", var_barrel_2[i].get()+var_ladder_2[i].get(),"moduleID :",var_module_2[i].get(),"port :",var_port_2[i].get(),"status :",dict_status[var_status_2[i].get()],"total bad :",N_bad_ch_map[var_module_2[i].get()])
    print('')
    if (note1_entry.get() != ""):
        print('#log1 :', note1_entry.get())
    if (note2_entry.get() != ""):
        print('#log2 :',note2_entry.get())
    if (note3_entry.get() != ""):
        print('#log3 :',note3_entry.get())

    if (ROC1_entry.get() !=""):
        print('#Felix port 0 :',ROC1_entry.get())
    if (ROC2_entry.get() !=""):
        print('#Felix port 1 :',ROC2_entry.get())

    # load_map_print_sharp()    
    # for i in var_barrel :
    #     print(i.get())
    
    # for il in var_ladder :
    #     print(il.get())

def txt_saver_in_Fill_bad(N_bad_ch_map):
    
    ROC1_check_tag = 0
    ROC2_check_tag = 0
    
    for i in range (len(ladder_text)) :
        if (var_barrel[i].get() != "NaN "):
            ROC1_check_tag = 1
            break

    for i in range (len(ladder_text)) :
        if (var_barrel_2[i].get() != "NaN "):
            ROC2_check_tag = 1
            break
    

    if (ROC1_check_tag + ROC2_check_tag == 0):
        print('No ladder filled !!')
        return
    
    if (var_server.get() == "NaN"):
        print('no server !!!')
        return

    if (var_NS.get() == "NaN"):
        print(" No NS !!!")
        return

    if( file_name_entry.get()[ len(file_name_entry.get())-4 : ] != '.npy' ) :
        print('check the output file name')
        return
    
    if (ROC1_entry.get() + ROC2_entry.get() == ''):
        print("both Felix ch. 0 and 1 no entry ? ")
        return

    if ( ROC1_check_tag == 1 and ROC1_entry.get() == '' ):
        print('Felix ch. 0 should not be empty !')
        return

    if ( ROC2_check_tag == 1 and ROC2_entry.get() == '' ):
        print('Felix ch. 1 should not be empty !')
        return
    if (threshold_entry.get() == ''):
        print('Check the threshold !')
        return
    if (var_run_mode.get() == opt_run_mode[0]):
        print('Check the run mode !!')
        return
    if (var_run_mode.get() == opt_run_mode[2] or var_run_mode.get() == opt_run_mode[3]): # note : pedestal run or DAC Scan
        if (runtime_entry.get() == ''):
            print('In the ',var_run_mode.get(),'mode, no run time !!!')
            return

    txt_file = open("%s/INTTT_Commissioning_%s.txt"%(elog_gen_directory,str(date.today().strftime("%b-%d-%Y"))), "a")  # append mode
    txt_file.write(comma_remove("\n"))
    txt_file.write(comma_remove("====================================================================================\n"))

    txt_file.write(comma_remove("#Server :",var_server.get(),"\n"))
    txt_file.write(comma_remove("#File :", file_name_entry.get(),"\n"))
    txt_file.write(comma_remove("#Map  : ", map_name_entry.get(),"\n"))
    txt_file.write(comma_remove("#Side :", var_NS.get(),"\n"))
    txt_file.write(comma_remove("#Threshold : ",threshold_entry.get(),"\n"))
    txt_file.write(comma_remove("#Run_mode :",var_run_mode.get(),"\n"))
    if (var_run_mode.get() == opt_run_mode[2] or var_run_mode.get() == opt_run_mode[3]):
        txt_file.write(comma_remove("#Run_time :",runtime_entry.get(),"\n"))
    
    txt_file.write(comma_remove('\n'))

    if (ROC1_check_tag == 1):
        for i in range (len(ladder_text)) :
            if (var_barrel[i].get() != "NaN " and var_sharp[i].get() == False) :
                txt_file.write(comma_remove("ROC :",ROC1_entry.get(),"Ladder :", var_barrel[i].get()+var_ladder[i].get(),"moduleID :",var_module[i].get(),"port :",var_port[i].get(),"status :",dict_status[var_status[i].get()],"total bad :",N_bad_ch_map[var_module[i].get()],'\n'))

    
    if (ROC2_check_tag == 1):
        txt_file.write(comma_remove("\n"))
        for i in range (len(ladder_text)) :
            if (var_barrel_2[i].get() != "NaN " and var_sharp_2[i].get() == False) :
                txt_file.write(comma_remove("ROC :",ROC2_entry.get(),"Ladder :", var_barrel_2[i].get()+var_ladder_2[i].get(),"moduleID :",var_module_2[i].get(),"port :",var_port_2[i].get(),"status :",dict_status[var_status_2[i].get()],"total bad :",N_bad_ch_map[var_module_2[i].get()],'\n'))
    
    txt_file.write(comma_remove('\n'))
    
    if (note1_entry.get() != ""):
        txt_file.write(comma_remove('#log1 :', note1_entry.get(),"\n"))
    if (note2_entry.get() != ""):
        txt_file.write(comma_remove('#log2 :',note2_entry.get(),"\n"))
    if (note3_entry.get() != ""):
        txt_file.write(comma_remove('#log3 :',note3_entry.get(),"\n"))

    if (ROC1_entry.get() !=""):
        txt_file.write(comma_remove('#Felix port 0 :',ROC1_entry.get(),"\n"))
    if (ROC2_entry.get() !=""):
        txt_file.write(comma_remove('#Felix port 1 :',ROC2_entry.get(),"\n"))

    # load_map_print_sharp()    
    # for i in var_barrel :
    #     print(i.get())
    
    # for il in var_ladder :
    #     print(il.get())


    txt_file.close()

    print( bcolors.OKGREEN + "elog txt gen for : ",file_name_entry.get(),"is done" + bcolors.ENDC)

def Fill_Bad_ch_PSQL():
    global check_print_tag

    N_bad_ch_map = {}

    if (check_print_tag == 0):
        print( bcolors.WARNING + "!!! Check print first !!!" + bcolors.ENDC)
        return

    if (var_run_mode.get() != opt_run_mode[1]):
        print( bcolors.WARNING + "The mode is",var_run_mode.get()," No fill_bad for now !" + bcolors.ENDC)
        return

    inputmap, ROC_name_array = read_map_info(map_name_entry.get())

    for SL in inputmap : # note : several ladders
        bad_ch_array = []
        bad_ch_txt_path = file_default_directory + "/folder_cali_ana" + "/folder_%s"%get_tail_name(file_name_entry.get()).replace(".npy","") + "/%i_%s"%(int(SL[0]),SL[2])+"/%s_bad_ch.txt"%SL[2]

        print(bad_ch_txt_path)
        if (os.path.isfile(bad_ch_txt_path)!= True):
            print(bcolors.FAIL + 'File is not found, please check', bcolors.ENDC)
            return

        with open( bad_ch_txt_path ) as file : # note : in one ladder, several chips
            for line in file :
                bad_ch_array.append(line.split())

        table_name = PCC.cali_info_table_name
        cali_file_word   = '\'%s\''%file_name_entry.get()
        ladder_pos_word  = '\'%s\''%SL[2][0:6] # note : B?L???
        port_word        = '\'%s\''%SL[1]
        data_server_word = '\'%s\''%var_server.get()

        # note : current it just fills the first line, the total number of bad channels
        update_word = 'UPDATE %s SET total_n_bad_ch = %i WHERE cali_file = %s AND ladder_pos = %s AND module_id = %i AND port = %s AND data_server = %s'%(table_name, int(bad_ch_array[0][1]), cali_file_word, ladder_pos_word, int(SL[0]), port_word, data_server_word)
        print(update_word)
        # note : for the printing
        N_bad_ch_map[str(SL[0])] = str(bad_ch_array[0][1])

        cur.execute(update_word)
        conn.commit()


    print_all_final(N_bad_ch_map) # note : print the file rresults
    txt_saver_in_Fill_bad(N_bad_ch_map) # note : output the txt to the txt file, the directory is "elog_gen_directory"
    print( bcolors.OKGREEN + "Fill bad done" + bcolors.ENDC)

    # note : this is just for easy checking, use the "pdfjam" to merge the pdf files automatically. 
    # os.system(". /home/inttdev/data/IR/folder_cali_ana/pdfjam_script.sh %s"%("folder_"+get_tail_name(file_name_entry.get()).replace(".npy","")))
    os.system("pdfjam --fitpaper true /home/inttdev/data/IR/folder_cali_ana/%s/*_B?L????/Overall_ampl_total_turnOn_*.pdf -o /home/inttdev/data/IR/folder_cali_ana/%s/Overall_ampl_total_turnOn_all.pdf 2>/dev/null"%( "folder_"+get_tail_name(file_name_entry.get()).replace(".npy",""),"folder_"+get_tail_name(file_name_entry.get()).replace(".npy","") ))
    os.system("pdfjam --fitpaper true /home/inttdev/data/IR/folder_cali_ana/%s/*_B?L????/Overall_ampl_total_width_*.pdf -o /home/inttdev/data/IR/folder_cali_ana/%s/Overall_ampl_total_width_all.pdf 2>/dev/null"%( "folder_"+get_tail_name(file_name_entry.get()).replace(".npy",""), "folder_"+get_tail_name(file_name_entry.get()).replace(".npy","") ))
    os.system("pdfjam --fitpaper true /home/inttdev/data/IR/folder_cali_ana/%s/*_B?L????/Overall_ch_entry_B?L????.pdf -o /home/inttdev/data/IR/folder_cali_ana/%s/Overall_ampl_total_ch_entry_all.pdf 2>/dev/null"%( "folder_"+get_tail_name(file_name_entry.get()).replace(".npy",""), "folder_"+get_tail_name(file_name_entry.get()).replace(".npy","") ))
    os.system("pdfjam --fitpaper true /home/inttdev/data/IR/folder_cali_ana/%s/*_B?L????/Overall_ch_entry_ampl_*.pdf -o /home/inttdev/data/IR/folder_cali_ana/%s/Overall_ampl_total_ch_entry_ampl_all.pdf 2>/dev/null"%( "folder_"+get_tail_name(file_name_entry.get()).replace(".npy",""), "folder_"+get_tail_name(file_name_entry.get()).replace(".npy","") ))

    file_name_entry.delete(0,'end') # note : the function moves here because it shold be the end of one calibration run.
    threshold_entry.delete(0,'end')
    var_run_mode.set(opt_run_mode[0])
    runtime_entry.delete(0,'end')
    check_print_tag = 0

    print(bcolors.OKBLUE + "file_name, threshold, run_mode, run_time and print_tag have been reset" + bcolors.ENDC)

# note : ==============================================================function end==============================================================================
# note : create tkinter window
root = tk.Tk()
root.title("INTT Commissioning, Barrel calibration test")
# root.maxsize(800, 800)
# root.geometry("1100x1000")
root.resizable(width=1,height=0)
root.update()

check_print_tag = 0


ladder_text = ["Ladder0", "Ladder1", "Ladder2", "Ladder3", "Ladder4", "Ladder5", "Ladder6"]
roc_text = ["Felix port 0", "Felix port 1"]
file_name_text = "File name : "
module_text = "Channel "
NS_text = "North or South : "
map_name_text = "Calib map : "
port_text = "port"
status_text = "status"
server_text = "data_server"
sharp_text  = "add #"
threshold_text = "Threshold : "
runtime_text = "Run time (s) : "

opt_barrrel = ["NaN ","B0L0","B0L1","B1L0","B1L1"]
opt_ladder  = ["00", "01", "02","03","04","05","06","07","08","09","10","11"]
opt_module  = [0,1,2,3,4,5,6]
opt_module_2  = [7,8,9,10,11,12,13]

opt_NS      = ["NaN","N","S"]
opt_port    = ["A1","A2","A3","B1","B2","B3","C1","C2","C3","D1","D2","D3"]

opt_server = ["NaN", "inttdaq", "test_server"]

opt_run_mode = ["Run_mode","Calibration","Pedestal","DAC0_Scan"]

file_default_directory = "/home/inttdev/data/IR"
map_default_directory  = "/home/inttdev/map" 
elog_gen_directory     = "/home/inttdev/data/IR/auto_elog_gen"

dict_status = {
    'NC'      : 'no-comment',
    'Good'    : 'good',
    'HE'      : 'half-entry',
    'DE'      : 'double-entry',
    'Noisy'   : 'noisy',
    'Weird'   : 'Weird',
    'HRN'     : 'half-row-nothing',
    'MC'      : 'missing-channels',
    'CE'      : 'some-chip-empty',
    'Nothing' : 'nothing'
    # 'test'    : 'code-testing'
    }

opt_status = list(dict_status.keys())

var_barrel = [tk.StringVar(root) for i in range(len(ladder_text))]
var_ladder = [tk.StringVar(root) for i in range(len(ladder_text))]
var_module = [tk.StringVar(root) for i in range(len(ladder_text))]
var_port   = [tk.StringVar(root) for i in range(len(ladder_text))]
var_status = [tk.StringVar(root) for i in range(len(ladder_text))]
var_sharp  = [tk.IntVar(root) for i in range(len(ladder_text))]
dropdown_barrel_list = []
dropdown_module_list = []
dropdown_ladder_list = []
dropdown_port_list   = []
dropdown_status_list = []
CB_sharp_list        = []


var_barrel_2 = [tk.StringVar(root) for i in range(len(ladder_text))]
var_ladder_2 = [tk.StringVar(root) for i in range(len(ladder_text))]
var_module_2 = [tk.StringVar(root) for i in range(len(ladder_text))]
var_port_2   = [tk.StringVar(root) for i in range(len(ladder_text))]
var_status_2 = [tk.StringVar(root) for i in range(len(ladder_text))]
var_sharp_2  = [tk.IntVar(root) for i in range(len(ladder_text))]
dropdown_barrel_list_2 = []
dropdown_module_list_2 = []
dropdown_ladder_list_2 = []
dropdown_port_list_2   = []
dropdown_status_list_2 = []
CB_sharp_list_2        = []




n_row = 1
# note : for file name 
file_name_label = tk.Label(root, text=file_name_text)
file_name_label.grid(row=n_row, column=1)

file_name_entry = tk.Entry(root,width=65,textvariable=tk.StringVar(value=""))
file_name_entry.grid(row=n_row, column=2, columnspan = 7)


button_browse = tk.Button(root, text="Find", fg='black', command=browse_file)
button_browse.grid(row=n_row, column=8 )

button_latest = tk.Button(root, text="Latest", fg='black', command=update_file_latest)
button_latest.grid(row=n_row, column=9 )

# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========

n_row += 1
separate_bar0 = tk.Label(root, text="-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------")
separate_bar0.grid(row=n_row, column = 1, columnspan = 10)

# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========

n_row += 1
# note : for the map in
button_browse_map = tk.Button(root, text="Find", fg='black', command=browse_map_file)
button_browse_map.grid(row=n_row, column=1 )

button_latest_map = tk.Button(root, text="Latest", fg='black', command=update_file_latest_map)
button_latest_map.grid(row=n_row, column=2 )

button_print_map = tk.Button(root, text="Print", fg='blue', command=load_map_print)
button_print_map.grid(row=n_row, column=3 )

button_load_map = tk.Button(root, text="Load", fg='blue', command=apply_map)
button_load_map.grid(row=n_row, column=4 )

# note : for the sharp
button_print_map_sharp = tk.Button(root, text="Print#", fg='red', command=load_map_print_sharp)
button_print_map_sharp.grid(row=n_row, column=5 )
 
button_load_map_sharp = tk.Button(root, text="Load#", fg='red', command=apply_map_sharp)
button_load_map_sharp.grid(row=n_row, column=6 )


n_row += 1
map_name_label = tk.Label(root, text=map_name_text)
map_name_label.grid(row=n_row, column=1)

map_name_entry = tk.Entry(root,width=85,textvariable=tk.StringVar(value=""))
map_name_entry.grid(row=n_row, column=2, columnspan = 7)
# note : empty the map slot
button_map_empty = tk.Button(root, width = 5, text="Empty", fg='red', command=empty_map_slot)
button_map_empty.grid(row=n_row, column=9 )

# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========

n_row += 1
separate_bar0 = tk.Label(root, text="-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------")
separate_bar0.grid(row=n_row, column = 1, columnspan = 10)

# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========

# note : for the data_server
n_row += 1
server_label = tk.Label(root, text=server_text)
server_label.grid(row=n_row, column=1)

var_server = tk.StringVar(root)
var_server.set(opt_server[0])

dropdown_server = tk.OptionMenu(root, var_server, *opt_server)
dropdown_server.grid(row=n_row,column=2)

# note : for the N or S
NS_label = tk.Label(root, fg='blue',text=NS_text)
NS_label.grid(row=n_row, column=3)

var_NS = tk.StringVar(root)
var_NS.set(opt_NS[0])

dropdown_NS = tk.OptionMenu(root, var_NS, *opt_NS)
dropdown_NS.grid(row=n_row,column=4)

# note : for the threshold 
threshold_label = tk.Label(root, fg='black',text=threshold_text)
threshold_label.grid(row=n_row, column=5)

threshold_entry = tk.Entry(root, width=5,textvariable=tk.StringVar(value=""))
threshold_entry.grid(row=n_row, column=6)

# note : for run_mode
# run_mode_label = tk.Label(root, fg='blue',text=run_mode_text)
# run_mode_label.grid(row=n_row, column=3)

var_run_mode = tk.StringVar(root)
var_run_mode.set(opt_run_mode[0])

dropdown_run_mode = tk.OptionMenu(root, var_run_mode, *opt_run_mode)
dropdown_run_mode.grid(row=n_row,column=7)

# note : for the threshold 
runtime_label = tk.Label(root, fg='blue',text=runtime_text)
runtime_label.grid(row=n_row, column=8)

runtime_entry = tk.Entry(root, width=5,textvariable=tk.StringVar(value=""))
runtime_entry.grid(row=n_row, column=9)

# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========

n_row += 1
separate_bar1 = tk.Label(root, text="-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------")
separate_bar1.grid(row=n_row, column = 1, columnspan = 10)

# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========

# note : create 7 ladders selections
# note : ROC1
n_row += 1
n_column = 1

ROC1_label = tk.Label(root, text=roc_text[0])
ROC1_label.grid(row=n_row, column=n_column,padx=10)

ROC1_entry = tk.Entry(root,width=10,textvariable=tk.StringVar(value=""))
ROC1_entry.grid(row=n_row+1, column=n_column, rowspan = 2, padx=10)

swap_ROC_button = tk.Button(root, text="Swap ROC", fg='black', command=swap_ROC_name)
swap_ROC_button.grid(row=n_row+3, column=n_column )

swap_button = tk.Button(root, text="Swap all", fg='red', command=swap_info)
swap_button.grid(row=n_row+4, column=n_column )

ladder_reset_button = tk.Button(root, text="set #, status", fg='blue', command=ladder_reset)
ladder_reset_button.grid(row=n_row+5, column=n_column )

# note : for the add #, ROC1
for i in range( len(ladder_text) ):
    var_sharp[i].set(value = False)
    CB_sharp = tk.Checkbutton(root, text=sharp_text, variable=var_sharp[i],onvalue=True, offvalue=False, activebackground='pink')
    CB_sharp.grid(row=n_row,column=n_column+i+2)
    CB_sharp_list.append(CB_sharp)


n_row += 1
# note : for the ladder, ROC1 
for i in range( len(ladder_text) ):

    var_ladder[i].set(opt_ladder[0])

    dropdown_ladder = tk.OptionMenu(root, var_ladder[i], *opt_ladder)
    dropdown_ladder.grid(row=n_row+2,column=n_column+i+2)
    dropdown_ladder_list.append(dropdown_ladder)

# note : for the barrel, ROC1
for i in range( len(ladder_text) ):
    
    label_ID = tk.Label(root, fg = '#526F88', text=ladder_text[i])
    label_ID.grid(row=n_row,column=n_column+i+2)

    var_barrel[i].set(opt_barrrel[0])

    dropdown_barrel = tk.OptionMenu(root, var_barrel[i], *opt_barrrel,command=lambda value, i=i: update_dropdown(var_barrel = value, var_ladder = var_ladder[i],dropdown_ladder=dropdown_ladder_list[i]))
    dropdown_barrel.grid(row=n_row+1,column=n_column+i+2)
    dropdown_barrel_list.append(dropdown_barrel)



# note : for the port, ROC1
n_row += 3
for i in range( len(ladder_text) ):

    label_port = tk.Label(root, text=port_text)
    label_port.grid(row=n_row,column=n_column+i+2)

    var_port[i].set(opt_port[0])

    dropdown_port = tk.OptionMenu(root, var_port[i], *opt_port)
    dropdown_port.grid(row=n_row+1,column=n_column+i+2)
    dropdown_port_list.append(dropdown_port)



# note : for the module, ROC1
n_row += 2
for i in range( len(ladder_text) ):

    label_module = tk.Label(root, fg = '#00A2FF', text=module_text)
    label_module.grid(row=n_row,column=n_column+i+2)

    var_module[i].set(opt_module[0])

    dropdown_module = tk.OptionMenu(root, var_module[i], *opt_module)
    dropdown_module.grid(row=n_row+1,column=n_column+i+2)
    dropdown_module_list.append(dropdown_module)



# note : for the status, ROC1
n_row += 2
for i in range( len(ladder_text) ):

    label_status = tk.Label(root, text=status_text)
    label_status.grid(row=n_row,column=n_column+i+2)

    var_status[i].set(opt_status[0])

    dropdown_status = tk.OptionMenu(root, var_status[i], *opt_status)
    dropdown_status.grid(row=n_row+1,column=n_column+i+2)
    dropdown_status_list.append(dropdown_status)

# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========

n_row += 2
separate_bar2 = tk.Label(root, text="-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------")
separate_bar2.grid(row=n_row, column = 1, columnspan = 10)

# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========


# note : ==================================================================================================================================

# note : for the ROC2
n_row += 2

ROC2_label = tk.Label(root, text=roc_text[1])
ROC2_label.grid(row=n_row, column=n_column,padx=10)
ROC2_entry = tk.Entry(root,width=10,textvariable=tk.StringVar(value=""))
ROC2_entry.grid(row=n_row+1, column=n_column, rowspan = 2, padx=10)


# note : for the add #, ROC2
for i in range( len(ladder_text) ):
    var_sharp_2[i].set(value = False)
    CB_sharp_2 = tk.Checkbutton(root, text=sharp_text, variable=var_sharp_2[i],onvalue=True, offvalue=False, activebackground='pink')
    CB_sharp_2.grid(row=n_row,column=n_column+i+2)
    CB_sharp_list_2.append(CB_sharp_2)



n_row += 1


# note : for the ladder, ROC2
for i in range( len(ladder_text) ):

    var_ladder_2[i].set(opt_ladder[0])
    dropdown_ladder_2 = tk.OptionMenu(root, var_ladder_2[i], *opt_ladder)
    dropdown_ladder_2.grid(row=n_row+2,column=n_column+i+2)
    dropdown_ladder_list_2.append(dropdown_ladder_2)

# note : for the barrel, ROC2
for i in range( len(ladder_text) ):

    label_ID_2 = tk.Label(root, fg = '#526F88', text=ladder_text[i])
    label_ID_2.grid(row=n_row,column=n_column+i+2)

    var_barrel_2[i].set(opt_barrrel[0])
    dropdown_barrel_2 = tk.OptionMenu(root, var_barrel_2[i], *opt_barrrel,command=lambda value_2, i=i: update_dropdown(var_barrel = value_2, var_ladder = var_ladder_2[i],dropdown_ladder=dropdown_ladder_list_2[i]))
    dropdown_barrel_2.grid(row=n_row+1,column=n_column+i+2)
    dropdown_barrel_list_2.append(dropdown_barrel_2)

# note : for the port, ROC2
n_row += 3
for i in range( len(ladder_text) ):

    label_port_2 = tk.Label(root, text=port_text)
    label_port_2.grid(row=n_row,column=n_column+i+2)

    var_port_2[i].set(opt_port[0])

    dropdown_port_2 = tk.OptionMenu(root, var_port_2[i], *opt_port)
    dropdown_port_2.grid(row=n_row+1,column=n_column+i+2)
    dropdown_port_list_2.append(dropdown_port_2)

# note : for the module, ROC2
n_row += 2
for i in range( len(ladder_text) ):

    label_module_2 = tk.Label(root, fg = '#00A2FF', text=module_text)
    label_module_2.grid(row=n_row,column=n_column+i+2)

    var_module_2[i].set(opt_module_2[0])
    dropdown_module_2 = tk.OptionMenu(root, var_module_2[i], *opt_module_2)
    dropdown_module_2.grid(row=n_row+1,column=n_column+i+2)
    dropdown_module_list_2.append(dropdown_module_2)




# note : for the status, ROC2
n_row += 2
for i in range( len(ladder_text) ):

    label_status_2 = tk.Label(root, text=status_text)
    label_status_2.grid(row=n_row,column=n_column+i+2)

    var_status_2[i].set(opt_status[0])

    dropdown_status_2 = tk.OptionMenu(root, var_status_2[i], *opt_status)
    dropdown_status_2.grid(row=n_row+1,column=n_column+i+2)
    dropdown_status_list_2.append(dropdown_status_2)


# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========

n_row += 2
separate_bar3 = tk.Label(root, text="-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------")
separate_bar3.grid(row=n_row, column = 1, columnspan = 10)

# ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= ========= =========


# note : for the note 
n_row += 3
note1_button = tk.Button(root, text="Note 1 : ", height= 1, command=clear_note_1)
note1_button.grid(row=n_row, column=1)

note1_entry = tk.Entry(root,width=95,textvariable=tk.StringVar(value=""))
note1_entry.grid(row=n_row, column=2, columnspan = 8)

n_row += 1
note2_button = tk.Button(root, text="Note 2 : ", height= 1, command=clear_note_2)
note2_button.grid(row=n_row, column=1)

note2_entry = tk.Entry(root,width=95,textvariable=tk.StringVar(value=""))
note2_entry.grid(row=n_row, column=2, columnspan = 8)

n_row += 1
note3_button = tk.Button(root, text="Note 3 : ", height= 1, command=clear_note_3)
note3_button.grid(row=n_row, column=1)

note3_entry = tk.Entry(root,width=95,textvariable=tk.StringVar(value=""))
note3_entry.grid(row=n_row, column=2, columnspan = 8)

# note : for the print
n_row += 1
button_print = tk.Button(root, text="Print", command=print_all)
button_print.grid(row=n_row, column=1 )

# note : fill to psql
button_psql = tk.Button(root, text="Fill PSQL", command=fill_psql)
button_psql.grid(row=n_row, column=2 )

# note : Run the ROOT ana, 
button_ROOT = tk.Button(root, text="Run ana", command=ROOT_ana_ladder_2)
button_ROOT.grid(row=n_row, column=3 )

# note : FIll the # of bad channels
button_Fill_bad_ch = tk.Button(root, text="Fill bad", fg='blue', command=Fill_Bad_ch_PSQL)
button_Fill_bad_ch.grid(row=n_row, column=4 )

# note : Open to psql GUI
button_GUI = tk.Button(root, text="Open PSQL", command=open_GUI_cmd)
button_GUI.grid(row=n_row, column=6 )

# note : for rest
button_reset = tk.Button(root, text="Reset", fg='red', command=reset_all)
button_reset.grid(row=n_row, column=8 )

# note : for close the window
button_close = tk.Button(root, text="Close", fg='red', command=close_win)
button_close.grid(row=n_row, column=9 )


root.mainloop()
