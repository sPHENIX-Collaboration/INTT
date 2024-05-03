#pragma once
#include <sys/types.h>
#include <sys/stat.h>

#include "constant_values.hh"

bool IsFileExist( const std::string& name )
{
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

string GetRunNum8digits( int run_num )
{
  return string( 8 - to_string(run_num).size(), '0' ) + to_string( run_num );
}

string GetRunType( int run_num = 26975 )
{
  for( int i=0; i<kRun_type_num; i++ )
    {
      string file_pre = kIntt_evt_dir + kRun_types[i] + "/"
	+ kRun_types[i] + "_intt";

      string file_suf = "-0000.evt";
      for( int j=0; j<kFelix_num; j++ )
	{
	  string file_path = file_pre + to_string( j ) + "-" + string( 8 - to_string(run_num).size(), '0' ) + to_string( run_num ) + file_suf;
	  cout << " - " << file_path << endl;
	  if( IsFileExist( file_path ) == true )
	    return kRun_types[i];
	    
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

  return ss_rtn.str();
}

int functions()
{

  GetRunType();
  return 0;
}
