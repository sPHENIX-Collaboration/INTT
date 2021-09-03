#include "FindLatestFile.hh"

#if defined( _WIN32 ) || defined( _WIN64 )
//#include <boost/date_time/posix_time/posix_time.hpp>
//#include "C:/local/boost_1_66_0_64/boost/config.hpp"
//#define BOOST_USER_CONFIG <C:/local/boost_1_66_0_64/boost/config/user.hpp>
//#include <C:/local/boost_1_66_0_64/boost/config.hpp>
//#include <C:/local/boost_1_66_0_64/boost/filesystem/path.hpp>
//#include <C:/local/boost_1_66_0_64/boost/filesystem/path.hpp>
//#include <C:\Program\ Files\ (x86)\Windows\ Kits\10\Include\10.0.10240.0\ucrt\direct.h>

#include "functions/msdirent.h"
//#include <filesystem>

string FindLatestFile( string dir_path ) { // ongoing...
//	string search_dir = "C:\\root_5.34.36\\macros";
	string search_dir = "C://root_5.34.36//macros";
	//gInterpreter->AddIncludePath("C:/local/boost_1_66_0_64");
/*
DIR* dp = opendir(search_dir.c_str());
	if (dp == NULL) {
		cerr << "The directory \"" << search_dir << "\" is not found." << endl;
		return "";
	}
*/

	return "";
}

#elif defined( __linux__ ) || defined( __APPLE__)

#include <dirent.h>
string FindLatestFile( string dir_path ){

  DIR* dp = opendir(dir_path.c_str());
  if (dp == NULL) {
    cerr << "The directory \"" << dir_path << "\" is not found." << endl;
    return "";
  }

  vector < string > vname;
  struct dirent* dent;
  // loop over dirent structures until NULL
  do
    {
      
      dent = readdir( dp );
      
      if( dent == NULL )
	break;

      // get the name of the file
      string name = dent->d_name;
      
      if( name == "." || name == ".." )
	continue;

      string suffix = "";
      if( name.find(".") == string::npos )
	suffix = name;
      else
	suffix = name.substr( name.find_last_of("."), name.size() - name.find_last_of(".")+1 );

      if( suffix != ".dat" )
	continue;

            
      vname.push_back( name );
    } 
  while( dent != NULL );
  
  // close the directory
  closedir(dp);

  // sort stored names
  sort( vname.begin() , vname.end() );
  
  return dir_path + vname[ vname.size()-1 ];
}

#endif // end of for linux and mac
