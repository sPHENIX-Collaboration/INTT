#ifndef __HOTCAHNNELAPPLY_H__
#define __HOTCAHNNELAPPLY_H__

const int NServer = 8;
const int NModule = 14;
const int NChips = 27; //NChips = # of chips + 1
const int NChan = 128;
std::string hot_input_path = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/HotChannelFinder/hotchannelmap/";
std::string root_input_path = "/sphenix/tg/tg01/commissioning/INTT/root_files/";

void HotChannelApply(bool MyMap[NServer][NModule][NChips][NChan],int RunNumber = 25152, int fnumber = 0,double rate_cut = 0);

#endif