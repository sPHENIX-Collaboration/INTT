#pragma once
#include <sys/types.h>
#include <sys/stat.h>

#include "constant_values.hh"

#include <boost/filesystem.hpp>


bool IsFileOrDirectoryExist(const string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool IsFileExist(const string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool IsDirectoryExist(const string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

namespace fs = boost::filesystem;
R__LOAD_LIBRARY( libboost_filesystem.so )
vector < string > GetFileList(const string& directory_path, const string& pattern="" ) 
{
    vector < string > entries;
    try 
      {
        for (auto& entry : fs::directory_iterator(directory_path) ) 
	  {
	    
            string name = entry.path().filename().string();
            if (fs::is_regular_file(entry.status())) 
	      {
                if (name.find(pattern) != string::npos) 
		  {
                    entries.push_back(name);
		  }
	      } 
            else if (fs::is_directory(entry.status())) 
	      {
                if (name.find(pattern) != string::npos) 
		  {
                    entries.push_back(name + "/");
		  }
	      }
	  }
      } 
    catch (const exception& e) 
      {
        cerr << "Error: " << e.what() << endl;
      }
    return entries;
}


string GetRunNum8digits( int run_num )
{
  return string( 8 - to_string(run_num).size(), '0' ) + to_string( run_num );
}

string GetRunType( int run_num = 26975 )
{

  for( int suffix_index=0; suffix_index<2; suffix_index++ )
    {
      string suffix = ".evt";
      if( suffix_index == 1 )
	suffix = ".prdf" ;
      
      for( int i=0; i<kRun_type_num; i++ )
	{
	  string file_pre = kIntt_evt_dir + kRun_types[i] + "/"
	    + kRun_types[i] + "_intt";
	  
	  string file_suf = "-0000" + suffix;
	  for( int j=0; j<kFelix_num; j++ )
	    {
	      string file_path = file_pre + to_string( j ) + "-" + string( 8 - to_string(run_num).size(), '0' ) + to_string( run_num ) + file_suf;
	      //cout << " - " << file_path << endl;
	      if( IsFileExist( file_path ) == true )
		{
		  cout << " " << file_path << " was found. The run type is " << kRun_types[i] << endl;
		  return kRun_types[i];
		}
	      
	    }
	  
	}
    }
  
  cerr <<  kIntt_evt_dir << "{run_type}/"
       << "{run_type}" << "_intt{FELIX}-"
       << string( 8 - to_string(run_num).size(), '0' ) + to_string( run_num )
       << "-0000.evt"
       << " is not found."
       << endl;
  return "NotFound";
}

/*!
  @brief A path to the raw data of run from intt_server is returned
  @details Only beam and cosmics runs are expected.
 */
string GetFilePath( int run, string run_type, int intt_server )
{
  assert( 0<=intt_server && intt_server <= 7 );
  
  stringstream ss_rtn;
  ss_rtn << kIntt_evt_dir << run_type << "/"
	 << run_type << "_intt" << intt_server << "-" << setw(8) << setfill( '0' ) << run << "-0000.evt";
  
  if( IsFileExist( ss_rtn.str() ) )
    return ss_rtn.str();
    
  ss_rtn.str( "" );
  ss_rtn << kIntt_evt_dir << run_type << "/"
	 << run_type << "_intt" << intt_server << "-" << setw(8) << setfill( '0' ) << run << "-0000.prdf";
  
  return ss_rtn.str();
  
}

vector < string > GetDsts( int run, bool is_official, string should_include="", string should_not_include="" )
{
  /*!
    @brief It returns a list of a path to DST files
    @param run The run number
    @param run_type The run type
    @param is_official A flag to use the official DST or not
    @param should_include You can require paths to include this string
    @param should_not_include You can require paths NOT to include this string
    @details This is used to get a path to DST files to be used.
    The arguments should_include/should_not_include is useful to include/exclude DST having no hot channels
   */
  string root_dir = kIntt_dst_dir;;

  cout << " DST search started..." << endl;
  string run_type = GetRunType( run );
  if( run_type == "NotFound" )
    {
      cerr << " Run type of run " << run << " is not found." << endl;
      cerr << " It means no evt/prdf files were found. It has to be accessible." << endl;
      cerr << " An empty list is returned." << endl;
      return vector < string >();
    }
  
  
  if( is_official == true )
    {

      root_dir = kOfficial_Dst_dir ;

      cout << " Official DST? Where is it?" << endl;
      string sub_dir = root_dir + "intt" + run_type;

      // temp solution for physics runs
      if( run_type == "physics" )
	 sub_dir = root_dir + "intt" + "beam";
      
      bool found = fs::exists( sub_dir );
      if( found == true )
	{
	  root_dir = sub_dir;
	}

      if( root_dir == kOfficial_Dst_dir )
	{
	  cerr << " Good directory is not found" << endl;
	  cerr << " An empty list is returned." << endl;
	  return vector < string >();
	}
      else
	{
	  cout << " OK, a subdirectory candidate found: " << root_dir << endl;
	}
      
      auto dir_list = GetFileList( root_dir ); // There must be only directories
      for( auto& dir: dir_list )
	{

	  int run_range_begin = stoi( dir.substr( string("run_").size(), 8 ) );
	  int run_range_end   = stoi( dir.substr( dir.find_last_of( "_" )+1, 8 ) );

	  if( run_range_begin <= run && run < run_range_end )
	    {
	      root_dir = root_dir + "/" + dir;
	      cout << " Subsub directory candidate found: " << root_dir << endl;
	      break;
	    }
	}
    }

  vector < string > rtn_candidates = GetFileList( root_dir, to_string(run) );
  sort( rtn_candidates.begin(), rtn_candidates.end() );
  vector < string > rtn;
  cout << " Then, this is the list of DST to be used;" << endl;
  for( auto& file : rtn_candidates )
    {

      if( should_include != "" && file.find( should_include ) == string::npos )
	continue;
      
      if( should_not_include != "" && file.find( should_not_include ) != string::npos )
	continue;

      rtn.push_back( root_dir + file );
      cout << "\t- " << root_dir + file << endl;

    }

  
  return rtn;
}

int functions()
{

  //GetRunType();
  auto a = GetDsts( 41300, true );
  
  auto b = GetDsts( 41300, false, "", "no_hot" );
  
  return 0;
}
