#od -tx4 -w10000000 /sphenix/tg/tg01/commissioning/INTT/data/evt_files/pedestal/pedestal_intt$2-000$1-0000.evt |  sed 's/cafeff80 /cafeff80\'$'\n/g' | awk '{print " "$1" "$NF}' | cat -n | less
#od -tx4 -w10000000 /sphenix/tg/tg01/commissioning/INTT/data/evt_files/pedestal/pedestal_intt$2-000$1-0000.evt |  sed 's/cafeff80 /cafeff80\'$'\n/g' | cat -n
ddump -n 0 -g -s /sphenix/tg/tg01/commissioning/INTT/data/evt_files/pedestal/pedestal_intt$2-000$1-0000.evt | od -tx4 -w1000000 |   sed 's/cafeff80 /cafeff80\'$'\n/g' 
#ddump -n 0 -g -s /sphenix/tg/tg01/commissioning/INTT/data/evt_files/pedestal/pedestal_intt$2-000$1-0000.evt | od -tx4 -w1000000 |   sed 's/cafeff80 /cafeff80\'$'\n/g'  | awk '{print " "$1" "$NF}' | cat -n | less
#ddump -g -s  /sphenix/tg/tg01/commissioning/INTT/data/evt_files/pedestal/pedestal_intt$2-000$1-0000.evt 

