#include "myfirstproject.h"
R__LOAD_LIBRARY(libmyfirstproject.so)

void myfirstproject(const char * filename)
{
  if(!filename)exit(1);

  std::string save_dir = "/bbox/commissioning/INTT/commissioning/hit_files/";

  std::string tree_file_name = filename;
  while(tree_file_name.find("/") != std::string::npos)
  {
      tree_file_name = tree_file_name.substr(tree_file_name.find("/") + 1);
  }

  tree_file_name = tree_file_name.substr(0, tree_file_name.find("."));
  tree_file_name += ".root";
  tree_file_name = save_dir + tree_file_name;

  std::cout << tree_file_name << std::endl;
  if(setup(tree_file_name))exit(1);

  pfileopen(filename);
  prun();

  std::cout << "Finished prun()" << std::endl;

  tree->SetDirectory(file);
  tree->Write();
  file->Write();
  file->Close();

  std::cout << "Wrote to file:" << std::endl;
  std::cout << "\t" << tree_file_name << std::endl;

  exit(0);
}
