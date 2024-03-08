#pragma once

#include <string>
#include <fstream>
#include <sstream>

class LadderConfig
{
public:
  int module_;
  std::string roc_port_;
  std::string ladder_name_;

  LadderConfig( int module, std::string port, std::string name ) :
    module_( module ),
    roc_port_( port ),
    ladder_name_( name )
  {};

  int GetModule(){ return module_; };
  std::string GetRocPort(){ return roc_port_; };
  std::string GetLadderName(){ return ladder_name_; };
};

class LadderMap
{
private:
  
public:
  std::vector < LadderConfig* > ladders_;
  std::string map_;
  LadderMap(){};
  LadderMap( std::string map ) : map_(map) 
  {
    std::ifstream ifs( map_.c_str() );
    if( ifs.fail() )
      {
	std::cerr << map_ << " is not found" << std::endl;
	return;
      }

    std::string line;
    while( getline( ifs, line ) )
      {
	if( line[0] == '#' || line[0] == ' ' || line == "" )
	  continue;

	std::stringstream ss(line);
	int module;
	std::string roc_port, ladder_name;
	//std::string stemp;
	ss >> module >> roc_port >> ladder_name;
	
	ladders_.push_back( new LadderConfig( module, roc_port, ladder_name ) );
      }
  }

  LadderConfig* GetLadderConfig( int felix_ch )
  {
    for( auto& config : ladders_ )
      {
	if( config->module_ == felix_ch )
	  return config;
      }

    cerr << "LadderConfig* GetLadderConfig( int felix_ch = " << felix_ch << " )" << endl;
    cerr << "felix ch (module) " << felix_ch << " is not found in the ladder map " << map_ << endl;
    cerr << "Dummy config is returned." << endl;

    return new LadderConfig( -1, "XX", "Dummy!!!" );
  };
  
  void Print()
  {
    int width = 3 + (7 + 3 + 2 ) + (9 + 2 + 2) + (7 + 7) + 1;
    std::cout << std::string( width, '-' ) << std::endl;
    for( auto& it : ladders_ )
      std::cout << " - Module "   << std::setw(3) << it->GetModule() << ", "
	   << "ROC port " << it->GetRocPort() << ", "
	   << "Ladder "   << it->GetLadderName()
	   << std::endl;
    std::cout << std::string( width, '-' ) << std::endl;
  };

};
