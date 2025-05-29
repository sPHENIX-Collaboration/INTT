#include "InttEvent.h"
#include "ConvertInttData.h"
R__LOAD_LIBRARY(libInttEvent.so)

void runConvertInttData(const char * filename)
{
  //std::string save_dir = "/home/phnxrc/INTT/commissioning/hit_files/";
<<<<<<< Updated upstream
  //std::string save_dir = "./";
  //std::string save_dir = "/bbox/commissioning/INTT/data/root_files/2024/";
  std::string save_dir = "/sphenix/tg/tg01/commissioning/INTT/data/root_files/2024/";
  
=======
  std::string save_dir = "./";
  //std::string save_dir = "/bbox/commissioning/INTT/commissioning/hit_files/";

>>>>>>> Stashed changes
  std::string tree_file_name = filename;
  while(tree_file_name.find("/") != std::string::npos)
  {
    tree_file_name = tree_file_name.substr(tree_file_name.find("/") + 1);
  }
  tree_file_name = tree_file_name.substr(0, tree_file_name.find("."));
  tree_file_name += "_event_base.root";
  //  tree_file_name += ".root";
  tree_file_name = save_dir + tree_file_name;

  std::cout << tree_file_name << std::endl;


  // read and generate InttTree

  if ( filename != NULL)
    {
      Init(tree_file_name.c_str());
      Run(filename);
    }

}
