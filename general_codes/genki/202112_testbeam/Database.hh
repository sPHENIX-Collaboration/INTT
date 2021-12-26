/*! * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  @file Database.hh
  @brief Run class and Database class are in this file.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

/*!
  @class Run
  @brief a class contains information of a run ( one row in the database)
*/
class Run
{
public:
  /*! @brief The run number */
  int run_num_ = -1;

  /*! @brief The name of the data (.dat) */
  string data_ = "";

  /*! @brief Source of this run. It can be calibration, source (i.e. radiation source), cosmic, beam, and lost_data (i.e. no data sotred)*/
  string source_ = "";

  /*! @brief The operation mode. It can be self or CAMAC */
  string DAQ_mode_ = "";

  /*! @brief The version of the setup.*/
  string setup_ver_ = "";

  /*! @brief The ID of the ROC in use .*/
  string roc_ = "";

  /*! @brief The ROC column which was connected to the top of the FEM. */
  string fem_top_ = "";

  /*! @brief The ROC column which was connected to the bottom of the FEM. */
  string fem_bottom_ = "";

  /*! @brief The number of ladder in use in this run. */
  int ladder_num_in_use_ = -1;

  /*! @brief A vector array of ladder name.*/
  vector < string > ladders_;

  /*! @brief A vector array of ROC ports. The index corresponds to that of ladders_*/
  vector < string > roc_ports_;
  vector < int > modules_;
  vector < string > conversion_;
  vector < string > bus_extenders_;
  vector < string > camac_adcs_;
  vector < string > camac_tdcs_;

  /*! position of the beam. 0 is the position of run21. Direction is north -> south. Sometimes, position was not recorded. In such cases, position was estimated using its hit map.*/
  double beam_position_;
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
  static void PrintVector( T& values, string separator = " ", string last = "\n" );
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

  double dtemp;
  ss >> dtemp;
  beam_position_ = dtemp;
  
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

  cout << setw(width) << left << " - Beam position" << ": " << beam_position_ << endl;
  cout << setw(width) << left << " - description" << ": " << description_ << endl;

}

/*! 
  @class Database
  @brief a class to provide run infromation
  @details
  # Database class -- Get run information easily! --
  ## How To Use
    1. Generate database from the run_list (Google spread sheet)
         - It's in https://docs.google.com/spreadsheets/d/1XZ7cTRjl9ra9Fn2CyHDyzguJkPW6anMDin0mukZS7D0/edit#gid=0
             1. File > Download > tab separated vertial (.tsv)
             2. Put the tsv file as you want
    2. Include this file:
         - #include "Database.hh"
    3. Make an instance of this class:
    @code
         string database_file = "here/there/everywhere/database.tsv";
         Database* db = new Database( database_file );
    @endcode
    4. Set a data file as a run to be checked:
    @code
         string data = "data/NWU_fphx_raw_20211128-0947_0.root";
         db->SetRun( data );
    @endcode
    5. Get information, for example:
    @code
         string ROC = db->GetRoc();
         int ladder_num = db->GetLadderNumInUse();
         vector < int > modules = db->GetModules();
    @endcode

    ## Example
    A example macro database_demo.cc is shown:

  @include database_demo.cc
*/
class Database
{
private :
  string file_; // file path to the database
  vector < Run* > runs_; // runs

  int current_run_num_ = -1;
  Run* current_run_;
  
  void Init();
  vector < string > GetPartsInUse( string parts );
  
public:
  /*! @brief A constructor.*/
  Database( string file = "documents/run_list - Setup.tsv" );

  /*! @brief You can set a run to treat by a file name.*/
  bool SetRun( string data );

  /*! @brief You can se a run to treat by a run number.*/
  bool SetRun( int run_num );

  /*! @brief It returns an instance of Run.*/
  Run* GetRun(){ return current_run_;};

  /*! @brief It return the number of runs in the run list. It means the last run number.*/
  int GetNumberOfRuns(){ return runs_.size(); };

  /*! @brief It returns all instances of Run.*/
  vector < Run* > GetRuns(){ return runs_;};

  /*! @brief The run number is returned*/
  int GetId				(){ return current_run_->run_num_		;};

  /*! @brief The name of .dat is returned*/
  string GetData			(){ return current_run_->data_			;};

  /*! @brief The timing when this run was started is returned. TDatime is returned.*/
  TDatime GetDatime                     ();

  /*! @brief It returns the source of data in thir run. It can be calibration, source, cosmic, beam, and lost_data.*/
  string GetSource			(){ return current_run_->source_		;};

  /*! @brief It returns the operation mode. It can be self or CAMAC.*/
  string GetDAQMode			(){ return current_run_->DAQ_mode_		;};
  string GetSetupVer			(){ return current_run_->setup_ver_		;};
  string GetRoc				(){ return current_run_->roc_			;};

  /*! @brief It returns the name of ROOT file. By adding the path to the file, you can open the ROOT file. See the introcution macro database_demo.cc.*/
  string GetRootFile			();

  /*! @brief It returns the run nummber.*/
  int GetRunNum				(){ return current_run_->run_num_		;};
  string GetFemTop			(){ return current_run_->fem_top_		;};
  string GetFemBottom			(){ return current_run_->fem_bottom_		;};

  /*! @brief It returns the number of ladders used in this run.*/
  int GetLadderNumInUse			(){ return current_run_->ladder_num_in_use_	;};

  /*! @brief It returns the name of all ladders in the setup. The name of a ladder in the setup but not used is also returned. To get only ladder names in use, use the function GetLaddersInUse.*/
  vector < string > GetLadders		(){ return current_run_->ladders_		;};

  /*! @brief It returns the name of ladders used in this run.*/
  vector < string > GetLaddersInUse	(){ return this->GetPartsInUse("ladder");};

  /*! @brief It returns ROC port of all ladders. If the ladder was not connected, "-" is returned. To get ROC port of ladders used in this run, use GetRocPortsInUse.*/
  vector < string > GetRocPorts	(){ return current_run_->roc_ports_		;};

  /*! @brief It returns ROC ports used in this run. It doesn't work properly for run21 because a ladder was connected but didn't joined in the data taking*/
  vector < string > GetRocPortsInUse	(){ return this->GetPartsInUse("roc");};

  /*! @brief It returns all module numbers.*/
  vector < int > GetModules		(){ return current_run_->modules_		;};
  vector < string > GetConversion	(){ return current_run_->conversion_		;};

  /*! @brief It returns bus extender configuration. To get configuration only for ladders used in this run, use GetBusExtendersInUse.*/
  vector < string > GetBusExtenders	(){ return current_run_->bus_extenders_		;};

  /*! @brief It returns bus extender configuration for ladders used in this run. "-" means no bus extender was used for the ladder.*/
  vector < string > GetBusExtendersInUse(){ return this->GetPartsInUse("bex");};

  /*! @brief It returns information of CAMAC ADCs. ? means no information for this channel in the run list.*/
  vector < string > GetCamacAdcs	(){ return current_run_->camac_adcs_		;};

  /*! @brief It returns information of CAMAC TDCs. ? means no information for this channel in the run list.*/
  vector < string > GetCamacTdcs	(){ return current_run_->camac_tdcs_		;};

  /*! @brief It returns position of the setup. 0 cm means the position in run21, and the beam was on the chip11. The direction is from north to south. It's the same direction as in MC.*/
  double GetBeamPosition                (){ return current_run_->beam_position_;};

  /*! @brief It returns the description for this run in the run list.*/
  string GetDescription			(){ return current_run_->description_		;};

  /*!
    @brief It returns a cut to eliminate data, which has strange module.
    @details For example: (module==1||module==6||module==5)
   */
  string GetModuleCut			(){ return current_run_->GetModuleCut()		;};

  /*! @brief It returns true if the source is "beam" */
  bool IsBeamRun();
  
  void Print();
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

string Database::GetRootFile()
{
  return current_run_->data_.substr(0, current_run_->data_.find_last_of(".") ) + ".root";
}

bool Database::IsBeamRun()
{
  if( current_run_->source_ == "beam" )
    return true;

  return false;
}

vector < string > Database::GetPartsInUse( string parts )
{

  vector < string > parts_in_use;
  vector < string > roc_ports = current_run_->roc_ports_;
  vector < string > all_parts;

  if( parts == "ladder" )
    all_parts = current_run_->ladders_;
  else if( parts == "bex" )
    all_parts = current_run_->bus_extenders_;
  else if( parts == "roc" )
    all_parts = current_run_->roc_ports_;
  
  for( int i=0; i<roc_ports.size(); i++ )
    {
      if( roc_ports[i] != "-" )
	parts_in_use.push_back( all_parts[i] );
    }

  return parts_in_use;
}

void Database::Print()
{

  int width = 10;
  for( auto& run : runs_ )
    {
      cout << " ";
      cout << setw(2) << left << run->run_num_ << " ";
      cout << setw(width) << left << run->data_ << " ";
      cout << setw(5) << left << run->source_ << " ";

      cout << setw(10) << left << run->DAQ_mode_ << " ";
      cout << setw(3) << left << run->setup_ver_ << " ";
      cout << setw(2) << left << run->roc_ << " ";
      cout << setw(3) << left << run->fem_top_ << " ";
      cout << setw(3) << left << run->fem_bottom_ << " ";
      cout << setw(4) << left << run->ladder_num_in_use_ << " ";
      Run::PrintVector( run->ladders_, " ", "," );
      
      Run::PrintVector( run->roc_ports_, " ", "," );
      
      Run::PrintVector( run->modules_, " ", "," );
      
      Run::PrintVector( run->conversion_, " ", "," );
      
      Run::PrintVector( run->bus_extenders_, " ", "," );
      
      Run::PrintVector( run->camac_adcs_, " ", "," );
      
      Run::PrintVector( run->camac_tdcs_, " ", "," );
      
      cout << setw(width) << left << run->beam_position_ ;
      //cout << setw(width) << left << "\"" << run->description_ << "\"";
      cout << endl;
      
    }
  
}
