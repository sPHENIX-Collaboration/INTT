#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include "HotChannelApply.h"

void HotChannelApply(bool MyMap[NServer][NModule][NChips][NChan], int RunNumber = 25152, int fnumber = 0, double rate_cut = 0)
{
  bool IsHotChan[14][27][128] = {false};
  std::string hot_file = hot_input_path + "intt" + to_string(fnumber) + "-000" + to_string(RunNumber) + "-0000_hot_list.txt";
  std::cout << hot_file << std::endl;

  std::ifstream file(hot_file.c_str());
  if (!file.is_open())
  {
    std::cerr << "Cannot find the file." << std::endl;
    return 1;
  }
  double cut;
  int module;
  int chip_id;
  int chan_id;
  std::string line;
  std::string output;
  int i = 0;
  while (std::getline(file, line))
  {
    if (i == 0)
    {
      i++;
      continue;
    }
    std::istringstream iss(line);

    iss >> cut >> module >> chip_id >> chan_id;
    if (cut >= rate_cut)
      IsHotChan[module][chip_id][chan_id] = true;
  }
  for (int i = 0; i < 14; i++)
  {
    for (int j = 0; j < 27; j++)
    {
      for (int k = 0; k < 128; k++)
      {
        MyMap[fnumber][i][j][k] = IsHotChan[i][j][k];
        if (IsHotChan[i][j][k] == true)
          std::cout << i << " " << j << " " << k << std::endl;
      }
    }
  }
  file.close();
};
