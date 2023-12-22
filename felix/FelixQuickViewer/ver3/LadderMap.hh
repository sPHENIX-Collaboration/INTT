#pragma once

#include <string>
#include <fstream>

class LadderConfig
{
public:
  int module_;
  string roc_port_;
  string ladder_name_;

  LadderConfig( int module, string port, string name ) :
    module_( module ),
    roc_port_( port ),
    ladder_name_( name )
  {};

  int GetModule(){ return module_; };
  string GetRocPort(){ return roc_port_; };
  string GetLadderName(){ return ladder_name_; };
};

class LadderMap
{
private:

  
public:
  vector < LadderConfig* > ladders_;
  
  LadderMap(){};
  LadderMap( string map )
  {
    ifstream ifs( map.c_str() );
    if( ifs.fail() )
      {
	cerr << map << " is not found" << endl;
	return;
      }

    string line;
    while( getline( ifs, line ) )
      {
	if( line[0] == '#' || line[0] == ' ' || line == "" )
	  continue;

	stringstream ss(line);
	int module;
	string roc_port, ladder_name;
	//string stemp;
	ss >> module >> roc_port >> ladder_name;
	
	ladders_.push_back( new LadderConfig( module, roc_port, ladder_name ) );
      }
  }

  void Print()
  {
    int width = 3 + (7 + 3 + 2 ) + (9 + 2 + 2) + (7 + 7) + 1;
    cout << string( width, '-' ) << endl;
    for( auto& it : ladders_ )
      cout << " - Module "   << setw(3) << it->GetModule() << ", "
	   << "ROC port " << it->GetRocPort() << ", "
	   << "Ladder "   << it->GetLadderName()
	   << endl;
    cout << string( width, '-' ) << endl;
  };

};
