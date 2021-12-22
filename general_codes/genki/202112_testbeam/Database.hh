/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
    Database class -- Get run information easily! --
// ------------------------------------------------------------------------------- //  

  How To Use
    1. Generate database from the run_list (Google spread sheet)
         It's in https://docs.google.com/spreadsheets/d/1XZ7cTRjl9ra9Fn2CyHDyzguJkPW6anMDin0mukZS7D0/edit#gid=0
         1-1 File > Download > tab separated vertial (.tsv)
         1-2 Put the tsv file as you want
    2. Include this file:
         #include "Database.hh"
    3. Make an instance of this class:
         string database_file = "here/there/everywhere/database.tsv";
         Database* db = new Database( database_file );
    4. Set a data file as a run to be checked:
         string data = "data/NWU_fphx_raw_20211128-0947_0.root";
         db->SetRun( data );
    5. Get information, for example:
         string ROC = db->GetRoc();
         int ladder_num = db->GetLadderNumInUse();
         vector < int > modules = db->GetModules();
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

/* a class contains information of a run ( one row in the database) */
class Run
{
public:
  int run_num_ = -1;
  string data_ = "";
  string source_ = "";
  string DAQ_mode_ = "";
  string setup_ver_ = "";
  string roc_ = "";
  string fem_top_ = "";
  string fem_bottom_ = "";
  int ladder_num_in_use_ = -1;
  vector < string > ladders_;
  vector < string > roc_ports_;
  vector < int > modules_;
  vector < string > conversion_;
  vector < string > bus_extenders_;
  vector < string > camac_adcs_;
  vector < string > camac_tdcs_;
  string description_ = "";

  Run(){};
  Run( string header, string line );

  // to count the number of channel for CAMAC ADC and TDC
  int CountWords( string line, string words );
  string GetModuleCut(); // something like: (module==6||module==8||module==1)
  
  // add elements in the vector variable (2nd argument)
  template < class T >
  void PushElements( stringstream& ss, vector < T >& values, int num );
  
  void Print();  

  template < class T >
  void PrintVector( T& values, string separator = " ", string last = "\n" );
};

string Run::GetModuleCut()
{
  string cut = "(";
  for( auto& module : modules_ )
    {
      cut += "module==" + to_string( module ) + "||";
    }

  cut = cut.substr( 0, cut.size() - 2 ) + ")"; // remove the last ||
  return cut;
}

template < class T >
void Run::PushElements( stringstream& ss, vector < T >& values, int num )
{
  // repeat the processes (read->push_back)
  for( int i=0; i<num; i++ )
    {
      T value;
      ss >> value;
    values.push_back( value );
    }
 
}

int Run::CountWords( string line, string words )
{
  int counter = 0;

  // repeat loop until the key words is not found in the rest of line
  while( line.find( words ) != string::npos )
    {
      counter++;

      // remove part of line
      line = line.substr( line.find( words ) + words.size(), line.size() );
    }

  return counter;
}

// constructor
Run::Run( string header, string line )
{

  // read simply 
  stringstream ss( line );
  ss >> run_num_ >> data_ >> source_ >> DAQ_mode_ >> setup_ver_ >> roc_
     >> fem_top_ >> fem_bottom_ >> ladder_num_in_use_;

  // read 4 elements and put them in ladders_ 
  PushElements( ss, ladders_, 4 );
  PushElements( ss, roc_ports_, 4 );

  // module config in the database is something like "615". It needs to be separated
  string stemp;
  ss >> stemp;

  // loop over all words
  for( int i=0; i<stemp.size(); i++ )
    modules_.push_back( stoi( string(1,stemp[i]) ) );

  PushElements( ss, conversion_, 4 );
  PushElements( ss, bus_extenders_, 4 );

  PushElements( ss, camac_adcs_, CountWords(header, "CAMAC ADC" ) );
  PushElements( ss, camac_tdcs_, CountWords(header, "CAMAC TDC" ) );

  // the remains in this line is description about this run
  description_ = line.substr( (int)ss.tellg() + 1 );
}

template < class T >
void Run::PrintVector( T& values, string separator, string last )
{
  for( auto& value : values )
    cout << value << separator;

  cout << last;
}

void Run::Print()
{
  int width = 10;
  cout << "=== Run ===" << endl;
  cout << setw(width) << left << " - Data" << ": " << data_ << endl;
  cout << setw(width) << left << " - Source" << ": " << source_ << endl;

  cout << setw(width) << left << " - DAQ mode" << ": " << DAQ_mode_ << endl;
  cout << setw(width) << left << " - Setup ver" << ": " << setup_ver_ << endl;
  cout << setw(width) << left << " - ROC" << ": " << roc_ << endl;
  cout << setw(width) << left << " - FEM top" << ": " << fem_top_ << endl;
  cout << setw(width) << left << " - FEM bottom" << ": " << fem_bottom_ << endl;
  cout << setw(width) << left << " - #Ladder in use" << ": " << ladder_num_in_use_ << endl;
  cout << setw(width) << left << " - Ladders" << ": ";
  PrintVector( ladders_ );

  cout << setw(width) << left << " - ROC ports" << ": ";
  PrintVector( roc_ports_ );

  cout << setw(width) << left << " - Modules" << ": ";
  PrintVector( modules_ );

  cout << setw(width) << left << " - Conversion" << ": ";
  PrintVector( conversion_ );

  cout << setw(width) << left << " - Bus Extenders" << ": ";
  PrintVector( bus_extenders_ );

  cout << setw(width) << left << " - CAMAC ADC" << ": ";
  PrintVector( camac_adcs_ );

  cout << setw(width) << left << " - CAMAC TDC" << ": ";
  PrintVector( camac_tdcs_ );

  cout << setw(width) << left << " - description" << ": " << description_ << endl;

}

/*! a class to provide run infromation */
class Database
{
private :
  string file_; // file path to the database
  vector < Run* > runs_; // runs

  int current_run_num_ = -1;
  Run* current_run_;
  
  void Init();
public:
  Database( string file = "documents/run_list - Setup.tsv" );

  bool SetRun( string data ); // you need to focus on a run by setting it
  bool SetRun( int run_num ); // you can set the run nummber
  Run* GetRun(){ return current_run_;}; // if you can treat Run class, it may be good to get it
  vector < Run* > GetRuns(){ return runs_;};
  
  int GetId				(){ return current_run_->run_num_		;};
  string GetData			(){ return current_run_->data_			;};
  TDatime GetDatime                     ();
  string GetSource			(){ return current_run_->source_		;};
  string GetDAQMode			(){ return current_run_->DAQ_mode_		;};
  string GetSetupVer			(){ return current_run_->setup_ver_		;};
  string GetRoc				(){ return current_run_->roc_			;};
  string GetRoot			();
  string GetFemTop			(){ return current_run_->fem_top_		;};
  string GetFemBottom			(){ return current_run_->fem_bottom_		;};
  int GetLadderNumInUse			(){ return current_run_->ladder_num_in_use_	;};
  vector < string > GetLadders		(){ return current_run_->ladders_		;};
  vector < string > GetRocPorts	(){ return current_run_->roc_ports_		;};
  vector < int > GetModules		(){ return current_run_->modules_		;};
  vector < string > GetConversion	(){ return current_run_->conversion_		;};
  vector < string > GetBusExtenders	(){ return current_run_->bus_extenders_		;};
  vector < string > GetCamacAdcs	(){ return current_run_->camac_adcs_		;};
  vector < string > GetCamacTdcs	(){ return current_run_->camac_tdcs_		;};
  string GetDescription			(){ return current_run_->description_		;};
  string GetModuleCut			(){ return current_run_->GetModuleCut()		;};

};

// constructor
Database::Database( string file )
{
  file_ = file;
  Init();
}

void Database::Init()
{

  // open the database file
  ifstream ifs( file_.c_str() );

  // if the file was not opened, show warning
  if( ifs.fail() )
    {
      cerr << file_ << " is not found" << endl;
      return;
    }

  string header, line;
  // read the header, it's also needd for the Run class
  getline( ifs, header );

  // loop over all runs
  while( getline( ifs, line ) )
    {
      // make an instance of this run
      Run* run = new Run( header, line );

      // if this line contains no data, stop reading the file
      if( run->data_ == "" )
	break;
      
      //run->Print();
      runs_.push_back( run );
      //      break;
    }

  ifs.close();
}   

bool Database::SetRun( string data )
{

  // comparison is done for only data name without suffix for better usability
  string data_name = data.substr( data.find_last_of( "/" ) + 1,
				  data.find_last_of( "." ) - data.find_last_of( "/" ) -1);

  cout << " Look for " << data_name << ".* in the database...";
  // loop over all runs in the database
  for( auto& run: runs_ )
    {

      // check whether this run is the one you want to set or not
      if( run->data_.substr(0, run->data_.find_last_of( "." )) == data_name )
	{
	  current_run_num_ = run->run_num_;
	  //cout << "\t---> found" << endl;

	  current_run_ = run;
	  return true;
	}
    }

  cout << "\t---> NOT found" << endl;
  cerr << data << " is not found in the database" << endl;
  current_run_ = new Run(); // set default one
  return false;
}

bool Database::SetRun( int run_num )
{
  // loop over all runs in the database
  for( auto& run: runs_ )
    {

      // check whether this run is the one you want to set or not
      if( run->run_num_ == run_num )
	{
	  current_run_num_ = run_num;
	  //cout << "\t---> found" << endl;

	  current_run_ = run;
	  return true;
	}
    }

  cout << "\t---> NOT found" << endl;
  cerr << "Run " << run_num << " is not found in the database" << endl;
  current_run_ = new Run(); // set default one
  current_run_num_ = -1;
  return false;
}

TDatime Database::GetDatime()
{
  int index_date = -1;

  string data =current_run_->data_;
  for (int i=0; i<data.size(); i++ )
    {
      try
	{
	  //cout << data.substr(i, 1);
	  int num = std::stoi( data.substr(i, 1) );
	}
      catch (const std::invalid_argument& e)
	{
	  continue;
        }
      index_date = i;
      break;
    }
  
  string year  = data.substr( index_date, 4 );
  string month = data.substr( index_date+4, 2 );
  string day   = data.substr( index_date+6, 2 );
  string hour  = data.substr( index_date+9, 2 );
  string min   = data.substr( index_date+11, 2 );
  string sec   = "0";
  
  TDatime rtn( stoi(year), stoi(month), stoi(day), stoi(hour), stoi(min), stoi(sec) );
  return rtn;
}

string Database::GetRoot()
{
  return current_run_->data_.substr(0, current_run_->data_.find_last_of(".") ) + ".root";
}
